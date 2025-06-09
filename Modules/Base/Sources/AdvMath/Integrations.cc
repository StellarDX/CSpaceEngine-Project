#include "CSE/Base/AdvMath.h"
#include "CSE/Base/ConstLists.h"
#include <memory>

_CSE_BEGIN
_SCICXX_BEGIN

//////////////////////////////////// 积分基类 ///////////////////////////////////

float64 DefiniteIntegratingFunction::operator()(Function1D f, float64 a, float64 b)const
{
    if (a == b) {return 0;}

    // 此处由于积分上下限可以是无穷，所以需要一些准备工作。
    Function1D F;
    float64 A, B;
    float64 Scale = 1;

    if (b < a)
    {
        std::swap(b, a);
        Scale = -1;
    }

    // 无穷积分转化为-1到1的积分
    if (isinf(a) && isinf(b))
    {
        if (sgn(a) > 0 || sgn(b) < 0){throw std::logic_error("Invalid limits");}
        A = -1;
        B = 1;
        F = [&f](float64 t)
        {
            return f(t / (1. - t * t)) * ((1. + t * t) / pow((1. - t * t), 2));
        };
    }

    // 单边无穷积分转化为0到1的积分
    else if (!isinf(a) && isinf(b))
    {
        if (sgn(b) < 0) {throw std::logic_error("Invalid limits");}
        A = 0;
        B = 1;
        F = [a, &f](float64 t)
        {
            return f(a + t / (1. - t)) / pow(1. - t, 2);
        };
    }
    else if (isinf(a) && !isinf(b))
    {
        if (sgn(a) > 0) {throw std::logic_error("Invalid limits");}
        A = 0;
        B = 1;
        F = [b, &f](float64 t)
        {
            return f(b - (1. - t) / t) / (t * t);
        };
    }

    else
    {
        F = f;
        A = a;
        B = b;
    }

    return Scale * Run(F, A, B);
}

float64 SampleBasedIntegratingFunction::Run(Function1D f, float64 a, float64 b)const
{
    return Run(GetSamplesFromFunction(f, a, b, 0));
}

std::vector<vec2> SampleBasedIntegratingFunction::GetEvenlySpacedSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples)
{
    if (!Samples) {Samples = 33;} // 2^5 + 1
    std::vector<vec2> Result;
    float64 Step = (b - a) / float64(Samples - 1);
    for (uint64 i = 0; i < Samples; ++i)
    {
        float64 X = a + i * Step;
        Result.push_back({X, f(X)});
    }
    return Result;
}

std::vector<vec2> SampleBasedIntegratingFunction::GetSamplesFromParametricCurve(Function1D x, Function1D y, float64 a, float64 b, uint64 Samples)
{
    if (!Samples) {Samples = 1025;} // 2^10 + 1
    std::vector<vec2> Result;
    float64 Step = (b - a) / float64(Samples - 1);
    for (uint64 i = 0; i < Samples; ++i)
    {
        float64 t = a + i * Step;
        Result.push_back({x(t), y(t)});
    }
    return Result;
}

std::vector<vec2> SampleBasedIntegratingFunction::GetSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples)const
{
    return GetEvenlySpacedSamplesFromFunction(f, a, b, Samples);
}

float64 SampleBasedIntegratingFunction::operator()(std::vector<vec2> Samples)const
{
    return Run(Samples);
}


//////////////////////////////// 高斯-克朗罗德积分 ///////////////////////////////

// 此处使用Boost库中的实现，表格启用四倍精度存储

#include "Integrations_GaussKronrod.tbl"

