/*=================================================================*\
  Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

  Developed at SunPro, a Sun Microsystems, Inc. business.
  Permission to use, copy, modify, and distribute this software is
  freely granted, provided that this notice is preserved.
\*=================================================================*/

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/ConstLists.h"
#include <cfloat>

_CSE_BEGIN

static const auto
    __HALF_LN2 = __Float64::FromBytes(0x3fd62e42fefa39ef).x,
    __LN_MAX_DBL = __Float64::FromBytes(0x40862e42fefa39ef).x,
    __HYP_OFLOW_THRES = __Float64::FromBytes(0x408633ce8fb9f87d).x;

/* sinh(x)
 * Method :
 * mathematically sinh(x) if defined to be (exp(x)-exp(-x))/2
 *	1. Replace x by |x| (sinh(-x) = -sinh(x)).
 *	2.
 *						    E + E/(E+1)
 *	    0        <= x <= 22     :  sinh(x) := --------------, E=expm1(x)
 *							2
 *
 *	    22       <= x <= lnovft :  sinh(x) := exp(x)/2
 *	    lnovft   <= x <= ln2ovft:  sinh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  sinh(x) := x*shuge (overflow)
 *
 * Special cases:
 *	sinh(x) is |x| if x is +INF, -INF, or NaN.
 *	only sinh(0)=0 is exact for finite x.
 */
float64 __cdecl sinh(float64 _X)
{
    //x is INF or NaN
    if (isinf(_X) || isnan(_X)) {return _X;}

    float64 h = std::signbit(_X) ? -0.5 : 0.5;
    float64 absx = abs(_X);

    // |x| in [0,22], return sign(x)*0.5*(E+E/(E+1)))
    if (absx < 22)
    {
        // sinh(tiny) = tiny with inexact
        // 丹霞：原实现的这里用了一个巨大的数加上x大于1这个判断条件，
        // 当然它在x < 0x1p-28的情况下是恒成立的，看上去有些多此一
        // 举，实际上在某些编译器下（如英特尔的ICC）如果直接返回x，
        // 相关计算会被“吃掉”。新版本的编译器应该不会再出现这种问题
        // 了，所以这里直接返回。当然我也不能保证101%不会出问题。
        if (absx < 0x1p-28) {return _X;}

        float64 t = __IEEE854_EXPXM1128F_C64F(absx);
        if (absx < 1) {return h * (2.0 * t - t * t / (t + 1.0));}
        return h * (t + t / (t + 1.0));
    }

    // |x| in [22, log(maxdouble)] return 0.5*exp(|x|)
    if (absx < __LN_MAX_DBL)
    {
        return h * __IEEE754_EXP64F(absx);
    }

    // |x| in [log(maxdouble), overflowthresold]
    if (absx <= __HYP_OFLOW_THRES)
    {
        float64 w = __IEEE754_EXP64F(0.5 * absx);
        float64 t = h * w;
        return t * w;
    }

    return ::copysign(__Float64::FromBytes(POS_INF_DOUBLE), _X);
}

float64 __cdecl csch(float64 _X)
{
    if (isnan(_X)) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}

    float64 h = std::signbit(_X) ? -2 : 2;
    float64 absx = abs(_X);
    if (absx == 0)
    {
        return ::copysign(__Float64::FromBytes(POS_INF_DOUBLE), _X);
    }

    if (absx < 22)
    {
        if (absx < 0x1p-28) {return 1. / _X;}
        float64 t = __IEEE854_EXPXM1128F_C64F(absx);
        return h * (t + 1.0) / (t * (t + 2.0));
    }

    if (absx < __LN_MAX_DBL)
    {
        return h * __IEEE754_EXP64F(-absx);
    }

    if (absx <= __HYP_OFLOW_THRES)
    {
        float64 w = __IEEE754_EXP64F(-0.5 * absx);
        float64 t = h * w;
        return t * w;
    }

    return ::copysign(__Float64::FromBytes(POS_ZERO_DOUBLE), _X);
}

/* cosh(x)
 * Method :
 * mathematically cosh(x) if defined to be (exp(x)+exp(-x))/2
 *	1. Replace x by |x| (cosh(x) = cosh(-x)).
 *	2.
 *							[ exp(x) - 1 ]^2
 *	    0        <= x <= ln2/2  :  cosh(x) := 1 + -------------------
 *							   2*exp(x)
 *
 *						  exp(x) +  1/exp(x)
 *	    ln2/2    <= x <= 22     :  cosh(x) := -------------------
 *							  2
 *	    22       <= x <= lnovft :  cosh(x) := exp(x)/2
 *	    lnovft   <= x <= ln2ovft:  cosh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only cosh(0)=1 is exact for finite x.
 */
