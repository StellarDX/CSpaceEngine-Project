#include "CSE/Base/AdvMath.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN
_SCICXX_BEGIN

////////////////////////////////// 微分方程基类 //////////////////////////////////

float64 OrdinaryDifferentialEquation::CurrentPoint() const
{
    return Direction ? (StateBuffer.begin())->first : (--StateBuffer.end())->first;
}

float64 OrdinaryDifferentialEquation::PrevPoint() const
{
    if (StateBuffer.size() <= 1) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
    if (Direction)
    {
        auto begin = StateBuffer.begin();
        return (++begin)->first;
    }
    auto end = --StateBuffer.end();
    return (--end)->first;
}

void __cdecl OrdinaryDifferentialEquation::InvokeRun() noexcept(0)
{
    if (State != Processing) {throw std::logic_error("Engine is finished.");}

    auto CP = CurrentPoint();
    if (CurrentPoint() == EndPoint)
    {
        State = Succeeded;
        return;
    }

    int ExitCode = Run();

    if (ExitCode)
    {
        State = StateCode(ExitCode);
        return;
    }

    if (((Direction ? -1. : 1.) * (CurrentPoint() - EndPoint)) > 0)
    {
        State = Succeeded;
        return;
    }
}



////////////////////////////////// 龙格-库塔引擎 /////////////////////////////////

#include "ODEs_RungeKutta.tbl"

void RungeKuttaODEEngine::Init(ValueArray InitState, float64 First, float64 Last)
{
    Init(InitState, First, Last, __Float64::FromBytes(BIG_NAN_DOUBLE));
}

void RungeKuttaODEEngine::Clear()
{
    State = Processing;
    EndPoint = 0;
    Direction = 0;
    StateBuffer.clear();
    Interpolants.clear();
    CurrentFx = ValueArray(0.);
    KTable.fill(0);
    AbsStep = 0;
}

float64 RungeKuttaODEEngine::RMSNorm(std::vector<float64> Input)
{
    float64 SumTemp = 0;
    for (size_t i = 0; i < Input.size(); ++i){SumTemp += pow(Input[i], 2);}
    return sqrt(SumTemp / float64(Input.size()));
}

void RungeKuttaODEEngine::Init(ValueArray InitState, float64 First, float64 Last, float64 InitStep)
{
    StateBuffer.insert({First, InitState});
    EndPoint = Last;
    CurrentFx = Invoker(First, InitState);
    if (CurrentFx.size() != InitState.size())
    {
        throw std::logic_error("Solution count is not equal to parameter count.");
    }
    Direction = __Float64(EndPoint - CurrentPoint()).Negative;
    State = Processing;
    if (isnan(InitStep)){SetInitStep();}
    else {AbsStep = InitStep;}
}

void RungeKuttaODEEngine::SetInitStep()
{
    if (StateBuffer.empty()) {AbsStep = __Float64::FromBytes(POS_INF_DOUBLE);}

    float64 RelToler = pow(10, -RelTolerNLog);
    float64 AbsToler = pow(10, -AbsTolerNLog);

    ValueArray y0 = StateBuffer.begin()->second;
    ValueArray Scale;
    for (uint64 i = 0; i < EquationCount; ++i)
    {
        Scale.push_back(AbsToler + abs(y0[i]) * RelToler);
    }

    float64 d0 = RMSNorm(y0 / Scale), d1 = RMSNorm(CurrentFx / Scale), h0;

    if (d0 < 1E-5 || d1 < 1E-5) {h0 = 1E-6;}
    else {h0 = 0.01 * d0 / d1;}

    ValueArray y1 = y0 + h0 * (Direction ? -1 : 1) * CurrentFx, f1;
    f1 = Invoker(CurrentPoint() + h0 * (Direction ? -1 : 1), y1);

    float64 d2 = RMSNorm((f1 - CurrentFx) / Scale) / h0;

    float64 h1;
    if (d1 <= 1E-15 && d2 <= 1E-15){h1 = max(1E-6, h0 * 1E-3);}
    else {h1 = yroot((0.01 / max(d1, d2)), ErrExponent);}

    AbsStep = min(100 * h0, h1);
}

