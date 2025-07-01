#include "CSE/Base/AdvMath.h"

_CSE_BEGIN
_SCICXX_BEGIN

////////////////////////////////// 布伦特反函数 //////////////////////////////////

void BrentInverseFunction::CheckParameters()
{
    if (Domain[0] > Domain[1])
    {
        std::swap(Domain[0], Domain[1]);
        std::swap(OpenDomain[0], OpenDomain[1]);
    }
    if (Range[0] > Range[1]) {std::swap(Range[0], Range[1]);}
}

vec2 BrentInverseFunction::GetReferencePoints(vec2 Domain)const
{
    if (!isinf(Domain[0]) && !isinf(Domain[1]))
    {
        float64 Step = Domain[1] - Domain[0];
        return {Domain[0] + Step / 4., Domain[1] - Step / 4.};
    }
    else if (!isinf(Domain[0]) && isinf(Domain[1]))
    {
        return {Domain[0] + 1., Domain[0] + 2.};
    }
    else if (isinf(Domain[0]) && !isinf(Domain[1]))
    {
        return {Domain[1] - 2., Domain[1] - 1.};
    }
    else {return {0, 1};}
}

vec2 BrentInverseFunction::GetReferenceValues(vec2 Domain, int Trend) const
{
    vec2 Result = Range;
    if (isinf(Result[0]))
    {
        if ((!isinf(Domain[0]) && Trend == 1) || (!isinf(Domain[1]) && Trend == -1))
        {
            Result[0] = (Trend == 1) ? OriginalFunction(Domain[0]) : OriginalFunction(Domain[1]);
        }
    }
    if (isinf(Result[1]))
    {
        if ((!isinf(Domain[1]) && Trend == 1) || (!isinf(Domain[0]) && Trend == -1))
        {
            Result[1] = (Trend == 1) ? OriginalFunction(Domain[1]) : OriginalFunction(Domain[0]);
        }
    }
    return Result;
}

float64 BrentInverseFunction::operator()(float64 x) const
{
    vec2 RefPoints = GetReferencePoints(Domain);
    float64 a1 = OriginalFunction(RefPoints[1]), a2 = OriginalFunction(RefPoints[0]);
    int Trend = sgn(OriginalFunction(RefPoints[1]) - OriginalFunction(RefPoints[0]));
    if (!(Trend))
    {
        throw std::logic_error("Function is not strictly monotonic");
    }
    vec2 RefValues = GetReferenceValues(Domain, Trend);

    auto Bound = [this, &Trend](float64 x)
    {
        if (!isinf(Domain[0]) && (x < Domain[0] || (x == Domain[0] && OpenDomain[0])))
        {
            return __Float64::FromBytes(NEG_INF_DOUBLE) * Trend;
        }
        else if (!isinf(Domain[1]) && (x > Domain[1] || (x == Domain[1] and OpenDomain[1])))
        {
            return __Float64::FromBytes(POS_INF_DOUBLE) * Trend;
        }

        return OriginalFunction(x);
    };

    if (!isinf(RefValues[0]))
    {
        bool Reject;
        if ((OpenDomain[0] && Trend == 1) || (OpenDomain[1] && Trend == -1))
        {
            Reject = x <= RefValues[0];
        }
        else {Reject = x < RefValues[0];}
        if (Reject)
        {
            throw std::logic_error("Requested values lower than the"
                " lower limit of the range");
        }
    }

    if (!isinf(RefValues[1]))
    {
        bool Reject;
        if ((OpenDomain[1] and Trend == 1) or (OpenDomain[0] and Trend == -1))
        {
            Reject = x >= RefValues[1];
        }
        else {Reject = x > RefValues[1];}
        if (Reject)
        {
            throw std::logic_error("Requested values higher than the"
                " higher limit of the image");
        }
    }

    if (!isinf(RefPoints[1]) && Bound(RefPoints[1]) == x)
    {
        return RefPoints[1];
    }
    if (!isinf(RefPoints[0]) && Bound(RefPoints[0]) == x)
    {
        return RefPoints[0];
    }

    auto LossFunction = [&Bound, x](float64 _x)
    {
        return pow(Bound(_x) - x, 2);
    };

    BrentUnboundedMinimizer Minimizer;
    vec2 Result = Minimizer.Run(LossFunction, {RefPoints[0], RefPoints[1]});
    return Result.x;
}


//////////////////////////////////// 二分搜索 ////////////////////////////////////

float64 BisectionRootFindingEngine::Run(float64 x, uint64* IterCount, uint64* FCallCount) const
{
    Function1D Func([x, this](float64 _x){return OriginalFunction(_x) - x;});
    uint64 MaxIter = floor(pow(10, MaxIteration));
    float64 AbsTol = pow(10, -AbsoluteTolernece);
    float64 RelTol = pow(10, -RelativeTolerence);

    float64 xmin = First, xmax = Last;
    float64 fmin = Func(xmin);
    float64 fmax = Func(xmax);
    if (FCallCount) {(*FCallCount) = 2;}
    if (!fmin) {return xmin;}
    if (!fmax) {return xmax;}
    if (fmin * fmax >= 0)
    {
        throw std::logic_error("No change of sign between f(a) and f(b), "
            "either there is no root to find, or there are multiple roots in the interval.");
    }

    uint64 Count = 0;
    float64 Result;
    for (; (Count <= MaxIter); ++Count)
    {
        float64 xmid = (xmin + xmax) / 2;
        float64 fmid = Func(xmid);
        if (FCallCount) {++(*FCallCount);}
        if (((xmid == xmax) || (xmid == xmin)) || (!fmid) ||
            (abs(xmax - xmin) < AbsTol + RelTol * abs(xmid)))
        {
            Result = xmid;
            break;
        }
        else if (sgn(fmid) * sgn(fmin) < 0)
        {
            xmax = xmid;
        }
        else
        {
            xmin = xmid;
            fmin = fmid;
        }
    }

    if (IterCount) {(*IterCount) = Count;}
    return Result;
}

