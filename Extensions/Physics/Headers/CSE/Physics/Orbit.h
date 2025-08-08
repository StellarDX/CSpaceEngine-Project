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
    ustring   RefPlane        = _NoDataStr;
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
*                                         基本元素                                        *
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
    OrbitElemType InitialState;
    OrbitElemType CurrentState;

    OrbitElemType CheckParams(const OrbitElemType& InitElems);

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
    Angle MeanAnomaly()const;
    Angle MeanLongitude()const;
    Angle Inclination()const;
    Angle LongitudeOfAscendingNode()const;
    CSEDateTime TimeOfPerihelion()const;
    Angle ArgumentOfPerihelion()const;
    Angle LongitudeOfPerihelion()const;
    Angle EccentricAnomaly()const;
    Angle TrueAnomaly()const;
};

bool KeplerCompute(OrbitElems& InitElems);

// ---------------------------------------- 开普勒方程 ---------------------------------------- //

/*
    丹霞：开普勒方程是航天动力学基础方程，也是开普勒定律的数学描述，其定义为M = E - e * sin(E)。它看似简单但
    实则是一个超越方程，这意味着无法使用初等或者解析的方式精确求解这个方程的逆，所以实际计算的时候，数值算法还是
    逃不了的_(:з」∠)_。当然这个问题也是一个困扰了学界200年之久的问题，直到像牛顿迭代这样的数值算法的出现，这
    个问题才得以解决。牛顿迭代法的实现非常简单，仅需要函数的导函数和一个初值就可以快速收敛。不过问题同样出现在这
    个初值的选择上，因为它是影响牛顿迭代速度的一个关键因子。学界对于这个初始值的算法可以说五花八门，例如2006年
    美国海军天文台在文献[1]中就给出了一种三阶初值估计算法，但是由此算法得到的初值接牛顿迭代仍然可能出现收敛慢的
    情况，故可以认为它是一种“不太稳定”的算法。2021年Richard J. Mathar在文献[2]中也提出了一种更好的初值确定
    方法，而且实验的结果也算是比较好看的了。本研究最初尝试先使用幂级数展开到前几项以确定一个初始值然后再接牛顿迭
    代，但效果依然不理想，直到看到了文献[3]。文献[3]中提到了三种算法，分别是增强型牛顿迭代，增强型Markley算法
    和分段五次多项式拟合，并且在近抛物线轨道的近日点附近会自动切换到二分法以保证求解精度，并且除了增强型牛顿迭代
    以外，另外两种算法都没有循环结构，所以速度也非常快。经后续实验，此方法在64位浮点下精度可达1-2个ULP，可以认
    为是开普勒方程反函数的“正解”。此处的开普勒方程求解算法使用文献[3]中的实现。

    对于双曲线轨道的开普勒方程，其定义为M = e * sinh(E) - E，Virginia Raposo-Pulido和Jesus Pelaez给出
    了一种四倍精度的HKE–SDG算法[4]，此算法使用多项式拟合接牛顿迭代实现，经实验仅需3次迭代就能在4倍精度下到达大
    约1-2个ulp的精度。另外2024年3月初，广东工业大学的吴柏生老师等人在SCI发表了一种新的快速求解算法，这里称它为
    “吴柏生算法”[5]。吴柏生算法的原理大致是分段帕德逼近接一次施罗德迭代，仅需要评估不超过三个超越函数，所以它的
    速度可能是很快的。不过由于吴柏生算法并未开源，故目前无法验证其准确性。所以本文仍然使用HKE–SDG算法计算双曲开
    普勒方程。

    参考文献：
    [1] Murison M A .A Practical Method for Solving the Kepler Equation[J].  2006.
        DOI:10.13140/2.1.5019.6808.
    [2] Mathar R J .Improved First Estimates to the Solution of Kepler's Equation[J].  2021.
        DOI:10.48550/arXiv.2108.03215.
    [3] Tommasini D , Olivieri D N .Two fast and accurate routines for solving the elliptic 
        Kepler equation for all values of the eccentricity and mean anomaly[J].天文学与天体物理, 
        2022, 658:A196.DOI:10.1051/0004-6361/202141423.
    [4] Raposo-Pulido V ,J. Peláez.An efficient code to solve the Kepler equation. Hyperbolic
        case[J].Astronomy and Astrophysics, 2018, 619.DOI:10.1051/0004-6361/201833563.
    [5] Wu B , Zhou Y , Lim C W ,et al.A new method for solving the hyperbolic Kepler
        equation[J].Applied Mathematical Modelling, 2024, 127(000):7.
        DOI:10.1016/j.apm.2023.12.017.
*/

#define _KE_BEGIN namespace KE {
#define _KE_END }
#define _KE KE::

_KE_BEGIN

/**
 * @brief 椭圆开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @param Eccentricity 离心率
 * @return 平近点角
 */
Angle __Elliptical_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly);

/**
 * @brief 抛物线开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
Angle __Parabolic_Keplerian_Equation(Angle EccentricAnomaly);

/**
 * @brief 双曲开普勒方程
 * @param EccentricAnomaly 偏近点角
 * @param Eccentricity 离心率
 * @return 平近点角
 */
Angle __Hyperbolic_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly);

/**
 * @brief 椭圆开普勒方程的反函数
 */
