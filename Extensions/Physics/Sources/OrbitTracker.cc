/**
 * @author: StellarDX Astronomy
 *
 * @brief: 天体轨道跟踪器，根据轨道六根数计算卫星的实时位置和速度
 *         使用的参数如下：
 *         轨道长半径(a)：卫星轨道的半径，表示卫星到中心的平均距离。
 *         轨道离心率(e)：卫星轨道的离心率，表示轨道的椭圆程度。
 *         轨道倾角(i)：卫星轨道与赤道面的夹角。
 *         升交点赤经(Ω)：卫星升交点在赤道面上的投影(位置)。
 *         近地点幅角(ω)：卫星近地点(最近点)的投影(位置)。
 *         平近点角(M)：卫星在升交点处的当地平交角。
 *
 *    编码过程简述：
 *      1.先计算当前状态下卫星的平均角速度，用于后续计算当前时刻下卫星的
 *        平近点角。具体可以解万有引力方程得到：G * ((M * m) / R^2) = m * R * ω^2 [a]
 *        其中G为万有引力常数，M为中心天体质量，m为卫星质量，R为两个天体
 *        之间的距离，ω为卫星的角速度。化简以后可以得到如下两个式：
 *        角速度与周期的转换：ω = 360 / T (T为周期) [b]
 *        联立[a]和[b]，得：ω = sqrt((G * M) / R^3)
 *        (注：这个数据只是在没有任何干扰的情况下得到的，实际上一个卫星
 *        的轨道可能会受到其他卫星的影响而发生变化)
 *      2.轨道六根数中唯一一个随时间变化的量是平近点角(M)，但是真正确定
 *        卫星位置需要用到真近点角，所以此处需要进行一些转换，公式如下： [1]
 *        平近点角计算偏近点角(E)：E = M + e * sin(E)  [c] (此方程由开普勒第二定律得到)
 *        偏近点角计算真近点角(φ)：φ = arctan((sqrt(1 - e^2) * sin(E)) / (cos(E) - e))  [d]
 *        很显然，式[c]无法直接得到精确解，此处使用牛顿迭代以获取近似值。
 *      3.按照@蓝羽提出的模型[2]，轨道坐标系经过三次方向余弦矩阵变换即
 *        可变为中心天体惯性系。代入剩下的根数即可得到位置与速度。
 *
 *    参考文献：
 *      [1] 范伟,王贵文,刘发发. GPS卫星轨道位置计算方法的研究 [J].
 *          山西师范大学学报(自然科学版), 2015, 29 (04): 63-68.
 *          DOI:10.16207/j.cnki.1009-4490.2015.04.015.
 *      [2] 轨道六根数 - 卫星百科(灰机Wiki)
 *          https://sat.huijiwiki.com/wiki/%E8%BD%A8%E9%81%93%E5%85%AD%E6%A0%B9%E6%95%B0
 *
 * @version:
 */

