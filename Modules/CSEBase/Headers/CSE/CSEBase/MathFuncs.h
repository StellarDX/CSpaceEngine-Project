/************************************************************
  CSpaceEngine Custom math functions.
  Copyright (C) StellarDX Astronomy
  SPDX-License-Identifier: GPL-2.0
***********************************************************/

#pragma once

#ifndef _CSE_MATH_BASE
#define _CSE_MATH_BASE

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/GLTypes.h>
#include <array>
#include <vector>
#include <complex>

/* ************************************************************************** *\
   genTypes in Templates can be multiple data types:
       genType  = float64, complex64, vec2, vec3, vec4
       genIType = int64, ivec2, ivec3, ivec4
       genUType = uint64, uvec2, uvec3, uvec4
       genBType = bool, bvec2, bvec3, bvec4
   and no genDTypes because all the types are already in 64-bit.
\* ************************************************************************** */

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

_CSE_BEGIN

using __Float64 = IEEE754_Dbl64;
using complex64 = std::complex<float64>;
using std::literals::operator""i;

#define CSE_POSITIVE_ZERO POS_ZERO_DOUBLE
#define CSE_NEGATIVE_ZERO NEG_ZERO_DOUBLE
#define CSE_POSITIVE_INF  POS_INF_DOUBLE
#define CSE_NEGATIVE_INF  NEG_INF_DOUBLE
#define CSE_NAN           BIG_NAN_DOUBLE

/****************************************************************************************\
*                                          E X P                                         *
\****************************************************************************************/

_EXTERN_C
// Implemention by FSF
extern const unsigned long long __Expf64_table[];
__Float64 __cdecl __IEEE754_EXPF64(__Float64 _X);
_END_EXTERN_C

/**
 * @details Returns the natural exponentiation of x. i.e., e^x.
 */
float64 __cdecl exp(float64 _X);

complex64 __cdecl expc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl exp(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE exp(_X[i]); }
    return _X;
}

/****************************************************************************************\
*                                          L O G                                         *
\****************************************************************************************/

_EXTERN_C
// Implemention by FSF and Stephen L. Moshier
extern const double __Lnf64_table[]; // For Double
__Float64 __cdecl __IEEE754_LOGF128_CF64(__Float64 _X); // quaduple precision function, but can be used for double precision.
__Float64 __cdecl __IEEE754_LNF64(__Float64 _X); // natural logarithm for double precision
_END_EXTERN_C

/**
 * @details Returns the logarithm of x, i.e. the value y which satisfies x=base^y.
 * @overload lb(x) (Scrapped feature)returns the binary logarithm of x.
 * @overload ln(x) returns the natural logarithm of x.
 * @overload log(x) returns the base 10 logarithm of x, i.e. the value y which satisfies x=10^y.
 * @param _X Specify the value of which to take the logarithm.
 * @param _Base - Specify the base value
 */
float64 __cdecl log(float64 _X, float64 _Base); // log x base on y
float64 __cdecl lb(float64 _X); // Unused
float64 __cdecl ln(float64 _X);
float64 __cdecl log(float64 _X);

complex64 __cdecl lbc(complex64 _X, int64 K_OFFSET = 0); // Unused
complex64 __cdecl lnc(complex64 _X, int64 K_OFFSET = 0);
complex64 __cdecl logc(complex64 _X, int64 K_OFFSET = 0);
complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl lb(genType _X); // Unused

template<typename genType> requires vecType<genType>
genType __cdecl ln(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE ln(_X[i]); }
    return _X;
}

template<typename genType> requires vecType<genType>
genType __cdecl log(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE log(_X[i]); }
    return _X;
}

template<typename genType> requires vecType<genType>
genType __cdecl log(genType _X, float64 _Base)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE log(_X[i], _Base); }
    return _X;
}

template<typename genType> requires vecType<genType>
genType __cdecl log(genType _X, genType _Base)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE log(_X[i], _Base[i]); }
    return _X;
}

/****************************************************************************************\
*                                    P O W E R                                           *
\****************************************************************************************/

