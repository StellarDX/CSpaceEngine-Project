/*
    Equation of state Templates
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

#include "CSE/Base/ConstLists.h"
#include "CSE/Physics/Materials.h"

_CSE_BEGIN
_EOS_BEGIN

///////////////////////////////////VINET///////////////////////////////////

float64 __Rose_Vinet_EOS_Template::Pressure(float64 Density)
{
    float64 Eta = Density / _Rho0;
    return 3. * _K0 * cbrt(Eta * Eta) * (1. - (1. / cbrt(Eta))) *
           exp(1.5 * (_K1 - 1.) * (1. - (1. / cbrt(Eta))));
}

float64 __Rose_Vinet_EOS_Template::dPressure(float64 Density) // derivate function
{
    float64 Eta = Density / _Rho0;
    return ((((_K0 * _Rho0) * cbrt(Eta * Eta)) * yroot(Eta, -3)) *
        exp(((1.5 * (_K1 - 1)) * (-1. / cbrt(Eta) + 1))) / ((_Rho0 * Density))) +
        ((((((_K0 * _Rho0 * _Rho0) * cbrt(Eta * Eta)) * exp(((1.5 * (_K1 - 1)) *
        (-1. / cbrt(Eta) + 1)))) * ((-yroot(Eta, -3)) + 1)) * 2) * Density /
        (_Rho0 * _Rho0) / (Density * Density)) + (((((((1.5 * _K0) * _Rho0) *
        cbrt(Eta * Eta)) * yroot(Eta, -3)) * exp(((1.5 * (_K1 - 1)) *
        (-1. / cbrt(Eta) + 1)))) * (_K1 - 1)) * ((-yroot(Eta, -3)) + 1) /
        ((_Rho0 * Density))); // Expression is too long to display...
}

float64 __Rose_Vinet_EOS_Template::operator()(float64 _Pressure)
{
    NewtonIterator It([this](float64 Rho){return Pressure(Rho);},
        [this](float64 Rho){return dPressure(Rho);});
    It._M_MaxIterLog = _NewtonMaxIterLog;
    It._M_TolerNLog = _NewtonTolarNLog;
    return It(_Pressure, _Rho0);
}

///////////////////////////////////BME///////////////////////////////////

float64 __Birch_Murnaghan_Isothermal_EOS_Template::Pressure(float64 Density)
{
    float64 Eta = Density / _Rho0;
    float64 Eta2 = Eta * Eta;
    float64 Eta5 = Eta2 * Eta2 * Eta;
    float64 Eta7 = Eta5 * Eta2;
    return 1.5 * _K0 * (cbrt(Eta7) - cbrt(Eta5)) * (1. + 0.75 * (_K1 - 4.) * (cbrt(Eta2) - 1.));
}

float64 __Birch_Murnaghan_Isothermal_EOS_Template::dPressure(float64 Density)
{
    float64 Eta = Density / _Rho0;
    float64 Eta2 = Eta * Eta;
    float64 Eta5 = Eta2 * Eta2 * Eta;
    float64 Eta7 = Eta5 * Eta2;
    return (((1.5 * _K0) * ((-0x1.aaaaaaaaaaaabp+0 * cbrt(Eta5) / Density) +
        (0x1.2aaaaaaaaaaabp+1 * cbrt(Eta7) / Density))) * (((0.75 * (_K1 - 4)) *
        (cbrt(Eta2) - 1)) + 1)) + ((((0.75 * _K0) * cbrt(Eta2)) * (_K1 - 4)) *
        ((-cbrt(Eta5)) + cbrt(Eta7)) / Density);
}

float64 __Birch_Murnaghan_Isothermal_EOS_Template::operator()(float64 _Pressure)
{
    NewtonIterator It([this](float64 Rho){return Pressure(Rho);},
        [this](float64 Rho){return dPressure(Rho);});
    It._M_MaxIterLog = _NewtonMaxIterLog;
    It._M_TolerNLog = _NewtonTolarNLog;
    return It(_Pressure, _Rho0);
}

///////////////////////////////////TFD///////////////////////////////////

const float64 __TFD_GAMMA_TABLE[] =
{
    +1.512E-2, +8.955E-2, +1.090E-1, +5.089E+0, -5.980E+0,
    +2.181E-3, -4.015E-1, +1.698E+0, -9.566E+0, +9.873E+0,
    -3.328E-4, +5.167E-1, -2.369E+0, +1.349E+1, -1.427E+1,
    -1.384E-2, -6.520E-1, +3.529E+0, -2.095E+1, +2.264E+1
};

float64 __Thomas_Fermi_Dirac_Huge_Pressure_Model::Density(float64 Pressure)
{
    float64 eps = cbrt(3. / (32. * pow(CSE_PI, 2) * pow(NProton, 2)));
    float64 phi = cbrt(3.) / 20. + eps / 4.;

    float64 x00 = 1. / (8.884E-3 + sqrt(eps) * 4.988E-1 + eps * 5.2604E-1);

    float64 alf = 1. / (1.941E-2 - sqrt(eps) * 6.277E-2 + eps * 1.076);
    float64 bet[6];
    bet[0] = x00 * phi - 1;
    bet[1] = bet[0] * alf + ((1. + bet[0]) / phi);
    const float64* gam = __TFD_GAMMA_TABLE;
    for (size_t i = 2; i <= 5; i++)
    {
        size_t j = (i - 2) * 5;
        bet[i] = 1. / pow(gam[j] + gam[j + 1] * pow(eps, 1. / 2.) + gam[j + 2] * eps + gam[j + 3] * pow(eps, 3. / 2.) + gam[j + 4] * pow(eps, 2), float64(i));
    }

    float64 ksi = pow(Pressure / 9.524E+13, 1. / 5.) * pow(NProton, -2. / 3.);
    float64 x0ksi = (1. + exp(-alf * ksi) * (bet[0] + bet[1] * ksi + bet[2] * pow(ksi, 2) + bet[3] * pow(ksi, 3) + bet[4] * pow(ksi, 4) + bet[5] * pow(ksi, 5))) / (ksi + phi);
    return 3.886 * (AtomicWeight * NProton) / pow(x0ksi, 3);
}

float64 __Thomas_Fermi_Dirac_Huge_Pressure_Model::operator()(float64 Pressure)
{
    return 1000. * Density(Pressure * 10.);
}

///////////////////////////////////EXP///////////////////////////////////

float64 __Exponential_Fit_EOS_Template::operator()(float64 Pressure)
{
    return _Rho0 + _Cx0 * pow(Pressure, _Power);
}

/****************************************************************************************\
*                                        Materials                                       *
\****************************************************************************************/