void GaussKronrodQuadrature::GetNodesAndWeights(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs)
{
    // 特殊值直接返回
    if (GetNodesAndWeightsSpecialCases(N, GaussCoeffs, KronrodCoeffs)) {return;}

    // 非特殊值则计算
    std::vector<float64> Nodes, GWeights, KWeights, GResult, KResult;

    auto __Solve_Polynomials = [&Nodes](std::vector<float64> Coeffs, const SolvePolyRoutine& Routine)
    {
        std::vector<complex64> Roots(Coeffs.size() - 1);
        SolvePoly(Coeffs, Roots, Routine);
        for (auto i : Roots)
        {
            if (cse::isnan(i.real())){throw std::logic_error("Failed to solve");}
            if (cse::abs(i.real()) < pow(10, -15)) {Nodes.push_back(0);}
            else if (i.real() > 0) {Nodes.push_back(i.real());}
        }
    };

    uint64 LegendreK = KronrodCoeffs ? ((N - 1) / 2) : N,
        LegendreKHalf = (LegendreK & 1) ? ((LegendreK - 1ULL) / 2ULL + 1ULL) : (LegendreK / 2ULL);
    auto LegendreCoeffs = LegendrePolynomialCoefficients(LegendreK);

    // 勒让德多项式包括下文的斯蒂尔杰斯多项式的解全部为实根，其分布关于原点中心对称，故勒
    // 让德多项式求解用到的初始值可以使用以下文献中的办法确定。此方法原先是为牛顿迭代求解
    // 设计，事实上本文使用的杜兰德-肯纳算法本质上也属于一种非线性牛顿迭代，所以本文认为此
    // 初始值同样适用。事实上求解速度确实快了一些。
    // [1] Szeg G .Inequalities for the zeros of Legendre polynomials and related
    //     functions[J].Transactions of the American Mathematical Society, 1936,
    //     39:1-17.DOI:10.1090/S0002-9947-1936-1501831-2.
    // [2] Tricomi F G. Sugli zeri dei polinomi sferici ed ultrasferici[J]. Annali
    //     di Matematica Pura ed Applicata, 1950, 31(1): 93-97.
    DurandKernerSolvePoly LegendreRoutine;
    LegendreRoutine.InitValue.clear();
    if (LegendreK & 1) {LegendreRoutine.InitValue.push_back(0);}
    for (int k = (LegendreK & 1) ? 1 : 0; k < LegendreKHalf; ++k)
    {
        float64 tet = ((cse::ceil(LegendreK * 0.5) - 0.5 * 0.5 - float64(k)) * CSE_PI) / (float64(LegendreK) + 0.5);
        float64 stet = sin(Angle::FromRadians(tet));
        float64 ctet = cos(Angle::FromRadians(tet));
        float64 inv = 1. / float64(LegendreK * LegendreK);
        float64 InsertValue = (1 - inv / 8. + inv / float64(8. * LegendreK) - (inv * inv / 384.) * (39. - 28. / (stet * stet))) * ctet;
        LegendreRoutine.InitValue.insert(LegendreRoutine.InitValue.begin(), -InsertValue);
        LegendreRoutine.InitValue.insert(LegendreRoutine.InitValue.end(), InsertValue);
    }

    __Solve_Polynomials(LegendreCoeffs, LegendreRoutine);

    // 计算高斯权重
    GWeights.resize(Nodes.size());
    Polynomial LegendrePolynomial(LegendreCoeffs);
    Polynomial LegendrePolynomialDerivative = LegendrePolynomial.Derivative();
    for (uint64 i = 0; i < Nodes.size(); ++i)
    {
        float64 x = Nodes[i];
        float64 p = LegendrePolynomialDerivative(x);
        GWeights[i] = 2. / ((1. - x * x) * p * p);
    }

    for (uint64 i = 0; i < Nodes.size(); ++i)
    {
        GResult.push_back(Nodes[i]);
        GResult.push_back(GWeights[i]);
    }

    *GaussCoeffs = GResult;

    if (!KronrodCoeffs) {return;}

    uint64 StieltjesK = (N - 1) / 2 + 1;
    auto StieltjesCoeffs = StieltjesPolynomialCoefficients(StieltjesK);

    // 斯蒂尔杰斯多项式的初始值目前没有什么好的确定方式，此处直接用相邻两个勒让德多项式解的中点值
    DurandKernerSolvePoly StieltjesRoutine;
    StieltjesRoutine.InitValue.clear();
    if (StieltjesK & 1) {StieltjesRoutine.InitValue.push_back(0);}
    std::vector<float64> GaussNodes = Nodes;
    std::sort(GaussNodes.begin(), GaussNodes.end());
    GaussNodes.insert(GaussNodes.end(), 1);
    for (uint64 i = 1; i < GaussNodes.size(); ++i)
    {
        float64 InsertValue = (GaussNodes[i] + GaussNodes[i - 1]) / 2.;
        StieltjesRoutine.InitValue.insert(StieltjesRoutine.InitValue.begin(), -InsertValue);
        StieltjesRoutine.InitValue.insert(StieltjesRoutine.InitValue.end(), InsertValue);
    }

    __Solve_Polynomials(StieltjesCoeffs, StieltjesRoutine);

    std::sort(Nodes.begin(), Nodes.end());

    // 计算克朗罗德权重
    KWeights.resize(Nodes.size());
    Polynomial StieltjesPolynomial(StieltjesCoeffs);
    Polynomial StieltjesPolynomialDerivative = StieltjesPolynomial.Derivative();
    unsigned Order = (N - 1) / 2;
    unsigned Start = Order & 1 ? 0 : 1;
    for (uint64 i = Start; i < Nodes.size(); i += 2)
    {
        float64 x = Nodes[i];
        float64 p = LegendrePolynomialDerivative(x);
        float64 Weight = 2 / ((1 - x * x) * p * p);
        KWeights[i] = Weight + 2. / ((Order + 1) * LegendrePolynomialDerivative(x) * StieltjesPolynomial(x));
    }
    for (uint64 i = Start ? 0 : 1; i < Nodes.size(); i += 2)
    {
        float64 x = Nodes[i];
        KWeights[i] = 2. / ((Order + 1) * LegendrePolynomial(x) * StieltjesPolynomialDerivative(x));
    }

    for (uint64 i = 0; i < Nodes.size(); ++i)
    {
        KResult.push_back(Nodes[i]);
        KResult.push_back(KWeights[i]);
    }

    *KronrodCoeffs = KResult;
}