float64 __cdecl cosh(float64 _X)
{
    float64 absx = abs(_X);

    //x is INF or NaN
    if (isinf(_X) || isnan(_X))
    {
        __Float64 ix(_X);
        ix.Negative = 0;
        return ix.x;
    }

    // cosh(tiny) = 1
    if (absx < 0x1p-55) {return 1;}

    // |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|))
    if (absx < __HALF_LN2)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(absx);
        float64 w = 1. + t;
        return 1. + (t * t) / (w + w);
    }

    float64 h = 0.5;

    // |x| in [0,22]
    if (absx < 22)
    {
        float64 t = __IEEE754_EXP64F(absx);
        return h * t + h / t;
    }

    // |x| in [22, log(maxdouble)] return half*exp(|x|)
    if (absx < __LN_MAX_DBL)
    {
        return h * __IEEE754_EXP64F(absx);
    }

    // |x| in [log(maxdouble), overflowthresold]
    if (absx <= __HYP_OFLOW_THRES)
    {
        float64 w = __IEEE754_EXP64F(h * absx);
        float64 t = h * w;
        return t * w;
    }

    return __Float64::FromBytes(POS_INF_DOUBLE);
}

complex64 __cdecl __GLIBCT_COSH64C(complex64 _X)
{
    float64 XReal = _X.real(), XImag = _X.imag();
    int RClass = std::fpclassify(XReal);
    int IClass = std::fpclassify(XImag);

    if (RClass >= FP_ZERO)
    {
        // Real part is finite.
        if (IClass >= FP_ZERO)
        {
            // Imaginary part is finite.
            const int t = int((DBL_MAX_EXP - 1) * CSE_LN2);
            float64 sinix, cosix;

            if (abs(XImag) > DBL_MIN)
            {
                sinix = __IBM_SIN64F(XImag);
                cosix = __IBM_COS64F(XImag);
            }
            else
            {
                sinix = XImag;
                cosix = 1;
            }

            if (abs(XReal) > t)
            {
                float64 exp_t = __IEEE754_EXP64F(t);
                float64 rx = abs(XReal);
                if (std::signbit(XReal)) {sinix = -sinix;}
                rx -= t;
                sinix *= exp_t / 2;
                cosix *= exp_t / 2;

                if (rx > t)
                {
                    rx -= t;
                    sinix *= exp_t;
                    cosix *= exp_t;
                }

                if (rx > t)
                {
                    // Overflow (original real part of x > 3t).
                    return {DBL_MAX * cosix, DBL_MAX * sinix};
                }
                else
                {
                    float64 exp_val = __IEEE754_EXP64F(rx);
                    return {exp_val * cosix, exp_val * sinix};
                }
            }
            else
            {
                return {cosh(XReal) * cosix, sinh(XReal) * sinix};
            }
        }
        else
        {
            return
            {
                XReal == 0 ? 0 : __Float64::FromBytes(BIG_NAN_DOUBLE).x,
                XImag - XImag
            };
        }
    }
    else if (RClass == FP_INFINITE)
    {
        // Real part is infinite.
        if (IClass > FP_ZERO)
        {
            // Imaginary part is finite.
            float64 sinix, cosix;

            if (abs(XImag) > DBL_MIN)
            {
                sinix = __IBM_SIN64F(XImag);
                cosix = __IBM_COS64F(XImag);
            }
            else
            {
                sinix = XImag;
                cosix = 1;
            }

            return
            {
                ::copysign(__Float64::FromBytes(POS_INF_DOUBLE), cosix),
                (::copysign(__Float64::FromBytes(POS_INF_DOUBLE), sinix) *
                    ::copysign(1, XReal))
            };
        }
        else if (IClass == FP_ZERO)
        {
            // Imaginary part is 0.0.
            return
            {
                __Float64::FromBytes(POS_INF_DOUBLE),
                XImag * ::copysign(1, XReal)
            };
        }
        else
        {
            return
            {
                __Float64::FromBytes(POS_INF_DOUBLE),
                XImag - XImag
            };
        }
    }
    else
    {
        return
        {
            __Float64::FromBytes(BIG_NAN_DOUBLE),
            XImag == 0 ? XImag : __Float64::FromBytes(BIG_NAN_DOUBLE).x
        };
    }
}

