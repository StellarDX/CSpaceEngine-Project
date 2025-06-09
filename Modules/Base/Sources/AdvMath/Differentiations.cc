#include "CSE/Base/AdvMath.h"

_CSE_BEGIN
_SCICXX_BEGIN

////////////////////////////////////// FDM /////////////////////////////////////

std::vector<float64> Adaptive1stOrderFDDerivativeFunction::Iterator::DerivativeWeight(uint64 n)
{
    if (CentralWeights.size() != 2 * n + 1)
    {
        std::vector<float64> hc;
        for (int64 i = -int64(n); i < int64(n) + 1; ++i)
        {
            hc.push_back(sgn(float64(i)) / pow(StepFactor, abs(float64(i)) - 1));
        }
        DynamicMatrix<float64> IVandermondeMatrixC = InverseVandermonde(hc);
        DynamicMatrix<float64> ResultVectorC;
        ResultVectorC.resize({1, 2 * n + 1});
        ResultVectorC.at(0, 1) = 1;
        DynamicMatrix<float64> WeightsC = IVandermondeMatrixC * ResultVectorC;
        CentralWeights = WeightsC.GetColumn(0);

        std::vector<float64> hf;
        for (int64 i = 0; i < 2 * n + 1; ++i)
        {
            hf.push_back(sgn(float64(i)) / pow(sqrt(StepFactor), i - 1));
        }
        DynamicMatrix<float64> IVandermondeMatrixF = InverseVandermonde(hf);
        DynamicMatrix<float64> ResultVectorF;
        ResultVectorF.resize({1, 2 * n + 1});
        ResultVectorF.at(0, 1) = 1;
        DynamicMatrix<float64> WeightsF = IVandermondeMatrixF * ResultVectorF;
        ForwardWeights = WeightsF.GetColumn(0);
    }

    return Direction == Center ? CentralWeights : ForwardWeights;
}

DynamicMatrix<float64> Adaptive1stOrderFDDerivativeFunction::Iterator::PreEvaluator()
{
    auto n = Terms;
    auto h = Step;
    auto c = StepFactor;
    auto d = sqrt(c);

    std::vector<float64> CenterSamples;
    std::vector<float64> ForwardSamples;
    if (!IterCount)
    {
        for (int i = 0; i < n; ++i)
        {
            float64 InsertValue = h / pow(c, i);
            CenterSamples.insert(CenterSamples.begin(), -InsertValue);
            CenterSamples.insert(CenterSamples.end(), InsertValue);
        }
        for (int j = 0; j < 2 * n; ++j)
        {
            ForwardSamples.push_back(h / pow(d, j));
        }
    }
    else
    {
        float64 InsertValue = h / pow(c, n - 1);
        CenterSamples.insert(CenterSamples.begin(), -InsertValue);
        CenterSamples.insert(CenterSamples.end(), InsertValue);
        ForwardSamples.push_back(h / pow(c, n - 1));
        ForwardSamples.push_back((h / d) / pow(c, n - 1));
    }

    uint64 FinalSize = !IterCount ? 2 * n : 2;
    DynamicMatrix<float64> Result;
    Result.resize({1, FinalSize});
    Result.fill(Input);
    switch (Direction)
    {
    case Center:
        Result += DynamicMatrix<float64>({CenterSamples});
        break;
    case Forward:
        Result += DynamicMatrix<float64>({ForwardSamples});
        break;
    case Backward:
        Result -= DynamicMatrix<float64>({ForwardSamples});
        break;
    }
    return Result;
}

