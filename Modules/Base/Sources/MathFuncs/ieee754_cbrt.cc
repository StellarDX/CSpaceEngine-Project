// Fast cube root function for double and quaduple precision.
/*
   Copyright (C) 2018-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library. Adapted for CSpaceEngine

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.
*/

// Unpretentious code. (朴实无华的代码)

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN

float64 __cdecl cbrt(float64 _X)
{
    return __IEEE754_CBRT64F(_X);
}

std::array<complex64, 3> __cdecl cbrtc(complex64 _X)
{
    return
    {
        yrootc(_X, {3, 0}, 0),
        yrootc(_X, {3, 0}, 1),
        yrootc(_X, {3, 0}, 2),
    };
}

_EXTERN_C

/*
 *	Cube root for double precision
 */

__Float64 __cdecl __IEEE754_CBRT64F(__Float64 _X)
{
    __Float64 x = _X;
    __Float64 xm, ym, u, t2;
    int xe;

    static const __Float64 factor[5] =
    {
        0.62996052494743658238360530363912, // 1 / cbrt(2)^2
        0.79370052598409973737585281963615, // 1 / cbrt(2)
        1.00000000000000000000000000000000,
        1.25992104989487316476721060727820, // cbrt(2)
        1.58740105196819947475170563927230, // cbrt(2)^2
    };

    /* Reduce X.  XM now is an range 1.0 to 0.5.  */
    xm = ::frexp(fabs(x), &xe);

    /* If X is not finite or is null return it (with raising exceptions
       if necessary.
       Note: *Our* version of `frexp' sets XE to zero if the argument is
       Inf or NaN.  This is not portable but faster.  */
    if (xe == 0 && std::fpclassify(x.x) <= FP_ZERO) { return x; }

    u = (0.354895765043919860
         + ((1.50819193781584896
             + ((-2.11499494167371287
                 + ((2.44693122563534430
                     + ((-1.83469277483613086
                         + (0.784932344976639262
                            - 0.145263899385486377
                                * xm)
                            * xm)
                        * xm))
                    * xm))
                * xm))
            * xm));
    t2 = u * u * u;
    ym = u * (t2 + 2.0 * xm) / (2.0 * t2 + xm) * factor[2 + xe % 3].x;

    return ::ldexp(x > 0.0 ? ym.x : -ym.x, xe / 3);
}

/*							cbrt(x)
 *
 *	Cube root, quaduple precision
 *
 *
 *
 * SYNOPSIS:
 *
 * quaduple x, y, cbrt();
 *
 * y = cbrt( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the cube root of the argument, which may be negative.
 *
 * Range reduction involves determining the power of 2 of
 * the argument.  A polynomial of degree 2 applied to the
 * mantissa, and multiplication by the cube root of 1, 2, or 4
 * approximates the root to within about 0.1%.  Then Newton's
 * iteration is used three times to converge to an accurate
 * result.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       -8,8       100000      1.3e-34     3.9e-35
 *    IEEE    exp(+-707)    100000      1.3e-34     4.3e-35
 *
 */

/*
    Cephes Math Library Release 2.2: January, 1991
    Copyright 1984, 1991 by Stephen L. Moshier
    Adapted for glibc October, 2001.
    Adapted for CSpaceEngine, 2025. (From glibc)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <https://www.gnu.org/licenses/>.
*/

__Float64 __cdecl __IEEE754_CBRT128F_C64F(__Float64 _X)
{
    static const __Float64 CBRT2 = 1.259921049894873164767210607278228350570251;
    static const __Float64 CBRT4 = 1.587401051968199474751705639272308260391493;
    static const __Float64 CBRT2I = 0.7937005259840997373758528196361541301957467;
    static const __Float64 CBRT4I = 0.6299605249474365823836053036391141752851257;

    __Float64 x = _X;
    int e, rem, sign;
    __Float64 z;

    if (!std::isfinite(x.x)) {return x + x;}
    if (x == 0) {return x;}

    if (x > 0)
    {
        sign = 1;
    }
    else
    {
        sign = -1;
        x = -x;
    }

    z = x;
    // extract power of 2, leaving mantissa between 0.5 and 1
    x = ::frexp(x, &e);

    // Approximate cube root of number between 0.5 and 1,
    // peak relative error = 1.2e-6
    x = ((((1.3584464340920900529734e-1 * x
            - 6.3986917220457538402318e-1) * x
           + 1.2875551670318751538055e0) * x
          - 1.4897083391357284957891e0) * x
         + 1.3304961236013647092521e0) * x
        + 3.7568280825958912391243e-1;

    // exponent divided by 3
    if (e >= 0)
    {
        rem = e;
        e /= 3;
        rem -= 3 * e;
        if (rem == 1) {x *= CBRT2;}
        else if (rem == 2) {x *= CBRT4;}
    }
    else
    {				// argument less than 1
        e = -e;
        rem = e;
        e /= 3;
        rem -= 3 * e;
        if (rem == 1) {x *= CBRT2I;}
        else if (rem == 2) {x *= CBRT4I;}
        e = -e;
    }

    // multiply by power of 2
    x = ::ldexp(x, e);

    // Newton iteration
    for (int i = 0; i < 3; ++i)
    {
        x -= (x - (z / (x * x))) / 3.;
    }

    if (sign < 0) {x = -x;}
    return x;
}

