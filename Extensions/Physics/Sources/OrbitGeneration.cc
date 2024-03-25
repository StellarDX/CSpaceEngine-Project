/*
    Member functions of Orbital Arrays
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

#include "CSE/Physics/Orbit.h"

_CSE_BEGIN
_ORBIT_BEGIN

/**
 * @brief Titius-Bode array
 * @link Reference: Wikipedia. Titius-Bode law
 * https://en.wikipedia.org/wiki/Titius_Bode_law
 */
float64 __General_Titius_Bode_Array::operator[](int64 _Idx)
{
    if (_Idx == 0) { return Base; }
    return Base + Scale * pow(2, _Idx - 1.);
}

/**
 * @brief Blagg's formulation of Titius-Bode array
 * @link Reference:
 * Blagg, M.A. (1913). "On a suggested substitute
 * for Bode's law". Monthly Notices of the Royal
 * Astronomical Society. 73: 414–22.
 * doi:10.1093/mnras/73.6.414.
 * Lobban, G.G.; Roy, A.E.; Brown, J.C. (October
 * 1982). "A review of Blagg's formula in the light
 * of recently discovered planetary moons and
 * rings". Journal of the British Astronomical
 * Association. 92 (6): 260–263.
 * Bibcode:1982JBAA...92..260L.
 */
float64 __Blagg_Titius_Bode_Array::operator[](int64 _Idx)
{
    switch (Method)
    {
    case Trigonometric:
    {
        int64 n = _Idx + Offset;
        float64 Psi = Alf + n * Bet - 27.5;
        float64 f = 0.249 + 0.86 * ((cos(Psi) / (3. - cos(2. * Psi))) +
            (1. / (6. - 4. * cos(2. * (Psi - 30.)))));
        return Ax0 * pow(1.7275, (float64)n) * (Bx0 + f);
    }
    case Polynomial:
    {
        int64 n = _Idx + Offset;
        float64 tet = Alf + n * Bet;
        float64 E1 = 0.396 * pow(cos(tet - 27.4), 1);
        float64 E2 = 0.168 * pow(cos(tet - 60.4), 2);
        float64 E3 = 0.062 * pow(cos(tet - 28.1), 3);
        float64 E4 = 0.053 * pow(cos(tet - 77.2), 4);
        float64 E5 = 0.009 * pow(cos(tet - 22.0), 5);
        float64 E7 = 0.012 * pow(cos(tet - 40.4), 7);
        float64 f = 0.4594 + E1 + E2 + E3 + E4 + E5 + E7;
        return Ax0 * pow(1.7275, (float64)n) * (Bx0 + f);
    }
    }
    return 0; // Unreachable, make compiler happy - _ -
}

/**
 * @brief Orbital resonance model based on Dermott's period law
 * @link Wikipedia. Dermott's law
 * https://en.wikipedia.org/wiki/Dermott%27s_law
 */
float64 Dermott_Period_Array::operator[](int64 _Idx)
{
    return Tx0 * pow(Cx0, (float64)_Idx);
}

/**
 * @brief Another exponential fittong model for Extrasolar system
 * @link Reference:
 * Poveda, Arcadio & Lara, Patricia (2008). "The
 * exo-planetary system of 55 Cancri and the
 * Titus–Bode law". Revista Mexicana de
 * Astronomía y Astrofísica (44): 243–246.
 */
float64 Exponential_Fitting_Array::operator[](int64 _Idx)
{
    return Cx0 * exp(Cx1 * _Idx);
}

_ORBIT_END
_CSE_END