float64 BisectionRootFindingEngine::operator()(float64 x) const
{
    return Run(x);
}


///////////////////////////////// 豪斯霍尔德迭代族 ////////////////////////////////

float64 HouseholderIteratorGroup::Run(float64 x, uint64* IterCount, uint64* FCallCount) const
{
    Function1D Func([x, this](float64 _x){return OriginalFunction(_x) - x;});
    uint64 AbsTol = pow(10, -AbsoluteTolerence);
    uint64 RelTol = pow(10, -RelativeTolerence);
    uint64 MaxIter = floor(pow(10, MaxIteration));

    float64 x0 = ReferencePoint;
    uint64 Iter = 0;
    for (; Iter <= MaxIter; ++Iter)
    {
        // 第一次检验
        float64 f = Func(x0);
        if (FCallCount) {++(*FCallCount);}
        if (!f)
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x0;
        }

        // 应用Faa di Bruno公式
        std::vector<float64> df;
        for (auto& i : DerivativeFunctions)
        {
            df.push_back(i(x0));
        }
        if (FCallCount) {(*FCallCount) += df.size();}
        DynamicMatrix<float64> B = BellPolynomialsTriangularArray(df);
        uint64 n = Order();
        float64 gn0 = (n == 1) ? (1. / f) : 0, gn1 = 0;
        for (uint64 k = 1; k <= n - 1; ++k)
        {
            gn0 += ((((k & 1) ? -1 : 1) * tgamma(k + 1)) / pow(f, k + 1)) * B.at(n - 1, k);
        }
        for (uint64 k = 1; k <= n; ++k)
        {
            gn1 += ((((k & 1) ? -1 : 1) * tgamma(k + 1)) / pow(f, k + 1)) * B.at(n, k);
        }
        // 处理分母为0的情况
        if (!gn1) {throw std::logic_error("Derivative was zero.");}

        float64 Step = n * gn0 / gn1;
        float64 x1 = x0 + Step;
        if (abs(Step) <= (AbsTol + RelTol * abs(x0)))
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x1;
        }
        x0 = x1;
    }
    throw std::logic_error("Failed to converge");
}

float64 HouseholderIteratorGroup::operator()(float64 x) const
{
    switch (Order())
    {
    case 1:
        return Newton([x, this](float64 _x)
            {return OriginalFunction(_x) - x;},
            DerivativeFunctions[0], ReferencePoint,
            nullptr, nullptr, MaxIteration,
            AbsoluteTolerence, RelativeTolerence);
        break;
    case 2:
        return Halley([x, this](float64 _x)
            {return OriginalFunction(_x) - x;},
            DerivativeFunctions[0], DerivativeFunctions[1],
            ReferencePoint, nullptr, nullptr, MaxIteration,
            AbsoluteTolerence, RelativeTolerence);
    default:
        return Run(x);
        break;
    }
}

float64 HouseholderIteratorGroup::Newton(Function1D Func, Function1D DFunc, float64 x0,
    uint64* IterCount, uint64* FCallCount, float64 MaxIteration,
    float64 AbsoluteTolerence, float64 RelativeTolerence)
{
    uint64 AbsTol = pow(10, -AbsoluteTolerence);
    uint64 RelTol = pow(10, -RelativeTolerence);
    uint64 MaxIter = floor(pow(10, MaxIteration));

    uint64 Iter = 0;
    for (; Iter <= MaxIter; ++Iter)
    {
        float64 f = Func(x0);
        if (FCallCount) {++(*FCallCount);}
        if (!f)
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x0;
        }

        float64 df = DFunc(x0);
        if (!df) {throw std::logic_error("Derivative was zero.");}

        float64 Step = f / df;
        float64 x1 = x0 - Step;
        if (abs(Step) <= (AbsTol + RelTol * abs(x0)))
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x1;
        }
        x0 = x1;
    }
    throw std::logic_error("Failed to converge");
}

float64 HouseholderIteratorGroup::Halley(Function1D Func, Function1D DFunc, Function1D D2Func, float64 x0,
    uint64* IterCount, uint64* FCallCount, float64 MaxIteration,
    float64 AbsoluteTolerence, float64 RelativeTolerence)
{
    uint64 AbsTol = pow(10, -AbsoluteTolerence);
    uint64 RelTol = pow(10, -RelativeTolerence);
    uint64 MaxIter = floor(pow(10, MaxIteration));

    uint64 Iter = 0;
    for (; Iter <= MaxIter; ++Iter)
    {
        float64 f = Func(x0);
        if (FCallCount) {++(*FCallCount);}
        if (!f)
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x0;
        }

        float64 df = DFunc(x0);
        float64 d2f = D2Func(x0);
        float64 gn0 = f * df;
        float64 gn1 = (df * df) - ((f * d2f) / 2.);
        if (!gn1) {throw std::logic_error("Derivative was zero.");}

        float64 Step = gn0 / gn1;
        float64 x1 = x0 - Step;
        if (abs(Step) <= (AbsTol + RelTol * abs(x0)))
        {
            if (IterCount) {(*IterCount) = Iter;}
            return x1;
        }
        x0 = x1;
    }
    throw std::logic_error("Failed to converge");
}

_SCICXX_END
_CSE_END
