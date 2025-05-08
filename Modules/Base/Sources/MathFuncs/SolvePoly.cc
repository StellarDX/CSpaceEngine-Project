// 求解多项式方程的方法

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/MathFuncs.h"

_CSE_BEGIN

int DurandKernerSolvePoly(std::vector<float64> Coeffs, std::vector<complex64>& Roots, _SOLVEPOLY_CONFIG Conf)
{
    if (Coeffs[0] == 0 || Coeffs.empty())
    {
        throw std::logic_error("Highest power of polynomial can't be zero.");
    }
    float64 Base = Coeffs[0];
    for (size_t i = 0; i < Coeffs.size(); i++)
    {
        Coeffs[i] /= Base;
    }

    // The clue to the method now is to combine the fixed-point iteration for P with similar iterations for Q, R, S into a simultaneous iteration for all roots.
    // Initialize p, q, r, s:
    for (size_t i = 0; i < Coeffs.size() - 1; i++)
    {
        Roots.push_back(_CSE powc(Conf.BASE, complex64((float64)i)));
    }

    // Make the substitutions
    auto f = [&](complex64 x)->complex64
    {
        complex64 SIG = 0;
        for (size_t i = 0; i < Coeffs.size(); i++)
        {
            SIG += Coeffs[i] * _CSE powc(x, (complex64)(Coeffs.size() - i - 1.));
        }
        return SIG;
    };

    auto g = [&](complex64 x, uint64 Ignore)->complex64
    {
        complex64 PI = 1;
        for (size_t i = 0; i < Roots.size(); i++)
        {
            if (i != Ignore) { PI *= x - Roots[i]; }
        }
        return PI;
    };

    uint64 it = 0;
    float64 RealError = pow(10, -Conf.P_ERROR), LastError;

    while (it < _CSE pow(10, Conf.MAXITER_LOG))
    {
        std::vector<float64> Diffs;
        for (size_t i = 0; i < Coeffs.size() - 1; i++)
        {
            complex64 Diff = f(Roots[i]) / g(Roots[i], i);
            Diffs.push_back(abs(Diff));
            Roots[i] -= Diff;
        }

        // Re-iterate until the numbers p, q, r, s essentially stop changing relative to the desired precision.
        // They then have the values P, Q, R, S in some order and in the chosen precision. So the problem is solved.
        auto MaxDiff = abs(*(_CSE max(Diffs.begin(), Diffs.end())));
        if (MaxDiff < RealError) {break;}
        //if (it && MaxDiff > LastError) {break;}

        LastError = MaxDiff;
        ++it;
    }

    return it;
}

_CSE_END
