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

#include "CSE/Physics/Illuminants.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN

float64 GetAbsMagnFromAppMagnDist(float64 AppMagn, float64 Dist)
{
    return AppMagn - 5. * (log(Dist) - 1.);
}

float64 GetAbsMagnFromAppMagnParallax(float64 AppMagn, float64 Parallax)
{
    return AppMagn + 5. * (log(Parallax) + 1.);
}

float64 GetAbsMagnBolFromLumBol(float64 LumBol)
{
    return -2.5 * log(LumBol / SolarLumBol);
}

float64 GetLumBolFromAbsMagnBol(float64 AbsMagnBol)
{
    return SolarLumBol * pow(10, -0.4 * AbsMagnBol);
}

_CSE_END
