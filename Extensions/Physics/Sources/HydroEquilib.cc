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

#include "CSE/Physics/HydroDynamic.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_HYD_BEGIN

void __Hydrostatic_Equilibrium_Object_Constructor::
    __Sort_Compositions(ArrayType Masses, EOSArray EOSs)
{
    if (Masses.size() != EOSs.size())
    {
        throw std::logic_error("Number of masses is not equal to number of EOSs.");
    }

    // Bond mass with composition
    std::vector<std::pair<float64, EOSPointer>> MassFractions;
    for (int i = 0; i < Masses.size(); ++i)
    {
        MassFractions.push_back({Masses[i], EOSs[i]});
    }

    std::sort(MassFractions.begin(), MassFractions.end(),
    [](std::pair<float64, EOSPointer> Left, std::pair<float64, EOSPointer> Right)
    {
        return Left.second->BaseDensity() > Right.second->BaseDensity();
    });

    // Fill data
    for (int i = 0; i < MassFractions.size(); ++i)
    {
        _M_Layers.push_back(MassFractions[i].first);
        _M_Interior.push_back(MassFractions[i].second);
    }
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::
    __Get_Density(float64 Pressure)
{
    return _M_Interior[_M_Index]->Density(Pressure);
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::__Total_Mass()const
{
    float64 Sum = 0;
    for (size_t i = 0; i <= _M_Index; ++i)
    {
        Sum += _M_Layers[i];
    }
    return Sum;
}

int __Hydrostatic_Equilibrium_Object_Constructor::
    __Get_Segment_From_Radius(float64 Rad)
{
    if (Rad > this->Radius()) {return -1;}
    size_t Segment = 0;
    for (int i = 1; i < _M_RadBoundary.size(); ++i)
    {
        if (Rad >= _M_RadBoundary[i - 1] && Rad <= _M_RadBoundary[i])
        {
            Segment = i - 1;
        }
    }
    return Segment;
}

__Hydrostatic_Equilibrium_Object_Constructor::
    __Hydrostatic_Equilibrium_Object_Constructor
    (ArrayType MassFractions, EOSArray CompositionEOSs) noexcept(0)
{
    __Sort_Compositions(MassFractions, CompositionEOSs);
    _M_MaxRadius = 8. * JupiterRadius; // Size limit for brown dwarfs
    _M_InitPressure = 1E14;
    _M_SurfPressure = StandardAtm;
}

#define _NHEOC_EQCOUNT __Hydrostatic_Equilibrium_Object_Constructor::EquationCount

fvec<_NHEOC_EQCOUNT> __Hydrostatic_Equilibrium_Object_Constructor::
    __Derivate_Equations(float64 Radius, fvec<_NHEOC_EQCOUNT> Variables)
{
    float64 Radius2 = Radius * Radius;
    float64 SpeedOfLight2 = float64(SpeedOfLight) * SpeedOfLight;

    // Calculate Gravity.
    float64 Gravity = (GravConstant * Variables[_Eq_Mass]) / Radius2;
    if (isnan(Gravity)) {Gravity = 0;}

    // Pick density from Equation of state.
    float64 CurrentDensity = __Get_Density(Variables[_Eq_Pressure]);

    // if enabled Relativity, calculate it.
    float64 RelativisticScale = 1.;
    if (EnableRelativity)
    {
        RelativisticScale =
            (1. + Variables[_Eq_Pressure] / (CurrentDensity * SpeedOfLight2)) *
            (1. + (4. * CSE_PI * Radius2 * Radius * Variables[_Eq_Pressure]) / (Variables[_Eq_Mass] * SpeedOfLight2)) /
            (1. - (2. * GravConstant * Variables[_Eq_Mass]) / (Radius * SpeedOfLight2));
        if (isinf(RelativisticScale) || isnan(RelativisticScale)) {RelativisticScale = 1.;}
    }

    return fvec<_NHEOC_EQCOUNT>
    {
        // The Hydrostatic equilibrium
        -Gravity * CurrentDensity * RelativisticScale,
        // Mass of a spherical shell
        4. * CSE_PI * Radius2 * CurrentDensity,
        // Moment of inertia
        (8. / 3.) * CSE_PI * (Radius2 * Radius2) * CurrentDensity,
    };
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::Mass() const
{
    return __Total_Mass();
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::Radius() const
{
    return TargetRadius();
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::InertiaMoment() const
{
    return _M_Equations[_M_Index](TargetRadius())[_Eq_InertiaMoment];
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::MeanDensity() const
{
    return Mass() / ((4. / 3.) * CSE_PI * Radius() * Radius() * Radius());
}

__Hydrostatic_Equilibrium_Object_Constructor::InteriorType
    __Hydrostatic_Equilibrium_Object_Constructor::Interior() const
{
    InteriorType Interiors;
    for (int i = 0; i < _M_Interior.size(); ++i)
    {
        Interiors.insert({_M_Interior[i]->MaterialName(), _M_Layers[i] / Mass() * 100.});
    }
    return Interiors;
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::CoreDensity() const
{
    return _M_Interior[0]->Density(CorePressure());
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::CorePressure() const
{
    return _M_InitPressure;
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::Mass(float64 Radius)
{
    int Segment = __Get_Segment_From_Radius(Radius);
    if (Segment < 0) {throw std::logic_error("Radius out of range");}
    return _M_Equations[Segment](Radius)[_Eq_Mass];
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::Density(float64 Radius)
{
    int Segment = __Get_Segment_From_Radius(Radius);
    if (Segment < 0) {throw std::logic_error("Radius out of range");}
    return _M_Interior[Segment]->Density(Pressure(Radius));
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::Pressure(float64 Radius)
{
    int Segment = __Get_Segment_From_Radius(Radius);
    if (Segment < 0) {throw std::logic_error("Radius out of range");}
    return _M_Equations[Segment](Radius)[_Eq_Pressure];
}

float64 __Hydrostatic_Equilibrium_Object_Constructor::InertiaMoment(float64 Radius)
{
    int Segment = __Get_Segment_From_Radius(Radius);
    if (Segment < 0) {throw std::logic_error("Radius out of range");}
    return _M_Equations[Segment](Radius)[_Eq_InertiaMoment];
}

Object __Hydrostatic_Equilibrium_Object_Constructor::ToObject() const
{
    return Object
    {
        .Mass = Mass(),
        .Dimensions = vec3(Radius() * 2.),
        .InertiaMoment = InertiaMoment() / (Mass() * Radius() * Radius()),
        .Interior = Interior()
    };
}

int __Hydrostatic_Equilibrium_Object_Constructor::Run()
{
    float64 RealError = pow(10, -_M_NLogError);
    float64 InitRadius = 0;
    fvec<_NHEOC_EQCOUNT> InitState = {_M_InitPressure, 0, 0};
    _M_RadBoundary.push_back(0);

    for (int i = 0; i < _M_Layers.size(); ++i)
    {
        _M_Equations.push_back(_M_DerivativeEquation);
    }

    for (int i = 0; i < _M_Equations.size(); ++i)
    {
        _M_Equations[i].Init(InitState, InitRadius, _M_MaxRadius);
        while (_M_Equations[i].CurrentState() == _M_Equations[i].Processing)
        {
            _M_Equations[i].InvokeRun();
            _M_Equations[i].SaveDenseOutput();
            auto CurrentState = _M_Equations[i].GetCurrentState();
            float64 TotalMass = __Total_Mass();
            if (CurrentState.second[_Eq_Mass] > TotalMass)
            {
                auto CurrentBlk = _M_Equations[i].GetCurrentBlock();
                BisectionSearcher MassSearcher = [this, i](float64 Radius)
                {
                    return _M_Equations[i](Radius)[_Eq_Mass];
                };
                _M_TargetRadius = MassSearcher(TotalMass,
                    CurrentBlk.second.first(), CurrentBlk.second.last());

                if (i < _M_Equations.size() - 1)
                {
                    ++_M_Index;
                    InitRadius = _M_TargetRadius;
                    InitState = _M_Equations[i](_M_TargetRadius);
                    _M_RadBoundary.push_back(_M_TargetRadius);
                    break;
                }
                else
                {
                    auto FinalState = _M_Equations[i](_M_TargetRadius);
                    if (abs(FinalState[_Eq_Pressure] - _M_SurfPressure) > RealError)
                    {
                        _M_TargetRadius = 0;
                        _M_RadBoundary.clear();
                        _M_RadBoundary.push_back(0);
                        _M_Index = 0;
                        _M_InitPressure = _M_InitPressure - FinalState[_Eq_Pressure] + _M_SurfPressure;
                        for (int j = 0; j < _M_Equations.size(); ++j)
                        {
                            _M_Equations[j].Clear();
                        }
                        InitRadius = 0;
                        InitState = {_M_InitPressure, 0, 0};
                        i = -1;
                        break;
                    }

                    else if (_M_Equations[i].CurrentState() == _M_Equations[i].Failed)
                    {
                        _M_TargetRadius = _M_Equations[i].CurrentPoint();
                    }

                    _M_RadBoundary.push_back(_M_TargetRadius);
                    break;
                }
            }
        }
    }

    return 0;
}

_HYD_END
_CSE_END
