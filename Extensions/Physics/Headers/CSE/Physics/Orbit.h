/*
    Orbit engine for CSpaceEngine
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

#ifndef _CSE_ORBIT_
#define _CSE_ORBIT_

#include "CSE/Base.h"
#include "CSE/Object.h"

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

#define _ORBIT_BEGIN namespace Orbit {
#define _ORBIT_END }
#define _ORBIT Orbit::

_CSE_BEGIN
_ORBIT_BEGIN

class OrbitCalculateException : public std::logic_error
{
public:
    OrbitCalculateException(char const* e) : std::logic_error(e) {}
};

using KeplerianOrbitElems = Object::OrbitParams;

/****************************************************************************************\
*                                    Satellite Tracker                                   *
\****************************************************************************************/

typedef class __Real_Time_Satellite_Tracker_And_Predictor
{
public:
    using OrbitElemsType = KeplerianOrbitElems;

    /**
     * @brief Real-time state of a object.
     */
    struct OrbitState
    {
        ustring   RefPlane       = _NoDataStr;       // Reference System
        float64   GravParam      = _NoDataDbl;       // Gravity Parameter (G*M)
        float64   Time           = _NoDataDbl;       // Julian date
        vec3      Position       = vec3(_NoDataDbl); // XYZ-pos in meters
        vec3      Velocity       = vec3(_NoDataDbl); // Velocity in m/s (m/day in SE)
    };

protected:
    OrbitElemsType _M_OrbitElems; // Store orbit-elements
    float64        _M_Epoch;      // Current time

    void _Check_Params();
    void _GetCenterObjectMass();
    float64 _EccentricAnomaly(float64 _MeanAnomaly) const;
    float64 _TrueAnomaly(float64 _EccAnomaly) const;
    float64 _SemiMajorAxis() const;
    float64 _SemiLatusRectum() const;
    OrbitState _Gen(float64 _TAnomaly)const;

public:
    __Real_Time_Satellite_Tracker_And_Predictor
        (OrbitElemsType const& Orbit, float64 ParentMass = _NoDataDbl);
    __Real_Time_Satellite_Tracker_And_Predictor
        (Object const& Obj, float64 ParentMass = _NoDataDbl);

    void AddMsecs(int64 Ms);
    void AddSeconds(int64 Sec);
    void AddHours(int64 Hrs);
    void AddDays(int64 Days);
    void AddYears(int64 Years);
    void AddCenturies(int64 Centuries);

    void ToCurrentDate();
    void SetDate(CSEDateTime DateTime);
    void SetDateJD(float64 JD);
    void Reset();

    float64 MeanMotion()const;
    float64 MeanAnomaly()const;
    float64 EccentricAnomaly()const;
    float64 TrueAnomaly()const;
    OrbitElemsType OrbitElems()const;
    CSEDateTime Epoch()const;
    float64 EpochJD()const;
    OrbitState State()const;

    float64 CurrentMeanAnomaly()const;
    float64 CurrentEccentricAnomaly()const;
    float64 CurrentTrueAnomaly()const;
    OrbitElemsType CurrentOrbitElems()const;
    CSEDateTime CurrentDateTime()const;
    float64 CurrentDateTimeJD()const;
    OrbitState CurrentState()const;
}SatelliteTracker, *PSatelliteTracker;

using OrbitStateType = SatelliteTracker::OrbitState;



/****************************************************************************************\
*                                       Convertors                                       *
\****************************************************************************************/

/**
 * @brief Convert orbit state vectors to Keplerian orbit parameters
 * @note only states generated by SatelliteTracker can return result correctly.
 * @param State State vectors
 * @param Tracker (Optional) also create a tracker for result.
 * @return Keplerian orbit elements
 */
KeplerianOrbitElems StateVectorsToKeplerianElements(OrbitStateType State, PSatelliteTracker Tracker = nullptr);

/**
 * @brief Autometically convert a binary orbit to keplerian elements and fill into itself.
 * @param Par Input params
 */
void BinaryOrbitToKeplerianElems(KeplerianOrbitElems* Par);



/****************************************************************************************\
*                                     Orbital models                                     *
\****************************************************************************************/

struct __General_Titius_Bode_Array
{
    float64 Base;
    float64 Scale;
    float64 operator[](int64 _Idx);
};

static const __General_Titius_Bode_Array SolarSys_TBL{.Base = 0.4, .Scale = 0.3};

struct __Blagg_Titius_Bode_Array
{
    enum {Trigonometric, Polynomial} Method = Trigonometric;
    float64 Ax0;
    float64 Bx0;
    float64 Alf;
    float64 Bet;
    int64   Offset;
    float64 operator[](int64 _Idx);
};