void Adaptive1stOrderFDDerivativeFunction::Iterator::PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx)
{
    uint64 n = Terms;

    DynamicMatrix<float64> SolutionArray;
    DynamicMatrix<float64> ReducedSolutionArray;
    switch (Direction)
    {
    case Center:
        if (!IterCount)
        {
            SolutionArray = fx;
            SolutionArray.AddRow(n, Intermediates.GetRow(0));
            ReducedSolutionArray = SolutionArray;
        }
        else
        {
            SolutionArray = Intermediates;
            SolutionArray.AddRow(0, fx.GetRow(0));
            SolutionArray.AddRow(SolutionArray.row(), fx.GetRow(1));
            ReducedSolutionArray.resize({1, 1});
            ReducedSolutionArray.at(0, 0) = SolutionArray.at(0, n);
            for (int i = 0; i < n; ++i)
            {
                ReducedSolutionArray.AddRow(i, SolutionArray.GetRow(i));
                ReducedSolutionArray.AddRow(ReducedSolutionArray.row() - i,
                    SolutionArray.GetRow(SolutionArray.row() - i - 1));
            }
        }
        break;
    case Forward:
    case Backward:
        if (!IterCount)
        {
            SolutionArray = fx;
            SolutionArray.AddRow(0, Intermediates.GetRow(0));
            ReducedSolutionArray = SolutionArray;
        }
        else
        {
            SolutionArray = Intermediates;
            SolutionArray.AddRow(SolutionArray.row(), fx.GetRow(0));
            SolutionArray.AddRow(SolutionArray.row(), fx.GetRow(1));
            ReducedSolutionArray.resize({1, 1});
            ReducedSolutionArray.at(0, 0) = SolutionArray.at(0, 0);
            for (int i = 0; i < 2 * n; ++i)
            {
                ReducedSolutionArray.AddRow(ReducedSolutionArray.row() - i,
                    SolutionArray.GetRow(SolutionArray.row() - i - 1));
            }
        }
        break;
    }

    Intermediates = SolutionArray;

    std::vector<float64> WeightArray = DerivativeWeight(n);
    LastOutput = Output;
    float64 NewSolution = 0;
    for (std::size_t i = 0; i < ReducedSolutionArray.row(); ++i)
    {
        NewSolution += WeightArray[i] * ReducedSolutionArray.at({0, i});
    }
    if (Direction == Backward) {NewSolution *= -1;}
    Output = NewSolution / Step;

    Step /= StepFactor;
    LastError = Error;
    Error = abs(Output - LastOutput);
}

bool Adaptive1stOrderFDDerivativeFunction::Iterator::CheckTerminate()
{
    if (IterCount)
    {
        bool IsInfinite = !(std::isfinite(Input) && std::isfinite(Output));
        if (IsInfinite)
        {
            State = ValueError;
            return 1;
        }
    }

    if (IterCount > 1)
    {
        bool EstimateError = Error < AbsoluteTolerence + RelativeTolerence * abs(Output);
        if (EstimateError)
        {
            State = Finished;
            return 1;
        }

        auto IsErrorIncrease = Error > LastError * 10;
        if (IsErrorIncrease)
        {
            State = decltype(State)(ErrorIncrease);
            return 1;
        }
    }
    return 0;
}

float64 Adaptive1stOrderFDDerivativeFunction::operator()(float64 x)const
{
    Iterator it;
    it.Input = x;
    it.Intermediates.resize({1, 1});
    it.Intermediates.at(0, 0) = OriginalFunction(x);
    it.Step = InitialStepSize;
    it.StepFactor = StepFactor;
    it.AbsoluteTolerence = pow(10, -AbsoluteTolerence);
    it.RelativeTolerence = pow(10, -RelativeTolerence);
    it.State = Iterator::InProgress;
    it.Terms = uint64(FDMOrder + 1) / 2ULL;
    it.Direction = Direction;
    it.Run(OriginalFunction, MaxIteration);
    return it.State == it.Finished ? it.Output : it.LastOutput;
}



//////////////////////////////// 黎曼-刘维尔导数 ////////////////////////////////

std::vector<float64> RiemannLiouvilleBinomialFDDerivativeFunction::Iterator::DerivativeWeight(uint64 n)
{
    if (Weights.size() != n + 1)
    {
        Weights.resize(n + 1);
        Weights[0] = 1;
        for (uint64 j = 1; j <= n; j++)
        {
            Weights[j] = Weights[j - 1] * (1. - (DerivativeOrder + 1.) / float64(j));
        }
    }

    return Weights;
}

