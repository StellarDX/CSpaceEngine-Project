/*****************************************************************************
 *   Copyright (C) 2004-2018 The pykep development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   Adapted for CSpaceEngine by StellarDX, 2025                             *
 *                                                                           *
 *   https://gitter.im/esa/pykep                                             *
 *   https://github.com/esa/pykep                                            *
 *                                                                           *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

_CSE_BEGIN
_ORBIT_BEGIN
_Lambert_BEGIN

/** Constructs a Lambert problem.
 *
 * \param[in] Dep first cartesian position
 * \param[in] Dst second cartesian position
 * \param[in] TOF time of flight
 * \param[in] GP gravity parameter
 * \param[in] Dir when 1 a retrograde orbit is assumed
 * \param[in] Rev maximum number of multirevolutions to compute
 */
__ESA_PyKep_Lambert_Solver::__ESA_PyKep_Lambert_Solver
    (const vec3& Dep, const vec3& Dst, const float64 &TOF,
    const float64& GP, const bool& Dir, const uint64& Rev)
{
    this->Departure = Dep;
    this->Destination = Dst;
    this->TimeOfFlight = TOF;
    this->GravParam = GP;
    this->Retrograde = Dir;
    this->Revolutions = Rev;
}

vec3 __ESA_PyKep_Lambert_Solver::VectorizedTimeEquationDerivatives(float64 x, float64 T)
{
    float64 Lam2 = TransferAngle * TransferAngle;
    float64 Lam3 = TransferAngle * Lam2;
    float64 umx2 = 1. - x * x;
    float64 y1 = sqrt(1. - Lam2 * umx2);
    float64 y2 = y1 * y1;
    float64 y3 = y2 * y1;

    float64 T1 = 1.0 / umx2 * (3.0 * T * x - 2.0 + 2.0 * Lam3 * x / y1);
    float64 T2 = 1.0 / umx2 * (3.0 * T + 5.0 * x * T1 + 2.0 * (1.0 - Lam2) * Lam3 / y3);
    float64 T3 = 1.0 / umx2 * (7.0 * x * T2 + 8.0 * T1 - 6.0 * (1.0 - Lam2) * Lam2 * Lam3 * x / y3 / y2);

    return {T1, T2, T3};
}

float64 __ESA_PyKep_Lambert_Solver::TransferTimeEquation(float64 x, uint64 N)
{
    float64 Dist = abs(x - 1);
    if (Dist < BattinBreakpoint) {return BattinSeries(x, N);}
    else if (Dist > LancasterBreakPoint) {return Lancaster(x, N);}
    else {return Lagrange(x, N);}
}

float64 __ESA_PyKep_Lambert_Solver::Lagrange(float64 x, uint64 N)
{
    float64 a = 1.0 / (1.0 - x * x);
    if (a > 0) // Elliptical
    {
        Angle alfa = Angle::FromDegrees(2.0 * arccos(x).ToDegrees());
        Angle beta = Angle::FromDegrees(2.0 * arcsin(sqrt(TransferAngle * TransferAngle / a)).ToDegrees());
        if (TransferAngle < 0.0) {beta = -beta;}
        return (a * sqrt(a) * ((alfa - sin(alfa)) - (beta - sin(beta)) + 2.0 * M_PI * N)) / 2.0;
    }
    else // Hyperbolic
    {
        float64 alfa = 2.0 * arcosh(x);
        float64 beta = 2.0 * arsinh(sqrt(-TransferAngle * TransferAngle / a));
        if (TransferAngle < 0.0) {beta = -beta;}
        return -a * sqrt(-a) * ((beta - sinh(beta)) - (alfa - sinh(alfa))) / 2.0;
    }
}