bool GaussKronrodQuadrature::GetNodesAndWeightsSpecialCases(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs)
{
    bool OK = 0;
    uint64 GaussOrder = KronrodCoeffs ? ((N - 1) / 2) : N;

    switch (GaussOrder)
    {
    case 7:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian07_Table), std::end(__Gaussian07_Table));
        OK = 1;
        break;
    case 10:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian10_Table), std::end(__Gaussian10_Table));
        OK = 1;
        break;
    case 15:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian15_Table), std::end(__Gaussian15_Table));
        OK = 1;
        break;
    case 20:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian20_Table), std::end(__Gaussian20_Table));
        OK = 1;
        break;
    case 25:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian25_Table), std::end(__Gaussian25_Table));
        OK = 1;
        break;
    case 30:
        *GaussCoeffs = std::vector<float64>(std::begin(__Gaussian30_Table), std::end(__Gaussian30_Table));
        OK = 1;
        break;
    default:
        break;
    }

    if (!KronrodCoeffs) {return OK;}

    uint64 KronrodOrder = N;

    switch (KronrodOrder)
    {
    case 15:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod15_Table), std::end(__Kronrod15_Table));
        OK = 1;
        break;
    case 21:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod21_Table), std::end(__Kronrod21_Table));
        OK = 1;
        break;
    case 31:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod31_Table), std::end(__Kronrod31_Table));
        OK = 1;
        break;
    case 41:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod41_Table), std::end(__Kronrod41_Table));
        OK = 1;
        break;
    case 51:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod51_Table), std::end(__Kronrod51_Table));
        OK = 1;
        break;
    case 61:
        *KronrodCoeffs = std::vector<float64>(std::begin(__Kronrod61_Table), std::end(__Kronrod61_Table));
        OK = 1;
        break;
    default:
        break;
    }

    return OK;
}

