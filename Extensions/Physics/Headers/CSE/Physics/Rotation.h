/*
    Rotation engine for CSpaceEngine
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

#ifndef _CSE_ROTATION_
#define _CSE_ROTATION_

#include "CSE/Base.h"
#include "CSE/Object.h"
#include "CSE/Astronomy.h"

#define _ROT_BEGIN namespace Rotation {
#define _ROT_END }
#define _ROT Rotation::

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

_CSE_BEGIN
_ROT_BEGIN

/**
 * @brief 经典物体自转参数
 */
struct SimpleRotationalElems
{
    float64     RotationEpoch     = _NoDataDbl; // J2000
    Angle       Obliquity         = _NoDataDbl; // degrees
    Angle       EqAscendNode      = _NoDataDbl; // degrees
    Angle       RotationOffset    = _NoDataDbl; // degrees
    float64     RotationPeriod    = _NoDataDbl; // seconds
    float64     Precession        = _NoDataDbl; // seconds

    SimpleRotationalElems& operator=
        (const Object::SimpleRotationModel& P)noexcept;
    operator Object::SimpleRotationModel();
};

/**
 * @brief IAU/IAG 建议案 (WGCCRE) 的复杂自转模型参数。
 *
 * 用于存储描述物体精确旋转状态的参数。
 * 它基于物体的北极方向 (由赤经和赤纬定义) 和本初子午线位置来描述旋转。
 * 支持长期项（线性速率）和周期项（三角函数级数）。
 */
struct WGCCREComplexRotationalElems
{
    float64     Epoch             = _NoDataDbl; // J2000
    Angle       PoleRA            = _NoDataDbl; // degrees
    Angle       PoleRARate        = _NoDataDbl; // degrees/century
    Angle       PoleDec           = _NoDataDbl; // degrees
    Angle       PoleDecRate       = _NoDataDbl; // degrees/century
    Angle       PrimeMeridian     = _NoDataDbl; // degrees
    Angle       RotationRate      = _NoDataDbl; // degrees/day
    Angle       RotationAccel     = _NoDataDbl; // degrees/century^2

    // 时间单位，Diurnal为天，Secular为世纪
    enum PeriodicTermsMode : uint8_t {Diurnal = 0, Secular = 1}
    PeriodicTermsUnit;

    struct PeriodicTermsElems
    {
        float64 PoleRAAmpScale;   // RA.A  [RA:  Σ(A * sin(δ * t^2 + ω * t + φ)]
        float64 PoleDecAmpScale;  // Dec.A [Dec: Σ(A * cos(δ * t^2 + ω * t + φ)]
        float64 PrimeMerAmpScale; // PM.A  [PM:  Σ(A * sin(δ * t^2 + ω * t + φ)]
        Angle   Phase;            // φ
        Angle   Frequency;        // ω
        Angle   FrequencyRate;    // δ
    };

    std::vector<PeriodicTermsElems> PeriodicTerms;

    WGCCREComplexRotationalElems& operator=
        (const Object::IAURotationModel& P)noexcept;
    operator Object::IAURotationModel();
};

/**
 * @brief 自转跟踪器，一切自转工具的地基
 */
__interface RotationTracker
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
    virtual void Reset() = 0;

    virtual void NorthPolePos(Sexagesimal* RA, Sexagesimal* Dec)const = 0;
    virtual Angle RotationPhase()const = 0;
};

/**
 * @brief 基于IAU自转模型的自转跟踪器
 * 「是非成败转头空，青山依旧在，几度夕阳红。」
 */
class IAU_WGCCRERotationTracker : public RotationTracker
{
public:
    using Mybase    = RotationTracker;
    using BaseType  = WGCCREComplexRotationalElems;

    // 默认黄赤交角以地球的为准，转经典参数会用到
    static const Sexagesimal DefaultNorthEclipticPoleRA;
    static const Sexagesimal DefaultNorthEclipticPoleDec;

protected:
    Sexagesimal NorthEclipticPoleRA;
    Sexagesimal NorthEclipticPoleDec;
    BaseType    InitialState;
    BaseType    CurrentState;

    BaseType CheckParams(const BaseType& InitElems);

public:
    IAU_WGCCRERotationTracker(const BaseType& InitElems);

    void AddMsecs(int64 Ms) override;
    void AddSeconds(int64 Sec) override;
    void AddHours(int64 Hrs) override;
    void AddDays(int64 Days)override;
    void AddYears(int64 Years)override;
    void AddCenturies(int64 Centuries)override;

    void ToCurrentDate()override;
    void SetDate(CSEDateTime DateTime)override;
    void SetDate(float64 JD)override;
    void Reset()override;

    BaseType GetCurrentState()const;
    void NorthPolePos(Sexagesimal* RA, Sexagesimal* Dec) const override;
    Angle RotationPhase() const override;
};

float64 SynodicRotationPeriod(float64 RotationPeriod, float64 OrbitalPeriod);
float64 StellarRotationPeriod(float64 RotationPeriod, float64 Precession = 0.0);
float64 TropicalOrbitalPeriod(float64 SiderealOrbitalPeriod, float64 Precession = 0.0);

_ROT_END
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
