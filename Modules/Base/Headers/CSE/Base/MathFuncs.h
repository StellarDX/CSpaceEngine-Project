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
//                      Solving Equations                      //
//                                                             //
// Finds roots of Polynomial equations.                        //
// Linear and Quadratic function use common algorithm.         //
// Cubic function using Fan Shengjin's algorithm.              //
// polynomials using Durand-Kerner method.                     //
//                                                             //
// Port by StellarDX, 2022.                                    //
/////////////////////////////////////////////////////////////////

template<typename _IterC, typename _IterR>
int __Solve_Linear_Unchecked(_IterC CoeffsBegin, _IterR RootsBegin)
{
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    *RootsBegin = complex64(-b / a);

    return 0;
}

template<typename _IterC, typename _IterR>
int __Solve_Quadratic_Unchecked(_IterC CoeffsBegin, _IterR RootsBegin)
{
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    float64 c = *(CoeffsBegin + 2);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    float64 del = b * b - 4.0 * a * c;

    *(RootsBegin + 0) = (-b + _CSE sqrtc(complex64(del))[0]) / (2.0 * a);
    *(RootsBegin + 1) = (-b - _CSE sqrtc(complex64(del))[0]) / (2.0 * a);

    return 0;
}

int __Is_Zero(float64 _In, float64 _Err);

template<typename _IterC, typename _IterR>
int __Solve_Cubic_Unchecked(_IterC CoeffsBegin, _IterR RootsBegin, float64 p_Error)
{
    // Reference:
    // NATURAL SCIENCE JOURNAL OF HAINAN TEACHERES COLLEGE,Hainan Province,China. Vol. 2,No. 2; Dec, 1989,
    // A new extracting formula and a new distinguishing means on the one variable cubic equation., Fan Shengjin. PP.91-98 .
    // Obituary: The author of this algorithm was passed away at Sep 8th 2018, age of 63.
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    float64 c = *(CoeffsBegin + 2);
    float64 d = *(CoeffsBegin + 3);
    float64 RealError = pow(10, -p_Error);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    float64 A = b * b - 3.0 * a * c;
    float64 B = b * c - 9.0 * a * d;
    float64 C = c * c - 3.0 * b * d;

    float64 del = B * B - 4.0 * A * C;

    if (__Is_Zero(A, RealError) && __Is_Zero(B, RealError))
    {
        *(RootsBegin + 0) = -b / (3.0 * a);
        *(RootsBegin + 1) = *(RootsBegin + 0); // -c / b
        *(RootsBegin + 2) = *(RootsBegin + 0); // -3 * d / c

        return 1;
    }

    if (__Is_Zero(del, RealError))
    {
        float64 K = B / A;

        *(RootsBegin + 0) = -b / a + K;
        *(RootsBegin + 1) = -K / 2;
        *(RootsBegin + 2) = *(RootsBegin + 1);

        return 3;
    }

    if (del > 0)
    {
        float64 Y1 = A * b + (3. * a * (-B - _CSE sqrt(del)) / 2.);
        float64 Y2 = A * b + (3. * a * (-B + _CSE sqrt(del)) / 2.);

        *(RootsBegin + 0) = (-b - (_CSE cbrt(Y1) + _CSE cbrt(Y2))) / (3. * a);
        *(RootsBegin + 1) = (-2. * b + (_CSE cbrt(Y1) + _CSE cbrt(Y2)) + _CSE sqrt(3.) * (_CSE cbrt(Y1) - _CSE cbrt(Y2)) * 1i) / (6. * a);
        *(RootsBegin + 2) = (-2. * b + (_CSE cbrt(Y1) + _CSE cbrt(Y2)) - _CSE sqrt(3.) * (_CSE cbrt(Y1) - _CSE cbrt(Y2)) * 1i) / (6. * a);

        return 2;
    }

    if (del < 0)
    {
        float64 tet = arccos((2 * A * b - 3 * a * B) / (2. * _CSE sqrt(_CSE pow(A, 3)))); // (+2*k*PI)

        *(RootsBegin + 0) = (-b - 2. * _CSE sqrt(A) * _CSE cos(tet / 3.)) / (3. * a);
        *(RootsBegin + 1) = (-b + _CSE sqrt(A) * (_CSE cos(tet / 3) + _CSE sqrt(3) * _CSE sin(tet / 3))) / (3. * a);
        *(RootsBegin + 2) = (-b + _CSE sqrt(A) * (_CSE cos(tet / 3) - _CSE sqrt(3) * _CSE sin(tet / 3))) / (3. * a);

        return 4;
    }

    return 0;
}

