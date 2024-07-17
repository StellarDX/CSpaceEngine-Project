/*
    Equation of states
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

#ifndef _EOS_
#define _EOS_

#include "CSE/Base.h"

#define _EOS_BEGIN namespace EOS {
#define _EOS_END }
#define _EOS EOS::

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
_EOS_BEGIN

/****************************************************************************************\
*                                      EOS Templates                                     *
\****************************************************************************************/

/**
 * @brief Rose-Vinet equation of state template.
 * @link https://en.wikipedia.org/wiki/Rose%E2%80%93Vinet_equation_of_state
 */
typedef class __Rose_Vinet_EOS_Template
{
public:
    float64 _K0;
    float64 _K1;
    float64 _Rho0;

    // Newton iterator config
    float64 _NewtonMaxIterLog = 3;
    float64 _NewtonTolarNLog  = 8;

    __Rose_Vinet_EOS_Template(float64 _Kx0, float64 _Kx1, float64 _Rx0)
        : _K0(_Kx0), _K1(_Kx1), _Rho0(_Rx0) {}

    float64 K0()const { return _K0; }
    float64 K1()const { return _K1; }
    float64 Rho0()const { return _Rho0; }

protected:
    float64 Pressure(float64 Density);
    float64 dPressure(float64 Density); // derivate function

public:

    /**
     * @param Pressure in Pascal
     * @return Density in Kg/m^3
     */
    float64 operator()(float64 Pressure);
}RoseVinetEOS;

/**
 * @brief Birch-Murnaghan equation of state template.
 * @link https://en.wikipedia.org/wiki/Birch%E2%80%93Murnaghan_equation_of_state
 */
typedef class __Birch_Murnaghan_Isothermal_EOS_Template
{
public:
    bool    _4TH = 0;
    float64 _K0;
    float64 _K1;
    float64 _K2;
    float64 _Rho0;

    // Newton iterator config
    float64 _NewtonMaxIterLog = 3;
    float64 _NewtonTolarNLog  = 8;

    __Birch_Murnaghan_Isothermal_EOS_Template(float64 _Kx0, float64 _Kx1, float64 _Rx0)
        : _4TH(0), _K0(_Kx0), _K1(_Kx1), _K2(0), _Rho0(_Rx0) {}
    // __Birch_Murnaghan_Isothermal_EOS_Template(float64 _Kx0, float64 _Kx1, float64 _Kx2, float64 _Rx0)
    //     : _4TH(1), _K0(_Kx0), _K1(_Kx1), _K2(_Kx2), _Rho0(_Rx0) {}

    float64 K0()const { return _K0; }
    float64 K1()const { return _K1; }
    float64 K2()const { return _K2; }
    float64 Rho0()const { return _Rho0; }

//protected:
    float64 Pressure(float64 Density);
    float64 dPressure(float64 Density); // derivate function

public:

    /**
     * @param Pressure in Pascal
     * @return Density in Kg/m^3
     */
    float64 operator()(float64 Pressure);
}BirchMurnaghanEOS;

extern const float64 __TFD_GAMMA_TABLE[20];

/**
 * @brief Thomas-Fermi-Dirac method for huge-pressure.
 * In very huge pressure(1E13 dyne/cm^2, 1TPa), interaction forces between atoms
 * become less important. So matter's pressure-density relation will only affect
 * by relative atomic mass and atomic number. But this method is not accurate in
 * normal pressure because of crystallizations and chemical bonds.
 * @link Reference: E. E. SALPZTXR AND H. S. ZAPOISKV (Received 9 February 1967)
 * "Theoretical High-Pressure Equations of State including Correlation Energy"
 * https://lweb.cfa.harvard.edu/~lzeng/papers/salpeter_zapolsky_1967.pdf
 */
typedef class __Thomas_Fermi_Dirac_Huge_Pressure_Model
{
public:
    float64 AtomicWeight;
    float64 NProton;

    __Thomas_Fermi_Dirac_Huge_Pressure_Model(float64 _Ax0, float64 _Zx0)
        : AtomicWeight(_Ax0), NProton(_Zx0) {}

    float64 A()const { return AtomicWeight; }
    float64 Z()const { return NProton; }

protected:
    /**
     * @brief Unit of the input value is dyne/cm^2, which equals 0.1 pa.
     * and return value is g/cm^3, which equals 1000 Kg/m^3
     */
    float64 __Density(float64 Pressure);

public:

    /**
     * @param Pressure in Pascal
     * @return Density in Kg/m^3
     */
    float64 operator()(float64 Pressure);
}TFDHugePressureEOS;

/**
 * @brief Fast exponential EOS model.
 * @link Reference: S. Seager, M. Kuchner, C. A. Hier-Majumder, and B. Militzer
 * "MASS-RADIUS RELATIONSHIPS FOR SOLID EXOPLANETS" https://arxiv.org/abs/0707.2895
 */