DynamicMatrix<float64> RiemannLiouvilleBinomialFDDerivativeFunction::Iterator::PreEvaluator()
{
    std::vector<float64> Samples;
    for (int64 k = 0; k <= Terms; ++k)
    {
        Samples.push_back(k * (-Step));
    }
    DynamicMatrix<float64> Result;
    Result.resize({1, Terms + 1});
    Result.fill(Input);
    Result += {Samples};
    return Result;
}

void RiemannLiouvilleBinomialFDDerivativeFunction::Iterator::PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx)
{
    std::vector<float64> WeightArray = DerivativeWeight(Terms);
    LastOutput = Output;
    float64 NewSolution = 0;
    for (std::size_t i = 0; i <= Terms; ++i)
    {
        NewSolution += WeightArray[i] * fx.at({0, i});
    }
    Output = NewSolution / pow(Step, DerivativeOrder);
    Step /= StepFactor;
    LastError = Error;
    Error = abs(Output - LastOutput);
}

bool RiemannLiouvilleBinomialFDDerivativeFunction::Iterator::CheckTerminate()
{
    if (IterCount)
    {
        auto IsFinite = [](float64 x) {return !isinf(x) && !isnan(x);};
        bool IsInfinite = !(IsFinite(Input) && IsFinite(Output));
        if (IsInfinite)
        {
            State = ValueError;
            return 1;
        }
    }

    if (IterCount > 1)
    {
        bool EstimateError = Error < AbsoluteTolerence + RelativeTolerence * abs(Output);
        if (EstimateError)
        {
            State = Finished;
            return 1;
        }

        auto IsErrorIncrease = Error > LastError * 10;
        if (IsErrorIncrease)
        {
            State = decltype(State)(ErrorIncrease);
            return 1;
        }
    }
    return 0;
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::IntegralInline(Function1D Func, float64 x)const
{
    return (*Engine)(Func, InitValue, x);
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::DerivativeInline(Function1D Func, float64 x)const
{
    Iterator it;
    it.DerivativeOrder = ceil(DerivativeOrder);
    it.Input = x;
    it.Step = pow(10, -InitialStepSize);
    it.StepFactor = StepFactor;
    it.AbsoluteTolerence = pow(10, -AbsoluteTolerence);
    it.RelativeTolerence = pow(10, -RelativeTolerence);
    it.State = Iterator::InProgress;
    it.Terms = ceil(DerivativeOrder);
    it.Run(Func, MaxIteration);
    return it.State == it.Finished ? it.Output : it.LastOutput;
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::Binomial(uint64 x)const
{
    if (x == 0) {return OriginalFunction(x);}
    return DerivativeInline(OriginalFunction, x);
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::RiemannLiouville(float64 x) const
{
    if (x == 0) {return OriginalFunction(x);}
    float64 alf = DerivativeOrder;
    uint64 n = ceil(alf);
    return DerivativeInline([this, alf, n](float64 _x)
    {
        return IntegralInline([this, alf, n, _x](float64 t)
        {
            return OriginalFunction(t) / pow(_x - t, alf + 1 - n);
        }, _x);
    }, x) / tgamma(n - alf);
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::Caputo(float64 x) const
{
    if (x == 0) {return OriginalFunction(x);}
    float64 alf = DerivativeOrder;
    uint64 n = ceil(alf);
    return IntegralInline([this, x, alf, n](float64 t)
    {
        return DerivativeInline(OriginalFunction, t) / pow(x - t, alf + 1 - n);
    }, x) / tgamma(n - alf);
}

float64 RiemannLiouvilleBinomialFDDerivativeFunction::operator()(float64 x) const
{
    if (CheckInt(__Float64(DerivativeOrder).Bytes)) {return Binomial(x);}
    return RiemannLiouville(x);
}

_SCICXX_END
_CSE_END