float64 __cdecl sech(float64 _X)
{
    float64 absx = abs(_X);

    //x is NaN
    if (isnan(_X))
    {
        __Float64 ix(_X);
        ix.Negative = 0;
        return ix.x;
    }

    if (absx < 0x1p-55) {return 1;}

    if (absx < __HALF_LN2)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(absx);
        float64 w = 1. + t;
        return (2.0 * w) / (2.0 * w + t * t);
    }

    float64 h = 2;

    if (absx < 22)
    {
        float64 t = __IEEE754_EXP64F(absx);
        return (h * t) / (t * t + 1.0);
    }

    if (absx < __LN_MAX_DBL)
    {
        return h * __IEEE754_EXP64F(-absx);
    }

    if (absx <= __HYP_OFLOW_THRES)
    {
        float64 w = __IEEE754_EXP64F(-0.5 * absx);
        float64 t = h * w;
        return t * w;
    }

    return __Float64::FromBytes(POS_ZERO_DOUBLE);
}

/* tanh(x)
 * Return the Hyperbolic Tangent of x
 *
 * Method :
 *				       x    -x
 *				      e  - e
 *	0. tanh(x) is defined to be -----------
 *				       x    -x
 *				      e  + e
 *	1. reduce x to non-negative by tanh(-x) = -tanh(x).
 *	2.  0      <= x <= 2**-55 : tanh(x) := x*(one+x)
 *					        -t
 *	    2**-55 <  x <=  1     : tanh(x) := -----; t = expm1(-2x)
 *					       t + 2
 *						     2
 *	    1      <= x <=  22.0  : tanh(x) := 1-  ----- ; t=expm1(2x)
 *						   t + 2
 *	    22.0   <  x <= INF    : tanh(x) := 1.
 *
 * Special cases:
 *	tanh(NaN) is NaN;
 *	only tanh(0)=0 is exact for finite argument.
 */
float64 __cdecl tanh(float64 _X)
{
    float64 absx = abs(_X);

    // x is INF or NaN
    if (isinf(_X)) {return ::copysign(1, _X);}
    if (isnan(_X)) {return _X;}

    // x == +-0
    if (absx == 0) {return _X;}
    // tanh(small) = small
    if (absx < 0x1p-55) {return _X * (1.0 + _X);}

    // |x| < 1
    if (absx < 1)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(-2.0 * absx);
        return ::copysign(-t / (t + 2.0), _X);
    }

    // |x| < 22
    if (absx < 22)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(2.0 * absx);
        return ::copysign(1.0 - 2.0 / (t + 2.0), _X);
    }

    // |x| > 22, return +-1
    return ::copysign(1.0, _X);
}

float64 __cdecl coth(float64 _X)
{
    float64 absx = abs(_X);

    // x is INF or NaN
    if (isinf(_X)) {return ::copysign(1, _X);}
    if (isnan(_X)) {return _X;}

    // x == +-0
    if (absx == 0)
    {
        return ::copysign(__Float64::FromBytes(POS_INF_DOUBLE), _X);
    }
    // tanh(small) = small
    if (absx < 0x1p-55) {return 1.0 / _X + _X / 3.0;} // 洛朗级数的前2项

    // |x| < 1
    if (absx < 1)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(-2.0 * absx);
        return ::copysign(-1.0 - 2.0 / t, _X);
    }

    // |x| < 22
    if (absx < 22)
    {
        float64 t = __IEEE854_EXPXM1128F_C64F(2.0 * absx);
        return ::copysign(1.0 + 2.0 / t, _X);
    }

    // |x| > 22, return +-1
    return ::copysign(1.0, _X);
}

/* arsinh(x)
 * Method :
 *      Based on
 *              arsinh(x) = sgn(x) * ln(abs(x) + sqrt(x*x+1))
 *      we have
 *      arsinh(x) := x  if  1+x*x=1,
 *                := sgn(x)*(ln(x)+ln2)) for large |x|, else
 *                := sgn(x)*ln(2*abs(x)+1/(abs(x)+sqrt(x*x+1))) if|x|>2, else
 *                := sgn(x)*ln(1 + (abs(x) + x^2/(1 + sqrt(1+x^2))))
 */
float64 __cdecl arsinh(float64 _X)
{
    float64 absx = abs(_X), y;

    if (isinf(_X) || isnan(_X) || absx < 0x1p-28)
    {
        return _X; // return x inexact except 0
    }
    else if (absx > 0x1p-28 && absx < 2)
    {
        float64 t = absx * absx;
        y = __IEEE854_LN1PX128F_C64F(absx + t / (1.0 + __IBM_SQRT64F(1.0 + t)));
    }
    else if (absx > 2 && absx < 0x1p+28)
    {
        y = __IEEE854_LN128F_C64F(2.0 * absx + 1.0 / (sqrt (absx * absx + 1.0) + absx));
    }
    else
    {
        y = __IEEE854_LN128F_C64F(abs(_X)) + CSE_LN2;
    }

    return ::copysign(y, _X);
}