float64 GaussKronrodQuadrature::GIntegrate(Function1D f, float64 *pL1) const
{
    uint64 N = (Order - 1ULL) / 2ULL;
    uint64 GaussStart = 1;
    float64 Result = 0;

    auto NodeIndex = [](uint64 Node) {return 2 * Node;};
    auto WeightIndex = [](uint64 Weight) {return 2 * Weight + 1;};

    if (N & 1)
    {
        Result = f(0) * GaussCoefficients[WeightIndex(0)];
    }
    else
    {
        Result = 0;
        GaussStart = 0;
    }

    float64 L1 = abs(Result);
    for (uint64 i = GaussStart; i < GaussCoefficients.size() / 2ULL; ++i)
    {
        float64 fp = f(GaussCoefficients[NodeIndex(i)]);
        float64 fm = f(-GaussCoefficients[NodeIndex(i)]);
        Result += (fp + fm) * GaussCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * GaussCoefficients[WeightIndex(i)];
    }

    if (pL1) {*pL1 = L1;}
    return Result;
}

float64 GaussKronrodQuadrature::GKAdaptiveIntegrate(Function1D f, float64 a, float64 b, uint64 Level, float64 Tol, float64* LastErr, float64* L1)const
{
    float64 Error;
    float64 Mean = (b + a) / 2.;
    float64 Scale = (b - a) / 2.;

    auto F = [&](float64 x)
    {
        return f(Scale * x + Mean);
    };

    float64 r1 = GKNonAdaptiveIntegrate(F, &Error, L1);
    float64 Estimate = Scale * r1;

    float64 Temp = Estimate * pow(10, -Tolerence);
    float64 Tol1 = abs(Temp);
    if (Tol == 0) {Tol = Tol1;}

    if (Level && (Tol < Error) && (Tol1 < Error))
    {
        float64 Mid = (a + b) / 2;
        float64 L1N;
        Estimate = GKAdaptiveIntegrate(f, a, Mid, Level - 1, Tol / 2, LastErr, L1);
        Estimate += GKAdaptiveIntegrate(f, Mid, b, Level - 1, Tol / 2, &Error, &L1N);
        if (LastErr) {*LastErr += Error;}
        if (L1) {*L1 += L1N;}
        return Estimate;
    }

    if(L1) {*L1 *= Scale;}
    if (LastErr) {*LastErr = Error;}

    return Estimate;
}

float64 GaussKronrodQuadrature::GKNonAdaptiveIntegrate(Function1D f, float64 *Error, float64 *pL1)const
{
    uint64 N = Order;
    uint64 GaussStart = 2;
    uint64 KronrodStart = 1;
    uint64 GaussOrder = (N - 1ULL) / 2ULL;

    float64 GaussResult = 0;
    float64 KronrodResult = 0;
    float64 fp, fm;

    auto NodeIndex = [](uint64 Node) {return 2 * Node;};
    auto WeightIndex = [](uint64 Weight) {return 2 * Weight + 1;};

    if (GaussOrder & 1)
    {
        fp = f(0);
        KronrodResult = fp * KronrodCoefficients[WeightIndex(0)];
        GaussResult = fp * GaussCoefficients[WeightIndex(0)];
    }
    else
    {
        fp = f(0);
        KronrodResult = fp * KronrodCoefficients[WeightIndex(0)];
        GaussStart = 1;
        KronrodStart = 2;
    }

    float64 L1 = abs(KronrodResult);

    for (uint64 i = GaussStart; i < KronrodCoefficients.size() / 2ULL; i += 2)
    {
        fp = f(KronrodCoefficients[NodeIndex(i)]);
        fm = f(-KronrodCoefficients[NodeIndex(i)]);
        KronrodResult += (fp + fm) * KronrodCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * KronrodCoefficients[WeightIndex(i)];
        GaussResult += (fp + fm) * GaussCoefficients[WeightIndex(i / 2)];
    }

    for (uint64 i = KronrodStart; i < KronrodCoefficients.size() / 2ULL; i += 2)
    {
        fp = f(KronrodCoefficients[NodeIndex(i)]);
        fm = f(-KronrodCoefficients[NodeIndex(i)]);
        KronrodResult += (fp + fm) * KronrodCoefficients[WeightIndex(i)];
        L1 += (abs(fp) + abs(fm)) * KronrodCoefficients[WeightIndex(i)];
    }

    if (pL1) {*pL1 = L1;}
    if (Error)
    {
        *Error = max(abs(KronrodResult - GaussResult), abs(KronrodResult * DOUBLE_EPSILON * 2.));
    }

    return KronrodResult;
}

