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

struct KeplerianOrbitElems
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

    KeplerianOrbitElems& operator=
        (const Object::OrbitParams& P)noexcept;
    operator Object::OrbitParams();
};

struct EquinoctialOrbitElems
{
    ustring   RefPlane        = _NoDataStr;
    float64   Epoch           = _NoDataDbl;
    float64   GravParam       = _NoDataDbl;
    float64   PericenterDist  = _NoDataDbl;
    float64   Period          = _NoDataDbl;
    float64   EccentricityF   = _NoDataDbl; // Eccentricity vector components (f, g)
    float64   EccentricityG   = _NoDataDbl;
    float64   InclinationH    = _NoDataDbl; // Inclination vector components (h, k)
    float64   InclinationK    = _NoDataDbl;
    Angle     MeanLongitude   = _NoDataDbl;
};

struct OrbitStateVectors
{
    ustring   RefPlane        = _NoDataStr;       // Reference System
    float64   GravParam       = _NoDataDbl;       // Gravity Parameter (G*M)
    float64   Time            = _NoDataDbl;       // Julian date
    vec3      Position        = vec3(_NoDataDbl); // XYZ-pos in meters
    vec3      Velocity        = vec3(_NoDataDbl); // Velocity in m/s (m/day in SE)
};

// 地心惯性系映射CSE直角坐标用的矩阵
// 一般，x轴为天体赤道与黄道交线，指向春分点；z轴由中心天体中心指向北极；y轴根据右手定则确定。
static const mat3 ECIFrameToCSECoord = {1, 0, 0, 0, 0, -1, 0, 1, 0};

// CSE直角坐标映射地心惯性系用的矩阵
// CSE直角坐标的X轴为春分线，Y轴指向北极，Z轴按右手定则确定。
static const mat3 CSECoordToECIFrame = {1, 0, 0, 0, 0, 1, 0, -1, 0};

/****************************************************************************************\
*                                         基本元素                                        *
\****************************************************************************************/

/**
 * @brief 物体跟踪器，一切轨道工具的地基
 */
__interface SatelliteTracker
{
public:
    virtual void AddMsecs(int64 Ms) = 0;
    virtual void AddSeconds(int64 Sec) = 0;
    virtual void AddHours(int64 Hrs) = 0;
    virtual void AddDays(int64 Days) = 0;
    virtual void AddYears(int64 Years) = 0;
    virtual void AddCenturies(int64 Centuries) = 0;

    virtual void ToCurrentDate() = 0;
    virtual void SetDate(CSEDateTime DateTime) = 0;
    virtual void SetDate(float64 JD) = 0;
    virtual void Move(Angle MeanAnomalyOffset) = 0;
    virtual void Reset() = 0;

    virtual KeplerianOrbitElems KeplerianElems()const = 0;
    virtual EquinoctialOrbitElems EquinoctialElems()const = 0;
    virtual OrbitStateVectors StateVectors(mat3)const = 0;
};

class KeplerianSatelliteTracker : public SatelliteTracker
{
public:
    using Mybase    = SatelliteTracker;
    using BaseType  = KeplerianOrbitElems;

protected:
    BaseType InitialState;
    BaseType CurrentState;
    Angle    AngularVelocity;

    BaseType CheckParams(const BaseType& InitElems);

public:
    KeplerianSatelliteTracker(const BaseType& InitElems);
    KeplerianSatelliteTracker(const OrbitStateVectors& InitState);

    void AddMsecs(int64 Ms)override;
    void AddSeconds(int64 Sec)override;
    void AddHours(int64 Hrs)override;
    void AddDays(int64 Days)override;
    void AddYears(int64 Years)override;
    void AddCenturies(int64 Centuries)override;

    void ToCurrentDate()override;
    void SetDate(CSEDateTime DateTime)override;
    void SetDate(float64 JD)override;
    void Move(Angle MeanAnomalyOffset)override;
    void Reset()override;

    KeplerianOrbitElems KeplerianElems()const override;
    EquinoctialOrbitElems EquinoctialElems()const override;
    OrbitStateVectors StateVectors
        (mat3 AxisMapper = ECIFrameToCSECoord)const override;

    static KeplerianOrbitElems StateVectorstoKeplerianElements
        (OrbitStateVectors State, mat3 AxisMapper = CSECoordToECIFrame);
};

class EquinoctialSatelliteTracker : public SatelliteTracker
{
    // TODO...
};

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

    抛物线这种情况反而是最简单的，定义为M = (1 / 2) * E + (1 / 6) * E^3，直接就是一个多项式[6]。此方程可使
    用三次方程求根公式（如范盛金算法）求解，结果必然为1实2虚，其中唯一实根就是要得到的解。

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
    [6] 平近点角 - 卫星百科(灰机Wiki)
        https://sat.huijiwiki.com/wiki/%E5%B9%B3%E8%BF%91%E7%82%B9%E8%A7%92
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
class __Piecewise_Quintic_Inverse_Keplerian_Equation
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
    __Piecewise_Quintic_Inverse_Keplerian_Equation(float64 e);

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