float64 __ESA_PyKep_Lambert_Solver::BattinSeries(float64 x, uint64 N)
{
    float64 K = TransferAngle * TransferAngle;
    float64 E = x * x - 1.0;
    float64 rho = abs(E);
    float64 z = sqrt(1 + K * E);

    float64 eta = z - TransferAngle * x;
    float64 S1 = (1.0 - TransferAngle - x * eta) / 2.;

    auto Hypgeom = [](float64 z, float64 tol)
    {
        float64 Sj = 1.0;
        float64 Cj = 1.0;
        float64 err = 1.0;
        float64 Cj1 = 0.0;
        float64 Sj1 = 0.0;
        int j = 0;
        while (err > tol)
        {
            Cj1 = Cj * (3.0 + j) * (1.0 + j) / (2.5 + j) * z / (j + 1);
            Sj1 = Sj + Cj1;
            err = abs(Cj1);
            Sj = Sj1;
            Cj = Cj1;
            ++j;
        }
        return Sj;
    };

    float64 Q = Hypgeom(S1, pow(10, -BattinHypGeomTolerence));
    Q = 4.0 / 3.0 * Q;
    return (eta * eta * eta * Q + 4.0 * TransferAngle * eta) / 2.0 + N * M_PI / pow(rho, 1.5);
}

float64 __ESA_PyKep_Lambert_Solver::Lancaster(float64 x, uint64 N)
{
    float64 K = TransferAngle * TransferAngle;
    float64 E = x * x - 1.0;
    float64 rho = abs(E);
    float64 z = sqrt(1 + K * E);

    float64 y = sqrt(rho);
    float64 g = x * z - TransferAngle * E;
    float64 d = 0;
    if (E < 0)
    {
        float64 l = arccos(g).ToRadians();
        d = N * CSE_PI + l;
    }
    else
    {
        float64 f = y * (z - TransferAngle * x);
        d = ln(f + g);
    }
    return (x - TransferAngle * z - d / y) / E;
}

float64 __ESA_PyKep_Lambert_Solver::FastHouseholderIterate(float64 T, float64 x0, uint64 N, float64 Tolerence, uint64 MaxIter, uint64* Iter)
{
    uint64 it = 0;
    float64 x = 0;
    for ( ; it < MaxIter; ++it)
    {
        float64 TransferTime = TransferTimeEquation(x0, N);
        vec3 DT = VectorizedTimeEquationDerivatives(x0, TransferTime);
        float64 LagTime = TransferTime - T;
        float64 DT2 = DT[0] * DT[0];
        float64 dx = LagTime * (DT2 - LagTime * DT[1] / 2.0) / (DT[0] * (DT2 - LagTime * DT[1]) + DT[2] * LagTime * LagTime / 6.0);
        x = x0 - dx;
        float64 Tol = abs(dx);
        if (Tol < Tolerence) {break;}
        x0 = x;
    }
    if (Iter) {*Iter = it + 1;}
    return x;
}

void __ESA_PyKep_Lambert_Solver::ZeroCheck()
{
    if (TimeOfFlight <= 0)
    {
        CSESysDebug("PyKep Lambert Solver", CSEDebugger::WARNING, "Time of flight is negative!");
        Retrograde ^= 1;
        TimeOfFlight = -TimeOfFlight;
    }
    if (GravParam <= 0)
    {
        CSESysDebug("PyKep Lambert Solver", CSEDebugger::WARNING, "Gravity parameter is zero or negative!");
        if (GravParam == 0) {GravParam = GravConstant * SolarMass;}
        else {GravParam = -GravParam;}
    }
}

std::tuple<float64, float64, float64, float64, float64, vec3, vec3, vec3, vec3>
    __ESA_PyKep_Lambert_Solver::PrepareIntermediateVariables()
{
    vec3 ECIDeparture = (AxisMapper * matrix<1, 3>{Departure})[0];
    vec3 ECIDestination = (AxisMapper * matrix<1, 3>{Destination})[0];

    Chord = linalg::distance(ECIDestination, ECIDeparture);

    float64 R1 = linalg::EuclideanNorm(ECIDeparture);
    float64 R2 = linalg::EuclideanNorm(ECIDestination);
    SemiPerimeter = (Chord + R1 + R2) / 2.;

    vec3 ir1 = ECIDeparture / R1;
    vec3 ir2 = ECIDestination / R2;
    vec3 ih = linalg::cross(ir1, ir2);
    ih /= linalg::EuclideanNorm(ih);
    if (ih.z == 0)
    {
        throw std::logic_error
            ("The angular momentum vector has no z component, impossible to define automatically clock or "
             "counterclockwise");
    }
    float64 Lambda2 = 1. - Chord / SemiPerimeter;
    TransferAngle = sqrt(Lambda2);

    vec3 it1, it2;
    if (ih.z < 0.0) // Transfer angle is larger than 180 degrees as seen from above the z axis
    {
        TransferAngle = -TransferAngle;
        it1 = linalg::cross(ir1, ih);
        it2 = linalg::cross(ir2, ih);
    }
    else
    {
        it1 = linalg::cross(ih, ir1);
        it2 = linalg::cross(ih, ir2);
    }
    it1 /= linalg::EuclideanNorm(it1);
    it2 /= linalg::EuclideanNorm(it2);

    if (Retrograde)
    {
        TransferAngle = -TransferAngle;
        it1 = -it1;
        it2 = -it2;
    }

    float64 Lambda3 = TransferAngle * Lambda2;
    float64 T = sqrt(2.0 * GravParam / SemiPerimeter / SemiPerimeter / SemiPerimeter) * TimeOfFlight;

    return std::tuple{R1, R2, T, Lambda2, Lambda3, ir1, ir2, it1, it2}; // export the variables will be used in next steps.
}

