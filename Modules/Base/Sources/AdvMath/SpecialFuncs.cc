#include "CSE/Base/AdvMath.h"
#include <memory>

_CSE_BEGIN
_SCICXX_BEGIN

std::vector<float64> ElementarySymmetricPolynomial(std::vector<float64> Coeffs)
{
    // 此处使用动态规划，时间复杂度O(n^2)，可能还有一种基于FFT分治的算法速度更快
    auto n = Coeffs.size();
    struct BufferType {std::size_t Iter; float64 Value;};
    std::shared_ptr<std::vector<BufferType>[]> Buffer(new std::vector<BufferType>[n]);
    std::vector<float64> Results;
    Results.push_back(1);
    for (std::size_t i = 0; i < n; ++i)
    {
        if (!i)
        {
            Buffer[i].resize(n);
            for (std::size_t j = 0; j < n; ++j)
            {
                Buffer[i].push_back({j, Coeffs[j]});
            }
        }
        else
        {
            for (std::size_t j = 0; j < Buffer[i - 1].size(); ++j)
            {
                for (std::size_t k = Buffer[i - 1].at(j).Iter + 1; k < n; ++k)
                {
                    Buffer[i].push_back({k, Buffer[i - 1].at(j).Value * Coeffs[k]});
                }
            }
        }

        float64 Res = 0;
        for (auto j : Buffer[i])
        {
            Res += j.Value;
        }
        Results.push_back(Res);
    }
    return Results;
}

DynamicMatrix<float64> Vandermonde(std::vector<float64> V)
{
    uint64 n = V.size();
    DynamicMatrix<float64> Result;
    Result.resize(uvec2(n));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Result.at({i, j}) = pow(V[i], j);
        }
    }
    return Result;
}

DynamicMatrix<float64> InverseVandermonde(std::vector<float64> V)
{
    uint64 n = V.size();
    DynamicMatrix<float64> Inv;
    Inv.resize(uvec2(n));
    for (int i = 0; i < n; ++i)
    {
        std::vector<float64> ESPCoeffs(V);
        ESPCoeffs.erase(ESPCoeffs.begin() + i);
        auto ESP = ElementarySymmetricPolynomial(ESPCoeffs);

        float64 Scale = 1;
        for (int j = 0; j < n; ++j)
        {
            Scale *= (i == j) ? 1 : V[i] - V[j];
        }

        for (int j = 0; j < n; ++j)
        {
            float64 Sign = ((n - j - 1) % 2) ? -1 : 1;
            Inv.at({j, i}) = Sign * ESP[n - j - 1] / Scale;
        }
    }
    return Inv;
}

float64 Polynomial::operator()(float64 x) const
{
    if (!Coefficients.size()) {return 0;}
    float64 S = 0;
    for (uint64 i = 0; i < Coefficients.size(); ++i)
    {
        S += Coefficients[i] * pow(x, MaxPower() - i);
    }
    return S;
}

Polynomial Polynomial::Derivative() const
{
    if (!Coefficients.size()) {return Polynomial();}
    std::vector<float64> NewCoeffs;
    for (uint64 i = 0; i < Coefficients.size() - 1; ++i)
    {
        NewCoeffs.push_back(Coefficients[i] * (MaxPower() - i));
    }
    return Polynomial(NewCoeffs);
}

std::vector<float64> LegendrePolynomialCoefficients(uint64 n)
{
    auto __fact = [](float64 x) {return tgamma(x + 1);};
    std::vector<float64> Coeffs(n + 1);
    for (uint64 k = 0; k <= uint64(floor(float64(n) / 2.)); ++k)
    {
        Coeffs[2. * k] = ((k % 2 ? -1 : 1) *
            (__fact(n) * __fact(2 * n - 2 * k)) /
            (__fact(k) * __fact(n - k) * __fact(n - 2 * k))) /
            (pow(2, n) * __fact(n));
    }
    return Coeffs;
}

std::vector<float64> StieltjesPolynomialCoefficients(uint64 N)
{
    uint64 n = N - 1ULL;
    uint64 q = n - 2ULL * (n / 2ULL);
    uint64 r = (n + 3ULL) / 2ULL;
    std::vector<float64> a(r + 1);
    a[r] = 1;
    for (uint64 k = 1; k < r; ++k)
    {
        float64 Sik = 1;
        a[r - k] = 0;
        for (uint64 i = r + 1 - k; i <= r; ++i)
        {
            Sik *= float64((n - q + 2 * (i + k - 1)) * (n + q + 2 * (k - i + 1)) * (n - 1 - q + 2 * (i - k)) * (2 * (k + i - 1) - 1 - q - n)) /
                float64((n - q + 2 * (i - k)) * (2 * (k + i - 1) - q - n) * (n + 1 + q + 2 * (k - i)) * (n - 1 - q + 2 * (i + k)));
            a[r - k] -= a[i] * Sik;
        }
    }

    auto __Polynomial_Add = [](std::vector<float64> P1, std::vector<float64> P2)
    {
        auto& PL = P1.size() > P2.size() ? P1 : P2;
        auto& PS = P1.size() < P2.size() ? P1 : P2;
        if (!PS.size()) {return PL;}
        std::vector<float64> Result(PL);
        auto PLF = Result.rbegin();
        for (auto PSF = PS.rbegin(); PSF != PS.rend(); ++PSF, ++PLF)
        {
            *PLF += *PSF;
        }
        return Result;
    };

    std::vector<float64> Result;
    for (int i = 1; i <= r; ++i)
    {
        auto LegendrePolynomial = LegendrePolynomialCoefficients(2 * i - 1 - q);
        for (int j = 0; j < LegendrePolynomial.size(); ++j)
        {
            LegendrePolynomial[j] *= a[i];
        }
        Result = __Polynomial_Add(Result, LegendrePolynomial);
    }

    return Result;
}

uint64 __Int_Comb(uint64 n, uint64 k)
{
    if (k > n) {return 0;}
    if (k == 0 || k == n) {return 1;}
    k = min(k, n - k);
    uint64 res1 = 1, res2 = 1;
    for (uint64 i = 1; i <= k; i++)
    {
        res1 *= (n - k + i);
        res2 *= i;
    }
    return res1 / res2;
}

DynamicMatrix<float64> BellPolynomialsTriangularArray(std::vector<float64> x)
{
    // 动态规划计算贝尔多项式
    float64 N = x.size();
    DynamicMatrix<float64> B(uvec2(N + 1));
    B.fill(__Float64::FromBytes(BIG_NAN_DOUBLE));

    B.at(0, 0) = 1;
    for (uint64 n = 1; n <= N; ++n)
    {
        B.at(n, 0) = 0;
        B.at(n, 1) = x[n - 1];
        B.at(n, n) = pow(x[0], n);
    }

    for (uint64 n = 2; n < N; ++n)
    {
        for (uint64 k = 1; k < n; ++k)
        {
            B.at(n + 1, k + 1) = 0;
            for (uint64 i = 0; i <= n - k; ++i)
            {
                B.at(n + 1, k + 1) += __Int_Comb(n, i) * x[i] * B.at(n - i, k);
            }
        }
    }

    return B;
}

_SCICXX_END
_CSE_END
