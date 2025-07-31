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