float64 GaussKronrodQuadrature::Run(Function1D f, float64 a, float64 b)const
{
    return GaussOnly ? GaussIntegrate(f, a, b) : GaussKronrodIntegrate(f, a, b);
}

float64 GaussKronrodQuadrature::GaussIntegrate(Function1D f, float64 a, float64 b, float64* L1Norm)const
{
    float64 Avg = (a + b) / 2.;
    float64 Scale = (b - a) / 2.;
    auto F = [&](float64 x)
    {
        return f(Avg + Scale * x);
    };

    float64 Estimate = Scale * GIntegrate(F, L1Norm);

    if (L1Norm) {*L1Norm *= Scale;}
    return Estimate;
}

float64 GaussKronrodQuadrature::GaussKronrodIntegrate(Function1D f, float64 a, float64 b, float64* LastError, float64* L1Norm)const
{
    return GKAdaptiveIntegrate(f, a, b, MaxLevels, 0, LastError, L1Norm);
}



///////////////////////////////// 牛顿-科特斯积分 ////////////////////////////////

#include "Integrations_NewtonCotes.tbl"

#define __Newton_Cotes_Func_Disable \
if (Level == 0) {throw std::logic_error("This function is disabled when zero-leveled.");}

void NewtonCotesFormulae::GetEvenlySizedParameters(uint64 N, std::vector<float64>* Weight, float64* Error)
{
    if (!N) {throw std::logic_error("N is zero");}
    if (N <= 14)
    {
        GetSpecialCaseParameters(N, Weight, Error);
        return;
    }

    std::vector<float64> SamplePos;
    for (int i = 0; i <= N; ++i)
    {
        SamplePos.push_back(i);
    }
    GetWeightsErrorsFromSamples(SamplePos, 1, Weight, Error);
}

void NewtonCotesFormulae::GetSpecialCaseParameters(uint64 N, std::vector<float64>* Weight, float64* Error)
{
    Weight->resize(N + 1);
    for (int i = 0; i <= N; ++i)
    {
        Weight->at(i) = N * float64(__Newton_Cotes_Table[N - 1].Weights[i]) /
            float64(__Newton_Cotes_Table[N - 1].Scale);
    }
    *Error = float64(__Newton_Cotes_Table[N - 1].ErrorCoeff1) /
        float64(__Newton_Cotes_Table[N - 1].ErrorCoeff2);
}

void NewtonCotesFormulae::GetParametersFromSamples(std::vector<float64> SamplePos, std::vector<float64>* Weight, float64* Error)
{
    if (SamplePos.size() <= 1) {throw std::logic_error("Requires at least 2 sample points");}
    Weight->resize(SamplePos.size());
    bool IsEqual = 1;
    for (uint64 i = 1; i < SamplePos.size(); ++i)
    {
        IsEqual &= (SamplePos[i] - SamplePos[i - 1] == 1);
    }
    GetWeightsErrorsFromSamples(SamplePos, IsEqual, Weight, Error);
}

