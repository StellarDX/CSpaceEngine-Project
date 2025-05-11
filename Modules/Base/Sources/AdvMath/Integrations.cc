#include "CSE/Base/AdvMath.h"
#include "CSE/Base/ConstLists.h"

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

std::vector<vec2> SampleBasedIntegratingFunction::GetSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples)
{
    return std::vector<vec2>();
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
    uint64 N = Order;
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
    return GaussKronrodIntegrate(f, a, b);
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

_SCICXX_END
_CSE_END
