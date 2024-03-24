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

class __Real_Time_Satellite_Tracker_And_Predictor
{
public:
    using OrbitElemsType = Object::OrbitParams;

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
    OrbitElemsType _M_OrbitElems;
    float64        _M_Epoch;

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
