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

// 地心惯性系映射CSE直角坐标用的矩阵
// 一般，x轴为天体赤道与黄道交线，指向春分点；z轴由中心天体中心指向北极；y轴根据右手定则确定。
static const mat3 ECIFrameToCSECoord = {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}};

// CSE直角坐标映射地心惯性系用的矩阵
// CSE直角坐标的X轴为春分线，Y轴指向北极，Z轴按右手定则确定。
static const mat3 CSECoordToECIFrame = {{1, 0, 0}, {0, 0, 1}, {0, -1, 0}};

/****************************************************************************************\
*                                         基本元素                                        *
\****************************************************************************************/

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

// ---------------------------------------------------------------------------------------------

float64 GetSemiMajorAxisFromPericenterDist(float64 Eccentricity, float64 PericenterDist);
float64 GetPericenterDistFromSemiMajorAxis(float64 Eccentricity, float64 SemiMajorAxis);
Angle GetTrueAnomalyFromEccentricAnomaly(float64 Eccentricity, Angle EccentricAnomaly);
Angle GetEccentricAnomalyFromTrueAnomaly(float64 Eccentricity, Angle TrueAnomaly);
float64 GetSemiLatusRectumFromPericenterDist(float64 Eccentricity, float64 PericenterDist);
Angle GetArgOfLatitude(Angle ArgOfPericen, Angle Anomaly);
Angle PeriodToAngularVelocity(float64 Period);
Angle PericenterDistToAngularVelocity(float64 Eccentricity, float64 PericenterDist, float64 GravParam);

// ----------------------------------------- 两行根数 ----------------------------------------- //

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



/****************************************************************************************\
*                                         定理定律                                        *
\****************************************************************************************/

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

    float64 EBoundary         = 0.99;
    float64 MBoundary         = 0.0045;

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
Angle KeplerianEquation(float64 Eccentricity, Angle EccentricAnomaly);
Angle InverseKeplerianEquation(float64 Eccentricity, Angle MeanAnomaly);

// ---------------------------------------- 兰伯特问题 ---------------------------------------- //

#define _Lambert_BEGIN namespace LambertsProblem {
#define _Lambert_END }
#define _Lambert LambertsProblem::

_Lambert_BEGIN

class __Lambert_Solver_Base
{
protected:
    float64 GravParam;    // 中心物体引力常数(GM)
    vec3    Departure;    // 始发坐标
    vec3    Destination;  // 终到坐标
    float64 TimeOfFlight; // 持续时间
    bool    Retrograde;   // 出发方向，0 = 由西向东，1 = 由东向西
    uint64  Revolutions;  // 环绕中心物体的最多圈数

public:
    mat3 AxisMapper    = CSECoordToECIFrame;
    mat3 InvAxisMapper = ECIFrameToCSECoord;

    virtual void Run() = 0;
    virtual OrbitStateVectors Dep()const = 0;
    virtual OrbitStateVectors Dst()const = 0;
    virtual KeplerianOrbitElems Kep()const = 0;
};

/**
 * @brief Lambert Problem Solver from PyKep by ESA, Adapted for CSpaceEngine.
 *
 * @details
 * This class represent a Lambert's problem. When instantiated it assumes a prograde orbit (unless otherwise stated)
 * and evaluates all the solutions up to a maximum number of multiple revolutions.
 * After the object is instantiated the solutions can be retreived using the appropriate getters. Note that the
 * number of solutions will be N_max*2 + 1, where N_max is the maximum number of revolutions.
 *
 * @note The class has been tested extensively via monte carlo runs checked with numerical propagation. Compared
 * to the previous Lambert Solver in the keplerian_toolbox it is 1.7 times faster (on average as defined
 * by lambert_test.cpp). With respect to Gooding algorithm it is 1.3 - 1.5 times faster (zero revs - multi revs).
 * The algorithm is described in detail in the publication below and its original with the author.
 *
 * @author Dario Izzo (dario.izzo@gmail.com)
 */
