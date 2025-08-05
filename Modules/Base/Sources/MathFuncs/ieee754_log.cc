/*
* Common logarithm, for double and quaduple precision
*/

/*
    Cephes Math Library Release 2.2:  January, 1991
    Copyright 1984, 1991 by Stephen L. Moshier
    Adapted for glibc November, 2001
    Adapted for CSpaceEngine, 2023 (From glibc)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

// Unspectacular code. (貌不惊人的代码)

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/Algorithms.h"
#include <cfloat>

_CSE_BEGIN

float64 __cdecl log(float64 _X) { return __IEEE754_LOG128F_C64F(_X); };
float64 __cdecl log(float64 _X, float64 _Base) { return log(_X) / log(_Base);}
complex64 __cdecl logc(complex64 _X, int64 _K) { return __GLIBCT_LOG64C(_X, _K);}
complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 _K1, int64 _K2) {return lnc(_X, _K1) / lnc(_Base, _K2);}

_EXTERN_C

/*
 * SYNOPSIS:
 *
 * float64/float128 x, y, log();
 * y = log(x);
 *
 * DESCRIPTION:
 *
 * Returns the base 10 logarithm of x.
 *
 * The argument is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x^2 + x^3 P(x)/Q(x).
 *
 * Otherwise, setting  z = 2(x-1)/x+1),
 *
 *     log(x) = z + z^3 P(z)/Q(z).
 *
 * ACCURACY:
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0.5, 2.0     30000      2.3e-34     4.9e-35
 *    IEEE     exp(+-10000)  30000      1.0e-34     4.1e-35
 *
 * In the tests over the interval exp(+-10000), the logarithms
 * of the random arguments were uniformly distributed over
 * [-10000, +10000].
 */