_EXTERN_C
// Power Implemention by FSF
extern const struct __GNU_Table_Powlnt __Powf64_ln_table[];
int CheckInt(uint64_t iy);
__Float64 __cdecl __IEEE754_POWF64(__Float64 _X, __Float64 _Power);
// Square root Implemention by IBM
extern const double __IBM_inroot_table[128];
__Float64 __cdecl __IBM_SQRTF64(__Float64 _X);
// Cube root Implemention by FSF
__Float64 __cdecl __IEEE754_CBRTF64(__Float64 _X);
_END_EXTERN_C

/**
 * @details Returns the value of x raised to the y power, i.e. x^y.
 * @param _X - Specify the value to raise to the power y.
 * @param _Power - Specify the power to which to raise x.
 */
float64 __cdecl pow(float64 _X, float64 _Power);

complex64 __cdecl powc(complex64 _X, complex64 _Power, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl pow(genType _X, float64 _Power)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE pow(_X[i], _Power); }
    return _X;
}

template<typename genType> requires vecType<genType>
genType __cdecl pow(genType _X, genType _Power)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE pow(_X[i], _Power[i]); }
    return _X;
}

template<typename _Ty, size_t _Size>
_GL GBasicMatrix<_Ty, _Size, _Size> __cdecl pow(_GL GBasicMatrix<_Ty, _Size, _Size> _A, uint64 _Power)
{
    _STL_VERIFY(_Power > 0, "matrix power can't be zero.");
    _GL GBasicMatrix<_Ty, _Size, _Size> m0;
    for (size_t i = 0; i < _Power; i++)
    {
        m0 = _A * _A;
    }
    return m0;
}

/**
 * @details Returns the square root of x, i.e. the value √x.
 */
float64 __cdecl sqrt(float64 _X);

std::array<complex64, 2> __cdecl sqrtc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl sqrt(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE sqrt(_X[i]); }
    return _X;
}

/**
 * @details Returns the inverse of the square root of x, i.e. the value 1/√x
 */
float64 __cdecl inversesqrt(float64 _X);

complex64 __cdecl inversesqrtc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl inversesqrt(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE inversesqrt(_X[i]); }
    return _X;
}

/**
 * @details Returns the cube root of x, i.e. the value 3√x.
 */
float64 __cdecl cbrt(float64 _X);
std::array<complex64, 3> __cdecl cbrtc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl cbrt(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE cbrt(_X[i]); }
    return _X;
}

/**
 * @details Returns the nth-root of x, i.e. the value n√x.
 * @param _X - Specify the value to find root.
 * @param _Expo - Specify the value of exponential of root.
 */
float64 __cdecl yroot(float64 _X, float64 _Expo);
std::vector<complex64> __cdecl yrootc(complex64 _X, complex64 _Expo, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl yroot(genType _X, float64 _Expo)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE yroot(_X[i], _Expo); }
    return _X;
}

template<typename genType> requires vecType<genType>
genType __cdecl yroot(genType _X, genType _Expo)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE yroot(_X[i], _Expo[i]); }
    return _X;
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

/**
 * @details Converts degrees into radians.
 */
float64 __cdecl Radians(float64 _Deg);

/**
 * @details converts radians into degrees.
 */
float64 __cdecl Degrees(float64 _Rad);

// Trigonometric functions for Degrees Implemented by OpenCV
extern const float64 __CV_SinCos_Tab[];
float64 __cdecl __CV_SIN_DEGREES(float64 _X);
float64 __cdecl __CV_COS_DEGREES(float64 _X);
float64 __cdecl __CV_TAN_DEGREES(float64 _X);

_EXTERN_C
// Trigonometric functions for Radians Implemented by IBM
extern const union __IBM_Table_Type440 __IBM_sincostab;
extern const __Float64 __IBM_TANF64_XFGTBL[186][4];
__Float64 __cdecl __IBM_SINF64(__Float64 x);
__Float64 __cdecl __IBM_COSF64(__Float64 x);
__Float64 __cdecl __IBM_TANF64(__Float64 x);
_END_EXTERN_C