class __ESA_PyKep_Lambert_Solver : public __Lambert_Solver_Base
{
public:
    using Mybase = __Lambert_Solver_Base;

    struct StateBlock
    {
        uint64  Iteration;
        vec3    DepVelocity;
        vec3    DstVelocity;
        float64 XResult;
    };

protected:
    std::vector<StateBlock> StateBuffer;

    float64 Chord;
    float64 SemiPerimeter;
    float64 TransferAngle;

    float64 MaxRevoDetectTolerence    = 13;
    uint64  MaxRevoDetectIterCount    = 12;
    float64 BattinBreakpoint          = 0.01;
    float64 LancasterBreakPoint       = 0.2;
    float64 BattinHypGeomTolerence    = 11;
    uint64  ProbMaxRevolutions;

    float64 HouseholderPivotTolerence = 5;
    uint64  HouseholderPivotMaxIter   = 15;
    float64 HouseholderLeftTolerence  = 8;
    uint64  HouseholderLeftMaxIter    = 15;
    float64 HouseholderRightTolerence = 8;
    uint64  HouseholderRightMaxIter   = 15;

    vec3 VectorizedTimeEquationDerivatives(float64 x, float64 T);
    float64 TransferTimeEquation(float64 x, uint64 N);
    float64 Lagrange(float64 x, uint64 N);
    float64 BattinSeries(float64 x, uint64 N);
    float64 Lancaster(float64 x, uint64 N);

    float64 FastHouseholderIterate(float64 T, float64 x0, uint64 N,
        float64 Tolerence, uint64 MaxIter, uint64* Iter);

    void ZeroCheck();
    std::tuple<float64, float64, float64, float64,
        float64, vec3, vec3, vec3, vec3> PrepareIntermediateVariables();
    std::tuple<float64, float64> DetectMaxRevolutions
        (float64 T, float64 Lambda2, float64 Lambda3);
    void HouseholderSolve(float64 T, float64 T00, float64 T1,
        float64 Lambda2, float64 Lambda3);
    void ComputeTerminalVelocities(float64 R1, float64 R2, float64 Lambda2,
        vec3 ir1, vec3 ir2, vec3 it1, vec3 it2);

public:
    __ESA_PyKep_Lambert_Solver(const vec3& Dep, const vec3& Dst,
        const float64& TOF, const float64& GP,
        const bool& Dir = 0, const uint64& Rev = 5);

    void Run()override;

    OrbitStateVectors Dep()const override;
    OrbitStateVectors Dst()const override;
    KeplerianOrbitElems Kep()const override;

    size_t SolutionCount()const;
    OrbitStateVectors ExportState(uint64 Index, bool Pos)const;
    KeplerianOrbitElems Kep(uint64 Index)const;

    ustring ToString()const;
};

using DefaultLambertSolver = __ESA_PyKep_Lambert_Solver;

_Lambert_END


/****************************************************************************************\
*                                         多体问题                                        *
\****************************************************************************************/

// ----------------------------------------- 行星推演 ----------------------------------------- //

#define _PSim_BEGIN namespace PlanSim {
#define _PSim_END }
#define _PSim PlanSim::

_PSim_BEGIN

/*
    丹霞：这里原本想搞一个行星推演的功能的，大概就是封装一堆物体的质量和初始状态，这个状态可以是
    状态向量或轨道根数，然后以时间为自变量可以获取到此时间点时这个系统中的物体的轨道根数。简单来
    说就是创建了一个连续的，系统中各个物体的状态与时间的函数。这个功能的实现可能可以从高斯摄动方
    程和拉格朗日行星运动方程去下手，当然网上也有一些用初等方式简单模拟的，但那种方法在短期模拟的
    表现可能好一些，但如果把时间线拉长可能会出现较多的精度丢失。
*/

