/************************************************************
  CSpaceEngine Internal algorithms.
***********************************************************/

/*
    CSpaceEngine Astronomy Library
    Copyright (C) StellarDX Astronomy.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _CSE_MATH_ALGORITHMS
#define _CSE_MATH_ALGORITHMS

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/GLTypes.h"
#include "CSE/Base/MathFuncs.h"
#include <complex>
#include <initializer_list>

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

// Don't let win32api windef.h define min and max as macros
// if included after this header.
#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#define _USE_CSE_ALGORITHMS \
using cse::abs;\
using cse::sgn;\
using cse::floor;\
using cse::ceil;\
using cse::FractionalPart;\
using cse::fract;\
using cse::mod;\
using cse::min;\
using cse::max;\
using cse::clamp;\
using cse::mix;\
using cse::step;\
using cse::smoothstep;\
using cse::isinf;\
using cse::isnan;\
using cse::isfinite;\
using cse::FPClassify;\


_CSE_BEGIN

/**
 * @brief Returns the absolute value of x.
 */
_NODISCARD float64 abs(float64 _Xx)noexcept;
_NODISCARD int64 abs(int64 _Xx)noexcept;
_NODISCARD float64 abs(complex64 _Xx)noexcept;

template<std::size_t N>
fvec<N> __cdecl abs(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE abs(_X[i]))
}

template<std::size_t N>
ivec<N> __cdecl abs(ivec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE abs(_X[i]))
}

/**
 * @brief Returns -1.0 if x is less than 0.0, 0.0 if x is equal to 0.0, and +1.0 if x is greater than 0.0.
 */
_NODISCARD float64 sgn(float64 _Xx)noexcept;
_NODISCARD int64 sgn(int64 _Xx)noexcept;

template<std::size_t N>
fvec<N> __cdecl sgn(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sgn(_X[i]))
}

template<std::size_t N>
ivec<N> __cdecl sgn(ivec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE sgn(_X[i]))
}

/**
 * @brief Returns a value equal to the nearest integer that is less than or equal to x.
 */
_NODISCARD float64 floor(float64 _Xx)noexcept;

template<std::size_t N>
fvec<N> __cdecl floor(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE floor(_X[i]))
}

/**
 * @brief Returns a value equal to the nearest integer that is greater than or equal to x.
 */
_NODISCARD float64 ceil(float64 x)noexcept;

template<std::size_t N>
fvec<N> __cdecl ceil(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ceil(_X[i]))
}

/**
 * @brief Returns the fractional part of x.
 */
_NODISCARD float64 FractionalPart(float64 _Xx)noexcept;

template<std::size_t N>
fvec<N> __cdecl FractionalPart(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE FractionalPart(_X[i]))
}

_NODISCARD float64 fract(float64 _Xx)noexcept;

template<std::size_t N>
fvec<N> __cdecl fract(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE fract(_X[i]))
}

/**
 * @brief Returns the value of x modulo y.
 */
_NODISCARD float64 mod(float64 _Left, float64 _Right)noexcept;

template<std::size_t N>
fvec<N> __cdecl mod(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE mod(_Left[i], _Right))
}

template<std::size_t N>
fvec<N> __cdecl mod(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE mod(_Left[i], _Right[i]))
}

/**
 * @brief returns the minimum of the two parameters or array.
 */
_NODISCARD constexpr float64 min(float64 _Left, float64 _Right) noexcept(noexcept(_Left < _Right));
_NODISCARD constexpr int64 min(int64 _Left, int64 _Right) noexcept(noexcept(_Left < _Right));
_NODISCARD constexpr uint64 min(uint64 _Left, uint64 _Right) noexcept(noexcept(_Left < _Right));

template<std::size_t N>
fvec<N> __cdecl min(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE min(_Left[i], _Right))
}

template<std::size_t N>
fvec<N> __cdecl min(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE min(_Left[i], _Right[i]))
}

template<std::size_t N>
ivec<N> __cdecl min(ivec<N> _Left, int64 _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE min(_Left[i], _Right))
}

