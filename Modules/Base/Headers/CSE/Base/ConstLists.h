﻿#pragma once

// This file include some constants for astronomical calculating
// all values are in SI base unit unless otherwise explicitly stated.

#ifndef _AS_CONSTANTS
#define _AS_CONSTANTS

// Define _USE_CSE_DEFINES before including "CSEConstants.h" to expose these definitions for constants.

//#if defined(_USE_CSE_DEFINES)

//#if !defined(_CSE_MATH_MODULE)
//#include <cmath> // Use STD math when CSE Math module is missing.
//#endif

#define _CONSTS cse::consts::

namespace cse{
namespace consts{

// List1: Math Defines
// -------------------------------------------------------------------------
//      Name          Value                                  Note
#define CSE_E         2.7182818284590452353602874713527l  // e
#define CSE_LBE       1.4426950408889634073599246810019l  // lb(e)
#define CSE_LOGE      0.43429448190325182765112891891661l // log(e)
#define CSE_LN2       0.69314718055994530941723212145818l // ln(2)
#define CSE_LN10      2.3025850929940456840179914546844l  // ln(10)
#define CSE_PI        3.1415926535897932384626433832795l  // Pi
#define CSE_2PI       6.283185307179586476925286766559l   // Pi * 2
#define CSE_PI_D2     1.5707963267948966192313216916398l  // Pi / 2
#define CSE_PI_D4     0.78539816339744830961566084581988l // Pi / 4
#define CSE_1D_PI     0.31830988618379067153776752674503l // 1 / Pi
#define CSE_2D_PI     0.63661977236758134307553505349006l // 2 / Pi
#define CSE_2D_SQRTPI 1.1283791670955125738961589031215l  // 2 / sqrt(Pi)
#define CSE_SQRT2     1.4142135623730950488016887242097l  // sqrt(2)
#define CSE_SQRT1_D2  0.70710678118654752440084436210485l // sqrt(1 / 2)


// List2: Common physic constants
// -------------------------------------------------------------------------
//      Name            Value/Equation          Note
#define GravConstant    6.67430E-11          // 2022 CODATA Value, reference from NIST
#define SpeedOfLight    299792458            // 2022 CODATA Value
#define PPM2Percent     0.0001               // by definition, Parts-per million to percent
#define GasConstant     _CONSTS Equation21   // 2022 CODATA Value
#define AvogadroConst   6.02214076E+23       // 2022 CODATA Value
#define BoltzmannConst  1.380649E-23         // 2022 CODATA Value
#define PlanckConst     6.62607015E-34       // 2022 CODATA Value
#define StBConstant     _CONSTS Equation22   // by definition
#define ElemCharge      1.602176634E-19      // the electric charge carried by a single proton
#define VacPermeability 1.25663706127E-6     // 2022 CODATA Value, the magnetic permeability in a classical vacuum
#define VacPermittivity _CONSTS Equation23   // the value of the absolute dielectric permittivity of classical vacuum
#define FineStructConst _CONSTS Equation24   // 2022 CODATA Value
#define AtomMassConst   1.66053906892E-27    // 2022 CODATA Value
#define StandardAtm     101325               // Standard atmosphere, on sea level
#define StdSurfGravity  9.80665              // Standard Surface garvity of Earth
// -------------------------------------------------------------------------
extern const double Equation21;
extern const double Equation22;
extern const double Equation23;
extern const double Equation24;


// List3: Units of mass
// Reference: https://ssd.jpl.nasa.gov/?planet_phys_par
// -------------------------------------------------------------------------
//      Name              Value                Note
#define ElectronMass      9.1093837139E-31  // 2022 CODATA Value
#define ProtonMass        1.67262192595E-27 // 2022 CODATA Value
#define NeutronMass       1.67492750056E-27 // 2022 CODATA Value
#define LunarMass         7.34579E+22       // DE440
#define EarthMass         5.9721684E+24     // DE440
#define JupiterMass       1.898125E+27      // value from Jovian Satellite ephemeris JUP310
#define SolarMass         1.988475E+30      // IAU 2015 Best estimate value
#define NorminalSolarMass 1.988416E+30      // Given by solving Kepler's third law


// List4: Units of length
// -------------------------------------------------------------------------
//      Name                Value/Equation        Note
#define LunarRadius         1738100            //
#define LunarPolarRadius    1736000            //
#define EarthRadius         6378137            // Equatorial radius provided by IUGG
#define EarthPolarRadius    6356752.31414      // Polar radius provided by GRS 80
#define JupiterRadius       71492000           // IAU 2015 definition
#define JupiterPolarRadius  66854000           // IAU 2015 definition
#define SolarRadius         695660000          // Helioseismic estimates
#define NorminalSolarRadius 695700000          // IAU 2015 definition
#define AU                  149597870700       // IAU 2012 definition
#define LightYear           9460730472580800   // IAU 1976 System of Astronomical Constants
#define Parsec              _CONSTS Equation41 // exact by the 2015 definition
// -------------------------------------------------------------------------
extern const double Equation41;

/*
// List5: Base densities of compositions
// -------------------------------------------------------------------------
//      Name               Simulated Value    Real value    Note
#define FerrumDensity      7874            // 7874        | Near Room temp.
#define FerrumLiqDensity   6980            // 6980        | At melting point
#define HexaferrumDensity  9225            // 10690       |
#define SilicateDensity    3726            // variable    | commonly applied to chemical compounds which are derived from metal oxide and silicon dioxide.
#define MgSiO3Density      3192            // 3192        |
#define Al2SiO5Density     2800            // 2800 - 2900 |
#define ArgonDensity       1400            //             |
#define ArgonSTPDensity    1.784           // 1.784       | At STP
#define ArgonLiqDensity    1395.4          // 1395.4      | At boiling point
#define ArgonGasDensity    1.633           // 1.633       |
#define ArgonTriDensity    1623            // 1623        | At triple point
#define SO2Density         1500            //             |
#define SO2LiqDensity      1434            // 1434        | at 0 °C (liquid); vapor pressure: 2538 mm Hg at 21.1 °C
#define SO2GasDensity      2.619           // 2.619       |
#define OxygenDensity      1360            //             |
#define OxygenLiqDensity   1140            // 1140        | at -183 °C (Boiling point)
#define OxygenGasDensity   1.429           // 1.429       | At STP
#define CO2Density         1600            //             |
#define CO2SolDensity      1562            // 1562        | at 1 atm and −78.5 °C
#define CO2LiqDensity      1101            // 1101        | at saturation −37 °C
#define CO2GasDensity      1.977           // 1101        | at 1 atm and 0 °C
#define IceDensity         916.7           // 916.7       |
#define Water0DCDensity    999.84283       // 999.84283   | 1 atm, VSMOW
#define WaterMaxDensity    999.97495       // 999.97495   | at 3.983035(670) °C, maximum value
#define WaterDensity       997.04702       // 997.04702   | at 25 °C
#define Water95DCDensity   961.88791       // 961.88791   | at 95 °C
#define NitrogenDensity    850             //             |
#define NitrogenLiqDensity 808             // 808         | At boiling point
#define NitrogenGasDensity 1.2506          // 1.2506      | at 0 °C, 1013 mbar
#define NH3Density         860             //             |
#define NH3GasDensity      0.86            // 0.86        | 1.013 bar at boiling point
#define NH3STPDensity      0.769           // 0.769       | At STP
#define NH315DCDensity     0.73            // 0.73        | 1.013 bar at 15 °C
#define NH3LiqDensity      681.9           // 681.9       | at −33.3 °C
#define NH3SolDensity      817             // 817         | at −80 °C (transparent solid)
#define CH4Density         900             //             |
#define CH4GasDensity      0.717           // 0.717       | at 0 °C, 1 atm
#define CH425DCDensity     0.657           // 0.657       | at 25 °C, 1 atm
#define CH4LiqDensity      422.8           // 422.8       | at −162 °C
#define HeliumDensity      125             // 125         | At boiling point
#define HeliumGasDensity   0.1786          // 0.1786      | at STP
#define HydrogenDensity    70.8            //             |
#define HydrogenGasDensity 0.08988         // 0.08988     |
#define HydrogenLiqDensity 70.99           // 70.99       | At boiling point
#define HydrogenSolDensity 76.3            // 76.3        | At melting point
*/

// List6: Units of Time
// -------------------------------------------------------------------------
//      Name          Value                Note
#define SynodicDay    86400             // mean synodic day(Solar day) in seconds(Sunrise to Sunrise), is approx. 24 hours
#define SiderealDay   86164.09053083288 // IERS Value, Earth's rotation period relative to the precessing mean vernal equinox
#define StellarDay    86164.098903691   // IERS Value
#define JulianYear    31557600          // Length of a Julian year, equals 365.25 days
#define TropicalYear  31556925.18747072 // mean tropical year on January 1, 2000, equals 365.2421896698 days
#define SiderealYear  31558149.7635456  // J2000.0 epoch, equals 365.256363004 days
#define J2000         2451545           // Julian day of 2000.01.01 12:00:00.00


// List7: Units of Luminosity
// -------------------------------------------------------------------------
//      Name          Value         Note
#define SolarLum      3.828E+26  // nominal solar luminosity defined by IAU
#define SolarLumBol   3.0128E+28 // zero points of the absolute and apparent bolometric magnitude scales (Mbol_Sun = ~-4.74)

/*
// List8: Molar Masses (in g/mol)
// Values computed by PubChem 2.1 (PubChem release 2021.05.07): https://pubchem.ncbi.nlm.nih.gov/
// -------------------------------------------------------------------------
//      Name          Value        Exact Value
#define CSEM_H2       2.016     // 2.0156500638
#define CSEM_He       4.00260   // 4.002603254
#define CSEM_Ne       20.180    // 19.99244018
#define CSEM_Ar       39.9      // 39.96238312
#define CSEM_Kr       83.80     // 83.91149773
#define CSEM_Xe       131.29    // 131.90415508
#define CSEM_O2       31.999    // 31.989829239
#define CSEM_N2       28.014    // 28.006148008
#define CSEM_CO       28.010    // 27.994914619
#define CSEM_CO2      44.009    // 43.989829239
#define CSEM_SO       48.07     // 47.96698579
#define CSEM_SO2      64.07     // 63.96190041
#define CSEM_Cl2      70.90     // 69.9377054(35Cl2), 73.931805(37Cl2)
#define CSEM_H2S      34.08     // 33.98772124
#define CSEM_H2O      18.015    // 18.010564683
#define CSEM_NH3      17.031    // 17.026549100
#define CSEM_CH4      16.043    // 16.0313001276
#define CSEM_C2H2     26.04     // 26.0156500638
#define CSEM_C2H4     28.05     // 28.0313001276
#define CSEM_C2H6     30.07     // 30.0469501914
#define CSEM_C3H8     44.10     // 44.062600255
#define CSEM_C8H18    114.23    // 114.140850574
*/
/*
// List9: Cosmology
// Can't promise that these values are correct.
// Because I don't know why I want to add these "meaningless" constants.
// Reference:
// https://en.wikipedia.org/wiki/Cosmological_constant
// https://en.wikipedia.org/wiki/Planck_units
// https://en.wikipedia.org/wiki/Hubble%27s_law
// -------------------------------------------------------------------------
//      Name              Value                 Note
#define DiracConstant     _CONSTS Equation81 // reduced Planck constant
#define PlanckLength      _CONSTS Equation82 // shortest measurable distance, approximately the size of a black hole where quantum and gravitational effects are at the same scale
#define PlanckMass        _CONSTS Equation83 // The dividing point between macro scale and micro scale
#define PlanckTime        _CONSTS Equation84 // the time it takes for a photon to travel a distance equal to the Planck length, and is the shortest possible time interval that can be measured.
#define PlanckTemp        _CONSTS Equation85 // At this temperature, the wavelength of light emitted by thermal radiation reaches the Planck length
#define HubbleParam       67.0               // (±1.5 | Format: (Km/s)/Mpc)Value by South Pole Telescope, 2022-12-12, CMB TT/TE/EE power spectrum. Less than 1σ discrepancy with planck. (Preprint)
#define HubbleParamSI     _CONSTS Equation86 // Hubble Parameter in SI (Format: 1/s)
#define CosmoDensityRatio 0.6889             // (±0.0056)ratio between the energy density due to the cosmological constant and the critical density of the universe, according to results published by the Planck Collaboration in 2018
#define CosmoConstant     _CONSTS Equation87 // constant coefficient of a term Albert Einstein temporarily added to his field equations of general relativity
#define StarLowMassLimit  1.4894586875E+29   // Dividing point between Brown dwarf and star, Result from Universe sandbox experiment(78.47MJ)
#define ChandrasekharLim  2.8633968E+30      // maximum mass of a stable white dwarf star (1.44MSun)
#define StarWDUpLimExp    1.590776E+31       // Result from Universe sandbox experiment(8MSun)
#define TOVLimit          4.7126739E+30      // an upper bound to the mass of cold, nonrotating neutron stars
#define StarBHLowLimExp   7.2910566667E+31   // Result from Universe sandbox experiment(36.6667MSun)
#define UpMassGapBH       8.948115E+31       // Stars more massive than this value will remain nothing after supernova explotion.
#define RadObsUniverse    8.8E+26            // Radius of Observable universe(in meters)
#define HubbleLength      _CONSTS Equation88 // The proper radius of a Hubble sphere
// -------------------------------------------------------------------------
extern const double Equation81;
extern const double Equation82;
extern const double Equation83;
extern const double Equation84;
extern const double Equation85;
extern const double Equation86;
extern const double Equation87;
extern const double Equation88;
*/
//#endif

}}

#endif