__Float64 __cdecl __IEEE754_LOG128F_C64F(__Float64 _X)
{
    __Float64 x = _X;
    int64 hx;
    #if 0 // _QUAD_PRECISION
    hx = x.parts64.msw;
    int64 lx = x.parts64.lsw;
    if (((hx & 0x7FFFFFFFFFFFFFFF) | lx) == 0) { return __Float64::FromBytes(CSE_NEGATIVE_INF); }
    if (hx >= 0x7FFF000000000000) { return __Float64::FromBytes(CSE_POSITIVE_INF); }
    #else
    hx = x.Bytes;
    if ((hx & 0x7FFFFFFFFFFFFFFF) == 0) { return __Float64::FromBytes(NEG_INF_DOUBLE); }
    if (hx >= 0x7FF0000000000000) { return __Float64::FromBytes(POS_INF_DOUBLE); }
    #endif
    if (hx < 0) { return __Float64::FromBytes(BIG_NAN_DOUBLE); }
    if (x == 1.0L) { return 0.0L; }

    int e;
    x = ::frexp(x, &e);

    __Float64
        L102A = 0.3125L,
        L102B = -1.14700043360188047862611052755069732318101185E-2L,
        L10EA = 0.5L,
        L10EB = -6.570551809674817234887108108339491770560299E-2L,
        SQRTH = 7.071067811865475244008443621048490392848359E-1L;

    __Float64 z, y;

    static auto neval = [](__Float64 x, const __Float64* p, int n)
    {
        __Float64 y;
        p += n;
        y = *p;
        p -= 1;
        do
        {
            y = y * x + (*p).x;
            p -= 1;
        }
        while (--n > 0);
        return y;
    };

    static auto deval = [](__Float64 x, const __Float64* p, int n)
    {
        __Float64 y;
        p += n;
        y = x.x + (*p).x;
        p -= 1;
        do
        {
            y = y * x + (*p).x;
            p -= 1;
        }
        while (--n > 0);
        return y;
    };

    if ((e > 2) || (e < -2))
    {
        if (x < SQRTH)
        {
            e -= 1;
            z = x - 0.5L;
            y = 0.5L * z + 0.5L;
        }
        else
        {
            z = x - 0.5L;
            z -= 0.5L;
            y = 0.5L * x + 0.5L;
        }

        x = z / y;
        z = x * x;

        static const __Float64 R[6] =
        {
            +1.418134209872192732479751274970992665513E+5L,
            -8.977257995689735303686582344659576526998E+4L,
            +2.048819892795278657810231591630928516206E+4L,
            -2.024301798136027039250415126250455056397E+3L,
            +8.057002716646055371965756206836056074715E+1L,
            -8.828896441624934385266096344596648080902E-1L
        };

        static const __Float64 S[6] =
        {
            +1.701761051846631278975701529965589676574E+6L,
            -1.332535117259762928288745111081235577029E+6L,
            +4.001557694070773974936904547424676279307E+5L,
            -5.748542087379434595104154610899551484314E+4L,
            +3.998526750980007367835804959888064681098E+3L,
            -1.186359407982897997337150403816839480438E+2L
        };

        y = x * (z * neval(z, R, 5) / deval(z, S, 5));

        z = y * L10EB;
        z += x * L10EB;
        z += e * L102B;
        z += y * L10EA;
        z += x * L10EA;
        z += e * L102A;
        return z;
    }

    if (x < SQRTH)
    {
        e -= 1;
        x = 2.0 * x - 1.0L;	/*  2x - 1  */
    }
    else
    {
        x = x - 1.0L;
    }

    z = x * x;

    static const __Float64 P[13] =
    {
        1.313572404063446165910279910527789794488E4L,
        7.771154681358524243729929227226708890930E4L,
        2.014652742082537582487669938141683759923E5L,
        3.007007295140399532324943111654767187848E5L,
        2.854829159639697837788887080758954924001E5L,
        1.797628303815655343403735250238293741397E5L,
        7.594356839258970405033155585486712125861E4L,
        2.128857716871515081352991964243375186031E4L,
        3.824952356185897735160588078446136783779E3L,
        4.114517881637811823002128927449878962058E2L,
        2.321125933898420063925789532045674660756E1L,
        4.998469661968096229986658302195402690910E-1L,
        1.538612243596254322971797716843006400388E-6L
    };
    static const __Float64 Q[12] =
    {
        3.940717212190338497730839731583397586124E4L,
        2.626900195321832660448791748036714883242E5L,
        7.777690340007566932935753241556479363645E5L,
        1.347518538384329112529391120390701166528E6L,
        1.514882452993549494932585972882995548426E6L,
        1.158019977462989115839826904108208787040E6L,
        6.132189329546557743179177159925690841200E5L,
        2.248234257620569139969141618556349415120E5L,
        5.605842085972455027590989944010492125825E4L,
        9.147150349299596453976674231612674085381E3L,
        9.104928120962988414618126155557301584078E2L,
        4.839208193348159620282142911143429644326E1L
    };

    y = x * (z * neval(x, P, 12) / deval(x, Q, 11));
    y = y - 0.5 * z;

    /*
     * Multiply log of fraction by log(e)
     * and base 2 exponent by log(2).
     */
    z = y * L10EB;
    z += x * L10EB;
    z += e * L102B;
    z += y * L10EA;
    z += x * L10EA;
    z += e * L102A;
    return z;
}