int RungeKuttaODEEngine::Run()
{
    auto t = CurrentPoint();
    ValueArray y = Direction ? StateBuffer.begin()->second : (--StateBuffer.end())->second;

    float64 RelToler = pow(10, -RelTolerNLog);
    float64 AbsToler = pow(10, -AbsTolerNLog);
    float64 MinStep = 10. * abs((nextafter(t, (Direction ? -1 : 1) *
        std::numeric_limits<float64>::infinity())) - t);

    float64 AbsH;
    if (AbsStep > MaxStep) {AbsH = MaxStep;}
    else if (AbsStep < MinStep) {AbsH = MinStep;}
    else {AbsH = AbsStep;}

    bool Accept = 0, Reject = 0;
    float64 h, NextT;
    ValueArray NewY, NewF;

    while (!Accept)
    {
        if (AbsH < MinStep) {return 1;}

        h = (Direction ? -1 : 1) * AbsH;
        NextT = t + h;

        if ((Direction ? -1 : 1) * (NextT - EndPoint) > 0)
        {
            NextT = EndPoint;
        }

        h = NextT - t;
        float64 NewAbsStep = abs(h);

        KTable.SetColumn(0, CurrentFx);
        for (int i = 1; i < NStages; ++i)
        {
            ValueArray dy(EquationCount);
            for (int j = 0; j < EquationCount; ++j)
            {
                dy[j] = 0;
                for (int k = 0; k < i; ++k)
                {
                    dy[j] += KTable.at(k, j) * ATable[i * StepTakenOrder + k];
                }
                dy[j] *= h;
            }
            KTable.SetColumn(i, Invoker(t + CTable[i] * h, y + dy));
        }

        DynamicMatrix<float64> KTableWithoutBack(KTable);
        KTableWithoutBack.DeleteColumn(NStages);
        DynamicMatrix<float64> BTable((float64*)(this->BTable), {1, NStages});
        auto YScale = KTableWithoutBack * BTable;
        NewY = y + h * YScale[0];
        NewF = Invoker(t + h, NewY);
        KTable.SetColumn(NStages, NewF);

        // ---------- Runge-Kutta solver End ---------- //

        ValueArray Scale;
        for (uint64 i = 0; i < y.size(); ++i)
        {
            Scale.push_back(AbsToler + max(y[i], NewY[i]) * RelToler);
        }

        // ---------- Error Esitmator Begin ---------- //

        DynamicMatrix<float64> ETable((float64*)(this->ETable), {1, NStages + 1});
        auto EstmErrorMat = KTable * ETable;
        ValueArray EstmError = (EstmErrorMat[0] * h) / Scale;

        float64 EstmErrNorm, EENTempSum = 0;
        for (size_t i = 0; i < EquationCount; ++i) {EENTempSum += pow(EstmError[i], 2);}
        EstmErrNorm = sqrt(EENTempSum / float64(EstmError.size()));

        // ---------- Error Esitmator End ---------- //

        float64 Factor;
        if (EstmErrNorm < 1)
        {
            if (EstmErrNorm == 0) {Factor = MaxFactor;}
            else {Factor = min(MaxFactor, FactorSafe * yroot(EstmErrNorm, -ErrExponent));}
            if (Reject) {Factor = min(1, Factor);}
            AbsH *= Factor;
            Accept = 1;
        }
        else
        {
            AbsH *= max(MinFactor, FactorSafe * yroot(EstmErrNorm, -ErrExponent));
            Reject = 1;
        }
    }

    StateBuffer.insert({NextT, NewY});
    AbsStep = AbsH;
    CurrentFx = NewF;

    return 0;
}

void RungeKuttaODEEngine::SaveDenseOutput()
{
    DynamicMatrix<float64> PTable({DenseOutputOrder, NStages + 1});
    for (int i = 0; i < DenseOutputOrder; ++i)
    {
        for (int j = 0; j < NStages + 1; ++j)
        {
            PTable.at(i, j) = *(this->PTable + (DenseOutputOrder) * j + i);
        }
    }

    decltype(StateBuffer.begin()) PrevState;
    if (Direction)
    {
        PrevState = ++StateBuffer.begin();
    }
    else
    {
        auto end = --StateBuffer.end();
        PrevState = --end;
    }

    Interpolants.insert({CurrentPoint(),
        DenseOutput(DenseOutputOrder, PrevState->first, CurrentPoint(),
        PrevState->second, KTable * PTable)});
}

inline OrdinaryDifferentialEquation::DenseOutput::ValueArray
RungeKuttaODEEngine::DenseOutput::operator()(float64 _Xx) const
{
    float64 x = (_Xx - First) / size();
    std::vector<float64> p;
    for (int i = 0; i < DenseOutputOrder; ++i)
    {
        p.push_back(pow(x, i + 1));
    }
    auto y = size() * (QTable * DynamicMatrix<float64>{p});
    return (DynamicMatrix<float64>{Base} + y).GetColumn(0);
}

RungeKuttaODEEngine::ValueArray RungeKuttaODEEngine::operator()(float64 _Xx) const
{
    DenseOutput Segment;
    for (auto i : Interpolants)
    {
        float64 Min = min(i.second.first(), i.second.last());
        float64 Max = max(i.second.first(), i.second.last());
        if (Min <= _Xx && _Xx <= Max)
        {
            Segment = i.second;
            break;
        }
    }
    return Segment(_Xx);
}

_SCICXX_END
_CSE_END