float64 __cdecl arcsch(float64 _X)
{
    return arsinh(1. / _X);
}

/* arcosh(x)
 * Method :
 *	Based on
 *		arcosh(x) = ln( x + sqrt(x*x-1) )
 *	we have
 *		arcosh(x) := ln(x)+ln2,	if x is large; else
 *		arcosh(x) := ln(2x-1/(sqrt(x*x-1)+x)) if x>2; else
 *		arcosh(x) := ln(1 + (t+sqrt(2.0*t+t*t))); where t=x-1.
 *
 * Special cases:
 *	arcosh(x) is NaN with signal if x<1.
 *	arcosh(NaN) is NaN without signal.
 */
float64 __cdecl arcosh(float64 _X)
{
    if (isinf(_X))
    {
        if (!std::signbit(_X)) {return _X;}
        return __Float64::FromBytes(S_NAN_DOUBLE);
    }
    if (isnan(_X)) {return _X;}
    if (_X >= 0x1p+28)
    {
        return __IEEE854_LN128F_C64F(_X) + CSE_LN2; // arcosh(huge) = ln(2x)
    }
    if (_X > 2)
    {
        float64 t = _X * _X;
        return __IEEE854_LN128F_C64F(2.0 * _X - 1.0 / (_X + __IBM_SQRT64F(t - 1.0)));
    }
    if (_X > 1)
    {
        double t = _X - 1.0;
        return __IEEE854_LN1PX128F_C64F(t + sqrt (2.0 * t + t * t));
    }
    if (_X == 1) {return 0;}
    return __Float64::FromBytes(S_NAN_DOUBLE);
}

float64 __cdecl arsech(float64 _X)
{
    return arcosh(1. / _X);
}

/* artanh(x)
   Method :
      1.Reduced x to positive by atanh(-x) = -atanh(x)
      2.For x>=0.5
                 1              2x                          x
    artanh(x) = --- * ln(1 + -------) = 0.5 * ln(1 + 2 * -------)
                 2            1 - x                       1 - x

    For x<0.5
    artanh(x) = 0.5*ln(1 + 2x+2x*x/(1-x))

   Special cases:
    artanh(x) is NaN if |x| > 1 with signal;
    artanh(NaN) is that NaN with no signal;
    artanh(+-1) is +-INF with signal.
 */
float64 __cdecl artanh(float64 _X)
{
    float64 absx = abs(_X), y;

    if (absx < 0x1p-28) {return _X;}
    else if (absx < 0.5)
    {
        float64 t = absx + absx;
        y = 0.5 * __IEEE854_LN1PX128F_C64F(t + t * absx / (1.0 - absx));
    }
    else if (absx < 1.0)
    {
        y = 0.5 * __IEEE854_LN1PX128F_C64F((absx + absx) / (1.0 - absx));
    }
    else
    {
        if (absx > 1.0) {return __Float64::FromBytes(S_NAN_DOUBLE);}
        y = __Float64::FromBytes(POS_INF_DOUBLE);
    }

    return ::copysign(y, _X);
}

float64 __cdecl arcoth(float64 _X)
{
    float64 absx = abs(_X), y;

    if (isinf(absx)) {y = __Float64::FromBytes(POS_ZERO_DOUBLE);}
    if (absx > 0x1p+28) {return 1. / _X;}
    else if (absx > 2)
    {
        y = 0.5 * __IEEE854_LN1PX128F_C64F((2.0 / absx) + 2 / (absx * (absx - 1.0)));
    }
    else if (absx > 1.0)
    {
        y = 0.5 * __IEEE854_LN1PX128F_C64F(2.0 / (absx - 1.0));
    }
    else
    {
        if (absx < 1.0) {return __Float64::FromBytes(S_NAN_DOUBLE);}
        y = __Float64::FromBytes(POS_INF_DOUBLE);
    }

    return ::copysign(y, _X);
}

_CSE_END

#if 0 // test program of sinh generated by Deepseek
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cfenv>
#include <vector>
#include <string>

#include <CSE/Base.h>

using namespace cse;

// 测试用例结构体
struct TestCase
{
    float64 input;
    std::string description;
};