bool KeplerCompute(KeplerianOrbitElems& InitElems);
float64 GetSemiMajorAxisFromPericenterDist(float64 Eccentricity, float64 PericenterDist);
float64 GetPericenterDistFromSemiMajorAxis(float64 Eccentricity, float64 SemiMajorAxis);
Angle KeplerianEquation(float64 Eccentricity, Angle EccentricAnomaly);
Angle InverseKeplerianEquation(float64 Eccentricity, Angle MeanAnomaly);
Angle GetTrueAnomalyFromEccentricAnomaly(float64 Eccentricity, Angle EccentricAnomaly);
Angle GetEccentricAnomalyFromTrueAnomaly(float64 Eccentricity, Angle TrueAnomaly);
float64 GetSemiLatusRectumFromPericenterDist(float64 Eccentricity, float64 PericenterDist);
Angle GetArgOfLatitude(Angle ArgOfPericen, Angle Anomaly);
Angle PeriodToAngularVelocity(float64 Period);
Angle PericenterDistToAngularVelocity(float64 Eccentricity, float64 PericenterDist, float64 GravParam);


/****************************************************************************************\
*                                         两行根数                                        *
\****************************************************************************************/

struct SpacecraftBasicData
{
    uint32_t     CatalogNumber;  // Satellite catalog number
    char         Classification; // Classification (U: unclassified, C: classified, S: secret)

    struct COSPAR_ID
    {
        int32_t  LaunchYear;     // last two digits of launch year
        uint32_t LaunchNumber;   // launch number of the year
        char     LaunchPiece[3]; // piece of the launch
    }IntDesignator;

    float64      D1MeanMotion;   // First derivative of mean motion; the ballistic coefficient (rev/day, per day) (Stored as deg/s in this condition)
    float64      D2MeanMotion;   // Second derivative of mean motion (rev/day³, decimal point assumed) (Stored as deg/s^3 in this condition)
    float64      BSTAR;          // B*, the drag term, or radiation pressure coefficient (units of 1/(Earth radius), decimal point assumed) (Stored as 1/m in this condition)
    uint32_t     EphemerisType;  // Ephemeris type (always zero; only used in undistributed TLE data)
    uint32_t     ElementSet;     // Element set number. Incremented when a new TLE is generated for this object.
    uint32_t     RevolutionNum;  // Revolution number at epoch (revolutions)
};

class TLE // Two-line element set
{
public:
    static const auto TitleLength      = 24; // 一个24个字符的名称（与NORAD SATCAT中的名称长度一致）
    static const auto DataLength       = 69; // 第1行和第2行是标准的双线轨道元素集格式，与NORAD和NASA使用的格式相同

    static const auto L1LineNumber     = 0;
    static const auto L1CatalogNumber  = 2;
    static const auto L1Classification = 7;
    static const auto L1COSPARIDYD     = 9;
    static const auto L1COSPARIDP      = 14;
    static const auto L1EpochI         = 18;
    static const auto L1EpochF         = 24;
    static const auto L1D1MeanMotion   = 33;
    static const auto L1D2MeanMotionM  = 44;
    static const auto L1D2MeanMotionE  = 50;
    static const auto L1BSTARM         = 53;
    static const auto L1BSTARE         = 59;
    static const auto L1EphemerisType  = 62;
    static const auto L1ElementSet     = 64;
    static const auto L1Checksum       = 68;

    static const auto L2LineNumber     = 0;
    static const auto L2CatalogNumber  = 2;
    static const auto L2Inclination    = 8;
    static const auto L2AscendingNode  = 17;
    static const auto L2Eccentricity   = 26;
    static const auto L2ArgOfPericen   = 34;
    static const auto L2MeanAnomaly    = 43;
    static const auto L2MeanMotionI    = 52;
    static const auto L2MeanMotionF    = 55;
    static const auto L2Revolutions    = 63;
    static const auto L2Checksum       = 68;

    enum SatelliteClassification : char
    {
        Unclassified = 'U',
        Classified   = 'C',
        Secret       = 'S'
    };

    char Title[TitleLength + 1]; // 防御性，因为C语言中字符串必须以\0结尾，
    char Line1[DataLength + 1];  // 而且format格式化输出会自动忽略最后一个字符。
    char Line2[DataLength + 1];

    TLE();
    TLE(char const* Name, char const* L1, char const* L2);
    TLE(char const* const* Data) : TLE(Data[0], Data[1], Data[2]) {}

    bool IsValid()const;

    void Get(void* Title, void* L1, void* L2)const;

    ustring SatelliteName()const;
    SpacecraftBasicData BasicData()const;
    KeplerianOrbitElems OrbitElems()const;