// ---------- Parameters for the Vinet ( V ) or Birch-Murnagham (BME) EOS Fits ---------- //
// Fit                   Atom or Compound      K0       K1   Rho0
const BirchMurnaghanEOS  Carbon_BME_Graphite  ( 33.8E9, 8.90, 2.25E3);
const BirchMurnaghanEOS  Ferrum_BME_Alpha     (162.5E9, 5.50, 7.86E3);
const RoseVinetEOS       Ferrum_Vinet_Epsilon (156.2E9, 6.08, 8.30E3);
const BirchMurnaghanEOS  FeS_BME              ( 35.0E9, 5.00, 4.77E3);
const BirchMurnaghanEOS  H2O_BME_IceVII       ( 23.7E9, 4.15, 1.46E3);
const BirchMurnaghanEOS  MgO_BME              (177.0E9, 4.00, 3.56E3);
const BirchMurnaghanEOS  MgSiO3_BME_en        (125.0E9, 5.00, 3.22E3);
const BirchMurnaghanEOS  MgFeSiO3_BME_pv      (266.0E9, 3.90, 4.26E3); // 22MgO.3FeO.25SiO2
const BirchMurnaghanEOS  SiC_BME              (227.0E9, 4.10, 3.22E3);

// ---------- Fits to the Merged Vinet/BME and TFD EOS of the Form Rho(P) = Rho0 + c * P^n ---------- //
// These fits are valid for the pressure range P < 1E+16 Pa
// Some typos are found in original paper, the following codes corrected them.
//                   Material                       Rho0  c        n
const ExponentialEOS Ferrum_Exponential_Epsilon    (8300, 0.00349, 0.528);
const ExponentialEOS MgSiO3_Exponential_Perovskite (4100, 0.00161, 0.541);
const ExponentialEOS MgFeSiO3_Exponential          (4260, 0.00127, 0.549);
const ExponentialEOS H2O_Exponential_IceVII        (1460, 0.00311, 0.513);
const ExponentialEOS Carbon_Exponential_Graphite   (2250, 0.00350, 0.514);
const ExponentialEOS SiC_Exponential               (3220, 0.00172, 0.537);