complex64 __cdecl __GLIBCT_LOG64C(complex64 _X, int64 _K)
{
    static const float64 log2 = 0.3010299956639811952137388947244930267682;

    float64 YReal, YImag;
    float64 XReal = _X.real();
    float64 XImag = _X.imag();
    int RClass = std::fpclassify(XReal);
    int IClass = std::fpclassify(XImag);

    if (RClass == FP_ZERO && IClass == FP_ZERO)
    {
        // Real and imaginary part are 0.0.
        YImag = std::signbit(XReal) ? ((CSE_PI + 2 * _K * CSE_PI) * CSE_LOGE) : 0;
        YImag = ::copysign(YImag, XImag);
        // Yes, the following line raises an exception.
        YReal = __Float64::FromBytes(NEG_INF_DOUBLE);
    }
    else if (RClass != FP_NAN && IClass != FP_NAN)
    {
        // Neither real nor imaginary part is NaN.
        float64 AbsReal = abs(XReal), AbsImag = abs(XImag);
        int Scale = 0;

        if (AbsReal < AbsImag) {std::swap(AbsReal, AbsImag);}

        if (AbsReal > DBL_MAX / 2)
        {
            Scale = -1;
            AbsReal = ::scalbn(AbsReal, Scale);
            AbsImag = (AbsImag >= DBL_MIN * 2 ? ::scalbn(AbsImag, Scale) : 0);
        }
        else if (AbsReal < DBL_MIN && AbsImag < DBL_MIN)
        {
            Scale = DBL_MANT_DIG;
            AbsReal = ::scalbn(AbsReal, Scale);
            AbsImag = ::scalbn(AbsImag, Scale);
        }

        if (AbsReal == 1 && Scale == 0)
        {
            YReal = (__IEEE854_LN1PX128F_C64F(AbsImag * AbsImag)
                * (CSE_LOGE / 2));
        }
        else if (AbsReal > 1 && AbsReal < 2 && AbsImag < 1 && Scale == 0)
        {
            float64 d2m1 = (AbsReal - 1) * (AbsReal + 1);
            if (AbsImag >= DOUBLE_EPSILON) {d2m1 += AbsImag * AbsImag;}
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) * (CSE_LOGE / 2);
        }
        else if (AbsReal < 1 && AbsReal >= 0.5
            && AbsImag < DOUBLE_EPSILON / 2 && Scale == 0)
        {
            float64 d2m1 = (DOUBLE_EPSILON - 1) * (AbsReal + 1);
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) * (CSE_LOGE / 2);
        }
        else if (AbsReal < 1 && AbsReal >= 0.5 && Scale == 0
            && AbsReal * AbsReal + AbsImag * AbsImag >= 0.5)
        {
            float64 d2m1 = __GLIBC_X2Y2M164F_MS(AbsReal, AbsImag, 1);
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) * (CSE_LOGE / 2);
        }
        else
        {
            float64 d = ::hypot(AbsReal, AbsImag);
            YReal = __IEEE754_LOG128F_C64F(d) - Scale * log2;
        }

        YImag = CSE_LOGE *
            (Arctan2(XImag, XReal).ToRadians() + (2 * _K * CSE_PI));
    }

    else
    {
        YImag = __Float64::FromBytes(BIG_NAN_DOUBLE);
        if (RClass == FP_INFINITE || IClass == FP_INFINITE)
        {
            // Real or imaginary part is infinite.
            YReal = __Float64::FromBytes(POS_INF_DOUBLE);
        }
        else {YReal = __Float64::FromBytes(BIG_NAN_DOUBLE);}
    }

    return complex64(YReal, YImag);
}

_END_EXTERN_C
_CSE_END

#if 0 // log test program generated by Deepseek
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <CSE/Base.h>

using namespace cse;

// 测试用例生成函数
std::vector<double> generate_test_cases()
{
    std::vector<double> cases;

    // 特殊值
    cases.push_back(0.0);
    cases.push_back(1.0);
    cases.push_back(std::numeric_limits<double>::infinity());
    cases.push_back(-std::numeric_limits<double>::quiet_NaN());

    // 接近0的值
    cases.push_back(std::numeric_limits<double>::min());
    cases.push_back(std::numeric_limits<double>::denorm_min());
    cases.push_back(1e-300);
    cases.push_back(1e-200);
    cases.push_back(1e-100);

    // 10的幂次方
    for (int i = -30; i <= 30; ++i)
    {
        cases.push_back(cse::pow(10.0, i));
    }

    // 2的幂次方
    for (int i = -10; i <= 10; ++i)
    {
        cases.push_back(cse::pow(2.0, i));
    }

    // 随机值
    cases.push_back(0.5);
    cases.push_back(1.5);
    cases.push_back(3.141592653589793);
    cases.push_back(2.718281828459045);
    cases.push_back(100.0);
    cases.push_back(12345.6789);
    cases.push_back(1e-5);
    cases.push_back(1e5);
    cases.push_back(1e50);
    cases.push_back(1e100);
    cases.push_back(1e200);
    cases.push_back(1e300);

    return cases;
}

