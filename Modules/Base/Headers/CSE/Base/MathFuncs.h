/*
    NOTICE: Math functions in this file and related source files is taken
    from serval math libraries, like glibc's math functions (licensed under
    LGPL-2.1 license), or IBM Accurate Mathematical Library (licensed under
    GPL-2.0 or LGPL-2.1? license) or other open-sourse project. In either
    event, it is copyrighted by the authors. What you see either here or
    related source files maybe used freely but it comes with no support or
    guarantee.
*/

#pragma once

#ifndef _CSE_MATH_BASE
#define _CSE_MATH_BASE

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/GLTypes.h>
#include <array>
#include <vector>
#include <complex>
#include <cassert>

#if defined _MSC_VER
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
#if defined _STL_DISABLE_CLANG_WARNINGS
_STL_DISABLE_CLANG_WARNINGS
#endif
#pragma push_macro("new")
#undef new
#endif

// 宇宙之大，粒子之微。
// 火箭之速，化工之巧。
// 星球之变，生命之谜。
// 日用其繁，无不数学。

_CSE_BEGIN

using __Float64 = IEEE754_Dbl64;
using complex64 = std::complex<float64>;
using std::literals::operator""i;

#define __stelcxx_array_math_function_body(type, var, expression)\
type##vec<N> _Res;\
__stelcxx_array_operation_ext(var, N, _Res[var] = expression;)\
return _Res;\

/****************************************************************************************\
*                                          E X P                                         *
\****************************************************************************************/

// Original function by FSF
_EXTERN_C
extern const uint64 __Exp64f_table[];
__Float64 __cdecl __IEEE754_EXP64F(__Float64 _X);
_END_EXTERN_C

/**
 * @brief Returns the natural exponentiation of x. i.e., e^x.
 */
float64 __cdecl exp(float64 _X);
complex64 __cdecl expc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl exp(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE exp(_X[i]))
}



/****************************************************************************************\
*                                          L O G                                         *
\****************************************************************************************/

// Original function by FSF and Stephen L. Moshier
_EXTERN_C
extern const double __Ln64f_table[]; // For Double
__Float64 __cdecl __IEEE754_LOG128F_C64F(__Float64 _X); // quaduple precision function, but can be used for double precision.
__Float64 __cdecl __IEEE754_LN64F(__Float64 _X); // natural logarithm for double precision
_END_EXTERN_C

/**
 * @brief Returns the logarithm of x, i.e. the value y which satisfies x=base^y.
 * @overload lb(x) (Scrapped feature)returns the binary logarithm of x.
 * @overload ln(x) returns the natural logarithm of x.
 * @overload log(x) returns the base 10 logarithm of x, i.e. the value y which satisfies x=10^y.
 * @param _X Specify the value of which to take the logarithm.
 * @param _Base - Specify the base value
 */
float64 __cdecl log(float64 _X, float64 _Base); // log x base on y
__declspec(deprecated("Binary logarithm \"lb\" function is unused in CSE."))
float64 __cdecl lb(float64 _X); // Unused
float64 __cdecl ln(float64 _X);
float64 __cdecl log(float64 _X);

__declspec(deprecated("Binary logarithm \"lb\" function is unused in CSE."))
complex64 __cdecl lbc(complex64 _X, int64 K_OFFSET = 0); // Unused
complex64 __cdecl lnc(complex64 _X, int64 K_OFFSET = 0);
complex64 __cdecl logc(complex64 _X, int64 K_OFFSET = 0);
complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 K_OFFSET = 0);

template<std::size_t N>
__declspec(deprecated("Binary logarithm \"lb\" function is unused in CSE."))
fvec<N> __cdecl lb(fvec<N> _X);

template<std::size_t N>
fvec<N> __cdecl ln(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ln(_X[i]))
}

template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i]))
}

template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X, float64 _Base)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i], _Base))
}

template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X, fvec<N> _Base)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i], _Base[i]))
}



/****************************************************************************************\
*                                    P O W E R                                           *
\****************************************************************************************/