/**
 * @details The standard trigonometric sine function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl sin(float64 _X);

complex64 __cdecl sinc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl sin(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE sin(_X[i]); }
    return _X;
}

/**
 * @details The standard trigonometric cosine function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl cos(float64 _X);

complex64 __cdecl cosc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl cos(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE cos(_X[i]); }
    return _X;
}

/**
 * @details The standard trigonometric tangent function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl tan(float64 _X);

complex64 __cdecl tanc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl tan(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE tan(_X[i]); }
    return _X;
}

/**
 * @details The standard trigonometric cotangent function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl ctg(float64 _X);

complex64 __cdecl ctgc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl ctg(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE ctg(_X[i]); }
    return _X;
}

/**
 * @details The standard trigonometric secant function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl sec(float64 _X);

complex64 __cdecl secc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl sec(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE sec(_X[i]); }
    return _X;
}

/**
 * @details The standard trigonometric cosecant function(Real number based on degrees, Complex based on radians).
 */
float64 __cdecl csc(float64 _X);

complex64 __cdecl cscc(complex64 _X);

template<typename genType> requires vecType<genType>
genType __cdecl csc(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE csc(_X[i]); }
    return _X;
}

/**
 * @details returns the axes of a two-dimensional XYZ coordinates divide the plane into four infinite regions.
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
int64 __cdecl Quadrant(float64 _X);

///////////////////////////////////// INVERSE ////////////////////////////////////

// Use degrees for real number functions, radians for complex functions.

_EXTERN_C
// Inverse Trigonometric functions for degrees and Radians Implemented by IBM and Stephen L. Moshier
extern const union __IBM_Table_Type2568 __IBM_asncs_table;
extern const __Float64 __ArctanF128_table_deg[];
extern const __Float64 __ArctanF128_table_rad[];
__Float64 __cdecl __IBM_ASINF64(__Float64 x, bool RadUnit = 0);
__Float64 __cdecl __IBM_ACOSF64(__Float64 x, bool RadUnit = 0);
__Float64 __cdecl __IEEE754_ATANF128_CF64(__Float64 x, bool RadUnit = 0);
_END_EXTERN_C

/**
 * @details Arc Sine. Returns the angle whose trigonometric sine is x. The range of values returned by asin is [-90, 90].
 */
float64 __cdecl arcsin(float64 _X);

complex64 __cdecl arcsinc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arcsin(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arcsin(_X[i]); }
    return _X;
}

/**
 * @details Arc cosine. Returns the angle whose trigonometric cosine is x. The range of values returned by acos is [0, 180].
 */
float64 __cdecl arccos(float64 _X);

complex64 __cdecl arccosc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arccos(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arccos(_X[i]); }
    return _X;
}

/**
 * @details Arc tangent. Returns the angle whose tangent is x. The value returned in this case is in the range [-90, 90].
 */
float64 __cdecl arctan(float64 _X);

complex64 __cdecl arctanc(complex64 _X, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arctan(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arctan(_X[i]); }
    return _X;
}

float64 __cdecl arcctg(float64 _X);

complex64 __cdecl arcctgc(complex64 _X, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arcctg(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arcctg(_X[i]); }
    return _X;
}

float64 __cdecl arcsec(float64 _X);

complex64 __cdecl arcsecc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arcsec(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arcsec(_X[i]); }
    return _X;
}

float64 __cdecl arccsc(float64 _X);

complex64 __cdecl arccscc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

template<typename genType> requires vecType<genType>
genType __cdecl arccsc(genType _X)
{
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = _CSE arccsc(_X[i]); }
    return _X;
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

float64 __cdecl arccrd(float64 _X, float64 _Radius = 1);

float64 __cdecl arcsiv(float64 _X);
float64 __cdecl arcvcs(float64 _X);
float64 __cdecl arccvs(float64 _X);
float64 __cdecl arccvc(float64 _X);

float64 __cdecl archvs(float64 _X);
float64 __cdecl archvc(float64 _X);
float64 __cdecl archcv(float64 _X);
float64 __cdecl archcc(float64 _X);

float64 __cdecl arcexs(float64 _X);
float64 __cdecl arcexc(float64 _X);

