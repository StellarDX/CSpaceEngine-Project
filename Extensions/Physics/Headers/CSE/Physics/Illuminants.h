/*
    Star magnitude and luminosity module
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

#ifndef __SPECTROSCOPY__
#define __SPECTROSCOPY__

#include "CSE/Base.h"

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

/****************************************************************************************\
*                                       Magnitudes                                       *
\****************************************************************************************/

/**
 * @brief Calculate absolute magnitude from visual magnitude and distance
 * @note where it is assumed that extinction from gas and dust is negligible.
 * Typical extinction rates within the Milky Way galaxy are 1 to 2 magnitudes
 * per kiloparsec, when dark clouds are taken into account.
 * @param AppMagn - visual magnitude
 * @param Dist - distance in parsec
 * @return absolute magnitude
 */
float64 GetAbsMagnFromAppMagnDist(float64 AppMagn, float64 Dist);

/**
 * @brief Calculate absolute magnitude from visual magnitude and parallax
 * @param AppMagn - visual magnitude
 * @param Dist - parallax in mas
 * @return absolute magnitude
 */
float64 GetAbsMagnFromAppMagnParallax(float64 AppMagn, float64 Parallax);

/**
 * @brief Calculate absolute bolometric magnitude from bolometric luminosity
 * @param LumBol - Bolometric luminosity in watts
 * @return absolute bolometric magnitude
 */
float64 GetAbsMagnBolFromLumBol(float64 LumBol);

/**
 * @brief Calculate bolometric luminosity from absolute bolometric magnitude
 * @param absolute bolometric magnitude
 * @return bolometric luminosity
 */
float64 GetLumBolFromAbsMagnBol(float64 AbsMagnBol);


/****************************************************************************************\
*                                 Stellar Classification                                 *
\****************************************************************************************/

struct SpectralType
{

};


/****************************************************************************************\
*                                       Illuminants                                      *
\****************************************************************************************/

class __Illuminant_Commission_Internationale_De_LEclairage
{

};

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