_END_EXTERN_C
_CSE_END

#if 0 // cbrt test program generated by Deepseek
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>

#include <CSE/Base.h>

using namespace cse;

// 测试用例结构体
struct TestCase
{
    double input;
    std::string description;
};

// 生成测试用例
std::vector<TestCase> generate_test_cases()
{
    std::vector<TestCase> cases =
    {
        // 常规正数
        {1.0, "1.0 (简单整数)"},
        {8.0, "8.0 (完美立方数)"},
        {27.0, "27.0 (完美立方数)"},
        {2.0, "2.0"},
        {3.0, "3.0"},
        {10.0, "10.0"},
        {100.0, "100.0"},
        {1000.0, "1000.0"},
        {0.125, "0.125 (完美立方数)"},
        {0.001, "0.001 (完美立方数)"},
        {1.5, "1.5"},
        {2.5, "2.5"},
        {3.7, "3.7"},
        {123.456, "123.456"},
        {0.987654, "0.987654"},

        // 常规负数
        {-1.0, "-1.0 (简单整数)"},
        {-8.0, "-8.0 (完美立方数)"},
        {-27.0, "-27.0 (完美立方数)"},
        {-2.0, "-2.0"},
        {-3.0, "-3.0"},
        {-10.0, "-10.0"},
        {-100.0, "-100.0"},
        {-1000.0, "-1000.0"},
        {-0.125, "-0.125 (完美立方数)"},
        {-0.001, "-0.001 (完美立方数)"},
        {-1.5, "-1.5"},
        {-2.5, "-2.5"},
        {-3.7, "-3.7"},
        {-123.456, "-123.456"},
        {-0.987654, "-0.987654"},

        // 边界值
        {0.0, "0.0 (零)"},
        {-0.0, "-0.0 (负零)"},
        {std::numeric_limits<double>::min(), "最小正正规数"},
        {std::numeric_limits<double>::denorm_min(), "最小正非正规数"},
        {std::numeric_limits<double>::max(), "最大有限值"},

        // 特殊值
        {std::numeric_limits<double>::infinity(), "正无穷大"},
        {-std::numeric_limits<double>::infinity(), "负无穷大"},
        {std::numeric_limits<double>::quiet_NaN(), "NaN"},

        // 极值
        {1e-300, "非常小的正数"},
        {1e300, "非常大的正数"},
        {-1e-300, "非常小的负数"},
        {-1e300, "非常大的负数"}
    };

    // 添加一些随机值

    for (int i = 0; i < 1000; ++i)
    {
        std::random_device Gen;
        double val = std::exp(std::log(1e-300) + (std::log(1e300) - std::log(1e-300)) * (Gen() / float64(Gen.max())));
        cases.push_back({val, "随机正数 " + std::to_string(i+1)});
        cases.push_back({-val, "随机负数 " + std::to_string(i+1)});
    }

    return cases;
}

// 计算相对误差
double relative_error(double exact, double approx)
{
    if (exact == 0.0)
    {
        return approx == 0.0 ? 0.0 : std::numeric_limits<double>::infinity();
    }
    return std::abs((approx - exact) / exact);
}

// 计算绝对误差
double absolute_error(double exact, double approx)
{
    return std::abs(approx - exact);
}

int main()
{
    // 生成测试用例
    auto test_cases = generate_test_cases();

    // 导出结果到CSV文件
    static const auto filename = "cbrt_test_report.csv";
    std::ofstream file(filename);

    // 写入表头
    file << "Input Value,Description,std::cbrt Result,__IEEE754_CBRT128F_C64F Result,"
         << "Absolute Error,Relative Error,ULP Difference\n";

    // 设置高精度输出
    file << std::scientific << std::setprecision(17);

    for (const auto& test_case : test_cases)
    {
        double input = test_case.input;
        double std_result = std::cbrt(input);
        double custom_result = __IEEE754_CBRT128F_C64F(input);

        double abs_err = absolute_error(std_result, custom_result);
        double rel_err = relative_error(std_result, custom_result);

        // 计算ULP差异
        int ulp_diff = 0;
        if (std::isfinite(std_result))
        {
            // 将double重新解释为int64_t以便比较位模式
            int64_t std_bits, custom_bits;
            memcpy(&std_bits, &std_result, sizeof(double));
            memcpy(&custom_bits, &custom_result, sizeof(double));

            // 考虑符号位差异
            if ((std_bits < 0) != (custom_bits < 0))
            {
                // 如果符号不同，计算绝对差异
                ulp_diff = std::abs(std_bits - custom_bits);
            }
            else
            {
                // 如果符号相同，计算有符号差异
                ulp_diff = std::abs(std_bits - custom_bits);
            }
        }

        file << input << ","
             << "\"" << test_case.description << "\","
             << std_result << ","
             << custom_result << ","
             << abs_err << ","
             << rel_err << ","
             << ulp_diff << "\n";
    }

    file.close();
    std::cout << "Results exported to " << filename << std::endl;

    return 0;
}
#endif