std::tuple<float64, float64> __ESA_PyKep_Lambert_Solver::DetectMaxRevolutions(float64 T, float64 Lambda2, float64 Lambda3)
{
    // 2.1 - Let's first detect the maximum number of revolutions for which there exists a solution
    uint64 MaxRev = uint64(T / CSE_PI);
    float64 T00 = arccos(TransferAngle).ToRadians() + TransferAngle * sqrt(1. - Lambda2);
    float64 T0 = T00 + MaxRev * CSE_PI;
    float64 T1 = 2.0 / 3.0 * (1.0 - Lambda3);

    if (MaxRev > 0)
    {
        // We use Modified Halley iterations to find xM and TM
        if (T < T0)
        {
            float64 RealTolerence = pow(10, -MaxRevoDetectTolerence);
            float64 TMin = T0;
            float64 x = 0, dx = 0, x1 = 0;
            for (int i = 0; i <= MaxRevoDetectIterCount; ++i)
            {
                vec3 DT = VectorizedTimeEquationDerivatives(x, TMin);
                if (DT[0] != 0)
                {
                    dx = DT[0] * DT[1] / (DT[1] * DT[1] - DT[0] * DT[2] / 2.0);
                }
                float64 Tolerence = abs(dx);
                x1 = x - dx;
                if (Tolerence < RealTolerence) {break;}
                TMin = TransferTimeEquation(x1, MaxRev);
                x = x1;
            }
            if (TMin > T) {--MaxRev;}
        }
    }

    // We exit this if clause with Nmax being the maximum number of revolutions
    // for which there exists a solution. We crop it to m_multi_revs
    ProbMaxRevolutions = min(MaxRev, Revolutions);

    // 2.2 We now allocate the memory for the output variables
    //StateBuffer.resize(ProbMaxRevolutions * 2 + 1);

    return std::tuple{T00, T1}; // export the variables will be used in next steps.
}

void __ESA_PyKep_Lambert_Solver::HouseholderSolve(float64 T, float64 T00, float64 T1, float64 Lambda2, float64 Lambda3)
{
    StateBlock Blk;

    // 3.1 zero-rev solution
    // 3.1.1 initial guess
    if (T >= T00)
    {
        Blk.XResult = -(T - T00) / (T - T00 + 4);
    }
    else if (T <= T1)
    {
        Blk.XResult = T1 * (T1 - T) / (2.0 / 5.0 * (1 - Lambda2 * Lambda3) * T) + 1;
    }
    else
    {
        Blk.XResult = pow((T / T00), log(2, T1 / T00)) - 1.0;
    }
    Blk.XResult = FastHouseholderIterate(T, Blk.XResult, 0,
        pow(10, -HouseholderPivotTolerence), HouseholderPivotMaxIter, &Blk.Iteration);
    StateBuffer.push_back(Blk);

    // 3.2 multi rev solutions
    float64 tmp;
    for (uint64 i = 1; i <= ProbMaxRevolutions; ++i)
    {
        // 3.2.1 left Householder iterations
        tmp = cbrt(pow((i * CSE_PI + CSE_PI) / (8.0 * T), 2.0));
        Blk.XResult = FastHouseholderIterate(T, (tmp - 1) / (tmp + 1), i,
            pow(10, -HouseholderLeftTolerence), HouseholderLeftMaxIter, &Blk.Iteration);
        StateBuffer.push_back(Blk);

        // 3.2.1 right Householder iterations
        tmp = cbrt(pow((8.0 * T) / (i * CSE_PI), 2.0));
        Blk.XResult = FastHouseholderIterate(T, (tmp - 1) / (tmp + 1), i,
            pow(10, -HouseholderRightTolerence), HouseholderRightMaxIter, &Blk.Iteration);
        StateBuffer.push_back(Blk);
    }
}