class __Planetary_Simulator
{
    // TODO （世纪难题待实现）
};

/*
    高斯摄动方程一般是用于非保守力导致的永久性摄动模拟，例如计算一个物体在已知摄动源的情况下状态
    随时间的变化的情况。也就是说这种方法在二体问题或一个主物体主导少数物体的系统表现较好。高斯摄
    动方程有三个参数，分别是径向力f_r，切向力f_u和法向力f_h，其中径向力的方向恒为物体的速度方向，
    也就是物体的加速度，法向力垂直于轨道平面，切向力的方向则根据径向力和法向力的叉积决定。此方程
    的求解结果为当前时间点对应的轨道根数。高斯摄动方程的定义如下：
        a'(t) = (2 * a^2 / h) * (e * sin(φ) * f_r + p / r * f_u)
        e'(t) = (sin(φ) * f_r + (e + cos(φ)) * cos(φ) * f_u - r / a * cos(i) * f_h) / h
        i'(t) = (r * cos(u)) / h * f_h
        Ω'(t) = (r * sin(u)) / (h * sin(i)) * f_h
        ω'(t) = (-cos(φ) * f_r + (1 + r / p) * sin(φ) * f_u) / (e * h) - (r * sin(u) * cos(i)) / (h * sin(i)) * f_h
        M'(t) = n + ((p * cos(φ) - 2 * e * r) * f_r + (p + r) * sin(φ) * f_u) / (e * h)
    其中：
        u = ω + φ 为纬度幅角
        p = a * (1 - e^2) 为半通径
        h = sqrt(μ * p) 为角动量
        φ 为真近点角
        r 为物体到中心物体的距离

    高斯摄动方程提供了摄动力如何具体影响各轨道要素的直观理解。并且可以得知这6个方程的右半部分都是
    不含t的，这也就意味着要求解这个方程，还需要建立f_r，f_u和f_h与t之间的关系，带入方程后这个方
    程就可以使用通用的微分方程求解器（如龙格库塔算法）求解了。另外，从方程也不难看出，纯径向摄动
    力不会改变轨道倾角，而纯法向摄动力不会直接改变轨道能量（半长轴）。
*/
class __Gauss_Perturbation_Planetary_Simulator : __Planetary_Simulator
{
    // TODO
};

/*
    拉格朗日行星运动方程同样是通过将行星轨道根数作为变量，建立微分方程组，揭示行星在摄动下的长期
    演化规律，这意味着这种方法在较复杂的多物体系统中表现更好。此方程使用一个统一的摄动势R作为参
    数，因此它也在模拟一些保守力的摄动（如各类引力摄动）时表现更好，拉格朗日行星运动方程的定义如
    下：
        a'(t) = 2 / (n * a * sqrt(1 - e^2)) * (R'_M(t) * e * sin(φ) + R'_ω(t) * (r / a))
        e'(t) = (1 - e^2) / (n * a^2 * e * sqrt(1 - e^2)) * (R'_M(t) * (p / r * sin(φ)) - R'_ω(t) * (r / a * cos(φ)))
        i'(t) = (R'_Ω(t) * cos(i) - R'_ω(t) * r / a * sin(ω + φ)) / (n * a^2 * sqrt(1 - e^2) * sin(i))
        Ω'(t) = R'_i(t) / (n * a^2 * sqrt(1 - e^2) * sin(i))
        ω'(t) = sqrt(1 - e^2) / (n * a^2 * e) * (R'_M(t) * (p / r * cos(φ)) + R'_ω(t) * (r / a * sin(φ))) - cos(i) * Ω'(t)
        M'(t) = n - (1 - e^2) / (n * a^2 * e * sqrt(1 - e^2)) * (R'_e(t) * e + R'_a(t) * a + R'_i(t) * sin(ω + φ) / sin(i))
    其中：
        n = sqrt(μ / a^3) 为平均角速度

    从以上定义可以看出，方程的右边也是没有直接含t的，这同样意味着需要用各轨道根数与摄动势和时间
    建立函数关系，带入上述定义后才能用微分方程求解器（如龙格库塔算法）求解。

    「拉格朗日方程是轨道摄动分析的瑰宝，它将复杂的摄动效应凝练为优雅的数学形式，使我们能够透过纷
    繁的表象，洞察摄动的本质。」
*/
class __Lagrange_Planetary_Simulator : __Planetary_Simulator
{
    // TODO
};