void NewtonCotesFormulae::GetWeightsErrorsFromSamples(std::vector<float64> SamplePos, bool IsEqual, std::vector<float64>* WeightOut, float64* ErrorOut)
{
    uint64 Level = SamplePos.size() - 1;
    if (SamplePos.front() != 0 || SamplePos.back() != Level)
    {
        throw std::logic_error("The sample positions must start at 0 and end at N");
    }

    // 权重计算参考以下文献：
    // Keesling J. Closed Newton-Cotes Integration[R/OL].
    // https://people.clas.ufl.edu/kees/files/NewtonCotes.pdf.
    std::vector<float64> B;
    for (uint64 N = 0; N <= Level; ++N)
    {
        B.push_back(pow(Level, N + 1) / (N + 1));
    }
    auto IV = InverseVandermonde(SamplePos);
    auto A = IV * DynamicMatrix{B};
    auto WResult = A.GetColumn(0);
    std::copy(WResult.begin(), WResult.end(), WeightOut->begin());

    // 误差系数的算法译自SciPy
    float64 EResult, EPower;
    if (!(Level % 2) && IsEqual)
    {
        EResult = Level / float64(Level + 3);
        EPower = Level + 2;
    }
    else
    {
        EResult = Level / float64(Level + 2);
        EPower = Level + 1;
    }
    for (int i = 0; i < SamplePos.size(); ++i)
    {
        EResult -= cse::pow(SamplePos[i] / float64(Level), EPower) * WResult[i];
    }
    *ErrorOut = EResult * (cse::pow(Level, EPower) / tgamma(EPower + 1));
}

bool NewtonCotesFormulae::IsEvenlySized(std::vector<vec2> Samples)
{
    if (Samples.size() <= 2) {return 1;}
    bool IsEqual = 1;
    auto Step = Samples[1].x - Samples[0].x;
    for (uint64 i = 2; i < Samples.size(); ++i)
    {
        auto CurrentStep = Samples[i].x - Samples[i - 1].x;
        IsEqual &= abs(CurrentStep - Step) < DOUBLE_EPSILON;
    }
    return IsEqual;
}

NewtonCotesFormulae::Block NewtonCotesFormulae::CreateBlock(uint64 N)const
{
    Block NewBlock;
    GetEvenlySizedParameters(N, &NewBlock.Weights, &NewBlock.ErrorCoeff);
    NewBlock.BStep = 1;
    return NewBlock;
}

NewtonCotesFormulae::Block NewtonCotesFormulae::CreateBlock(std::vector<vec2> Samples)const
{
    Block NewBlock;
    uint64 N = Samples.size() - 1;
    float64 a = Samples.front().x;
    float64 b = Samples.back().x;
    float64 TrueStep = (b - a) / float64(N);

    std::vector<float64> NormalizedX;
    for (int i = 0; i < Samples.size(); ++i)
    {
        NormalizedX.push_back((Samples[i].x - a) / TrueStep);
        NewBlock.Samples.push_back(Samples[i].y);
    }
    GetParametersFromSamples(NormalizedX, &NewBlock.Weights, &NewBlock.ErrorCoeff);
    NewBlock.BStep = TrueStep;

    return NewBlock;
}

float64 NewtonCotesFormulae::Block::Integrate()
{
    float64 sum = 0;
    for (uint64 i = 0; i < Samples.size(); ++i)
    {
        sum += Weights[i] * Samples[i];
    }
    return BStep * sum; // + ErrorEstimate()
}

float64 NewtonCotesFormulae::Block::ErrorEstimate()
{
    return 0; // TODO.
}

float64 NewtonCotesFormulae::Trapezoidal(std::vector<vec2> Samples)
{
    if (Samples.size() < 2) {throw std::logic_error("need at least 2 sample points");}
    float64 sum = 0;
    for (uint64 i = 1; i < Samples.size(); ++i)
    {
        float64 d = Samples[i].x - Samples[i - 1].x;
        sum += (Samples[i].y + Samples[i - 1].y) * d / 2.;
    }
    return sum;
}