/*
    Member functions of Satellite Tracker
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

_CSE_BEGIN
_ORBIT_BEGIN

void __Real_Time_Satellite_Tracker_And_Predictor::_Check_Params()
{
    if (IS_NO_DATA_DBL(_M_OrbitElems.Epoch)) {_M_OrbitElems.Epoch = J2000;}
    if (IS_NO_DATA_DBL(_M_OrbitElems.PericenterDist))
    {throw std::logic_error("Pericenter distance is required");}
    if (IS_NO_DATA_DBL(_M_OrbitElems.Period) && IS_NO_DATA_DBL(_M_OrbitElems.GravParam))
    {throw std::logic_error("Period or Parent body's mass is required");}
    else if (!IS_NO_DATA_DBL(_M_OrbitElems.Period) && IS_NO_DATA_DBL(_M_OrbitElems.GravParam))
    {
        _GetCenterObjectMass();
    }
    if (IS_NO_DATA_DBL(_M_OrbitElems.Eccentricity)) {_M_OrbitElems.Eccentricity = 0;}
    if (_M_OrbitElems.Eccentricity >= 1)
    {
        throw std::logic_error("Runaway objects tracking is not supported.");
        _M_OrbitElems.Period = __Float64::FromBytes(POS_INF_DOUBLE);
    }
    if (IS_NO_DATA_DBL(_M_OrbitElems.Inclination)) {_M_OrbitElems.Inclination = 0;}
    if (IS_NO_DATA_DBL(_M_OrbitElems.AscendingNode)) {_M_OrbitElems.AscendingNode = 0;}
    if (IS_NO_DATA_DBL(_M_OrbitElems.ArgOfPericenter)) {_M_OrbitElems.ArgOfPericenter = 0;}
    if (IS_NO_DATA_DBL(_M_OrbitElems.MeanAnomaly)) {_M_OrbitElems.MeanAnomaly = 0;}
}

void __Real_Time_Satellite_Tracker_And_Predictor::_GetCenterObjectMass()
{
    float64 SemiMajorAxis = _SemiMajorAxis();
    _M_OrbitElems.GravParam =
        (4. * CSE_PI * CSE_PI * SemiMajorAxis * SemiMajorAxis * SemiMajorAxis)
        / (_M_OrbitElems.Period * _M_OrbitElems.Period);
}

__Real_Time_Satellite_Tracker_And_Predictor::__Real_Time_Satellite_Tracker_And_Predictor
    (const OrbitElemsType &Orbit, float64 ParentMass)
    : _M_OrbitElems(Orbit), _M_Epoch(Orbit.Epoch)
{
    _Check_Params();
    _M_OrbitElems.GravParam = GravConstant * ParentMass;
}

__Real_Time_Satellite_Tracker_And_Predictor::__Real_Time_Satellite_Tracker_And_Predictor
    (const Object &Obj, float64 ParentMass)
    : __Real_Time_Satellite_Tracker_And_Predictor(Obj.Orbit, ParentMass)
{
    _Check_Params();
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddMsecs(int64 Ms)
{
    _M_Epoch += Ms / 86400000.;
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddSeconds(int64 Sec)
{
    _M_Epoch += Sec / 86400.;
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddHours(int64 Hrs)
{
    _M_Epoch += Hrs / 24.;
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddDays(int64 Days)
{
    _M_Epoch += Days;
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddYears(int64 Years)
{
    _M_Epoch += Years * 365.25;
}

void __Real_Time_Satellite_Tracker_And_Predictor::AddCenturies(int64 Centuries)
{
    _M_Epoch += Centuries * 36525;
}

void __Real_Time_Satellite_Tracker_And_Predictor::SetDate(CSEDateTime DateTime)
{
    GetJDFromDate(&_M_Epoch,
        DateTime.date().year(),
        DateTime.date().month(),
        DateTime.date().day(),
        DateTime.time().hour(),
        DateTime.time().minute(),
        DateTime.time().second() +
        DateTime.time().msec() / 1000.);
}

void __Real_Time_Satellite_Tracker_And_Predictor::SetDateJD(float64 JD)
{
    _M_Epoch = JD;
}

void __Real_Time_Satellite_Tracker_And_Predictor::ToCurrentDate()
{
    _M_Epoch = GetJDFromSystem();
}

void __Real_Time_Satellite_Tracker_And_Predictor::Reset()
{
    _M_Epoch = _M_OrbitElems.Epoch;
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::MeanMotion() const
{
    if (!isinf(_M_OrbitElems.Period) || !isnan(_M_OrbitElems.Period))
    {
        return 360. / _M_OrbitElems.Period;
    }
    else
    {
        float64 SemiMajorAxis = _SemiMajorAxis();
        return sqrt(_M_OrbitElems.GravParam / (SemiMajorAxis * SemiMajorAxis * SemiMajorAxis));
    }
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::MeanAnomaly() const
{
    return _M_OrbitElems.MeanAnomaly;
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::CurrentMeanAnomaly() const
{
    return mod(_M_OrbitElems.MeanAnomaly +
        MeanMotion() * (CurrentDateTimeJD() - EpochJD()) * SynodicDay, 360);
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::_EccentricAnomaly(float64 _MeanAnomaly)const
{
    // 牛顿迭代需要弧度三角函数才能得到正确结果
    float64 MeanAnomalyRadians = Angle::FromDegrees(_MeanAnomaly).ToRadians();
    auto KeplerEquation = [this, MeanAnomalyRadians](float64 EccAnomaly)
    {
        return MeanAnomalyRadians + _M_OrbitElems.Eccentricity
            * __IBM_SIN64F(EccAnomaly) - EccAnomaly;
    };
    auto KeplerEquationDerivative = [this](float64 EccAnomaly)
    {
        return _M_OrbitElems.Eccentricity * __IBM_COS64F(EccAnomaly) - 1;
    };
    NewtonIterator It(KeplerEquation, KeplerEquationDerivative);
    return Angle::FromRadians(It(0, MeanAnomalyRadians)).ToDegrees();
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::EccentricAnomaly() const
{
    return _EccentricAnomaly(MeanAnomaly());
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::CurrentEccentricAnomaly() const
{
    return _EccentricAnomaly(CurrentMeanAnomaly());
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::_TrueAnomaly(float64 _EccAnomaly) const
{
    float64 EccAnomaly = _EccAnomaly;
    float64 TrAnomaly = Arctan2((sqrt(1. - _M_OrbitElems.Eccentricity *
        _M_OrbitElems.Eccentricity) * sin(EccAnomaly)),
        (cos(EccAnomaly) - _M_OrbitElems.Eccentricity));
    return TrAnomaly > 0 ? TrAnomaly : TrAnomaly + 360;
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::TrueAnomaly() const
{
    return _TrueAnomaly(EccentricAnomaly());
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::CurrentTrueAnomaly() const
{
    return _TrueAnomaly(CurrentEccentricAnomaly());
}

__Real_Time_Satellite_Tracker_And_Predictor::OrbitElemsType
__Real_Time_Satellite_Tracker_And_Predictor::OrbitElems() const
{
    return _M_OrbitElems;
}

__Real_Time_Satellite_Tracker_And_Predictor::OrbitElemsType
__Real_Time_Satellite_Tracker_And_Predictor::CurrentOrbitElems() const
{
    auto _OrbitElems = _M_OrbitElems;
    _OrbitElems.Epoch = CurrentDateTimeJD();
    _OrbitElems.MeanAnomaly = CurrentMeanAnomaly();
    return _OrbitElems;
}

CSEDateTime __Real_Time_Satellite_Tracker_And_Predictor::Epoch() const
{
    return JDToDateTime(_M_OrbitElems.Epoch);
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::EpochJD() const
{
    return _M_OrbitElems.Epoch;
}

CSEDateTime __Real_Time_Satellite_Tracker_And_Predictor::CurrentDateTime() const
{
    return JDToDateTime(_M_Epoch);
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::CurrentDateTimeJD() const
{
    return _M_Epoch;
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::_SemiMajorAxis() const
{
    return _M_OrbitElems.PericenterDist / (1. - _M_OrbitElems.Eccentricity);
}

float64 __Real_Time_Satellite_Tracker_And_Predictor::_SemiLatusRectum() const
{
    float64 SemiMajorAxis = _SemiMajorAxis();
    if (_M_OrbitElems.Eccentricity >= 0 && _M_OrbitElems.Eccentricity < 1)
    {
        return SemiMajorAxis * (1. - _M_OrbitElems.Eccentricity * _M_OrbitElems.Eccentricity);
    }
    else if (_M_OrbitElems.Eccentricity == 1)
    {
        return SemiMajorAxis;
    }
    else if (_M_OrbitElems.Eccentricity > 1)
    {
        return SemiMajorAxis * (_M_OrbitElems.Eccentricity * _M_OrbitElems.Eccentricity - 1.);
    }
    return SemiMajorAxis; // Unreachable
}

__Real_Time_Satellite_Tracker_And_Predictor::OrbitState
__Real_Time_Satellite_Tracker_And_Predictor::_Gen(float64 _TAnomaly) const
{
    float64 SemiLatusRectum = _SemiLatusRectum();

    #if defined(SATELLITE_TRACKER_COORD_Realistic) ||\
        defined(SATELLITE_TRACKER_COORD_SpaceEngine)

    vec3 Pos = (SemiLatusRectum / (1. + _M_OrbitElems.Eccentricity * cos(_TAnomaly))) * vec3
        (cos(_M_OrbitElems.AscendingNode) * cos(_M_OrbitElems.ArgOfPericenter + _TAnomaly) -
        sin(_M_OrbitElems.AscendingNode) * sin(_M_OrbitElems.ArgOfPericenter + _TAnomaly) *
        cos(_M_OrbitElems.Inclination),
        sin(_M_OrbitElems.AscendingNode) * cos(_M_OrbitElems.ArgOfPericenter + _TAnomaly) +
        cos(_M_OrbitElems.AscendingNode) * sin(_M_OrbitElems.ArgOfPericenter + _TAnomaly) *
        cos(_M_OrbitElems.Inclination),
        sin(_M_OrbitElems.ArgOfPericenter + _TAnomaly) * sin(_M_OrbitElems.Inclination));

    vec3 Vel = sqrt(_M_OrbitElems.GravParam / SemiLatusRectum) * vec3
        (-cos(_M_OrbitElems.AscendingNode) * (sin(_M_OrbitElems.ArgOfPericenter + _TAnomaly) +
        _M_OrbitElems.Eccentricity * sin(_M_OrbitElems.ArgOfPericenter)) -
        sin(_M_OrbitElems.AscendingNode) * (cos(_M_OrbitElems.ArgOfPericenter + _TAnomaly) +
        _M_OrbitElems.Eccentricity * cos(_M_OrbitElems.ArgOfPericenter)) * cos(_M_OrbitElems.Inclination),
        -sin(_M_OrbitElems.AscendingNode) * (sin(_M_OrbitElems.ArgOfPericenter + _TAnomaly) +
        _M_OrbitElems.Eccentricity * sin(_M_OrbitElems.ArgOfPericenter)) +
        cos(_M_OrbitElems.AscendingNode) * (cos(_M_OrbitElems.ArgOfPericenter + _TAnomaly) +
        _M_OrbitElems.Eccentricity * cos(_M_OrbitElems.ArgOfPericenter)) * cos(_M_OrbitElems.Inclination),
        (cos(_M_OrbitElems.ArgOfPericenter + _TAnomaly) + _M_OrbitElems.Eccentricity *
        cos(_M_OrbitElems.ArgOfPericenter)) * sin(_M_OrbitElems.Inclination));

    #if defined(SATELLITE_TRACKER_COORD_SpaceEngine)
    #pragma message("We don't know how the velocity calculation in SpaceEngine work, this result only for reference.")
    std::swap(Pos.y, Pos.z);
    Pos.z = -Pos.z;
    std::swap(Vel.y, Vel.z);
    Vel.z = -Vel.z;
    #endif

    #else
    #error Invalid coordinate system.
    #endif

    return
    {
        .RefPlane = _M_OrbitElems.RefPlane,
        .GravParam = _M_OrbitElems.GravParam,
        .Time = CurrentDateTimeJD(),
        .Position = Pos,
        .Velocity = Vel
    };
}

__Real_Time_Satellite_Tracker_And_Predictor::OrbitState
__Real_Time_Satellite_Tracker_And_Predictor::State() const
{
    return _Gen(TrueAnomaly());
}

__Real_Time_Satellite_Tracker_And_Predictor::OrbitState
__Real_Time_Satellite_Tracker_And_Predictor::CurrentState() const
{
    return _Gen(CurrentTrueAnomaly());
}

_ORBIT_END

_CSE_END