static const __Blagg_Titius_Bode_Array SolarSys_BLF
    {.Ax0 = 0.4162, .Bx0 = 2.0250, .Alf = 112.4, .Bet = 56.6, .Offset = -2};
static const __Blagg_Titius_Bode_Array Jupiter_BLF
    {.Ax0 = 0.4523, .Bx0 = 1.8520, .Alf = 113.0, .Bet = 36.0, .Offset = -2};
static const __Blagg_Titius_Bode_Array Saturn_BLF
    {.Ax0 = 3.0740, .Bx0 = 0.0071, .Alf = 118.0, .Bet = 10.0, .Offset = -3};
static const __Blagg_Titius_Bode_Array Uranus_BLF
    {.Ax0 = 2.9800, .Bx0 = 0.0805, .Alf = 125.7, .Bet = 12.5, .Offset = -2};

struct Dermott_Period_Array
{
    float64 Tx0;
    float64 Cx0;
    float64 operator[](int64 _Idx);
};

static const Dermott_Period_Array Jovian_DML    {.Tx0 = 0.444, .Cx0 = 2.03};
static const Dermott_Period_Array Saturnian_DML {.Tx0 = 0.462, .Cx0 = 1.59};
static const Dermott_Period_Array Uranian_DML   {.Tx0 = 0.760, .Cx0 = 1.80};

struct Exponential_Fitting_Array
{
    float64 Cx0;
    float64 Cx1;
    float64 operator[](int64 _Idx);
};

static const Exponential_Fitting_Array Rho1Cnc_Exp{.Cx0 = 0.0142, .Cx1 = 0.9975};



/****************************************************************************************\
*                                  Two-Line element set                                  *
\****************************************************************************************/

/**
 * @brief Two-line element set decoder
 */
class TLE
{
public:
    using TLELine = char[69];
    using TLEData = TLELine[3];

    struct SpacecraftBasicData // Basic data of the spacecraft, in first line.
    {
        uint32_t    CatalogNumber;
        std::string Classification;

        struct COSPAR_ID
        {
            int32_t     LaunchYear;
            uint32_t    LaunchNumber;
            std::string LaunchPiece;
        }IntDesignator;

        float64     D1MeanMotion;
        float64     D2MeanMotion;
        float64     BSTAR;
        uint32_t    EphemerisType;
        uint32_t    ElementSet;
        uint32_t    RevolutionNum;

        std::string COSPAR()const;
        std::string ToString()const;
    };

    TLEData _M_Data;

    bool IsValid()const;

    std::string Title()const;
    SpacecraftBasicData BasicData()const;
    KeplerianOrbitElems Orbit()const;

    static TLE FromString(std::string _Input);
};



/****************************************************************************************\
*                                Multiple-object problems                                *
\****************************************************************************************/

/**
 * @brief Calculate roche limit of two objects
 * @param Primary Primary object
 * @param Companion Companion object
 * @param Mode Mode of function, 0 = Rigid, 1 = Fluid, default is 0
 */
float64 RocheLimit(const Object& Primary, const Object& Companion, int Mode = 0);

/**
 * @brief Calculate Hill sphere of two objects
 * @param Primary Primary object
 * @param Companion Companion object
 * @return Hill sphere of the companion
 */
float64 HillSphere(const Object& Primary, const Object& Companion);

/**
 * @brief Calculate distances of Lagrange points from the primary body
 * @param PrimaryMass Mass of the primary body
 * @param CompanionMass Mass of the secondary body
 * @param Separation Distance between two bodies
 * @param TolerNLog (Equation Solver error, Newton Iterator error)
 * @return 5 Lagrange point distances
 * @link https://en.wikipedia.org/wiki/Lagrange_point
 */
fvec<5> LagrangePointDistances(float64 PrimaryMass, float64 CompanionMass, float64 Separation, vec2 TolerNLog = vec2(15, 5));

extern const KeplerianOrbitElems __DefOrbitData;
int __cdecl MakeOrbit(Object* Primary, Object* Companion, Object* ThirdAttractor = nullptr, KeplerianOrbitElems Args = __DefOrbitData);
std::shared_ptr<Object> __cdecl MakeBinary(Object* Primary, Object* Companion, Object* ThirdAttractor = nullptr, KeplerianOrbitElems Args = __DefOrbitData);

/**
 * @brief Calculate orbital period using Kepler's 3rd law.
 */
float64 GetPeriodFromPericenterDist(float64 CenterObjMass, float64 PericenterDist, float64 Eccentricity);

/**
 * @brief Calculate Semi-Major Axis using Kepler's 3rd law.
 */
float64 GetSemiMajorAxisFromPeriod(float64 CenterObjMass, float64 Period);

_ORBIT_END
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