// ---------- Parameters for the TFD Model ---------- //
//                     Mineral or element  A       Z          Note
const TFDHugePressureEOS Bismuth_TFD      (209.00, 83.00); // Bi
const TFDHugePressureEOS Plumbum_TFD      (207.00, 82.00); // Pb
const TFDHugePressureEOS Aurum_TFD        (197.00, 79.00); // Au
const TFDHugePressureEOS Galena_TFD       (119.50, 49.00); // PbS
const TFDHugePressureEOS Stannum_TFD      (119.00, 50.00); // Sn
const TFDHugePressureEOS Cadmium_TFD      (112.00, 48.00); // Cd
const TFDHugePressureEOS Argentum_TFD     (108.00, 47.00); // Ag
const TFDHugePressureEOS Zinc_TFD         ( 65.00, 30.00); // Zn
const TFDHugePressureEOS Cuprum_TFD       ( 64.00, 29.00); // Cu
const TFDHugePressureEOS Nickel_TFD       ( 59.00, 28.00); // Ni
const TFDHugePressureEOS Ferrum_TFD       ( 56.00, 26.00); // Fe
const TFDHugePressureEOS Sphalerite_TFD   ( 48.50, 23.00); // ZnS
const TFDHugePressureEOS Troilite_TFD     ( 44.00, 21.00); // FeS
const TFDHugePressureEOS Pyrite_TFD       ( 38.00, 16.50); // FeS2
const TFDHugePressureEOS Wustite_TFD      ( 36.00, 17.00); // FeO
const TFDHugePressureEOS Oldhamite_TFD    ( 36.00, 18.00); // CaS
const TFDHugePressureEOS Magnetite_TFD    ( 33.12, 15.70); // Fe3O4
const TFDHugePressureEOS Hematite_TFD     ( 32.00, 15.20); // Fe2O3
const TFDHugePressureEOS Fayalite_TFD     ( 29.16, 14.01); // 2FeO.SiO2
const TFDHugePressureEOS Rutile_TFD       ( 26.66, 12.66); // TiO2
const TFDHugePressureEOS Andratite_TFD    ( 25.40, 12.50); // 3CaO.Fe2O3.3SiO2
const TFDHugePressureEOS Pyrope_TFD       ( 25.01, 12.15); // 3MgO.Al2O3.3SiO2
const TFDHugePressureEOS Grossularite_TFD ( 22.50, 11.20); // 3CaO.Al2O3.3SiO2
const TFDHugePressureEOS Diopside_TFD     ( 21.60, 10.80); // MgO.CaO.2SiO2
const TFDHugePressureEOS Anorthite_TFD    ( 21.39, 10.62); // CaO.Al2O3.2SiO2
const TFDHugePressureEOS Orthoclase_TFD   ( 21.39, 10.62); // K2O.Al2O3.6SiO2
const TFDHugePressureEOS Corundum_TFD     ( 20.40, 10.00); // Al2O3
const TFDHugePressureEOS Spinel_TFD       ( 20.29, 10.00); // MgO.Al2O3
const TFDHugePressureEOS Jadeite_TFD      ( 20.20, 10.00); // Na2O.Al2O3.4SiO2
const TFDHugePressureEOS Albite_TFD       ( 20.16, 10.00); // Na2O.Al2O3.6SiO2
const TFDHugePressureEOS Quartz_TFD       ( 20.00, 10.00); // SiO2
const TFDHugePressureEOS Forsterite_TFD   ( 20.00, 10.00); // 2MgO.SiO2
const TFDHugePressureEOS Enstatite_TFD    ( 20.00, 10.00); // MgO.SiO2
const TFDHugePressureEOS Periclase_TFD    ( 20.00, 10.00); // MgO
const TFDHugePressureEOS Spodumene_TFD    ( 18.60,  9.20); // Li2O.Al2O3.4SiO2
const TFDHugePressureEOS Beryl_TFD        ( 18.52,  9.17); // 3BeO.Al2O3.6SiO2

_EOS_END
_CSE_END