template<std::size_t N>
ivec<N> __cdecl min(ivec<N> _Left, ivec<N> _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE min(_Left[i], _Right[i]))
}

template<std::size_t N>
uvec<N> __cdecl min(uvec<N> _Left, uint64 _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE min(_Left[i], _Right))
}

template<std::size_t N>
uvec<N> __cdecl min(uvec<N> _Left, uvec<N> _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE min(_Left[i], _Right[i]))
}

// Iterator support
template<typename iterator> requires std::random_access_iterator<iterator>
_NODISCARD constexpr iterator min(iterator begin, iterator end)noexcept
{
    if (begin >= end){return end;}
    auto It = begin;
    while (begin != end)
    {
        It = *begin < *It ? begin : It;
        ++begin;
    }
    return It;
}

template<typename genType>
_NODISCARD constexpr genType min(std::initializer_list<genType> _Ilist)noexcept
{
    return *(_CSE min(_Ilist.begin(), _Ilist.end()));
}

/**
 * @brief returns the maximum of the two parameters or array.
 */
_NODISCARD constexpr float64 max(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left));
_NODISCARD constexpr int64 max(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left));
_NODISCARD constexpr uint64 max(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left));

template<std::size_t N>
fvec<N> __cdecl max(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE max(_Left[i], _Right))
}

template<std::size_t N>
fvec<N> __cdecl max(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE max(_Left[i], _Right[i]))
}

template<std::size_t N>
ivec<N> __cdecl max(ivec<N> _Left, int64 _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE max(_Left[i], _Right))
}

template<std::size_t N>
ivec<N> __cdecl max(ivec<N> _Left, ivec<N> _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE max(_Left[i], _Right[i]))
}

template<std::size_t N>
uvec<N> __cdecl max(uvec<N> _Left, uint64 _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE max(_Left[i], _Right))
}

template<std::size_t N>
uvec<N> __cdecl max(uvec<N> _Left, uvec<N> _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE max(_Left[i], _Right[i]))
}

// Iterator support
template<typename iterator> requires std::random_access_iterator<iterator>
_NODISCARD constexpr iterator max(iterator begin, iterator end)noexcept
{
    if (begin >= end){return end;}

    auto It = begin;
    while (begin != end)
    {
        It = *begin > *It ? begin : It;
        ++begin;
    }

    return It;
}

template<typename genType>
_NODISCARD constexpr genType max(std::initializer_list<genType> _Ilist)noexcept
{
    return *(_CSE max(_Ilist.begin(), _Ilist.end()));
}

/**
 * @brief Returns the value of x constrained to the range minVal to maxVal.
 * @param x - Specify the value to constrain.
 * @param MinVal - Specify the lower end of the range into which to constrain x.
 * @param MaxVal - Specify the upper end of the range into which to constrain x.
 */
float64 clamp(float64 x, float64 MinVal, float64 MaxVal);
int64 clamp(int64 x, int64 MinVal, int64 MaxVal);
uint64 clamp(uint64 x, uint64 MinVal, uint64 MaxVal);

template<std::size_t N>
fvec<N> __cdecl clamp(fvec<N> x, float64 MinVal, float64 MaxVal)
{
    __stelcxx_array_math_function_body(f, i, _CSE clamp(x[i], MinVal, MaxVal));
}