typedef class __Exponential_Fit_EOS_Template
{
public:
    float64 _Rho0;
    float64 _Cx0;
    float64 _Power;

    __Exponential_Fit_EOS_Template(float64 Rx0, float64 Cx0, float64 Px0)
        : _Rho0(Rx0), _Cx0(Cx0), _Power(Px0) {}

    float64 Rho0()const { return _Rho0; }
    float64 C()const { return _Cx0; }
    float64 n()const { return _Power; }

    /**
     * @param Pressure in Pascal
     * @return Density in Kg/m^3
     */
    float64 operator()(float64 Pressure);
}ExponentialEOS;



/****************************************************************************************\
*                                          EOSs                                          *
\****************************************************************************************/

// ---------- Parameters for the Vinet ( V ) or Birch-Murnagham (BME) EOS Fits ---------- //
extern const BirchMurnaghanEOS Carbon_BME_Graphite;
extern const BirchMurnaghanEOS Ferrum_BME_Alpha;
extern const RoseVinetEOS      Ferrum_Vinet_Epsilon;
extern const BirchMurnaghanEOS FeS_BME;
extern const BirchMurnaghanEOS H2O_BME_IceVII;
extern const BirchMurnaghanEOS MgO_BME;
extern const BirchMurnaghanEOS MgSiO3_BME_en;
extern const BirchMurnaghanEOS MgSiO3_BME_pv;
extern const BirchMurnaghanEOS MgFeSiO3_BME_pv;
extern const BirchMurnaghanEOS SiC_BME;

// ---------- Fits to the Merged Vinet/BME and TFD EOS of the Form Rho(P) = Rho0 + c * P^n ---------- //
extern const ExponentialEOS Ferrum_Exponential_Epsilon;
extern const ExponentialEOS MgSiO3_Exponential_Perovskite;
extern const ExponentialEOS MgFeSiO3_Exponential;
extern const ExponentialEOS H2O_Exponential_IceVII;
extern const ExponentialEOS Carbon_Exponential_Graphite;
extern const ExponentialEOS SiC_Exponential;

// ---------- TFD EOSs ---------- //
extern const TFDHugePressureEOS Bismuth_TFD;
extern const TFDHugePressureEOS Plumbum_TFD;
extern const TFDHugePressureEOS Aurum_TFD;
extern const TFDHugePressureEOS Galena_TFD;
extern const TFDHugePressureEOS Stannum_TFD;
extern const TFDHugePressureEOS Cadmium_TFD;
extern const TFDHugePressureEOS Argentum_TFD;
extern const TFDHugePressureEOS Zinc_TFD;
extern const TFDHugePressureEOS Cuprum_TFD;
extern const TFDHugePressureEOS Nickel_TFD;
extern const TFDHugePressureEOS Ferrum_TFD;
extern const TFDHugePressureEOS Sphalerite_TFD;
extern const TFDHugePressureEOS Troilite_TFD;
extern const TFDHugePressureEOS Pyrite_TFD;
extern const TFDHugePressureEOS Wustite_TFD;
extern const TFDHugePressureEOS Oldhamite_TFD;
extern const TFDHugePressureEOS Magnetite_TFD;
extern const TFDHugePressureEOS Hematite_TFD;
extern const TFDHugePressureEOS Fayalite_TFD;
extern const TFDHugePressureEOS Rutile_TFD;
extern const TFDHugePressureEOS Andratite_TFD;
extern const TFDHugePressureEOS Pyrope_TFD;
extern const TFDHugePressureEOS Grossularite_TFD;
extern const TFDHugePressureEOS Diopside_TFD;
extern const TFDHugePressureEOS Anorthite_TFD;
extern const TFDHugePressureEOS Orthoclase_TFD;
extern const TFDHugePressureEOS Corundum_TFD;
extern const TFDHugePressureEOS Spinel_TFD;
extern const TFDHugePressureEOS Jadeite_TFD;
extern const TFDHugePressureEOS Albite_TFD;
extern const TFDHugePressureEOS Quartz_TFD;
extern const TFDHugePressureEOS Forsterite_TFD;
extern const TFDHugePressureEOS Enstatite_TFD;
extern const TFDHugePressureEOS Periclase_TFD;
extern const TFDHugePressureEOS Spodumene_TFD;
extern const TFDHugePressureEOS Beryl_TFD;


/****************************************************************************************\
*                                       Materials                                        *
\****************************************************************************************/

__interface Material
{
    virtual ustring MaterialName()const = 0;
    virtual float64 BaseDensity()const = 0;
    virtual float64 Density(float64 Pressure)const = 0;
    virtual float64 MeltingCurve(float64 Pressure)const = 0;
};

_EOS_END
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
