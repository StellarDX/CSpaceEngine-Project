#pragma once

#ifndef _CSE_MATH_ALGORITHMS
#define _CSE_MATH_ALGORITHMS

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/gltypes.h"
#include "CSE/CSEBase/MathFuncs.h"
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

_CSE_BEGIN

/**
 * @details Returns the absolute value of x.
 */
_NODISCARD float64 abs(float64 _Xx)noexcept;
_NODISCARD int64 abs(int64 _Xx)noexcept;
_NODISCARD float64 abs(complex64 _Xx)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType __cdecl abs(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE abs(_Xx[i]); }
    return _Xx;
}

template<typename genIType> requires vecIType<genIType>
_NODISCARD genIType __cdecl abs(genIType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE abs(_Xx[i]); }
    return _Xx;
}

/**
 * @details Returns -1.0 if x is less than 0.0, 0.0 if x is equal to 0.0, and +1.0 if x is greater than 0.0.
 */
_NODISCARD float64 sgn(float64 _Xx)noexcept;
_NODISCARD int64 sgn(int64 _Xx)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType __cdecl sgn(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE sgn(_Xx[i]); }
    return _Xx;
}

template<typename genIType> requires vecIType<genIType>
_NODISCARD genIType __cdecl sgn(genIType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE sgn(_Xx[i]); }
    return _Xx;
}

/**
 * @details Returns a value equal to the nearest integer that is less than or equal to x.
 */
_NODISCARD int64 floor(float64 _Xx)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType floor(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE floor(_Xx[i]); }
    return _Xx;
}

/**
 * @details Returns a value equal to the nearest integer that is greater than or equal to x.
 */
_NODISCARD int64 ceil(float64 x)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType ceil(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE ceil(_Xx[i]); }
    return _Xx;
}

/**
 * @details Returns the fractional part of x.
 */
_NODISCARD float64 FractionalPart(float64 _Xx)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType FractionalPart(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE FractionalPart(_Xx[i]); }
    return _Xx;
}

_NODISCARD float64 fract(float64 _Xx)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType fract(genType _Xx)noexcept
{
    for (size_t i = 0; i < _Xx.size(); i++) { _Xx[i] = _CSE fract(_Xx[i]); }
    return _Xx;
}

/**
 * @details Returns the value of x modulo y.
 */
_NODISCARD float64 mod(float64 _Left, float64 _Right)noexcept;

template<typename genType> requires vecType<genType>
_NODISCARD genType mod(genType _Left, float64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE mod(_Left[i], _Right); }
    return _Left;
}

template<typename genType> requires vecType<genType>
_NODISCARD genType mod(genType _Left, genType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE mod(_Left[i], _Right[i]); }
    return _Left;
}

/**
 * @details returns the minimum of the two parameters or array.
 */
_NODISCARD constexpr float64 min(float64 _Left, float64 _Right) noexcept(noexcept(_Left < _Right));
_NODISCARD constexpr int64 min(int64 _Left, int64 _Right) noexcept(noexcept(_Left < _Right));
_NODISCARD constexpr uint64 min(uint64 _Left, uint64 _Right) noexcept(noexcept(_Left < _Right));

template<typename genType> requires vecType<genType>
_NODISCARD constexpr genType min(genType _Left, genType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genType> requires vecType<genType>
_NODISCARD constexpr genType min(genType _Left, float64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right); }
    return _Left;
}

template<typename genIType> requires vecIType<genIType>
_NODISCARD constexpr genIType min(genIType _Left, genIType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genIType> requires vecIType<genIType>
_NODISCARD constexpr genIType min(genIType _Left, int64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right); }
    return _Left;
}

template<typename genUType> requires vecUType<genUType>
_NODISCARD constexpr genUType min(genUType _Left, genUType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genUType> requires vecUType<genUType>
_NODISCARD constexpr genUType min(genUType _Left, uint64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE min(_Left[i], _Right); }
    return _Left;
}

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
 * @details returns the maximum of the two parameters or array.
 */
_NODISCARD constexpr float64 max(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left));
_NODISCARD constexpr int64 max(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left));
_NODISCARD constexpr uint64 max(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left));

template<typename genType> requires vecType<genType>
_NODISCARD constexpr genType max(genType _Left, genType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genType> requires vecType<genType>
_NODISCARD constexpr genType max(genType _Left, float64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right); }
    return _Left;
}

template<typename genIType> requires vecIType<genIType>
_NODISCARD constexpr genIType max(genIType _Left, genIType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genIType> requires vecIType<genIType>
_NODISCARD constexpr genIType max(genIType _Left, int64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right); }
    return _Left;
}

