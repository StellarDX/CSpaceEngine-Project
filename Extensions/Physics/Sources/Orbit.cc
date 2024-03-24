/*
    Orbit functions
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
#include "CSE/Base/ConstLists.h"

using namespace _CSE linalg;

_CSE_BEGIN
_ORBIT_BEGIN

void __Fix_Angle(float64* _Angle, vec3 _Ref1, vec3 _Ref2)
{
    if (dot(_Ref1, _Ref2) > 0)
    {
        if (*_Angle < 0) {*_Angle += 180;}
        //else if (*_Angle >= 180) {*_Angle -= 180;}
    }
    else
    {
        if (*_Angle < 0) {*_Angle = 180 - *_Angle;}
        else if (*_Angle < 180) {*_Angle = 360 - *_Angle;}
    }
}

KeplerianOrbitElems StateVectorsToKeplerianElements(OrbitStateType State, PSatelliteTracker Tracker)
{
    // Convert SpaceEngine coordinate to realistic coordinate
    #if defined(SATELLITE_TRACKER_COORD_SpaceEngine)
    State.Position.z = -State.Position.z;
    std::swap(State.Position.y, State.Position.z);
    State.Velocity.z = -State.Velocity.z;
    std::swap(State.Velocity.y, State.Velocity.z);
    #endif

    KeplerianOrbitElems Orbit;
    Orbit.RefPlane = State.RefPlane;
    Orbit.Epoch = State.Time;
    float64 Distance = FrobeniusNorm(State.Position);
    float64 Velocity = FrobeniusNorm(State.Velocity);
    float64 SemiMajorAxis = 1. / ((2. / Distance) - (Velocity * Velocity / State.GravParam));

    // Semi-Major Axis >= 0 -> Elliptical orbit
    // Semi-Major Axis == inf -> Parabolic orbit
    // Semi-Major Axis < 0 -> Hyperbolic orbit

    vec3 EccentricityVec = (1. / State.GravParam) *
        (((Velocity * Velocity) - (State.GravParam / Distance)) * State.Position -
        dot(State.Position, State.Velocity) * State.Velocity);

    // if length(EccentricityVec) == 0, orbit is circle
    // Then calculate angular momentum

    vec3 AngularMomentum = cross(State.Position, State.Velocity);

    if (isinf(SemiMajorAxis))
    {
        Orbit.PericenterDist = pow(FrobeniusNorm(AngularMomentum), 2) / State.GravParam;
        Orbit.Eccentricity = 1;
    }
    else
    {
        float64 Eccentricity = FrobeniusNorm(EccentricityVec);
        if (abs(Eccentricity) < 1e-13) {Eccentricity = 0;} // if zero, orbit is circle orbit.
        Orbit.PericenterDist = SemiMajorAxis - SemiMajorAxis * Eccentricity;
        Orbit.Eccentricity = Eccentricity;
    }

    vec3 ZAxis = vec3(0, 0, 1);
    Orbit.Inclination = arccos(dot(ZAxis, AngularMomentum) / FrobeniusNorm(AngularMomentum));
    if (Orbit.Inclination < 0) {Orbit.Inclination += 180;}

    // Calculate ascending line
    vec3 AscendingLine = cross(ZAxis, AngularMomentum);

    vec3 XAxis = vec3(1, 0, 0), YAxis = vec3(0, 1, 0);
    Orbit.AscendingNode = arccos(dot(AscendingLine, XAxis) / FrobeniusNorm(AscendingLine));
    // if dot(YAxis, AscendingLine) > 0, AscendingNode < 180
    __Fix_Angle(&Orbit.AscendingNode, YAxis, AscendingLine);

    if (Orbit.Eccentricity == 0)
    {
        // If orbit is circle, Argument of perihelion and mean anomaly
        // is undefined, calculate Argument of latitude instrad and
        // store it into Argument of perihelion, and set Mean anomaly 0.
        Orbit.ArgOfPericenter = arccos(dot(AscendingLine, State.Position)
            / (FrobeniusNorm(AscendingLine) * Distance));
        // if dot(Position, ZAxis) > 0, ArgOfPericenter < 180
        __Fix_Angle(&Orbit.ArgOfPericenter, State.Position, ZAxis);
        Orbit.MeanAnomaly = 0;
    }
    else
    {
        Orbit.ArgOfPericenter = arccos(dot(AscendingLine, EccentricityVec)
            / (FrobeniusNorm(AscendingLine) * Orbit.Eccentricity));
        // if dot(ZAxis, EccentricityVec) > 0, ArgOfPericenter < 180
        __Fix_Angle(&Orbit.ArgOfPericenter, ZAxis, EccentricityVec);

        float64 TrueAnomaly = arccos(dot(EccentricityVec, State.Position) /
            (Orbit.Eccentricity * Distance));
        // if dot(Position, Velocity) > 0, TrueAnomaly < 180
        __Fix_Angle(&TrueAnomaly, State.Position, State.Velocity);

        // Convert true anomaly to mean anomaly
        float64 EccentricAnomaly = Arctan2(sqrt(1. - Orbit.Eccentricity * Orbit.Eccentricity) *
            sin(TrueAnomaly), Orbit.Eccentricity + cos(TrueAnomaly));
        if (EccentricAnomaly < 0) {EccentricAnomaly += 360;}
        float64 EccentricAnomalyRad = Angle(EccentricAnomaly).ToRadians();
        Orbit.MeanAnomaly = Angle::FromRadians(EccentricAnomalyRad -
            Orbit.Eccentricity * __IBM_SIN64F(EccentricAnomalyRad)).ToDegrees();
    }

    if (Tracker) {*Tracker = SatelliteTracker(Orbit, State.GravParam / GravConstant);}
    return Orbit;
}

_ORBIT_END
_CSE_END
