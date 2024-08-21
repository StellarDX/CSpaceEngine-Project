/*
    Thermodynamic calculations
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

#ifndef _THERMODYNAMICS_
#define _THERMODYNAMICS_

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

#define _THERMAL_BEGIN namespace Thermodynamics {
#define _THERMAL_END }
#define _THERMAL Thermodynamics::

_THERMAL_BEGIN

class __First_Principle_Thermodynamic_Engine
{

};

_THERMAL_END
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