void __ESA_PyKep_Lambert_Solver::ComputeTerminalVelocities(float64 R1, float64 R2, float64 Lambda2, vec3 ir1, vec3 ir2, vec3 it1, vec3 it2)
{
    float64 gamma = sqrt(GravParam * SemiPerimeter / 2.0);
    float64 rho = (R1 - R2) / Chord;
    float64 sigma = sqrt(1 - rho * rho);
    for (size_t i = 0; i < StateBuffer.size(); ++i)
    {
        float64 y = sqrt(1.0 - Lambda2 + Lambda2 * StateBuffer[i].XResult * StateBuffer[i].XResult);
        float64 vr1 = gamma * ((TransferAngle * y - StateBuffer[i].XResult) - rho * (TransferAngle * y + StateBuffer[i].XResult)) / R1;
        float64 vr2 = -gamma * ((TransferAngle * y - StateBuffer[i].XResult) + rho * (TransferAngle * y + StateBuffer[i].XResult)) / R2;
        float64 vt = gamma * sigma * (y + TransferAngle * StateBuffer[i].XResult);
        float64 vt1 = vt / R1;
        float64 vt2 = vt / R2;
        StateBuffer[i].DepVelocity = vr1 * ir1 + vt1 * it1;
        StateBuffer[i].DstVelocity = vr2 * ir2 + vt2 * it2;
    }
}

void __ESA_PyKep_Lambert_Solver::Run()
{
    // 0 - Sanity checks
    ZeroCheck();

    // 1 - Getting lambda and T
    auto [R1, R2, T, Lambda2, Lambda3, ir1, ir2, it1, it2] = PrepareIntermediateVariables();

    // 2 - We now have lambda, T and we will find all x
    auto [T00, T1] = DetectMaxRevolutions(T, Lambda2, Lambda3);

    // 3 - We may now find all solutions in x,y
    HouseholderSolve(T, T00, T1, Lambda2, Lambda3);

    // 4 - For each found x value we reconstruct the terminal velocities
    ComputeTerminalVelocities(R1, R2, Lambda2, ir1, ir2, it1, it2);
}

OrbitStateVectors __ESA_PyKep_Lambert_Solver::Dep() const
{
    return
    {
        .RefPlane  = "Equator",
        .GravParam = GravParam,
        .Time      = J2000,
        .Position  = Departure,
        .Velocity  = (InvAxisMapper * matrix<1, 3>{StateBuffer[0].DepVelocity})[0]
    };
}

OrbitStateVectors __ESA_PyKep_Lambert_Solver::Dst() const
{
    return
    {
        .RefPlane  = "Equator",
        .GravParam = GravParam,
        .Time      = J2000,
        .Position  = Destination,
        .Velocity  = (InvAxisMapper * matrix<1, 3>{StateBuffer[0].DstVelocity})[0]
    };
}

KeplerianOrbitElems __ESA_PyKep_Lambert_Solver::Kep() const
{
    return KeplerianSatelliteTracker::StateVectorstoKeplerianElements(Dep(), AxisMapper);
}

size_t __ESA_PyKep_Lambert_Solver::SolutionCount() const
{
    return StateBuffer.size();
}

OrbitStateVectors __ESA_PyKep_Lambert_Solver::ExportState(uint64 Index, bool Pos) const
{
    if (Index >= SolutionCount()) {throw std::logic_error("State index out of range");}
    if (!Pos)
    {
        return
        {
            .RefPlane  = "Equator",
            .GravParam = GravParam,
            .Time      = J2000,
            .Position  = Departure,
            .Velocity  = (InvAxisMapper * matrix<1, 3>{StateBuffer[Index].DepVelocity})[0]
        };
    }
    else
    {
        return
        {
            .RefPlane  = "Equator",
            .GravParam = GravParam,
            .Time      = J2000,
            .Position  = Destination,
            .Velocity  = (InvAxisMapper * matrix<1, 3>{StateBuffer[Index].DstVelocity})[0]
        };
    }
}