_NODISCARD float64 sgn(float64 _Xx)noexcept;

template<typename _IterC, typename _IterR>
int __Solve_Quartic_Unchecked(_IterC CoeffsBegin, _IterR RootsBegin, float64 p_Error)
{
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    float64 c = *(CoeffsBegin + 2);
    float64 d = *(CoeffsBegin + 3);
    float64 e = *(CoeffsBegin + 4);
    float64 RealError = pow(10, -p_Error);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    float64 D = 3.0 * b * b - 8.0 * a * c;
    float64 E = - b * b * b + 4 * a * b * c - 8 * a * a * d;
    float64 F =  3.0 * b * b * b * b + 16.0 * a * a * c * c - 16.0 * a * b * b * c + 16.0 * a * a * b * d - 64.0 * a * a * a * e;

    float64 A = D * D - 3.0 * F;
    float64 B = D * F - 9.0 * E * E;
    float64 C = F * F - 3.0 * D * E * E;

    float64 del = B * B - 4.0 * A * C;

    if (__Is_Zero(D, RealError) && __Is_Zero(E, RealError) && __Is_Zero(F, RealError))
    {
        *(RootsBegin + 0) = -b / (4.0 * a);
        *(RootsBegin + 1) = *(RootsBegin + 0); // -(2 * c) / (3 * b)
        *(RootsBegin + 2) = *(RootsBegin + 0); // -(3 * d) / (2 * c)
        *(RootsBegin + 3) = *(RootsBegin + 0); // -(4 * e) / d

        return 1;
    }

    if (!__Is_Zero(D * E * F, RealError) && __Is_Zero(A, p_Error) && __Is_Zero(B, RealError) && __Is_Zero(C, RealError))
    {
        *(RootsBegin + 0) = (-b * D + 9. * E) / (4. * a * D);
        *(RootsBegin + 1) = (-b * D - 3. * E) / (4. * a * D);
        *(RootsBegin + 2) = *(RootsBegin + 1);
        *(RootsBegin + 3) = *(RootsBegin + 1);

        return 2;
    }

    if (__Is_Zero(E, RealError) && __Is_Zero(F, RealError) && !__Is_Zero(D, RealError))
    {
        *(RootsBegin + 0) = (-b + _CSE sqrtc(complex64(D))[0]) / (4. * a);
        *(RootsBegin + 1) = *(RootsBegin + 0);
        *(RootsBegin + 2) = (-b - _CSE sqrtc(complex64(D))[0]) / (4. * a);
        *(RootsBegin + 3) = *(RootsBegin + 2);

        return 3;
    }

    if (!__Is_Zero(A * B * C, RealError) && __Is_Zero(del, RealError))
    {
        *(RootsBegin + 0) = (-b + (2. * A * E) / B + _CSE sqrtc(complex64((2. * B) / A))[0]) / (4. * a);
        *(RootsBegin + 1) = (-b + (2. * A * E) / B - _CSE sqrtc(complex64((2. * B) / A))[0]) / (4. * a);
        *(RootsBegin + 2) = (-b - (2. * A * E) / B) / (4. * a);
        *(RootsBegin + 3) = *(RootsBegin + 2);

        return 4;
    }

    if (del > 0)
    {
        float64 z1 = A * D + 3. * ((-B + _CSE sqrt(del)) / 2.);
        float64 z2 = A * D + 3. * ((-B - _CSE sqrt(del)) / 2.);
        complex64 z = _CSE pow(D, 2.) - D * (_CSE cbrt(z1) + _CSE cbrt(z2)) + pow((_CSE cbrt(z1) + _CSE cbrt(z2)), 2.) - 3. * A;

        *(RootsBegin + 0) = (-b + sgn(E) * _CSE sqrt((D + _CSE cbrt(z1) + _CSE cbrt(z2)) / 3.) + _CSE sqrtc((2. * D - (_CSE cbrt(z1) + _CSE cbrt(z2)) + 2. * _CSE sqrtc(z)[0]) / 3.)[0]) / (4. * a);
        *(RootsBegin + 1) = (-b + sgn(E) * _CSE sqrt((D + _CSE cbrt(z1) + _CSE cbrt(z2)) / 3.) - _CSE sqrtc((2. * D - (_CSE cbrt(z1) + _CSE cbrt(z2)) + 2. * _CSE sqrtc(z)[0]) / 3.)[0]) / (4. * a);
        *(RootsBegin + 2) = (-b - sgn(E) * _CSE sqrt((D + _CSE cbrt(z1) + _CSE cbrt(z2)) / 3.)) / (4. * a) + (_CSE sqrtc((-2. * D + _CSE cbrt(z1) + _CSE cbrt(z2) + 2. * _CSE sqrtc(z)[0]) / 3.)[0] / (4. * a)) * 1i;
        *(RootsBegin + 3) = (-b - sgn(E) * _CSE sqrt((D + _CSE cbrt(z1) + _CSE cbrt(z2)) / 3.)) / (4. * a) - (_CSE sqrtc((-2. * D + _CSE cbrt(z1) + _CSE cbrt(z2) + 2. * _CSE sqrtc(z)[0]) / 3.)[0] / (4. * a)) * 1i;

        return 5;
    }

    if (del < 0)
    {
        float64 tet = arccos((3. * B - 2. * A * D) / (2. * A * _CSE sqrt(A)));
        float64 y1 = (D - 2. * _CSE sqrt(A) * _CSE cos(tet / 3.)) / 3.;
        float64 y2 = (D + _CSE sqrt(A) * (_CSE cos(tet / 3.) + _CSE sqrt(3.) * _CSE sin(tet / 3.))) / 3.;
        float64 y3 = (D + _CSE sqrt(A) * (_CSE cos(tet / 3.) - _CSE sqrt(3.) * _CSE sin(tet / 3.))) / 3.;

        if (__Is_Zero(E, RealError) && D > 0 && F > 0)
        {
            *(RootsBegin + 0) = (-b + _CSE sqrt(D + 2. * _CSE sqrt(F))) / (4. * a);
            *(RootsBegin + 1) = (-b - _CSE sqrt(D + 2. * _CSE sqrt(F))) / (4. * a);
            *(RootsBegin + 2) = (-b + _CSE sqrt(D - 2. * _CSE sqrt(F))) / (4. * a);
            *(RootsBegin + 3) = (-b - _CSE sqrt(D - 2. * _CSE sqrt(F))) / (4. * a);

            return 6;
        }

        if (__Is_Zero(E, RealError) && D < 0 && F > 0)
        {
            *(RootsBegin + 0) = -b / (4. * a) + (_CSE sqrt(-D + 2. * _CSE sqrt(F)) / (4. * a)) * 1i;
            *(RootsBegin + 1) = -b / (4. * a) - (_CSE sqrt(-D + 2. * _CSE sqrt(F)) / (4. * a)) * 1i;
            *(RootsBegin + 2) = -b / (4. * a) + (_CSE sqrt(-D - 2. * _CSE sqrt(F)) / (4. * a)) * 1i;
            *(RootsBegin + 3) = -b / (4. * a) - (_CSE sqrt(-D - 2. * _CSE sqrt(F)) / (4. * a)) * 1i;

            return 7;
        }

        if (__Is_Zero(E, RealError) && F < 0)
        {
            *(RootsBegin + 0) = (-2. * b + _CSE sqrt(2. * D + 2. * _CSE sqrt(A - F))) / (8. * a) + ((_CSE sqrt(-2. * D + 2. * _CSE sqrt(A - F))) / (8. * a)) * 1i;
            *(RootsBegin + 1) = (-2. * b + _CSE sqrt(2. * D + 2. * _CSE sqrt(A - F))) / (8. * a) - ((_CSE sqrt(-2. * D + 2. * _CSE sqrt(A - F))) / (8. * a)) * 1i;
            *(RootsBegin + 2) = (-2. * b - _CSE sqrt(2. * D + 2. * _CSE sqrt(A - F))) / (8. * a) + ((_CSE sqrt(-2. * D + 2. * _CSE sqrt(A - F))) / (8. * a)) * 1i;
            *(RootsBegin + 3) = (-2. * b - _CSE sqrt(2. * D + 2. * _CSE sqrt(A - F))) / (8. * a) - ((_CSE sqrt(-2. * D + 2. * _CSE sqrt(A - F))) / (8. * a)) * 1i;

            return 8;
        }

        if (!__Is_Zero(E, RealError)) // max(y1, y2, y3) == y2
        {
            if (D > 0 && F > 0)
            {
                *(RootsBegin + 0) = (-b + sgn(E) * _CSE sqrt(y1) + (_CSE sqrt(y2) + _CSE sqrt(y3))) / (4. * a);
                *(RootsBegin + 1) = (-b + sgn(E) * _CSE sqrt(y1) - (_CSE sqrt(y2) + _CSE sqrt(y3))) / (4. * a);
                *(RootsBegin + 2) = (-b - sgn(E) * _CSE sqrt(y1) + (_CSE sqrt(y2) - _CSE sqrt(y3))) / (4. * a);
                *(RootsBegin + 3) = (-b - sgn(E) * _CSE sqrt(y1) - (_CSE sqrt(y2) - _CSE sqrt(y3))) / (4. * a);

                return 9;
            }

            else
            {
                *(RootsBegin + 0) = (-b - _CSE sqrt(y2)) / (4. * a) + ((sgn(E) * _CSE sqrt(-y1) + _CSE sqrt(-y3)) / (4. * a)) * 1i;
                *(RootsBegin + 1) = (-b - _CSE sqrt(y2)) / (4. * a) - ((sgn(E) * _CSE sqrt(-y1) + _CSE sqrt(-y3)) / (4. * a)) * 1i;
                *(RootsBegin + 2) = (-b + _CSE sqrt(y2)) / (4. * a) + ((sgn(E) * _CSE sqrt(-y1) - _CSE sqrt(-y3)) / (4. * a)) * 1i;
                *(RootsBegin + 3) = (-b + _CSE sqrt(y2)) / (4. * a) - ((sgn(E) * _CSE sqrt(-y1) - _CSE sqrt(-y3)) / (4. * a)) * 1i;

                return 10;
            }
        }
    }

    return 0;
}

