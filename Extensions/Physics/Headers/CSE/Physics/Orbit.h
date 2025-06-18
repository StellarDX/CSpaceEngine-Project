/*
    轨道工具系列  Orbit engine
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

struct OrbitElems
{
    float64   Epoch           = _NoDataDbl;
    float64   GravParam       = _NoDataDbl;
    float64   PericenterDist  = _NoDataDbl;
    float64   Period          = _NoDataDbl;
    float64   Eccentricity    = _NoDataDbl;
    Angle     Inclination     = _NoDataDbl;
    Angle     AscendingNode   = _NoDataDbl;
    Angle     ArgOfPericenter = _NoDataDbl;
    Angle     MeanAnomaly     = _NoDataDbl;

    OrbitElems() {}

    OrbitElems(const Object::OrbitParams& P)
    {
        Epoch           = P.Epoch;
        GravParam       = P.GravParam;
        PericenterDist  = P.PericenterDist;
        Period          = P.Period;
        Eccentricity    = P.Eccentricity;
        Inclination     = Angle::FromDegrees(P.Inclination);
        AscendingNode   = Angle::FromDegrees(P.AscendingNode);
        ArgOfPericenter = Angle::FromDegrees(P.ArgOfPericenter);
        MeanAnomaly     = Angle::FromDegrees(P.MeanAnomaly);
    }

    operator Object::OrbitParams()
    {
        return
        {
            .Epoch           = Epoch,
            .Period          = Period,
            .PericenterDist  = PericenterDist,
            .GravParam       = GravParam,
            .Eccentricity    = Eccentricity,
            .Inclination     = Inclination.ToDegrees(),
            .AscendingNode   = AscendingNode.ToDegrees(),
            .ArgOfPericenter = ArgOfPericenter.ToDegrees(),
            .MeanAnomaly     = MeanAnomaly.ToDegrees(),
        };
    }
};

struct OrbitState
{
    ustring   RefPlane       = _NoDataStr;       // Reference System
    float64   GravParam      = _NoDataDbl;       // Gravity Parameter (G*M)
    float64   Time           = _NoDataDbl;       // Julian date
    vec3      Position       = vec3(_NoDataDbl); // XYZ-pos in meters
    vec3      Velocity       = vec3(_NoDataDbl); // Velocity in m/s (m/day in SE)
};

/****************************************************************************************\
*                                    Satellite Tracker                                   *
\****************************************************************************************/

/**
 * @brief 物体跟踪器，一切轨道工具的地基
 */
class SatelliteTracker
{
public:
    using OrbitElemType  = OrbitElems;
    using OrbitStateType = OrbitState;

protected:
    OrbitStateType InitialState;
    OrbitStateType CurrentState;

    OrbitElemType CheckParams(const OrbitElemType& InitElems);
    bool KeplerCompute(OrbitElemType& InitElems);

public:
    SatelliteTracker(const OrbitElemType& InitElems);
    SatelliteTracker(const OrbitStateType& InitState);

    void AddMsecs(int64 Ms);
    void AddSeconds(int64 Sec);
    void AddHours(int64 Hrs);
    void AddDays(int64 Days);
    void AddYears(int64 Years);
    void AddCenturies(int64 Centuries);

    void ToCurrentDate();
    void SetDate(CSEDateTime DateTime);
    void SetDate(float64 JD);
    void Reset();

    OrbitElemType KeplerianElems()const;
    OrbitState State()const;

    CSEDateTime Epoch()const;
    float64 EpochJD()const;
    float64 Aphelion()const;
    float64 Perihelion()const;
    float64 SemiMajorAxis()const;
    float64 MeanMotion()const;
    float64 Eccentricity()const;
    float64 SiderealOrbitalPeriod()const;
    float64 MeanAnomaly()const;
    float64 MeanLongitude()const;
    float64 Inclination()const;
    float64 LongitudeOfAscendingNode()const;
    CSEDateTime TimeOfPerihelion()const;
    float64 ArgumentOfPerihelion()const;
    float64 LongitudeOfPerihelion()const;
    float64 EccentricAnomaly()const;
    float64 TrueAnomaly()const;
};

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
