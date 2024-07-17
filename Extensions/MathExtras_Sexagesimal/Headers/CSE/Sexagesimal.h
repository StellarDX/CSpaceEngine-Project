/*
    CSE Sexagesimal number support
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

#ifndef _CSE_MATH_SEXAGESIMAL_
#define _CSE_MATH_SEXAGESIMAL_

#include "CSE/Base/MathFuncs.h"

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
_EXTERN_C

#pragma pack(push)
#pragma pack(2)
/**
 * @brief Dense-packed 80-bit Sexagesimal floating point.
 * Using Low-Endian.
 * Domain: (-360 0 0.0, 360 0 0.0).
 */
struct _Sexagesimal80
{
    float64  Seconds;    // Valid value: [0, 60)
    uint16_t Minutes :6; // Valid value: [0, 60)
    uint16_t Degrees :9; // Valid value: [0, 360)
    uint16_t Negative:1; // Sign bit;
};
#pragma pack(pop)

/**
 * @brief Construct a Sexagesimal number
 */
_Sexagesimal80 ConstructSexagesimal(bool Negative, uint32_t h, uint16_t m, float64 s);

/**
 * @brief Normalize a sexagesimal number
 */
void SexagesimalNormalize(_Sexagesimal80& _Sexagesimal);

/**
 * @brief Convert a sexagesimal number to decamal format
 * @param _Sexagesimal input sexagesimal number
 * @param TimezoneFormat If true, convertion will based on HMS instead of DMS
 * @return Number in decamal format
 */
float64 SexagesimalToDecimal(_Sexagesimal80 _Sexagesimal, bool TimezoneFormat = 0);

/**
 * @brief Convert a decamal number to sexagesimal format
 * @param Decimal Input decimal number
 * @param TimezoneFormat If true, convertion will based on HMS instead of DMS
 * @return Number in Sexagesimal format
 */
_Sexagesimal80 DecimalToSexagesimal(float64 Decimal, bool TimezoneFormat = 0);

/**
 * @brief Write a sexagesimal number to a string
 */
ustring SexagesimalToString(_Sexagesimal80 _Sexagesimal, char const* Format = "{}{} {:02} {}");

_END_EXTERN_C

using Sexagesimal = _Sexagesimal80;

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
