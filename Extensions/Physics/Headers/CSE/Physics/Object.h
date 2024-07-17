/*
    Object constructors
    Copyleft (L) StellarDX Astronomy.

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

#ifndef __OBJ_CONSTRUCTOR__
#define __OBJ_CONSTRUCTOR__

#include "CSE/Physics/ObjectConstructors/PlanetConstructor.h"
#include "CSE/Physics/ObjectConstructors/StellarConstructor.h"
#include "CSE/Physics/ObjectConstructors/WDConstructor.h"

_CSE_BEGIN _HYD_BEGIN
using PlanetConstructor = __Hydrostatic_Equilibrium_Object_Constructor;
_HYD_END _CSE_END

#endif
