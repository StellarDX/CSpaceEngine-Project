/*
    Hydrostatic equilibrium Object constructor
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

#ifndef __HYDRODYNAMIC__
#define __HYDRODYNAMIC__

#include "CSE/Base/AdvMath.h"
#include "CSE/Object.h"
#include "CSE/Physics/Materials.h"

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

#define _HYD_BEGIN namespace HydroDynamic {
#define _HYD_END }
#define _HYD HydroDynamic::

_CSE_BEGIN
_HYD_BEGIN

class __Hydrostatic_Equilibrium_Object_Constructor
{
public:
    static const uint64 EquationCount = 3;

    using ArrayType    = std::vector<float64>;

    using EOSType      = _EOS __EOS_Base;
    using EOSPointer   = EOSType*;
    using EOSArray     = std::vector<EOSPointer>;

    using ODEType      = DefaultODEFunction<EquationCount>; // P'(R), M'(R) and I'(R)
    using InteriorType = decltype(Object::Interior);

protected:
    ODEType   _M_Equations;    // Final Equations
    ArrayType _M_Layers;       // Total mass of layers, values in Kg
    EOSArray  _M_Interior;     // EOS of layers
    size_t    _M_Index = 0;    // Current layer

    float64   _M_InitPressure; // Initial pressure, value in Pascal
    float64   _M_SurfPressure; // Surface pressure, value in Pascal

    ArrayType _M_RadBoundary;  // Boundaries of layers, values in Meters.

    enum Variables
    {
        _Eq_Pressure,
        _Eq_Mass,
        _Eq_InertiaMoment,
    };

    void __Sort_Compositions(ArrayType Masses, EOSArray EOSs);

    float64 __Get_Density(float64 Pressure);

public:
    __Hydrostatic_Equilibrium_Object_Constructor
        (ArrayType MassFractions, EOSArray CompositionEOSs)noexcept(0);

    fvec<EquationCount> __Derivate_Equations(float64 Radius, fvec<EquationCount> Variables);

    /**
     * @brief Enable additional terms from Tolman-Oppenheimer-Volkoff equation
     */
    bool EnableTOVAdditionalTerms = false;

    float64 Mass()const;
    float64 Radius()const;
    float64 InertiaMoment()const;
    float64 MeanDensity()const;

    InteriorType Interior()const;

    float64 CoreDensity()const;
    float64 CorePressure()const;
    //float64 CoreTemperature()const; // TODO

    float64 Mass(float64 Radius);
    float64 Density(float64 Radius);
    float64 Pressure(float64 Radius);
    float64 InertiaMoment(float64 Radius);
    //float64 Temperature(float64 Radius); // TODO

    Object ToObject()const;

    void Run();
};

_HYD_END
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