float64 NewtonCotesFormulae::Simpson(std::vector<vec2> Samples)
{
    if (Samples.size() < 3) {throw std::logic_error("need at least 3 sample points");}
    uint64 N = Samples.size() - 1;

    if (N % 2)
    {
        float64 res = SimpsonImpl(std::vector<vec2>(Samples.begin(), Samples.end() - 1));
        float64 hn2 = Samples[N - 1].x - Samples[N - 2].x;
        float64 hn1 = Samples[N].x - Samples[N - 1].x;
        float64 alf = ((2. * hn1 * hn1) + (3. * hn1 * hn2)) / (6. * (hn2 + hn1));
        float64 bet = ((hn1 * hn1) + (3. * hn1 * hn2)) / (6. * hn2);
        float64 eta = (hn1 * hn1 * hn1) / (6. * hn2 * (hn2 + hn1));
        return res + alf * Samples[N].y + bet * Samples[N - 1].y - eta * Samples[N - 2].y;
    }

    return SimpsonImpl(Samples);
}

float64 NewtonCotesFormulae::SimpsonImpl(std::vector<vec2> Samples)
{
    if (IsEvenlySized(Samples)) // 等距样本使用辛普森1/4
    {
        float64 Result = 0;
        float64 Step = Samples[1].x - Samples[0].x;
        for (int i = 0; i < Samples.size() - 2; i += 2)
        {
            Result += Samples[i].y + 4. * Samples[i + 1].y + Samples[i + 2].y;
        }
        return (Step / 3.) * Result;
    }
    else // 非等距样本的辛普森
    {
        float64 Result = 0;
        for (int i = 0; i <= (Samples.size() - 1) / 2 - 1; ++i)
        {
            float64 h2i = Samples[2 * i + 1].x - Samples[2 * i].x;
            float64 h2ia1 = Samples[2 * i + 2].x - Samples[2 * i + 1].x;
            Result += ((h2i + h2ia1) / 6.) *
                ((2. - (h2ia1 / h2i)) * Samples[2 * i].y +
                (((h2i + h2ia1) * (h2i + h2ia1)) / (h2i * h2ia1)) * Samples[2 * i + 1].y +
                (2. - (h2i / h2ia1)) * Samples[2 * i + 2].y);
        }
        return Result;
    }
}

float64 NewtonCotesFormulae::Romberg(std::vector<vec2> Samples, DynamicMatrix<float64>* RichardsonExtrapolationTable)
{
    if (!IsEvenlySized(Samples))
    {
        throw std::logic_error("Samples are unequally spaced.");
    }

    // 此基于样本点的龙贝格积分实现译自SciPy
    float64 dx = Samples[1].x - Samples[0].x;
    uint64 NSamples = Samples.size();
    uint64 NSegments = NSamples - 1;

    uint64 n = 1, k = 0;
    while (n < NSegments)
    {
        n <<= 1;
        ++k;
    }
    if (n != NSegments)
    {
        throw std::logic_error("Number of samples must be 2^N + 1 and N > 0.");
    }

    DynamicMatrix<float64> R({k + 1, k + 1});
    float64 h = NSegments * dx;
    R.at(0, 0) = h * ((Samples.front().y + Samples.back().y) / 2.);
    uint64 Start = NSegments, Stop = NSegments, Step = NSegments;

    for (uint64 i = 1; i <= k; ++i)
    {
        Start >>= 1;
        float64 ysum = 0;
        for (uint64 j = Start; j < Stop; j += Step)
        {
            ysum += Samples[j].y;
        }
        Step >>= 1;
        R.at(0, i) = (R.at(0, i - 1) + h * ysum) / 2.;

        for (uint64 j = 1; j <= i; ++j)
        {
            R.at(j, i) = R.at(j - 1, i) + (R.at(j - 1, i) - R.at(j - 1, i - 1)) / ((1 << (2 * j)) - 1);
        }

        h /= 2.0;
    }

    if (RichardsonExtrapolationTable) {*RichardsonExtrapolationTable = R;}

    return R.at(k, k);
}