template<typename genUType> requires vecUType<genUType>
_NODISCARD constexpr genUType max(genUType _Left, genUType _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right[i]); }
    return _Left;
}
template<typename genUType> requires vecUType<genUType>
_NODISCARD constexpr genUType max(genUType _Left, uint64 _Right)noexcept
{
    for (size_t i = 0; i < _Left.size(); i++) { _Left[i] = _CSE max(_Left[i], _Right); }
    return _Left;
}

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
 * @details Returns the value of x constrained to the range minVal to maxVal.
 * @param x - Specify the value to constrain.
 * @param MinVal - Specify the lower end of the range into which to constrain x.
 * @param MaxVal - Specify the upper end of the range into which to constrain x.
 */
float64 clamp(float64 x, float64 MinVal, float64 MaxVal);
int64 clamp(int64 x, int64 MinVal, int64 MaxVal);
uint64 clamp(uint64 x, uint64 MinVal, uint64 MaxVal);

template<typename genType> requires vecType<genType>
genType clamp(genType x, genType MinVal, genType MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal[i], MaxVal[i]); }
    return x;
}
template<typename genType> requires vecType<genType>
genType clamp(genType x, float64 MinVal, float64 MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal, MaxVal); }
    return x;
}

template<typename genIType> requires vecIType<genIType>
genIType clamp(genIType x, genIType MinVal, genIType MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal[i], MaxVal[i]); }
    return x;
}
template<typename genIType> requires vecIType<genIType>
genIType clamp(genIType x, int64 MinVal, int64 MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal, MaxVal); }
    return x;
}

template<typename genUType> requires vecUType<genUType>
genUType clamp(genUType x, genUType MinVal, genUType MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal[i], MaxVal[i]); }
    return x;
}
template<typename genUType> requires vecUType<genUType>
genUType clamp(genUType x, uint64 MinVal, uint64 MaxVal)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE clamp(x[i], MinVal, MaxVal); }
    return x;
}

/**
 * @details Performs a linear interpolation between x and y using a to weight between them.
 * @param x - Specify the start of the range in which to interpolate.
 * @param y - Specify the end of the range in which to interpolate.
 * @param a - Specify the value to use to interpolate between x and y.
 */
float64 mix(float64 x, float64 y, float64 a);
float64 mix(float64 x, float64 y, bool a);
int64 mix(int64 x, int64 y, bool a);
uint64 mix(uint64 x, uint64 y, bool a);
bool mix(bool x, bool y, bool a);

template<typename genType> requires vecType<genType>
genType mix(genType x, genType y, genType a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a[i]); }
    return x;
}
template<typename genType> requires vecType<genType>
genType mix(genType x, genType y, float64 a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a); }
    return x;
}

template<typename genType, typename genBType> requires vecType<genType> && vecBType<genBType>
genType mix(genType x, genType y, genBType a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a[i]); }
    return x;
}

template<typename genIType, typename genBType> requires vecIType<genIType> && vecBType<genBType>
genIType mix(genIType x, genIType y, genBType a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a[i]); }
    return x;
}

template<typename genUType, typename genBType> requires vecUType<genUType> && vecBType<genBType>
genUType mix(genUType x, genUType y, genBType a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a[i]); }
    return x;
}

template<typename genBType> requires vecBType<genBType>
genBType mix(genBType x, genBType y, genBType a)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE mix(x[i], y[i], a[i]); }
    return x;
}

/**
 * @details Generates a step function by comparing x to edge.
 * @param edge - Specifies the location of the edge of the step function.
 * @param x - Specify the value to be used to generate the step function.
 */
float64 step(float64 edge, float64 x);

template<typename genType> requires vecType<genType>
genType step(genType edge, genType x)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE step(edge[i], x[i]); }
    return x;
}
template<typename genType> requires vecType<genType>
genType step(float64 edge, genType x)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE step(edge, x[i]); }
    return x;
}

/**
 * @details Performs smooth Hermite interpolation between 0 and 1 when x is between edge0 and edge1.
 * @ref https://en.wikipedia.org/wiki/Smoothstep
 * @param edge0 - Specifies the value of the lower edge of the Hermite function.
 * @param edge1 - Specifies the value of the upper edge of the Hermite function.
 * @param x
 */
float64 smoothstep(float64 edge0, float64 edge1, float64 x); // Original function in GLSL
float64 smoothstepAMD(float64 edge0, float64 edge1, float64 x); // Another version provide by AMD
float64 smoothstepKP(float64 edge0, float64 edge1, float64 x); // Ken Perlin's improved version
float64 inverse_smoothstep(float64 x); // Inverse function of smoothstep

template<typename genType> requires vecType<genType>
genType smoothstep(genType edge0, genType edge1, genType x)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE smoothstep(edge0[i], edge1[i], x[i]); }
    return x;
}
template<typename genType> requires vecType<genType>
genType smoothstep(float64 edge0, float64 edge1, genType x)
{
    for (size_t i = 0; i < x.size(); i++) { x[i] = _CSE smoothstep(edge0, edge1, x[i]); }
    return x;
}

// -------------------------------------------------------------------
// inf and nan
int64 isinf(float64 _X)throw();
int64 isnan(float64 _X)throw();

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