_EXTERN_C
// Power Implemention by FSF
extern const struct __GNU_Table_Powlnt __Pow64f_ln_table[];
int CheckInt(uint64_t iy);
__Float64 __cdecl __IEEE754_POW64F(__Float64 _X, __Float64 _Power);
// Square root Implemention by IBM
extern const double __IBM_inroot_table[128];
__Float64 __cdecl __IBM_SQRT64F(__Float64 _X);
// Cube root Implemention by FSF
__Float64 __cdecl __IEEE754_CBRT64F(__Float64 _X);
_END_EXTERN_C

/**
 * @brief Returns the value of x raised to the y power, i.e. x^y.
 * @param _X - Specify the value to raise to the power y.
 * @param _Power - Specify the power to which to raise x.
 */
float64 __cdecl pow(float64 _X, float64 _Power);
complex64 __cdecl powc(complex64 _X, complex64 _Power, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl pow(fvec<N> _X, float64 _Power)
{
    __stelcxx_array_math_function_body(f, i, _CSE pow(_X[i], _Power))
}

template<std::size_t N>
fvec<N> __cdecl pow(fvec<N> _X, fvec<N> _Power)
{
    __stelcxx_array_math_function_body(f, i, _CSE pow(_X[i], _Power[i]))
}

/**
 * @note This function have a time complexity of O(n^4)
 * maybe cause a high delay when inputing large values.
 */
template<std::size_t _Size>
matrix<_Size, _Size> __cdecl pow(matrix<_Size, _Size> _A, uint64 _Power)
{
    assert(_Power > 0);
    matrix<_Size, _Size> m0;
    for (size_t i = 0; i < _Power; i++) {m0 = _A * _A;}
    return m0;
}

/**
 * @brief Returns the square root of x, i.e. the value √x.
 */
float64 __cdecl sqrt(float64 _X);
std::array<complex64, 2> __cdecl sqrtc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl sqrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sqrt(_X[i]))
}

/**
 * @brief Returns the inverse of the square root of x, i.e. the value 1/√x
 */
float64 __cdecl inversesqrt(float64 _X);
std::array<complex64, 2> __cdecl inversesqrtc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl inversesqrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE inversesqrt(_X[i]))
}

/**
 * @brief Returns the cube root of x, i.e. the value 3√x.
 */
float64 __cdecl cbrt(float64 _X);
std::array<complex64, 3> __cdecl cbrtc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl cbrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE cbrt(_X[i]))
}

/**
 * @brief Returns the nth-root of x, i.e. the value n√x.
 * @param _X - Specify the value to find root.
 * @param _Expo - Specify the value of exponential of root.
 */