/*
    后续的一些研究记录：

    丹灵：冯康老师在1991年中国物理学会年会上曾说过：“在遥远的未来，太阳系呈现什么景象？地球会与
    其他星球相撞吗？也许有人认为，只要利用牛顿定律，按现有的计算方法编个程序，用超级计算机进行计
    算，花费足够的时间，总能得到结果。但结果可信吗？实际上，对这样复杂的计算，计算机或者根本得不
    出结果，或者得出一个错误的结果。这是计算方法问题，机器性能再好也无济于事，编程技巧再高也是无
    能为力的。”也就是说，自那个年代以前，几乎所有的微分方程求解算法都是“不辛”的，使用这些算法在
    长线求解时，会产生“能量漂移”的现象从而使得最终结果与事实“分道扬镳”。当然这里已经有的这两种龙
    格库塔算法也不例外。

    那么这个“辛”是如何定义的？我也弄不明白。只知道它解决了传统算法因长期对时间积分不能保持系统的
    能量守恒的问题。例如在求解谐振子、非线性振子、惠更斯振子、卡西尼振子、2维多晶格与准晶格定常
    流、利萨如图形、椭球面测地流线和开普勒运动这8种问题时，一切传统不辛算法，无论精度高低，都无
    一例外地全然失效，而一切辛算法，无论精度高低，则无一例外地拥有长期稳健的跟踪能力。

    因此，要实现这个功能，就必须引入一种“辛”的微分方程求解算法。然而，辛几何的研究在20世纪80年代
    后才出现，而辛数值研究到90年代才陆续出现，也就是说这门学科目前而言依旧处于理论研究状态。而且，
    “辛”算法也不是微分方程求解中的“银弹”，或者说微分方程求解领域至今从未出现，也不可能出现“银弹”，
    因为“鱼”和“熊掌”总是不可兼得的。“辛”算法相比以往的普通微分方程求解算法而言，最大的特点在于
    “长期稳定”，而非“短期高精度”。另外，绝大多数“辛”算法的延迟都是偏高的，这也就意味着它几乎只能
    用在电力系统动力学、量子力学、地学和天体力学等这类有长期仿真的领域，所以通用的“辛”算法在网上
    仍是“凤毛麟角”的存在，例如辛欧拉，辛龙格库塔等。其中辛欧拉是最简单的“辛”算法之一，鉴于大多数
    辛龙格库塔算法都未开源，所以本文在未来可能会引入辛欧拉算法，以牺牲部分精度换取长期的稳定性。

    参考文献：
    [1] 冯康, 秦孟兆. 哈密尔顿系统的辛几何算法[M]. 浙江科学技术出版社, 2003.
    [2] 模型总线. 模型总线使用小帮手（十四）：保辛算法的理论与实践[EB/OL]. 知乎, 2023
        https://zhuanlan.zhihu.com/p/659385470
    [3] 刘晓梅, 周钢, 朱帅.  Hamilton系统下基于相位误差的精细辛算法[J]. 应用数学和力学, 2019,
        40(6): 595-608. doi: 10.21656/1000-0887.390249
    [4] 孙浪, 刘福窑, 王颖, 等. 辛算法的分类与发展[J].天文学进展, 2021, 39(2): 23.
        DOI:10.3969/j.issn.1000-8349.2021.02.04.
    [5] 庞龙刚. 冯康与哈密顿系统的辛几何算法[EB/OL] 华中师范大学,
        http://www.ai4physics.cn/htmls/fengkang_and_his_symplectic_algorithm.html
    [6] hahakity.用 python 学哈密顿力学（1）[EB/OL].知乎, 2020
        https://zhuanlan.zhihu.com/p/266218742
    [7] hahakity.用 python 学哈密顿力学（2）[EB/OL].知乎, 2020
        https://zhuanlan.zhihu.com/p/265946306

*/
class __State_Vector_Planetary_Simulator : __Planetary_Simulator
{
    // TODO
};