KeplerianOrbitElems __ESA_PyKep_Lambert_Solver::Kep(uint64 Index) const
{
    if (Index >= SolutionCount()) {throw std::logic_error("State index out of range");}
    return KeplerianSatelliteTracker::StateVectorstoKeplerianElements(ExportState(Index, 0), AxisMapper);
}

ustring __ESA_PyKep_Lambert_Solver::ToString() const
{
    if (StateBuffer.empty()) {return "Failed to export string, is the Run() function had been called?";}

    using namespace std;
    vec3 ZeroRevDepVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[0].DepVelocity})[0];
    vec3 ZeroRevDstVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[0].DstVelocity})[0];
    std::string str = format(R"(Lambert's problem:
Gravitational parameter = {:.14g} m^3/s^2
Departure = ({:.14g}, {:.14g}, {:.14g}) m
Destination = ({:.14g}, {:.14g}, {:.14g}) m
Time of flight: {:.14g} s

Chord = {:.14g}
Semi-perimeter = {:.14g}
Lambda = {:.14g}
Non-dimensional time of flight = {:.14g}

Maximum number of revolutions: {}
Solutions:
0 revs, Iters: {}, x: {:.14g}, a: {:.14g}
    v1 = ({:.14g}, {:.14g}, {:.14g}) m/s, v2 = ({:.14g}, {:.14g}, {:.14g}) m/s
)", GravParam, Departure.x, Departure.y, Departure.z,
        Destination.x, Destination.y, Destination.z,
        TimeOfFlight,
        Chord, SemiPerimeter, TransferAngle,
        TimeOfFlight * sqrt(2 * GravParam / SemiPerimeter / SemiPerimeter / SemiPerimeter),
        ProbMaxRevolutions,
        StateBuffer[0].Iteration, StateBuffer[0].XResult,
        SemiPerimeter / 2.0 / (1 - StateBuffer[0].XResult * StateBuffer[0].XResult),
        ZeroRevDepVelocity.x, ZeroRevDepVelocity.y, ZeroRevDepVelocity.z,
        ZeroRevDstVelocity.x, ZeroRevDstVelocity.y, ZeroRevDstVelocity.z);

    for (uint64 i = 0; i < ProbMaxRevolutions; ++i)
    {
        vec3 LeftDepVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[1 + 2 * i].DepVelocity})[0];
        vec3 LeftDstVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[1 + 2 * i].DstVelocity})[0];
        vec3 RightDepVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[2 + 2 * i].DepVelocity})[0];
        vec3 RightDstVelocity = (InvAxisMapper * matrix<1, 3>{StateBuffer[2 + 2 * i].DstVelocity})[0];
        str += format(R"({} revs,  left. Iters: {}, x: {:.14g}, a: {:.14g}
    v1 = ({:.14g}, {:.14g}, {:.14g}) m/s, v2 = ({:.14g}, {:.14g}, {:.14g}) m/s
{} revs, right. Iters: {}, x: {:.14g}, a: {:.14g}
    v1 = ({:.14g}, {:.14g}, {:.14g}) m/s, v2 = ({:.14g}, {:.14g}, {:.14g}) m/s
)",
        i + 1,
        StateBuffer[1 + 2 * i].Iteration, StateBuffer[1 + 2 * i].XResult,
        SemiPerimeter / 2.0 / (1 - StateBuffer[1 + 2 * i].XResult * StateBuffer[1 + 2 * i].XResult),
        LeftDepVelocity.x, LeftDepVelocity.y, LeftDepVelocity.z,
        LeftDstVelocity.x, LeftDstVelocity.y, LeftDstVelocity.z,
        i + 1,
        StateBuffer[2 + 2 * i].Iteration, StateBuffer[2 + 2 * i].XResult,
        SemiPerimeter / 2.0 / (1 - StateBuffer[2 + 2 * i].XResult * StateBuffer[2 + 2 * i].XResult),
        RightDepVelocity.x, RightDepVelocity.y, RightDepVelocity.z,
        RightDstVelocity.x, RightDstVelocity.y, RightDstVelocity.z);
    }

    return ustring(str);
}

_Lambert_END
_ORBIT_END
_CSE_END