    std::string ToString(char Delim = '\n')const;
    static TLE FromString(char const* Data, char Delim = '\n');
    static int VerifyLine(const char* Line, int Size, int Checksum);
}__declspec(packed);

/**
 * @class OEM
 * @brief 轨道星历消息
 *
 * @details
 * @par 功能描述
 * 实现CCSDS 502.0-B-3标准的轨道星历消息数据结构，用于高精度轨道数据存储。
 *
 * @par 参考文献
 * [1] Orbit Data Messages[S/OL]. CCSDS 502.0-B-3. 2023. https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/01//502x0b3e1.pdf<br>
 * [2] Sease B. oem[C]. Github. https://github.com/bradsease/oem<br>
 * [3] 刘泽康. 中国空间站OEM来啦，快来和我们一起追"星"吧！[EB/OL]. (2023-09-13). https://www.cmse.gov.cn/xwzx/202309/t20230913_54312.html<br>
 *
 * @todo 目前只支持导入和导出，完整功能待实现
 */
class OEM
{
public:
    constexpr static const cstring KeyValueFmtString = "{} = {}";
    constexpr static const cstring SimplifiedISO8601String =
        "{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}";
    constexpr static const cstring EphemerisFmtString =
        "{} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g}";
    constexpr static const cstring EphemerisFmtStringWithAccel =
        "{} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g}";
    constexpr static const cstring CovarianceMatFmtString = "{:.8g}";

    std::string          OEMVersion;
    std::string          Classification;
    CSEDateTime          CreationDate;
    std::string          Originator;
    std::string          MessageID;

    struct ValueType
    {
        struct MetadataType
        {
            std::string  ObjectName;
            std::string  ObjectID;
            std::string  CenterName;
            std::string  RefFrame;
            CSEDateTime  RefFrameEpoch;
            std::string  TimeSystem;
            CSEDateTime  StartTime;
            CSEDateTime  UseableStartTime;
            CSEDateTime  UseableStopTime;
            CSEDateTime  StopTime;
            std::string  Interpolation;
            uint64       InterpolaDegrees = 0;
        }MetaData;

        struct EphemerisType
        {
            CSEDateTime  Epoch;
            vec3         Position;
            vec3         Velocity;
            vec3         Acceleration;
        };
        std::vector<EphemerisType> Ephemeris;

        struct CovarianceMatrixType
        {
            CSEDateTime  Epoch;
            std::string  RefFrame;
            matrix<6, 6> Data;
        };
        std::vector<CovarianceMatrixType> CovarianceMatrices;
    };

    using ValueSet = std::vector<ValueType>;
    ValueSet Data;

    // 这个表存的是Interpolation字段对应的插值工具或函数，通过指针侧载调用。
    // 值类型目前未实现，暂时上一个“能通过编译”的占位符。
    static const std::map<std::string, void*> InterpolationTools;

protected:
    static bool ParseComment(std::string Line);
    static void RemoveWhiteSpace(std::string& Line);
    static std::pair<std::string, std::string> ParseKeyValue(std::string Line);
    static std::vector<std::string> ParseRawData(std::string Line);
    static ValueType::EphemerisType ParseEphemeris(std::string Line);
    static void TransferHeader
        (std::map<std::string, std::string> Buf, OEM* out);
    static void TransferMetaData
        (std::map<std::string, std::string> Buf, OEM* out);
    static void TransferEphemeris
        (std::vector<ValueType::EphemerisType> Buf, OEM* out);
    static void TransferCovarianceMatrices
        (std::vector<ValueType::CovarianceMatrixType> Buf, OEM* out);

    static void ExportKeyValue
        (std::ostream& fout, std::string Key, std::string Value,
        bool Optional = 0, cstring Fmt = KeyValueFmtString);
    static void ExportEphemeris
        (std::ostream& fout, std::vector<ValueType::EphemerisType> Eph,
        cstring Fmt = EphemerisFmtString);
    static void ExportCovarianceMatrix
        (std::ostream& fout, std::vector<ValueType::CovarianceMatrixType> Mat,
        cstring KVFmt = KeyValueFmtString, cstring MatFmt = CovarianceMatFmtString);

public:
    static void Import(std::istream& fin, OEM* out);
    static OEM FromString(std::string Src);
    static OEM FromFile(std::filesystem::path Path);

    void Export(std::ostream& fout, cstring KVFmt = KeyValueFmtString,
        cstring EphFmt = EphemerisFmtString, cstring CMFmt = CovarianceMatFmtString)const;
    std::string ToString()const;
    void ToFile(std::filesystem::path Path)const;

    OrbitStateVectors operator()(CSEDateTime); // TODO
    OrbitStateVectors operator()(float64); // TODO
};

_ORBIT_END
_CSE_END

// 种豆南山下，草盛豆苗稀。
// 晨兴理荒秽，带月荷锄归。
// 道狭草木长，夕露沾我衣。
// 衣沾不足惜，但使愿无违。

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
    _STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif
