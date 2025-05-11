#include "CSE/Base/AdvMath.h"

_CSE_BEGIN
_SCICXX_BEGIN

////////////////////////////////////// FDM /////////////////////////////////////

std::vector<float64> FiniteDifferenceDerivativeFunction::Iterator::DerivativeWeight(uint64 n)
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

DynamicMatrix<float64> FiniteDifferenceDerivativeFunction::Iterator::PreEvaluator()
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

void FiniteDifferenceDerivativeFunction::Iterator::PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx)
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

bool FiniteDifferenceDerivativeFunction::Iterator::CheckTerminate()
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

float64 FiniteDifferenceDerivativeFunction::operator()(float64 x)const
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
    return it.Output;
}

_SCICXX_END
_CSE_END