#endif

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

    float64 del = _CSE pow(b, 2) - 4.0 * a * c;

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
    float64 a = *(CoeffsBegin + 0);
    float64 b = *(CoeffsBegin + 1);
    float64 c = *(CoeffsBegin + 2);
    float64 d = *(CoeffsBegin + 3);
    float64 RealError = pow(10, -p_Error);
    if (a == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }

    float64 A = _CSE pow(b, 2) - 3.0 * a * c;
    float64 B = b * c - 9.0 * a * d;
    float64 C = _CSE pow(c, 2) - 3.0 * b * d;

    float64 del = _CSE pow(B, 2) - 4.0 * A * C;

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

    float64 D = 3.0 * _CSE pow(b, 2.0) - 8.0 * a * c;
    float64 E = -_CSE pow(b, 3.0) + 4 * a * b * c - 8 * _CSE pow(a, 2.0) * d;
    float64 F = 3.0 * _CSE pow(b, 4.0) + 16.0 * _CSE pow(a, 2.0) * _CSE pow(c, 2.0) - 16.0 * a * _CSE pow(b, 2.0) * c + 16.0 * _CSE pow(a, 2.0) * b * d - 64.0 * _CSE pow(a, 3.0) * e;
    float64 A = _CSE pow(D, 2.0) - 3.0 * F;
    float64 B = D * F - 9.0 * _CSE pow(E, 2.0);
    float64 C = _CSE pow(F, 2.0) - 3.0 * D * _CSE pow(E, 2.0);

    float64 del = _CSE pow(B, 2) - 4.0 * A * C;

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

        if
        (
            !__Is_Zero(E, RealError)
            //&& max(y1, y2, y3) == y2
        )
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

    for (ssize_t i = 0; i < Roots.size(); ++i)
    {
        *(RootsBegin + i) = Roots[i];
    }

    return it;
}

/**
 * @details Finds the real root of a linear equation.
 * @param Coeffs - A 2-element vector to store coefficients(Sorted by descending powers)</param>
 * @param Roots - The roots are stored in this array
 */
template<typename InputArray, typename OutputArray>
int SolveLinear(const InputArray& Coeffs, OutputArray& Roots)
{
    // Check Coefficents array
    auto itc = Coeffs.begin();
    auto endc = Coeffs.end();
    if (endc - itc != 2) {throw std::logic_error("Number of Coefficients is not match. (correct size = 2)");}
    // Check Results container
    auto itr = Roots.begin();
    auto endr = Roots.end();
    if (endr - itr < 1) {throw std::logic_error("Root container is too small. (larger than 1)");}

    return __Solve_Linear_Unchecked(itc, itr);
}

/**
 * @details Finds the complex roots of a quadratic equation.
 * @param Coeffs - A 3-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 */
template<typename InputArray, typename OutputArray>
int SolveQuadratic(const InputArray& Coeffs, OutputArray& Roots)
{
    // Check Coefficents array
    auto itc = Coeffs.begin();
    auto endc = Coeffs.end();
    if (endc - itc != 3) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = Roots.begin();
    auto endr = Roots.end();
    if (endr - itr < 2) {throw std::logic_error("Root container is too small. (larger than 2)");}

    return __Solve_Quadratic_Unchecked(itc, itr);
}

/**
 * @details Finds the complex roots of a cubic equation.
 * @param Coeffs - A 4-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 * @param p_Error - Negative logarithm of errors, default is 10, means 10^-10
 * @return Formula number used
 */
template<typename InputArray, typename OutputArray>
int SolveCubic(const InputArray& Coeffs, OutputArray& Roots, int64 p_Error = 10)
{
    // Check Coefficents array
    auto itc = Coeffs.begin();
    auto endc = Coeffs.end();
    if (endc - itc != 4) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = Roots.begin();
    auto endr = Roots.end();
    if (endr - itr < 3) {throw std::logic_error("Root container is too small. (larger than 2)");}

    return __Solve_Cubic_Unchecked(itc, itr, p_Error);
}

/**
 * @details Finds the complex roots of a quartic equation.
 * @param Coeffs - A 5-element vector to store coefficients(Sorted by descending powers)
 * @param Roots - The roots are stored in this array
 * @param p_Error - Negative logarithm of errors, default is 10, means 10^-10
 * @return Formula number used
 */