float64 NewtonCotesFormulae::SingleIntegrate(std::vector<vec2> Samples)const
{
    __Newton_Cotes_Func_Disable
    if ((Samples.size() - 1) != Level)
    {
        throw std::logic_error("Number of samples doesn't match this level.");
    }
    return SingleIntegrateImpl(Samples, Level);
}

float64 NewtonCotesFormulae::SingleIntegrateImpl(std::vector<vec2> Samples, uint64 L) const
{
    if (IsEvenlySized(Samples))
    {
        Block Blk = CreateBlock(L);
        float64 h = Samples[1].x - Samples[0].x;
        for (int j = 0; j <= L; ++j)
        {
            Blk.Samples.push_back(Samples[j].y);
        }
        return h * Blk.Integrate();
    }
    else
    {
        Block CurrentBlock = CreateBlock(Samples);
        return CurrentBlock.Integrate();
    }
}

float64 NewtonCotesFormulae::CompositeIntegrate(std::vector<vec2> Samples)const
{
    __Newton_Cotes_Func_Disable
    if (!IsEvenlySized(Samples))
    {
        throw std::logic_error("Samples are unequally spaced.");
    }

    uint64 FN = (Samples.size() - 1) % Level, N = (Samples.size() - 1) - FN;
    Block Blk = CreateBlock(Level), FinalBlock;
    if (FN) { FinalBlock = CreateBlock(FN);}
    float64 sum = 0;
    float64 h = Samples[1].x - Samples[0].x;
    for (uint64 i = 0; i <= N - Level; i += Level)
    {
        for (int j = 0; j <= Level; ++j)
        {
            Blk.Samples.push_back(Samples[i + j].y);
        }
        sum += h * Blk.Integrate();
        Blk.Samples.clear();
    }
    if (FN)
    {
        for (uint64 j = N; j < Samples.size(); ++j)
        {
            FinalBlock.Samples.push_back(Samples[j].y);
        }
        sum += h * FinalBlock.Integrate();
    }
    return sum;
}

float64 NewtonCotesFormulae::DiscreteIntegrate(std::vector<vec2> Samples)const
{
    __Newton_Cotes_Func_Disable
    uint64 FN = (Samples.size() - 1) % Level, N = (Samples.size() - 1) - FN;
    float64 sum = 0;
    for (uint64 i = 0; i <= N - Level; i += Level)
    {
        sum += SingleIntegrateImpl(std::vector<vec2>
            (Samples.begin() + i, Samples.begin() + i + Level + 1), Level);
    }
    if (FN)
    {
        sum += SingleIntegrateImpl(std::vector<vec2>(Samples.begin() + N, Samples.end()), FN);
    }
    return sum;
}

float64 NewtonCotesFormulae::Run(std::vector<vec2> Samples) const
{
    switch (Level)
    {
    case 0:
        return Romberg(Samples);
    case 1:
        return Trapezoidal(Samples);
    case 2:
        return Simpson(Samples);
    }

    uint64 N = Samples.size() - 1;
    if (N == Level) {return SingleIntegrate(Samples);}
    else
    {
        if (IsEvenlySized(Samples)) {return CompositeIntegrate(Samples);}
        else {return DiscreteIntegrate(Samples);}
    }

    throw std::logic_error("No matching function for call to these samples.");
}



///////////////////////////////// 黎曼-刘维尔积分 ////////////////////////////////

float64 RiemannLiouvilleIntegratingFunction::operator()(float64 x) const
{
    return (*Engine)([&](float64 t)
    {
        return OriginalFunction(t) * pow(x - t, IntegralOrder - 1);
    }, InitValue.x, x) / tgamma(IntegralOrder) + InitValue.y;
}

_SCICXX_END
_CSE_END