int main()
{
    auto test_cases = generate_test_cases();

    // 创建CSV文件(Excel可以打开)
    std::ofstream out("log_test_report.csv");

    // 写入表头
    out << "Input Value,__IEEE754_LOG128F_C64F Result,std::log10 Result,"
        << "Absolute Difference,Relative Difference (%)\n";

    // 设置高精度输出
    out << std::scientific << std::setprecision(16);

    for (double x : test_cases)
    {
        double custom_result = __IEEE754_LOG128F_C64F(__Float64(x));
        double std_result;

        try {std_result = std::log10(x);}
        catch (...)
        {
            std_result = std::numeric_limits<double>::quiet_NaN();
        }

        // 计算差异
        double abs_diff = std::abs(custom_result - std_result);
        double rel_diff = (std_result != 0 && std::isfinite(std_result)) ?
            (abs_diff / std::abs(std_result)) * 100.0 : 0.0;

        // 处理特殊值
        /*auto handle_special = [](double v) -> std::string
        {
            if (std::isinf(v)) return (v > 0) ? "INF" : "-INF";
            if (std::isnan(v)) return "NAN";
            return std::to_string(v);
        };*/

        out << x << ","
            << custom_result << ","
            << std_result << ","
            << abs_diff << ","
            << rel_diff << "\n";
    }

    out.close();
    std::cout << "Results exported to log_test_report.csv" << std::endl;
}
#endif

#if 0 // Complex log test program generated by Deepseek
#include <iostream>
#include <complex>
#include <cmath>
#include <iomanip>

#include <CSE/Base.h>

using namespace cse;

int main()
{
    // 测试用例数组
    complex64 test_cases[] =
    {
        {0.0, 0.0},        // 实部和虚部都为0
        {1.0, 0.0},        // 实数为1，虚部为0
        {0.0, 1.0},        // 实部为0，虚数为1
        {1.0, 1.0},         // 实部和虚部都为1
        {10.0, 0.0},       // 实数10
        {0.0, 10.0},       // 纯虚数10i
        {2.5, 3.5},        // 一般复数
        {1e-10, 1e-10},    // 非常小的数
        {1e10, 1e10},      // 非常大的数
        {INFINITY, 0.0},   // 实部无穷大
        {0.0, INFINITY},   // 虚部无穷大
        {NAN, 0.0},        // 实部NaN
        {0.0, NAN},        // 虚部NaN
        {-1.0, 0.0},       // 负实数
        {0.5, 0.5},        // 小于1的复数
        {0.999, 0.001},    // 接近1的复数
        {1.001, 0.001}     // 略大于1的复数
    };

    std::cout << std::setprecision(15);
    std::cout << "Testing __GLIBCT_LOG64C vs std::log10\n";
    std::cout << "====================================\n";

    for (const auto& z : test_cases)
    {
        // 计算两种结果
        complex64 custom_log = __GLIBCT_LOG64C(z, 0);
        complex64 std_log = std::log10(z);

        // 计算差异
        double real_diff = std::abs(custom_log.real() - std_log.real());
        double imag_diff = std::abs(custom_log.imag() - std_log.imag());

        std::cout << "Input: (" << z.real() << ", " << z.imag() << "i)\n";
        std::cout << "  Custom log10: (" << custom_log.real() << ", " << custom_log.imag() << "i)\n";
        std::cout << "  Std log10:    (" << std_log.real() << ", " << std_log.imag() << "i)\n";
        std::cout << "  Real diff: " << real_diff << "\n";
        std::cout << "  Imag diff: " << imag_diff << "\n";
        std::cout << "------------------------------------\n";
    }

    return 0;
}
#endif