template<typename InputArray, typename OutputArray>
int SolveQuartic(const InputArray& Coeffs, OutputArray& Roots, int64 p_Error = 10)
{
    // Check Coefficents array
    auto itc = Coeffs.begin();
    auto endc = Coeffs.end();
    if (endc - itc != 5) {throw std::logic_error("Number of Coefficients is not match. (correct size = 3)");}
    // Check Results container
    auto itr = Roots.begin();
    auto endr = Roots.end();
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
// He extends Su's work and published a root-finding algorithm for solving sextic equation.
// Subsequently, Hua Luogeng tried to disprove Abel's theory (Abel has proved that
// quintic or higher equations have no root-finding formulae), but soon he had to admit
// that Abel's theory is "well-regulated and impeccable"
// So, he began to turn to find errors in Su Jiaju's theory, and published his second
// paper that disproved Su Jiaju's theory and his previous theory.

// (If they are correct, this library would contain their algorithms as functions
// to solving quintic and sextic equations, and would name them as "Su Jiaju's algorithm"
// and "Hua Luogeng's algorithm", because the formula for cubic functions is named
// "Fan Shengjin's algorithm" and for quartic function is "Shen Tianheng's algorithm",
// but both of Su and Hua are wrong...)

// A example method to solve quintic equation is written below by Shen Tianheng(?).
// But it can only solve equations has multiple-roots or equations in special conditions.
template<typename _IterC, typename _IterR>
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
    float64 K = pow(M, 4) * pow(N, 3) * M * N * P;

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

        *(RootsBegin + 0) = (-b - (yroot(5, y1) + yroot(5, y2))) / (5. * a);
        *(RootsBegin + 1) = ((-b + ((1. - sqrt(5)) / 4.) * (yroot(5, y1) + yroot(5, y2))) / (5. * a)) + (((sqrt(10. + 2. * sqrt(5)) / 4.) * (yroot(5, y1) - yroot(5, y2))) / (5. * a)) * 1i;
        *(RootsBegin + 2) = ((-b + ((1. - sqrt(5)) / 4.) * (yroot(5, y1) + yroot(5, y2))) / (5. * a)) - (((sqrt(10. + 2. * sqrt(5)) / 4.) * (yroot(5, y1) - yroot(5, y2))) / (5. * a)) * 1i;
        *(RootsBegin + 3) = ((-b + ((1. + sqrt(5)) / 4.) * (yroot(5, y1) + yroot(5, y2))) / (5. * a)) + (((sqrt(10. - 2. * sqrt(5)) / 4.) * (yroot(5, y1) - yroot(5, y2))) / (5. * a)) * 1i;
        *(RootsBegin + 4) = ((-b + ((1. + sqrt(5)) / 4.) * (yroot(5, y1) + yroot(5, y2))) / (5. * a)) - (((sqrt(10. - 2. * sqrt(5)) / 4.) * (yroot(5, y1) - yroot(5, y2))) / (5. * a)) * 1i;

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
        *(RootsBegin + 0) = (-b - yroot(5, pow(N, 2) / M) - yroot(5, pow(M, 3) / N)) / (5. * a);
        *(RootsBegin + 1) = (-b + ((1 - sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + ((1 + sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) + ((sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + (sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) * 1i;
        *(RootsBegin + 2) = (-b + ((1 - sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + ((1 + sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) - ((sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + (sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) * 1i;
        *(RootsBegin + 3) = (-b + ((1 + sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + ((1 - sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) + ((sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) - (sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) * 1i;
        *(RootsBegin + 4) = (-b + ((1 + sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) + ((1 - sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) - ((sqrt(10. - 2. * sqrt(5)) / 4.) * yroot(5, pow(N, 2) / M) - (sqrt(10. + 2. * sqrt(5)) / 4.) * yroot(5, pow(M, 3) / N)) * 1i;

        return 7;
    }

    return -(int)__Solve_Polynomial_Unchecked(CoeffsBegin, CoeffsBegin + 5, RootsBegin, { .P_ERROR = (float64)p_Error });
}

/**
 * @details finds complex roots of a polynomial using Durand-Kerner method.
 * @ref http://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
 * @param Coeffs - A array to store coefficients
 * @param Roots - The roots are stored in this array
 * @param Conf - Function configs
 * @return Number of iteration
 */
template<typename InputArray, typename OutputArray>
uint64 SolvePoly(const InputArray& Coeffs, OutputArray& Roots, _SOLVEPOLY_CONFIG Conf = _SOLVEPOLY_CONFIG())
{
    // Check Coefficents array
    auto itc = Coeffs.begin();
    auto endc = Coeffs.end();
    auto CoeffsSize = endc - itc;
    // Check Results container
    auto itr = Roots.begin();
    auto endr = Roots.end();
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

#endif
