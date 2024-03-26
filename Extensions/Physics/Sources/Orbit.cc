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

#define _CRT_SECURE_NO_WARNINGS

#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

using namespace std;
using namespace _CSE linalg;
using namespace _CSE ObjectLiterals;

_CSE_BEGIN
_ORBIT_BEGIN

// 「说出来你们可能不信，尽管我们花了巨量的时间测试和修Bug，但是这功能依然还有一堆Bug」

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

void BinaryOrbitToKeplerianElems(KeplerianOrbitElems *Par)
{
    Par->PericenterDist = Par->Separation;
    Par->Separation = _NoDataDbl; // Move Separation to PericenterDist, and clear it.
    if (!IS_NO_DATA_DBL(Par->PositionAngle))
    {
        Par->ArgOfPericenter = Par->PositionAngle;
        Par->PositionAngle = _NoDataDbl;
    }
    Par->Binary = false;
}

float64 RocheLimit(const Object& Primary, const Object& Companion, int Mode)
{
    switch (Mode)
    {
    case 0:
    default:
        if (!isfinite(MeanRadius(Companion)) || !isfinite(Mass(Primary)) || !isfinite(Mass(Companion)))
        {
            throw OrbitCalculateException("too few arguments to function \"RocheLimit\"");
        }

        if (!isfinite(MeanRadius(Primary)))
        {
            return EquatorialRadius(Companion) * cbrt(2. * (Mass(Primary) / Mass(Companion)));
        }
        else
        {
            float64 DensityP = MeanDensity(Primary);
            float64 DensityC = MeanDensity(Companion);
            return EquatorialRadius(Primary) * cbrt(2. * (DensityP / DensityC));
        }

    case 1:
        if (!isfinite(MeanRadius(Primary)) || !isfinite(MeanRadius(Companion)) ||
            !isfinite(Mass(Primary)) || !isfinite(Mass(Companion)))
        {
            throw OrbitCalculateException("too few arguments to function \"RocheLimit\"");
        }

        float64 DensityP = MeanDensity(Primary);
        float64 DensityC = MeanDensity(Companion);
        float64 PFlattening = Flattening(Primary).y;
        return 2.423 * EquatorialRadius(Primary) * cbrt(DensityP / DensityC) *
            cbrt(((1. + Mass(Companion) / (3. * Mass(Primary))) + (1. / 3.) *
            PFlattening * (1. + Mass(Companion) / Mass(Primary))) / (1. - PFlattening));
    }
}

float64 HillSphere(const Object& Primary, const Object& Companion)
{
    float64 _Eccentricity, _PericenterDist;
    if (isinf(Eccentricity(Companion))) { _Eccentricity = 0; }
    else { _Eccentricity = Eccentricity(Companion); }
    if (!isinf(Perihelion(Companion))) { _PericenterDist = Perihelion(Companion); }
    else if (!isinf(Companion.Orbit.Separation))
    {
        _PericenterDist = Companion.Orbit.Separation * (1. - _Eccentricity);
    }
    else
    {
        throw OrbitCalculateException("number of arguments to function \"HillSphere\" is not match?");
    }
    return _PericenterDist * cbrt(Mass(Companion) / (3. * Mass(Primary)));
}

fvec<5> LagrangePointDistances(float64 PrimaryMass, float64 CompanionMass, float64 Separation, vec2 TolerNLog)
{
    fvec<5> Distance;
    float64 Mu = CompanionMass / (PrimaryMass + CompanionMass);

    auto FindReal = [](complex64* Roots)
    {
        return std::find_if(Roots, Roots + 5, [](complex64 C){return abs(C.imag()) < 1e-12;});
    };

    // L1
    fvec<6> L1Coeffs{1., Mu - 3., 3. - 2. * Mu, -Mu, 2. * Mu, -Mu};
    complex64 L1Roots[5];
    SolvePoly(L1Coeffs, L1Roots, {.P_ERROR = TolerNLog.x});
    Distance[0] = Separation - Separation * FindReal(L1Roots)->real();

    // L2
    fvec<6> L2Coeffs{1., 3. - Mu, 3. - 2. * Mu, -Mu, -2. * Mu, -Mu};
    complex64 L2Roots[5];
    SolvePoly(L2Coeffs, L2Roots, {.P_ERROR = TolerNLog.x});
    Distance[1] = Separation + Separation * FindReal(L2Roots)->real();

    // L3
    auto L3Equation = [M1 = PrimaryMass, M2 = CompanionMass, R = Separation](float64 r)
    {
        return M1 / pow(R - r,2) + M2 / pow(2 * R - r, 2) -
               (M2 / (M1 + M2) * R + R - r) * (M1 + M2) / (R * R * R);
    };
    auto DL3Equation = [M1 = PrimaryMass, M2 = CompanionMass, R = Separation](float64 r)
    {
        return (2 * M1 / pow(R - r, 3)) + (2 * M2 / pow((2 * R) - r, 3)) -
               ((-M1) - M2) / (R * R * R);
    };
    NewtonIterator It(L3Equation, DL3Equation);
    It._M_TolerNLog = TolerNLog.y;
    Distance[2] = Separation - It(0, Separation * (7. / 12.) * Mu);

    // L4 and L5
    Distance[3] = Separation;
    Distance[4] = Separation;

    return Distance;
}

_ORBIT_END
_CSE_END

#undef _CRT_SECURE_NO_WARNINGS