class __Elliptical_Inverse_Keplerian_Equation
{
protected:
    float64 Eccentricity;
public:
    __Elliptical_Inverse_Keplerian_Equation(float64 e);
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 抛物线开普勒方程的反函数
 */
class __Parabolic_Inverse_Keplerian_Equation
{
public:
    __Parabolic_Inverse_Keplerian_Equation() {}
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 双曲开普勒方程的反函数
 */
class __Hyperbolic_Inverse_Keplerian_Equation
{
protected:
    float64 Eccentricity;
public:
    __Hyperbolic_Inverse_Keplerian_Equation(float64 e);
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 椭圆开普勒方程求解工具
 */
class __Enhanced_Inverse_Keplerian_Equation_Solver
    : public __Elliptical_Inverse_Keplerian_Equation
{
public:
    using Mybase = __Elliptical_Inverse_Keplerian_Equation;

protected:
    float64 AbsoluteTolerence = 14.522878745280337562704972096745; // 3E-15
    float64 RelativeTolerence = 15.657577319177793764036061134032; // 2.2E-16

    constexpr static const float64 EBoundary = 0.99;
    constexpr static const float64 MBoundary = 0.0045;

    virtual float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const = 0;
    virtual float64 BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)const;

public:
    __Enhanced_Inverse_Keplerian_Equation_Solver(float64 e) : Mybase(e) {}
    Angle operator()(Angle MeanAnomaly)const final;
};

// ENRKE
class __Newton_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;
protected:
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;
public:
    __Newton_Inverse_Keplerian_Equation(float64 e) : Mybase(e) {}
};

// ENMAKE
class __Markley_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;
protected:
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;
public:
    __Markley_Inverse_Keplerian_Equation(float64 e) : Mybase(e) {}
};

// ENP5KE
class __Piecewise_Inverse_Quintic_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver;

protected:
    std::vector<int64>             BlockBoundaries;
    std::vector<Angle>             Breakpoints;
    SciCxx::DynamicMatrix<float64> Coefficients;

    static void GetCoefficients1(float64 Eccentricity, float64 Tolerence,
        std::vector<Angle>* Grid/*, uint64* n*/);
    static void GetCoefficients2(float64 Eccentricity,
        const std::vector<Angle>& Grid, //uint64 n,
        std::vector<int64>* kvec, std::vector<Angle>* bp,
        SciCxx::DynamicMatrix<float64>* coeffs);

    uint64 FindInterval(float64 MRad)const;
    float64 BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)const override;
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;

public:
    __Piecewise_Inverse_Quintic_Keplerian_Equation(float64 e);

    static void GetCoefficients(float64 Eccentricity, float64 Tolerence,
        /*uint64* n,*/ std::vector<int64>* kvec, std::vector<Angle>* bp,
        SciCxx::DynamicMatrix<float64>* coeffs);
};

/**
 * @brief 抛物线开普勒方程求解工具
 */
class __Polynomial_Parabolic_Inverse_Keplerian_Equation
    : public __Parabolic_Inverse_Keplerian_Equation
{
public:
    Angle operator()(Angle MeanAnomaly)const;
};

/**
 * @brief 双曲开普勒方程求解工具
 */
class __SDGH_Equacion_Inversa_de_Keplerh
    : public __Hyperbolic_Inverse_Keplerian_Equation
{
public:
    using Mybase     = __Hyperbolic_Inverse_Keplerian_Equation;
    using STableType = float64;
    using PTableType = std::function<float64(float64, float64)>;

    constexpr static const uint64 SegmentTableSize  = 51;
    constexpr static const uint64 SegmentTableBound = 26;
    constexpr static const uint64 PolynomTableSize  = 50;
    constexpr static const uint64 PolynomTableBound = 26;

    static const STableType SegmentCoeffsTable[SegmentTableSize];
    static const PTableType TablaPolinomios[PolynomTableSize];

protected:
    float64 AbsoluteTolerence = 15.65;
    float64 RelativeTolerence = 15.65;
    float64 MaxIterations     = 1.69897;

    float64 SegmentTable[SegmentTableSize];

    float64 SingularCornerInitEstimator(float64 MRad)const;
    float64 SingularCornerInitEstimatorDOS(float64 MRad)const;
    float64 SingularCornerInitEstimatorTRES(float64 MRad)const;

    float64 NewtonInitValue(Angle MeanAnomaly)const;
    Angle Run(Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual)const;

public:
    __SDGH_Equacion_Inversa_de_Keplerh(float64 e);

    Angle operator()(Angle MeanAnomaly)const override;
    Angle operator()(Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual)const;

    static void GetSegments(float64 Eccentricity, float64* SegTable);
    static vec4 VectorizedHKE(float64 Eccentricity, float64 MRad, float64 Init);
};

using DefaultEllipticalIKE = KE::__Newton_Inverse_Keplerian_Equation;
using DefaultParabolicIKE  = KE::__Polynomial_Parabolic_Inverse_Keplerian_Equation;
using DefaultHyperbolicIKE = KE::__SDGH_Equacion_Inversa_de_Keplerh;

_KE_END

// ---------------------------------------------------------------------------------------------

/**
 * @brief 开普勒方程
 * @param Eccentricity 离心率
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
Angle KeplerianEquation(float64 Eccentricity, Angle EccentricAnomaly);

/**
 * @brief 开普勒方程的反函数
 * @param Eccentricity 离心率
 * @param MeanAnomaly 平近点角
 * @return 偏近点角
 */
Angle InverseKeplerianEquation(float64 Eccentricity, Angle MeanAnomaly);

Angle GetTrueAnomalyFromEccentricAnomaly(float64 Eccentricity, Angle EccentricAnomaly);

Angle GetEccentricAnomalyFromTrueAnomaly(float64 Eccentricity, Angle TrueAnomaly);

OrbitState KeplerianElementsToStateVectors(OrbitElems Elems);

OrbitElems StateVectorsToKeplerianElements(OrbitState Elems);

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