/**
 * @brief Finds the real root of a linear equation.
 * @param Coeffs - A 2-element vector to store coefficients(Sorted by descending powers)</param>
 * @param Roots - The roots are stored in this array
 */
template<typename InputArray, typename OutputArray>
int SolveLinear(const InputArray& Coeffs, OutputArray& Roots)
{
    // Check Coefficents array
    auto itc = std::begin(Coeffs);
    auto endc = std::end(Coeffs);
    if (endc - itc != 2) {throw std::logic_error("Number of Coefficients is not match. (correct size = 2)");}
    // Check Results container
    auto itr = std::begin(Roots);
    auto endr = std::end(Roots);
    if (endr - itr < 1) {throw std::logic_error("Root container is too small. (larger than 1)");}

    return __Solve_Linear_Unchecked(itc, itr);
}

/**
 * @brief Finds the complex roots of a quadratic equation.
 * @param Coeffs - A 3-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 */
template<typename InputArray, typename OutputArray>
int SolveQuadratic(const InputArray& Coeffs, OutputArray& Roots)
{
    // Check Coefficents array
    auto itc = std::begin(Coeffs);
    auto endc = std::end(Coeffs);
    if (endc - itc != 3) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = std::begin(Roots);
    auto endr = std::end(Roots);
    if (endr - itr < 2) {throw std::logic_error("Root container is too small. (larger than 2)");}

    return __Solve_Quadratic_Unchecked(itc, itr);
}