float64 __cdecl yroot(float64 _X, float64 _Expo);
std::vector<complex64> __cdecl yrootc(complex64 _X, complex64 _Expo, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl yroot(fvec<N> _X, float64 _Expo)
{
    __stelcxx_array_math_function_body(f, i, _CSE yroot(_X[i], _Expo))
}

template<std::size_t N>
fvec<N> __cdecl yroot(fvec<N> _X, fvec<N> _Expo)
{
    __stelcxx_array_math_function_body(f, i, _CSE yroot(_X[i], _Expo[i]))
}



/****************************************************************************************\
*                                      TRIGONOMETRY                                      *
\****************************************************************************************/

// Reference:
// https://en.wikipedia.org/wiki/Trigonometric_functions
// https://en.wikipedia.org/wiki/Chord_(geometry)
// https://en.wikipedia.org/wiki/Versine
// https://en.wikipedia.org/wiki/Exsecant
// image of the returns of these functions: https://en.wikipedia.org/wiki/File:Circle-trig6.svg

// Use degrees for real number functions, radians for complex functions.

//_EXTERN_C
//float64 mod2pi(float64 _X, int64 _Integer);
//_END_EXTERN_C

// A new convertor for angle unit converting
struct Angle
{
    #ifndef TRIGONOMETRY_USE_RADIANS // Recommand to use degrees for trigonometry due to reduce errors.
    constexpr static const float64 Turns    = 360.l;
    constexpr static const float64 Degrees  = 1.l;
    constexpr static const float64 Radians  = 57.295779513082320876798154814105l;
    constexpr static const float64 Gradians = 0.9l;
    #else
    constexpr static const float64 Turns    = 6.283185307179586476925286766559l;
    constexpr static const float64 Degrees  = 0.01745329251994329576923690768489l;
    constexpr static const float64 Radians  = 1.l;
    constexpr static const float64 Gradians = 0.0157079632679489661923132169164l;
    #endif

    float64 Data;

    Angle() {}
    Angle(float64 _Data) : Data(_Data) {}

    static Angle FromTurns(float64 _Turns) {return Angle{_Turns * Turns};}
    static Angle FromDegrees(float64 _Degs) {return Angle{_Degs * Degrees};}
    static Angle FromRadians(float64 _Rads) {return Angle{_Rads * Radians};}
    static Angle FromGradians(float64 _Gons) {return Angle{_Gons * Gradians};}

    constexpr float64 ToTurns()const {return Data / Turns;}
    constexpr float64 ToDegrees()const {return Data / Degrees;}
    constexpr float64 ToRadians()const {return Data / Radians;}
    constexpr float64 ToGradians()const {return Data / Gradians;}

    operator float64() {return Data;}
};

// Trigonometric functions for Degrees Implemented by OpenCV
extern const float64 __CV_SinCos_Tab[];
float64 __cdecl __CV_SIN_DEGREES(float64 _X);
float64 __cdecl __CV_COS_DEGREES(float64 _X);
float64 __cdecl __CV_TAN_DEGREES(float64 _X);

_EXTERN_C
// Trigonometric functions for Radians Implemented by IBM
extern const union __IBM_Table_Type440 __IBM_sincostab;
extern const __Float64 __IBM_TANF64_XFGTBL[186][4];
__Float64 __cdecl __IBM_SIN64F(__Float64 x);
__Float64 __cdecl __IBM_COS64F(__Float64 x);
__Float64 __cdecl __IBM_TAN64F(__Float64 x);
_END_EXTERN_C

/**
 * @brief The standard trigonometric sine function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl sin(Angle _X);
complex64 __cdecl sinc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl sin(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sin(_X[i]))
}

/**
 * @brief The standard trigonometric cosine function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl cos(Angle _X);
complex64 __cdecl cosc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl cos(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE cos(_X[i]))
}

/**
 * @brief The standard trigonometric tangent function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl tan(Angle _X);
complex64 __cdecl tanc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl tan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE tan(_X[i]))
}

/**
 * @brief The standard trigonometric cotangent function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl ctg(Angle _X);
complex64 __cdecl ctgc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl ctg(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ctg(_X[i]))
}

/**
 * @brief The standard trigonometric secant function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl sec(Angle _X);
complex64 __cdecl secc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl sec(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sec(_X[i]))
}

/**
 * @brief The standard trigonometric cosecant function
 * Real number based on degrees, Complex based on radians.
 */
float64 __cdecl csc(Angle _X);
complex64 __cdecl cscc(complex64 _X);

template<std::size_t N>
fvec<N> __cdecl csc(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE csc(_X[i]))
}

/**
 * @brief returns the axes of a two-dimensional XYZ coordinates divide the plane into four infinite regions.
 * @param _X - angle in degrees.
 * @returns
 * 0 -> x-pos,
 * 1 -> Quadrant I,
 * 2 -> y-pos,
 * 3 -> Quadrant II,
 * 4 -> x-neg,
 * 5 -> Quadrant III,
 * 6 -> y-neg,
 * 7 -> Quadrant IV
 */
int64 __cdecl Quadrant(Angle _X);
Angle __cdecl Arctan2(float64 _Y, float64 _X);

///////////////////////////////////// INVERSE ////////////////////////////////////

// Use degrees for real number functions, radians for complex functions.

_EXTERN_C
// Inverse Trigonometric functions for degrees and Radians Implemented by IBM and Stephen L. Moshier
extern const union __IBM_Table_Type2568 __IBM_asncs_table;
extern const __Float64 __ArctanF128_table_deg[];
extern const __Float64 __ArctanF128_table_rad[];
Angle __cdecl __IBM_ASIN64F(__Float64 x);
Angle __cdecl __IBM_ACOS64F(__Float64 x);
Angle __cdecl __IEEE754_ATAN128F_C64F(__Float64 x);
_END_EXTERN_C

