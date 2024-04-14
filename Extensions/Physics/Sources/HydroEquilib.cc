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

__Hydrostatic_Equilibrium_Object_Constructor::
    __Hydrostatic_Equilibrium_Object_Constructor
    (ArrayType MassFractions, EOSArray CompositionEOSs) noexcept(0)
{
    __Sort_Compositions(MassFractions, CompositionEOSs);
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

    return fvec<_NHEOC_EQCOUNT>
    {
        // The Hydrostatic equilibrium
        -Gravity * CurrentDensity * (EnableTOVAdditionalTerms ?
            ((1. + Variables[_Eq_Pressure] / (CurrentDensity * SpeedOfLight2)) *
            (1. + (4. * CSE_PI * Radius2 * Radius * Variables[_Eq_Pressure]) /
                (Variables[_Eq_Mass] * SpeedOfLight2)) *
            (1. / (1. - (2. * GravConstant * Variables[_Eq_Mass]) /
                (Radius * SpeedOfLight2)))) : 1.),

        // Mass of a spherical shell
        4. * CSE_PI * Radius2 * CurrentDensity,

        // Moment of inertia
        (8. / 3.) * CSE_PI * (Radius2 * Radius2) * CurrentDensity,
    };
}

void __Hydrostatic_Equilibrium_Object_Constructor::Run()
{

}

_HYD_END
_CSE_END