/**
 * @brief Finds the complex roots of a cubic equation.
 * @param Coeffs - A 4-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 * @param p_Error - Negative logarithm of errors, default is 10, means 10^-10
 * @return Formula number used
 */
template<typename InputArray, typename OutputArray>
int SolveCubic(const InputArray& Coeffs, OutputArray& Roots, int64 p_Error = 10)
{
    // Check Coefficents array
    auto itc = std::begin(Coeffs);
    auto endc = std::end(Coeffs);
    if (endc - itc != 4) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = std::begin(Roots);
    auto endr = std::end(Roots);
    if (endr - itr < 3) {throw std::logic_error("Root container is too small. (larger than 2)");}

    return __Solve_Cubic_Unchecked(itc, itr, p_Error);
}

/**
 * @brief Finds the complex roots of a quartic equation.
 * @param Coeffs - A 5-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 * @param p_Error - Negative logarithm of errors, default is 10, means 10^-10
 * @return Formula number used
 */
template<typename InputArray, typename OutputArray>
int SolveQuartic(const InputArray& Coeffs, OutputArray& Roots, int64 p_Error = 10)
{
    // Check Coefficents array
    auto itc = std::begin(Coeffs);
    auto endc = std::end(Coeffs);
    if (endc - itc != 5) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = std::begin(Roots);
    auto endr = std::end(Roots);
    if (endr - itr < 4) {throw std::logic_error("Root container is too small. (larger than 2)");}

    return __Solve_Quartic_Unchecked(itc, itr, p_Error);
}