/**
 * @brief Arc Sine. Returns the angle whose trigonometric sine is x.
 * The range of values returned by asin is [-90, 90].
 */
Angle __cdecl arcsin(float64 _X);
complex64 __cdecl arcsinc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arcsin(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcsin(_X[i]))
}

/**
 * @brief Arc cosine. Returns the angle whose trigonometric cosine is x.
 * The range of values returned by acos is [0, 180].
 */
Angle __cdecl arccos(float64 _X);
complex64 __cdecl arccosc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arccos(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arccos(_X[i]))
}

/**
 * @brief Arc tangent. Returns the angle whose tangent is x.
 * The value returned in this case is in the range [-90, 90].
 */
Angle __cdecl arctan(float64 _X);
complex64 __cdecl arctanc(complex64 _X, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arctan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arctan(_X[i]))
}

Angle __cdecl arcctg(float64 _X);
complex64 __cdecl arcctgc(complex64 _X, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arcctg(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcctg(_X[i]))
}

Angle __cdecl arcsec(float64 _X);
complex64 __cdecl arcsecc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arcsec(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcsec(_X[i]))
}

Angle __cdecl arccsc(float64 _X);
complex64 __cdecl arccscc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<std::size_t N>
fvec<N> __cdecl arccsc(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arccsc(_X[i]))
}

///////////////////////////////////// EXPAND ////////////////////////////////////

// A few functions were common historically, but are now seldom used,
// such as:
// the chord
// the versine (which appeared in the earliest tables), coversine, the haversine
// the exsecant and the excosecant.
// Define "_USE_FULL_TRIGONOMETRY_SYSTEM" to release them.

#ifdef _USE_FULL_TRIGONOMETRY_SYSTEM

float64 __cdecl crd(float64 _X, float64 _Radius = 1); // chord
float64 __cdecl arc(float64 _X, float64 _Radius = 1); // arc

float64 __cdecl siv(float64 _X); // versed sine
float64 __cdecl vcs(float64 _X); // versed cosine
float64 __cdecl cvs(float64 _X); // coversed sine
float64 __cdecl cvc(float64 _X); // coversed cosine

float64 __cdecl hvs(float64 _X); // haversed sine
float64 __cdecl hvc(float64 _X); // haversed cosine
float64 __cdecl hcv(float64 _X); // hacoversed sine
float64 __cdecl hcc(float64 _X); // hacoversed cosine

float64 __cdecl exs(float64 _X); // exsecant
float64 __cdecl exc(float64 _X); // excosecant

// Inverse functions

Angle __cdecl arccrd(float64 _X, float64 _Radius = 1);

Angle __cdecl arcsiv(float64 _X);
Angle __cdecl arcvcs(float64 _X);
Angle __cdecl arccvs(float64 _X);
Angle __cdecl arccvc(float64 _X);

Angle __cdecl archvs(float64 _X);
Angle __cdecl archvc(float64 _X);
Angle __cdecl archcv(float64 _X);
Angle __cdecl archcc(float64 _X);

Angle __cdecl arcexs(float64 _X);
Angle __cdecl arcexc(float64 _X);

#endif



/****************************************************************************************\
*                                        G A M M A                                       *
\****************************************************************************************/

/**
 * @brief gamma function
 * @note complex domain of this function has no plan to add.
 */
float64 gamma(float64 _X); // TODO

template<std::size_t N>
fvec<N> __cdecl gamma(fvec<N> _X); // TODO



/////////////////////////////////////////////////////////////////
//                            解方程                            //
//                                                             //
// 求解由以下形式定义的多项式方程的解(降幂排序)                    //
//      n                                                      //
//      Σ (a_i * x^(n - i - 1)) = 0                            //
//     i=0                                                     //
//                                                             //
// 其中一次和二次使用求根公式，三次使用范盛金的算法，四次使用沈天珩  //
// 的算法，五次及以上使用杜兰德-肯纳迭代。                         //
/////////////////////////////////////////////////////////////////

using InputArray  = const std::vector<float64>&;
using OutputArray = std::vector<complex64>&;