template<std::size_t N>
fvec<N> __cdecl clamp(fvec<N> x, fvec<N> MinVal, fvec<N> MaxVal)
{
    __stelcxx_array_math_function_body(f, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

template<std::size_t N>
ivec<N> __cdecl clamp(ivec<N> x, int64 MinVal, int64 MaxVal)
{
    __stelcxx_array_math_function_body(i, i, _CSE clamp(x[i], MinVal, MaxVal));
}

template<std::size_t N>
ivec<N> __cdecl clamp(ivec<N> x, ivec<N> MinVal, ivec<N> MaxVal)
{
    __stelcxx_array_math_function_body(i, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

template<std::size_t N>
uvec<N> __cdecl clamp(uvec<N> x, uint64 MinVal, uint64 MaxVal)
{
    __stelcxx_array_math_function_body(u, i, _CSE clamp(x[i], MinVal, MaxVal));
}

template<std::size_t N>
uvec<N> __cdecl clamp(uvec<N> x, uvec<N> MinVal, uvec<N> MaxVal)
{
    __stelcxx_array_math_function_body(u, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

/**
 * @brief Performs a linear interpolation between x and y using a to weight between them.
 * @param x - Specify the start of the range in which to interpolate.
 * @param y - Specify the end of the range in which to interpolate.
 * @param a - Specify the value to use to interpolate between x and y.
 */
float64 mix(float64 x, float64 y, float64 a);
float64 mix(float64 x, float64 y, bool a);
int64 mix(int64 x, int64 y, bool a);
uint64 mix(uint64 x, uint64 y, bool a);
bool mix(bool x, bool y, bool a);

template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, float64 a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a));
}

template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, fvec<N> a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a[i]));
}

template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a[i]));
}

template<std::size_t N>
ivec<N> __cdecl mix(ivec<N> x, ivec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(i, i, _CSE mix(x[i], y[i], a[i]));
}

template<std::size_t N>
uvec<N> __cdecl mix(uvec<N> x, uvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(u, i, _CSE mix(x[i], y[i], a[i]));
}

template<std::size_t N>
bvec<N> __cdecl mix(bvec<N> x, bvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(b, i, _CSE mix(x[i], y[i], a[i]));
}

/**
 * @brief Generates a step function by comparing x to edge.
 * @param edge - Specifies the location of the edge of the step function.
 * @param x - Specify the value to be used to generate the step function.
 */
float64 step(float64 edge, float64 x);

template<std::size_t N>
fvec<N> __cdecl step(float64 edge, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE step(edge, x[i]));
}

template<std::size_t N>
fvec<N> __cdecl step(fvec<N> edge, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE step(edge[i], x[i]));
}

/**
 * @brief Performs smooth Hermite interpolation between 0 and 1 when x is between edge0 and edge1.
 * @link https://en.wikipedia.org/wiki/Smoothstep
 * @param edge0 - Specifies the value of the lower edge of the Hermite function.
 * @param edge1 - Specifies the value of the upper edge of the Hermite function.
 * @param x
 */
float64 smoothstep(float64 edge0, float64 edge1, float64 x);
float64 __Smoothstep_GL(float64 edge0, float64 edge1, float64 x); // Original function in GLSL
float64 __Smoothstep_AMD(float64 edge0, float64 edge1, float64 x); // Another version provide by AMD
float64 __Smoothstep_KP(float64 edge0, float64 edge1, float64 x); // Ken Perlin's improved version
float64 inverse_smoothstep(float64 x); // Inverse function of smoothstep

template<std::size_t N>
fvec<N> __cdecl smoothstep(float64 edge0, float64 edge1, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE smoothstep(edge0, edge1, x[i]));
}

template<std::size_t N>
fvec<N> __cdecl smoothstep(fvec<N> edge0, fvec<N> edge1, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE smoothstep(edge0[i], edge1[i], x[i]));
}

// -------------------------------------------------------------------
// inf and nan

int64 isinf(float64 _X)throw();
int64 isnan(float64 _X)throw();
int64 isfinite(float64 _X)throw();

template<std::size_t N>
ivec<N> __cdecl isinf(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE isinf(_X[i]))
}

template<std::size_t N>
ivec<N> __cdecl isnan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE isnan(_X[i]))
}

template<std::size_t N>
ivec<N> __cdecl isfinite(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE isfinite(_X[i]))
}

// FPClassify函数在不同平台上返回值不同，此处统一下行为。
enum FPTypes
{
    Nan = 0,
    Inf = 1,
    Zero = 2,
    SubNormal = 3,
    Normal = 4
};

int64 FPClassify(float64 x)throw();

template<std::size_t N>
ivec<N> __cdecl FPClassify(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE FPClassify(_X[i]))
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