// 打印结果比较
void printComparison(float64 x, float64 ourResult, float64 stdResult)
{
    std::cout << std::scientific << std::setprecision(16);
    std::cout << "x = " << x << "\n";
    std::cout << "Our sinh(x) = " << ourResult << "\n";
    std::cout << "Std sinh(x) = " << stdResult << "\n";
    std::cout << "Difference = " << (ourResult - stdResult) << "\n\n";
}

int main()
{
    // 准备测试用例
    std::vector<TestCase> testCases =
    {
        {0.0, "Zero"},
        {-0.0, "Negative zero"},
        {1.0, "Small positive number"},
        {-1.0, "Small negative number"},
        {0x1p-28, "Tiny positive number near underflow threshold"},
        {-0x1p-28, "Tiny negative number near underflow threshold"},
        {0.5, "Positive number < 1"},
        {-0.5, "Negative number > -1"},
        {1.5, "Positive number > 1"},
        {-1.5, "Negative number < -1"},
        {10.0, "Medium positive number"},
        {-10.0, "Medium negative number"},
        {22.0, "Boundary case for first condition"},
        {-22.0, "Negative boundary case"},
        {23.0, "Just above 22 boundary"},
        {-23.0, "Negative just above 22 boundary"},
        {700.0, "Large positive number near overflow"},
        {-700.0, "Large negative number near overflow"},
        {710.0, "Very large positive number causing overflow"},
        {-710.0, "Very large negative number causing overflow"},
        {std::numeric_limits<float64>::infinity(), "Positive infinity"},
        {-std::numeric_limits<float64>::infinity(), "Negative infinity"},
        {std::numeric_limits<float64>::quiet_NaN(), "NaN"},
        {std::numeric_limits<float64>::denorm_min(), "Denormal min"},
        {-std::numeric_limits<float64>::denorm_min(), "Negative denormal min"},
        {__Float64::FromBytes(0x40862e42fefa39ef) - 1.0, "Just below log(maxdouble)"},
        {__Float64::FromBytes(0x40862e42fefa39ef), "log(maxdouble)"},
        {__Float64::FromBytes(0x408633ce8fb9f87d) - 1.0, "Just below overflow threshold"},
        {__Float64::FromBytes(0x408633ce8fb9f87d), "Overflow threshold"}
    };

    // 执行测试
    for (const auto& test : testCases)
    {
        std::cout << "Test case: " << test.description << "\n";

        float64 ourResult = cse::sinh(test.input);
        float64 stdResult = std::sinh(test.input);

        printComparison(test.input, ourResult, stdResult);
    }

    return 0;
}
#endif

#if 0 // test program of arsinh generated by Deepseek
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>

#include <CSE/Base.h>

using namespace cse;

// 辅助函数，打印比较结果
void print_comparison(double x, double custom_result, double std_result)
{
    double diff = custom_result - std_result;
    std::cout << std::scientific << std::setprecision(15);
    std::cout << "x = " << x << "\n";
    std::cout << "  Custom asinh: " << custom_result << "\n";
    std::cout << "  Std asinh:    " << std_result << "\n";
    std::cout << "  Difference:   " << diff << "\n";
    std::cout << "  Rel diff:     " << diff / std_result << "\n";
    std::cout << "----------------------------------------\n";
}

int main()
{
    // 测试用例设计
    const double test_cases[] =
    {
        // 特殊值和边界条件
        0.0,
        -0.0,
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::quiet_NaN(),

        // 小值区间 (|x| < 2^-28)
        1e-9,
        -1e-9,
        1e-20,
        -1e-20,

        // 中间区间 (2^-28 < |x| < 2)
        0.5,
        -0.5,
        1.0,
        -1.0,
        1.999,
        -1.999,
        0x1p-27,  // 刚好大于2^-28
        -0x1p-27,
        0x1p-28 * 1.1,  // 接近边界
        -0x1p-28 * 1.1,

        // 大值区间 (2 < |x| < 2^28)
        2.1,
        -2.1,
        10.0,
        -10.0,
        1e6,
        -1e6,
        0x1p+27,
        -0x1p+27,
        0x1p+28 * 0.9,  // 接近边界
        -0x1p+28 * 0.9,

        // 极大值区间 (|x| >= 2^28)
        0x1p+28,
        -0x1p+28,
        1e20,
        -1e20,
        std::numeric_limits<double>::max(),
        -std::numeric_limits<double>::max()
    };

    // 比较结果
    for (double x : test_cases)
    {
        double custom_result = arsinh(x);
        double std_result = asinh(x);

        print_comparison(x, custom_result, std_result);
    }

    return 0;
}
#endif