/* ************************************************************************** *\
   StellarDX: Finding the solving algorithms to a quintic or higher equation
   had troubled mathematicians for more than 300 years.
   Some people in history were tried to derivate the common solving algorithms,
   but failed. In 1926, Su Jiaju published an article which introduced his
   solution and derivation, and discovered a quintic equation solving formula.
   But soon, his theory has been disproved by Hua Luogeng.
   Now, it has been confirmed that solutions of a quintic or higher equation
   can't be expressed as nth roots. In other words, there is no common solving
   formulas for these equations. However, this doesn't mean that they are
   completely unsolvable. Iterations maybe the best method to find numerical
   roots. Here use the Durand–Kerner method to solve this problem.
\* ************************************************************************** */

// Addition:
// In fact, after reading Su Jiaju's paper, Hua Luogeng also made mistakes.
// He extends Su's work and published a root-finding algorithm for solving
// sextic equation. Subsequently, Hua Luogeng tried to disprove Abel's theory
// (Abel has proved that quintic or higher equations have no root-finding
// formulae), but soon he had to admit that Abel's theory is "well-regulated
// and impeccable" So, he began to turn to find errors in Su Jiaju's theory,
// and published his second paper that disproved Su Jiaju's theory and his
// previous theory.

// (If they are correct, this library would contain their algorithms as functions
// to solving quintic and sextic equations, and would name them as "Su Jiaju's
// algorithm" and "Hua Luogeng's algorithm", because the formula for cubic
// functions is named "Fan Shengjin's algorithm" and for quartic function is
// "Shen Tianheng's algorithm", but both of Su and Hua are wrong...)