void __Verify_Input_Output(InputArray Coeffs, OutputArray Roots, uint64 Power)noexcept(0);

/**
 * @brief 求解线性方程，如 ax + b = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 */
int SolveLinear(InputArray Coeffs, OutputArray Roots);

/**
 * @brief 求解二次方程，如 ax^2 + bx + c = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 */
int SolveQuadratic(InputArray Coeffs, OutputArray Roots);

/**
 * @brief 求解三次方程，如 ax^3 + bx^2 + cx + d = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 * @param Tolerence 误差的负对数
 */
int SolveCubic(InputArray Coeffs, OutputArray Roots, float64 Tolerence = 10);

/**
 * @brief 求解四次方程，如 ax^4 + bx^3 + cx^2 + dx + e = 0
 * @param Coeffs 参数，按x的降幂排序
 * @param Roots 方程的解
 * @param Tolerence 误差的负对数
 */
int SolveQuartic(InputArray Coeffs, OutputArray Roots, float64 Tolerence = 10);

/* ************************************************************************** *\
   丹霞：寻找五次或以上方程的解法是一个困扰了数学家们300多年的问题。历史上无数的数学
   家试图推导这些多项式方程求解算法，但无一例外都失败了。1824年，阿贝尔等人证明了五次
   及更高次的多项式方程没有一般的代数解法，即这样的方程不能由方程的系数经有限次四则运
   算和开方运算求解。

   1926年，苏家驹发表了一篇文章，声称自己发现了一个五次方程的求解算法。随后华罗庚在看
   了苏家驹的文章以后尝试继续推导六次方程的求解算法，这意味着他要尝试否定先前阿贝尔等
   人已经证实的“五次以上方程的解无法用系数表示”这一理论，但是当他看了阿贝尔的论文以后，
   认为其“条理精严，无懈可击”。随后他就转而开始尝试在苏家驹的论文中寻找漏洞，最终发表
   了他的成名之作《苏家驹之代数的五次方程式解法不能成立之理由》。

   阿贝尔定律已经证实五次以上方程的解无法使用系数组成的根式表示。也就是这些方程没有通
   用的求解算法。但是，这并不意味着它们完全没办法求解，此处使用杜兰德-肯纳算法求解这
   些方程。

   参考文献：
    [1] 苏家驹.代数的五次方程式之解法[J].学艺,1926.
    [2] 华罗庚.来件——苏家驹之代数的五次方程式解法不能成立之理由[J].科学,1930.
    [3] Wikipedia Contributors. Durand–Kerner method — Wikipedia, The Free
        Encyclopedia[A/OL]. (2025).
        https://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
\* ************************************************************************** */

__interface SolvePolyRoutine
{
public:
    virtual int Run(InputArray Coeffs, OutputArray Roots)const = 0;
};

class DurandKernerSolvePoly : public SolvePolyRoutine
{
public:
    std::vector<complex64> InitValue         = {};
    float64                AbsoluteTolerence = 14;
    float64                RelativeTolerence = 14;
    float64                MaxIter           = 3;

    static std::vector<complex64> GetInitValuePower(float64 Power, complex64 IValue = 0.4 + 0.9i);
    static std::vector<complex64> GetInitValueCircle(InputArray Coeffs);
    static std::vector<complex64> GetInitValueHomotopy(InputArray Coeffs, float64 a = 0.5);

    int Run(InputArray Coeffs, OutputArray Roots)const override;
};

/**
 * @brief 求解任意多项式方程的解
 * http://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
 * @param Coeffs 多项式的系数，降幂排序
 * @param Roots 方程的解
 * @param Routine 求解算法，默认使用杜兰德-肯纳算法
 * @return 迭代次数
 */
uint64 SolvePoly(InputArray Coeffs, OutputArray Roots, const SolvePolyRoutine& Routine = DurandKernerSolvePoly());

_CSE_END

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
_STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

// 「这个好像不太会。」
// 「这也不会，那也不会，那你天天上课学的啥？」

// 「老师教过我们一次函数，三角函数，不等式......」
// 「却从未教过我们怎么买菜，千克是多少......」

#endif
