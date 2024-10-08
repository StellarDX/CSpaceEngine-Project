/*
    Hydrostatic equilibrium Planet constructor
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

/**
 * @link References:
 * [1] Anderson O L , Baumgardner J R .Equations of state in planet interiors[J].
 * Lunar and Planetary Science Conference Proceedings, 1980.
 * [2] 龚盛夏,黄乘利.太阳系内类地行星内部结构模型研究进展[J].天文学进展, 2013,
 * 31(4):20.DOI:10.3969/j.issn.1000-8349.2013.04.01.
 * [3] Zeng L, Sasselov D. A Detailed Model Grid for Solid Planets from 0.1
 * through 100 Earth Masses[J/OL]. Publications of the Astronomical Society
 * of the Pacific, 2013, 125(925): 227. DOI:10.1086/669163.
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

#define _HYD_BEGIN namespace Constructors {
#define _HYD_END }
#define _HYD Constructors::

_CSE_BEGIN
_HYD_BEGIN

template<uint64 EquationCount> requires (EquationCount > 0)
class __Manipulatable_ODE_Engine : public _SCICXX __RK45_Runge_Kutta_Engine<EquationCount>
{
public:
    using _Mybase = _SCICXX __RK45_Runge_Kutta_Engine<EquationCount>;

    __Manipulatable_ODE_Engine() : _Mybase() {}

    __Manipulatable_ODE_Engine(std::function<typename _Mybase::_Fty> _Right)
        : _Mybase(_Right) {}

    __Manipulatable_ODE_Engine(typename _Mybase::_Fty* _PFunc)
        : _Mybase(_PFunc) {}

    template<typename _Functor>
    __Manipulatable_ODE_Engine(_Functor _Func)
        : _Mybase(_Func) {}

    template<typename _Functor>
    __Manipulatable_ODE_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    auto GetCurrentState()const
    {
        return *(this->_M_StateBuffer.rbegin());
    }

    auto GetCurrentBlock()const
    {
        return *(this->Interpolants.rbegin());
    }

    float64 GetNextStep()const {return _Mybase::AbsStep;}
    void SetNextStep(float64 NewStep) {_Mybase::AbsStep = NewStep;}
};

class __Hydrostatic_Equilibrium_Object_Constructor
{
public:
    static const uint64 EquationCount = 5;

    using ArrayType    = std::vector<float64>;

    using MaterialType = _EOS Material;
    using MaterialPtr  = MaterialType*;
    using MaterialArr  = std::vector<MaterialPtr>;

    using ODEType      = __Manipulatable_ODE_Engine<EquationCount>; // P'(R), M'(R) and I'(R)
    using OdeArray     = std::vector<ODEType>;
    using InteriorType = decltype(Object::Interior);

protected:
    float64     _M_NLogError   = 1.8; // Negative logarithm of error
    ArrayType   _M_Layers;            // Total mass of layers, values in Kg
    MaterialArr _M_Interior;          // EOS of layers
    float64     _M_Index       = 0;   // Current layer
    float64     _M_TargetRadius;      // Target Radius

    float64     _M_MaxRadius;         // Max radius, value in Meters
    float64     _M_InitPressure;      // Initial pressure, value in Pascal
    float64     _M_SurfPressure;      // Surface pressure, value in Pascal

    ArrayType   _M_RadBoundary;       // Boundaries of layers, values in Meters.

    float64     _M_IsConvection;      // Use Convection temperature function instead of Conduction
    float64     _M_EndogenousHeating; // Central heat flux in W/m^2
    float64     _M_HeatGenRate = 0;   // Rate of Heat Generation
    float64     _M_InitTemperature;   // Center Temperature

    enum Variables
    {
        _Eq_Pressure,
        _Eq_Mass,
        _Eq_InertiaMoment,
        _Eq_HeatFlow,
        _Eq_Temperature
    };

    ODEType _M_DerivativeEquation = // Final Equations
        [this](float64 Radius, fvec<EquationCount> Variables)
    {
        return __Derivate_Equations(Radius, Variables);
    };

    OdeArray _M_Equations;

    void __Sort_Compositions(ArrayType Masses, MaterialArr EOSs);
    float64 __Get_Density(float64 Pressure, float64 Tempertaure);
    float64 __Get_MeltingCurve(float64 Pressure);
    float64 __Get_SpecificHeatCapacity(float64 Pressure, float64 Tempertaure);
    float64 __Get_ThermalExpansion(float64 Pressure, float64 Tempertaure);
    float64 __Get_ThermalConductivity(float64 Pressure, float64 Tempertaure);
    float64 __Get_NextPhase(float64 Pressure);
    float64 __Total_Mass() const;
    int __Get_Segment_From_Radius(float64 Rad);

public:
    __Hydrostatic_Equilibrium_Object_Constructor
        (ArrayType MassFractions, MaterialArr CompositionEOSs)noexcept(0);

    fvec<EquationCount> __Derivate_Equations(float64 Radius, fvec<EquationCount> Variables);

    /**
     * @brief Enable additional terms from Tolman-Oppenheimer-Volkoff equation
     */
    bool EnableRelativity = false;

    constexpr static const float64 GasGiant1BarSurfPressure = 100000;

    void SetError(float64 NewError) {_M_NLogError = NewError;}
    void SetMaxRadius(float64 NewMaxRadius) {_M_MaxRadius = NewMaxRadius;}
    void SetInitPressure(float64 NewInitPressure) {_M_InitPressure = NewInitPressure;}
    void SetSurfPressure(float64 NewSurfPressure) {_M_SurfPressure = NewSurfPressure;}
    void SetHeatFlux(float64 NewHeatFlux){_M_EndogenousHeating = NewHeatFlux;}
    void SetHeatGenerationRate(float64 NewHeatGenerationRate){_M_HeatGenRate = NewHeatGenerationRate;}
    void SetInitTemperature(float64 NewInitTemperature) {_M_InitTemperature = NewInitTemperature;}

    float64 TargetRadius()const {return _M_TargetRadius;}

    float64 Mass()const;            // Total mass
    float64 Radius()const;          // Total radius
    float64 InertiaMoment()const;   // Moment of inertia
    float64 MeanDensity()const;     // Mean density
    float64 SurfTemperature()const; // Surface Temperature

    InteriorType Interior()const;   // Interior compossitions in mass fraction

    float64 CoreDensity()const;     // Core density
    float64 CorePressure()const;    // Core pressure
    float64 CoreTemperature()const; // Core Temperature (TODO)

    float64 Mass(float64 Radius);          // Mass function
    float64 Density(float64 Radius);       // Density function
    float64 Pressure(float64 Radius);      // Pressure function
    float64 InertiaMoment(float64 Radius); // Moment of inertia function
    float64 Temperature(float64 Radius);   // Temperature function (TODO)

    Object ToObject()const; // Export data as an object.

    int Run(); // Main function
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