// A example method to solve quintic equation is provided below by Shen Tianheng(?).
// But it can only solve equations has multiple-roots or in special conditions.
// CAUTION: This function is unverified and untested so maybe unsafe!
template<typename _IterC, typename _IterR>
int __Solve_Polynomial_Unchecked(_IterC CoeffsBegin, _IterC CoeffsEnd, _IterR RootsBegin, struct _SOLVEPOLY_CONFIG Conf);
template<typename _IterC, typename _IterR>
#ifndef _CRT_SECURE_NO_WARNINGS
__declspec(deprecated
("this function can only solve equations has multiple-roots or in special conditions and "
"it is unverified and untested so maybe unsafe! Suggest using \"SolvePoly\" instead of this."))
#endif
int __Solve_Quintic_Function_Fast_Unchecked(_IterC CoeffsBegin, _IterR RootsBegin, float64 p_Error)
{
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    float64 c = *(CoeffsBegin + 2);
    float64 d = *(CoeffsBegin + 3);
    float64 e = *(CoeffsBegin + 4);
    float64 f = *(CoeffsBegin + 5);
    float64 RealError = pow(10, -p_Error);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    float64 L = 2. * pow(b, 2) - 5. * a * c;
    float64 M = 4. * pow(b, 3) - 15. * a * b * c + 25. * pow(a, 2) * d;
    float64 N = 7. * pow(b, 4) + 25. * pow(a, 2) * pow(c, 2) - 35. * a * pow(b, 2) * c + 50. * pow(a, 2) * b * d - 125. * pow(a, 3) * e;
    float64 P = 4. * pow(b, 5) - 25. * a * pow(b, 3) * c + 125. * pow(a, 2) * pow(b, 2) * d - 625. * pow(a, 3) * b * e + 3125. * pow(a, 4) * f;

    float64 G = 4. * pow(L, 3) - 9. * pow(M, 2) + 8 * L * N;
    float64 H = 10. * pow(L, 2) * M - 6. * M * N + L * P;
    float64 J = 4. * pow(L, 4) - 4. * pow(L, 2) * N + 3. * M * P;
    float64 K = pow(M, 4) + pow(N, 3) - M * N * P;

    float64 E = 2. * pow(G, 2) * pow(L, 2) - 2. * pow(G, 2) * N + 3 * G * H * M - 4. * pow(H, 2) * L - G * J * L;
    float64 F = pow(G, 2) * P + 3. * G * J * M - 4. * H * J * L;

    float64 A = pow(F, 2) - 12. * E * L;
    float64 B = 6. * pow(F, 3) - 64. * pow(E, 2) * F * L - 72. * pow(E, 3) * M;
    float64 C = 3. * pow(F, 4) - 24. * pow(E, 2) * pow(F, 2) * L - 48. * pow(E, 3) * F * M - 80. * pow(E, 4) * pow(L, 2);
    float64 D = pow(F, 2) * G + 4. * E * F * H - 4. * pow(E, 2) * J;

    float64 del1 = pow(B, 2) - 4. * A * C, del2 = pow(P, 2) - 4. * pow(L, 5);

    if (__Is_Zero(L, RealError) && __Is_Zero(M, RealError) && __Is_Zero(N, RealError) && __Is_Zero(P, RealError))
    {
        *(RootsBegin + 0) = -(b / (5. * a));
        *(RootsBegin + 1) = *(RootsBegin + 0); // -(c/(2.*b))
        *(RootsBegin + 2) = *(RootsBegin + 0); // -(d/c)
        *(RootsBegin + 3) = *(RootsBegin + 0); // -((2.*e)/d)
        *(RootsBegin + 4) = *(RootsBegin + 0); // -((-5.*f)/e)

        return 1;
    }

    if (!__Is_Zero(L, RealError) && (__Is_Zero(G, RealError) && __Is_Zero(H, RealError) && __Is_Zero(J, RealError)))
    {
        if (__Is_Zero(7. * pow(L, 2) - 4. * N, RealError))
        {
            *(RootsBegin + 0) = -(b * L + 2. * M) / (5. * a * L);
            *(RootsBegin + 1) = (-2. * b * L + M) / (10. * a * L);
            *(RootsBegin + 2) = *(RootsBegin + 1);
            *(RootsBegin + 3) = *(RootsBegin + 1);
            *(RootsBegin + 4) = *(RootsBegin + 1);
        }
        else
        {
            *(RootsBegin + 0) = -(2. * b * L + 9. * M) / (10. * a * L);
            *(RootsBegin + 1) = *(RootsBegin + 0);
            *(RootsBegin + 2) = (-b * L + 3. * M) / (5. * a * L);
            *(RootsBegin + 3) = *(RootsBegin + 2);
            *(RootsBegin + 4) = *(RootsBegin + 2);
        }

        return 2;
    }

    if (!__Is_Zero(G, RealError) && (__Is_Zero(E, RealError) && __Is_Zero(F, RealError)))
    {
        if (__Is_Zero(pow(H, 2) + G * J, RealError))
        {
            *(RootsBegin + 0) = (-2. * B * G - 3. * H + sqrtc(complex64(20. * pow(G, 2) * L - 15. * pow(H, 2)))[0]) / (10. * a * G);
            *(RootsBegin + 1) = (-2. * B * G - 3. * H - sqrtc(complex64(20. * pow(G, 2) * L - 15. * pow(H, 2)))[0]) / (10. * a * G);
            *(RootsBegin + 2) = (-b * G + H) / (5. * a * G);
            *(RootsBegin + 3) = *(RootsBegin + 2);
            *(RootsBegin + 4) = *(RootsBegin + 2);
        }
        else
        {
            *(RootsBegin + 0) = -((b * G + 4. * H) / (5. * a * G));
            *(RootsBegin + 1) = (-b * G + H + sqrtc(complex64(pow(H, 2) + G * J))[0]) / (5. * a * G);
            *(RootsBegin + 2) = (-b * G + H - sqrtc(complex64(pow(H, 2) + G * J))[0]) / (5. * a * G);
            *(RootsBegin + 3) = *(RootsBegin + 1);
            *(RootsBegin + 4) = *(RootsBegin + 2);
        }

        return 3;
    }

    if (!__Is_Zero(E, RealError) && __Is_Zero(D, RealError))
    {
        *(RootsBegin + 0) = -((2. * b * E + F) / (10. * a * E));
        *(RootsBegin + 1) = *(RootsBegin + 0);

        if (del1 > 0)
        {
            float64 y1 = 10. * A * F + 15. * ((-B + sqrt(del1)) / 2.);
            float64 y2 = 10. * A * F + 15. * ((-B - sqrt(del1)) / 2.);

            *(RootsBegin + 2) = (-6. * b * E + 2. * F - (cbrt(y1) + cbrt(y2))) / (30. * a * E);
            *(RootsBegin + 3) = ((-12. * b * E + 4. * F + (cbrt(y1) + cbrt(y2))) / (60. * a * E)) + ((sqrt(3) * (cbrt(y1) - cbrt(y2))) / (60. * a * E)) * 1i;
            *(RootsBegin + 4) = ((-12. * b * E + 4. * F + (cbrt(y1) + cbrt(y2))) / (60. * a * E)) - ((sqrt(3) * (cbrt(y1) - cbrt(y2))) / (60. * a * E)) * 1i;
        }
        else
        {
            float64 tet = arccos((3. * B - 4. * A * F) / (2. * A * sqrt(-5. * A)));

            *(RootsBegin + 2) = (-3. * b * E + F - sqrt(-5. * A) * cos(tet / 3.)) / (15. * a * E);
            *(RootsBegin + 3) = (-6. * b * E + 2. * F + sqrt(-5. * A) * (cos(tet / 3.) + sqrt(3) * sin(tet / 3.))) / (30. * a * E);
            *(RootsBegin + 4) = (-6. * b * E + 2. * F + sqrt(-5. * A) * (cos(tet / 3.) - sqrt(3) * sin(tet / 3.))) / (30. * a * E);
        }

        return 4;
    }

    if (!__Is_Zero(D, RealError) && (__Is_Zero(M, RealError) && __Is_Zero(N, RealError)) && del2 > 0)
    {
        float64 y1 = (P + sqrt(del2)) / 2.;
        float64 y2 = (P - sqrt(del2)) / 2.;

        *(RootsBegin + 0) = (-b - (yroot(y1, 5) + yroot(y2, 5))) / (5. * a);
        *(RootsBegin + 1) = ((-b + ((1. - sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) + (((sqrt(10. + 2. * sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) * 1i;
        *(RootsBegin + 2) = ((-b + ((1. - sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) - (((sqrt(10. + 2. * sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) * 1i;
        *(RootsBegin + 3) = ((-b + ((1. + sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) + (((sqrt(10. - 2. * sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) * 1i;
        *(RootsBegin + 4) = ((-b + ((1. + sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) - (((sqrt(10. - 2. * sqrt(5)) / 4.) * (yroot(y1, 5) + yroot(y2, 5))) / (5. * a)) * 1i;

        return 5;
    }

    if (!__Is_Zero(D, RealError) && (__Is_Zero(M, RealError) && __Is_Zero(N, RealError)) && del2 < 0)
    {
        float64 tet = arccos(P / (2. * pow(L, 2) * sqrt(L)));

        *(RootsBegin + 0) = (-b - 2. * sqrt(L) * cos(tet / 5.)) / (5. * a);
        *(RootsBegin + 1) = (-b + sqrt(L) * (((1. - sqrt(5)) / 2.) * cos(tet / 5.) + (sqrt(10. + 2. * sqrt(5)) / 2.) * sin(tet / 5.))) / (5. * a);
        *(RootsBegin + 2) = (-b + sqrt(L) * (((1. - sqrt(5)) / 2.) * cos(tet / 5.) - (sqrt(10. + 2. * sqrt(5)) / 2.) * sin(tet / 5.))) / (5. * a);
        *(RootsBegin + 3) = (-b + sqrt(L) * (((1. + sqrt(5)) / 2.) * cos(tet / 5.) + (sqrt(10. - 2. * sqrt(5)) / 2.) * sin(tet / 5.))) / (5. * a);
        *(RootsBegin + 4) = (-b + sqrt(L) * (((1. + sqrt(5)) / 2.) * cos(tet / 5.) - (sqrt(10. - 2. * sqrt(5)) / 2.) * sin(tet / 5.))) / (5. * a);

        return 6;
    }

    if (!__Is_Zero(D, RealError) && !__Is_Zero(M * N, RealError) && (__Is_Zero(L, RealError) && __Is_Zero(K, RealError)))
    {
        *(RootsBegin + 0) = (-b - yroot(pow(N, 2) / M, 5) - yroot(pow(M, 3) / N, 5)) / (5. * a);
        *(RootsBegin + 1) = (-b + ((1 - sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + ((1 + sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) + ((sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + (sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) * 1i;
        *(RootsBegin + 2) = (-b + ((1 - sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + ((1 + sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) - ((sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + (sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) * 1i;
        *(RootsBegin + 3) = (-b + ((1 + sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + ((1 - sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) + ((sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) - (sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) * 1i;
        *(RootsBegin + 4) = (-b + ((1 + sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) + ((1 - sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) - ((sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(pow(N, 2) / M, 5) - (sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(pow(M, 3) / N, 5)) * 1i;

        return 7;
    }

    return -(int)__Solve_Polynomial_Unchecked(CoeffsBegin, CoeffsBegin + 5, RootsBegin, { .P_ERROR = (float64)p_Error });
}

struct _SOLVEPOLY_CONFIG
{
    float64          P_ERROR     = 15;
    float64          MAXITER_LOG = 3;
    complex64        BASE        = 0.4 + 0.9i;
};

template<typename iterator> requires std::random_access_iterator<iterator>
_NODISCARD constexpr iterator max(iterator begin, iterator end)noexcept;

template<typename _IterC, typename _IterR>
int __Solve_Polynomial_Unchecked(_IterC CoeffsBegin, _IterC CoeffsEnd, _IterR RootsBegin, _SOLVEPOLY_CONFIG Conf)
{
    std::vector<float64> _Coeffs(CoeffsBegin, CoeffsEnd);
    std::vector<complex64> Roots;

    if (_Coeffs[0] == 0 || _Coeffs.empty())
    {
        throw std::logic_error("Highest power of polynomial can't be zero.");
    }
    float64 Base = _Coeffs[0];
    for (size_t i = 0; i < _Coeffs.size(); i++)
    {
        _Coeffs[i] /= Base;
    }

    // The clue to the method now is to combine the fixed-point iteration for P with similar iterations for Q, R, S into a simultaneous iteration for all roots.
    // Initialize p, q, r, s:
    for (size_t i = 0; i < _Coeffs.size() - 1; i++)
    {
        Roots.push_back(_CSE powc(Conf.BASE, complex64((float64)i)));
    }

    // Make the substitutions
    auto f = [&](complex64 x)->complex64
    {
        complex64 SIG = 0;
        for (size_t i = 0; i < _Coeffs.size(); i++)
        {
            SIG += _Coeffs[i] * _CSE powc(x, (complex64)(_Coeffs.size() - i - 1.));
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

    while (it < _CSE pow(10, Conf.MAXITER_LOG))
    {
        std::vector<float64> Diffs;
        for (size_t i = 0; i < _Coeffs.size() - 1; i++)
        {
            complex64 Diff = f(Roots[i]) / g(Roots[i], i);
            Diffs.push_back(abs(Diff));
            Roots[i] -= Diff;
        }

        // Re-iterate until the numbers p, q, r, s essentially stop changing relative to the desired precision.
        // They then have the values P, Q, R, S in some order and in the chosen precision. So the problem is solved.
        auto MaxDiff = _CSE max(Diffs.begin(), Diffs.end());
        if (-pow(10, (float64)-Conf.P_ERROR) < *MaxDiff && *MaxDiff < pow(10, (float64)-Conf.P_ERROR))
        {
            break;
        }

        ++it;
    }

    for (size_t i = 0; i < Roots.size(); ++i)
    {
        *(RootsBegin + i) = Roots[i];
    }

    return it;
}

/**
 * @brief finds complex roots of a polynomial using Durand-Kerner method.
 * @link http://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
 * @param Coeffs - A array to store coefficients
 * @param Roots - The roots are stored in this array
 * @param Conf - Function configs
 * @return Number of iteration
 */
template<typename InputArray, typename OutputArray>
uint64 SolvePoly(const InputArray& Coeffs, OutputArray& Roots, _SOLVEPOLY_CONFIG Conf = _SOLVEPOLY_CONFIG())
{
    // Check Coefficents array
    auto itc = std::begin(Coeffs);
    auto endc = std::end(Coeffs);
    auto CoeffsSize = endc - itc;
    // Check Results container
    auto itr = std::begin(Roots);
    auto endr = std::end(Roots);
    if (endr - itr < CoeffsSize - 1) {throw std::logic_error("Root container is too small.");}

    return __Solve_Polynomial_Unchecked(itc, endc, itr, Conf);
}

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