_PSim_END


// ----------------------------------- 平面圆型限制性三体问题 ---------------------------------- //

class RocheLobe
{
public:
    float64 PrimaryMass;   // Primary at (0, 0, 0)
    float64 CompanionMass; // Companion at (Separation, 0, 0)
    float64 Separation;    // Distance between two objects

    mat3    AxisMapper    = Orbit::CSECoordToECIFrame;
    mat3    InvAxisMapper = Orbit::ECIFrameToCSECoord;

protected:
    float64 __Dimensionless_Potential_Impl(vec3 Pos)const;
    float64 __Eggelton_1983_Effective_Lobe_Radius(float64 MassRatio)const;
    float64 __Dimensionless_Potential_X_Derivative_Impl(vec3 Pos)const;
    float64 __Dimensionless_Potential_Y_Derivative_Impl(vec3 Pos)const;
    float64 __Dimensionless_Potential_Z_Derivative_Impl(vec3 Pos)const;
    std::array<vec3, 5> __Lagrange_Point_Impl(const SolvePolyRoutine& Routine)const;
    std::array<vec3, 20> __Equipotential_Dimensions_Impl(float64 PotentialOffset,
        const SolvePolyRoutine& SPRoutine)const;

public:
    vec3 BarycenterPos()const;
    // 等效势函数
    float64 DimensionlessPotential(vec3 Pos)const;
    // 引力势函数
    float64 Potential(vec3 Pos)const;
    // 等效势函数的X方向导函数
    float64 DimensionlessPotentialXDerivative(vec3 Pos)const;
    // 等效势函数的Y方向导函数
    float64 DimensionlessPotentialYDerivative(vec3 Pos)const;
    // 等效势函数的Z方向导函数
    float64 DimensionlessPotentialZDerivative(vec3 Pos)const;
    // 主星等效洛希瓣半径
    float64 PrimaryEffectiveLobeRadius()const;
    // 伴星等效洛希瓣半径
    float64 CompanionEffectiveLobeRadius()const;
    // 拉格朗日点
    std::array<vec3, 5> LagrangePoints(
        const SolvePolyRoutine& Routine = DurandKernerSolvePoly())const;
    // 洛希瓣尺寸
    std::array<vec3, 20> EquipotentialDimensions(float64 PotentialOffset = 0,
        const SolvePolyRoutine& SPRoutine = DurandKernerSolvePoly())const;
    // 物体尺寸
    std::array<vec3, 2> PhysicalDimensions(
        float64 PrimaryEffectiveRadius, float64 CompanionEffectiveRadius,
        const SolvePolyRoutine& SPRoutine = DurandKernerSolvePoly())const;
};

// ---------------------------------------------------------------------------------------------

float64 RigidRocheLimit(float64 PrimaryRadius, float64 PrimaryDensity, float64 CompanionDensity);
float64 FluidRocheLimit(float64 PrimaryMass, float64 PrimaryRadius, float64 PrimaryFlattening, float64 CompanionMass, float64 CompanionDensity);
float64 ApproxHillSphere(float64 PrimaryMass, float64 CompanionMass, float64 Separation);
float64 HillSphere(float64 PrimaryMass, float64 CompanionMass, float64 Separation, const SolvePolyRoutine& SPRoutine = DurandKernerSolvePoly());

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
