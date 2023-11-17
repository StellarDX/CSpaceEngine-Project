/************************************************************
  CSpaceEngine Internal Algorithms.
  Copyright (C) StellarDX Astronomy
  SPDX-License-Identifier: GPL-3.0
***********************************************************/

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/Algorithms.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

int __Is_Zero(float64 _In, float64 _Err)
{
    return _In < _Err && _In > -_Err;
}

/****************************************************************************************\
*                                      fast math                                         *
\****************************************************************************************/

////////////////////////////////////// ABS /////////////////////////////////////

_NODISCARD float64 abs(float64 _Xx)noexcept
{
	return (_Xx >= 0) ? _Xx : -_Xx;
}

_NODISCARD int64 abs(int64 _Xx)noexcept
{
	return (_Xx >= 0) ? _Xx : -_Xx;
}

_NODISCARD float64 abs(complex64 _Xx)noexcept
{
	return _CSE sqrt(_Xx.real() * _Xx.real() + _Xx.imag() * _Xx.imag());
}

////////////////////////////////////// SGN /////////////////////////////////////

_NODISCARD float64 sgn(float64 _Xx)noexcept
{
	if (_Xx == 0) { return 0; }
	return _Xx / _CSE abs(_Xx);
}

_NODISCARD int64 sgn(int64 _Xx)noexcept
{
	if (_Xx == 0) { return 0; }
	return _Xx / _CSE abs(_Xx);
}

////////////////////////////////////// FLOOR /////////////////////////////////////

_NODISCARD int64 floor(float64 _Xx)noexcept
{
	int64 i = int64(_Xx);
	return i - (i > _Xx);
}

////////////////////////////////////// CEIL /////////////////////////////////////

_NODISCARD int64 ceil(float64 _Xx)noexcept
{
	int64 i = int64(_Xx);
	return i + (i < _Xx);
}

////////////////////////////////////// FractionalPart /////////////////////////////////////

_NODISCARD float64 FractionalPart(float64 _Xx)noexcept
{
	return modf(_Xx, nullptr);
}

////////////////////////////////////// Fract /////////////////////////////////////

_NODISCARD float64 fract(float64 _Xx)noexcept
{
	return _Xx - floor(_Xx);
}

////////////////////////////////////// MOD /////////////////////////////////////

_NODISCARD float64 mod(float64 _Left, float64 _Right)noexcept
{
	return _Left - _Right * _CSE floor(_Left / _Right);
}

////////////////////////////////////// MIN /////////////////////////////////////

_NODISCARD constexpr float64 min(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left))
{
	// return smaller of _Left and _Right
	return _Right < _Left ? _Right : _Left;
}

_NODISCARD constexpr int64 min(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left))
{
	return _Right < _Left ? _Right : _Left;
}

_NODISCARD constexpr uint64 min(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left))
{
	return _Right < _Left ? _Right : _Left;
}

////////////////////////////////////// MAX /////////////////////////////////////

_NODISCARD constexpr float64 max(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left))
{
	// return larger of _Left and _Right
	return _Left < _Right ? _Right : _Left;
}

_NODISCARD constexpr int64 max(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left))
{
	return _Left < _Right ? _Right : _Left;
}

_NODISCARD constexpr uint64 max(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left))
{
	return _Left < _Right ? _Right : _Left;
}

////////////////////////////////////// CLAMP /////////////////////////////////////

float64 clamp(float64 x, float64 MinVal, float64 MaxVal)
{
	if (MinVal > MaxVal)
    {
        return __Float64::FromBytes(Q_NAN_DOUBLE);
	}
	return _CSE min(_CSE max(x, MinVal), MaxVal);
}

int64 clamp(int64 x, int64 MinVal, int64 MaxVal)
{
	if (MinVal > MaxVal)
	{
		return 0xFFFFFFFFFFFFFFFF;
	}
	return _CSE min(_CSE max(x, MinVal), MaxVal);
}

uint64 clamp(uint64 x, uint64 MinVal, uint64 MaxVal)
{
	if (MinVal > MaxVal)
	{
		return 0xFFFFFFFFFFFFFFFF;
	}
	return _CSE min(_CSE max(x, MinVal), MaxVal);
}

////////////////////////////////////// MIX /////////////////////////////////////

float64 mix(float64 x, float64 y, float64 a)
{
	return x * (1. - a) + y * a;
}

float64 mix(float64 x, float64 y, bool a)
{
	return x * (1. - a) + y * a;
}

int64 mix(int64 x, int64 y, bool a)
{
    return x * (1ll - a) + y * a;
}

uint64 mix(uint64 x, uint64 y, bool a)
{
    return x * (1ull - a) + y * a;
}

bool mix(bool x, bool y, bool a)
{
	return x * (1 - a) + y * a;
}

////////////////////////////////////// STEP /////////////////////////////////////
float64 step(float64 edge, float64 x)
{
	return x < edge ? 0. : 1.;
}

////////////////////////////////////// SmoothStep /////////////////////////////////////

float64 smoothstep(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1){return __Float64::FromBytes(Q_NAN_DOUBLE);}
	float64 t;  /* Or genDType t; */
	t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return t * t * (3.0 - 2.0 * t);
}

float64 smoothstepAMD(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1) { return __Float64::FromBytes(Q_NAN_DOUBLE); }
	if (x < edge0){return 0;}
	if (x >= edge1){return 1;}
	// Scale/bias into [0..1] range
	x = (x - edge0) / (edge1 - edge0);
	return x * x * (3 - 2 * x);
}

float64 smoothstepKP(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1) { return __Float64::FromBytes(Q_NAN_DOUBLE); }
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}

float64 inverse_smoothstep(float64 x)
{
	return 0.5 - _CSE sin(_CSE arcsin(1.0 - 2.0 * x) / 3.0);
}

////////////////////////////////////// INF and NAN /////////////////////////////////////

int64 isinf(float64 _X)throw()
{
    int64 ieee754 = __Float64(_X).Bytes;
	return (ieee754 & 0x7FFFFFFF00000000) == POS_INF_DOUBLE;
}

int64 isnan(float64 _X)throw()
{
    int64 ieee754 = __Float64(_X).Bytes;
	return (uint64(ieee754 >> 32) & 0x7FFFFFFF) + (uint64(ieee754) != 0) > 0x7FF00000;
}

_CSE_END
