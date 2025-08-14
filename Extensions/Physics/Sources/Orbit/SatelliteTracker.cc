/**
 * @author: StellarDX Astronomy
 *
 * @brief: 天体轨道跟踪器，根据轨道六根数计算物体的实时位置和速度
 *         使用的参数如下：
 *         近地点距离(P)：物体距离中心最近时的距离。
 *         （至于为何不用半长轴，是因为当轨道为抛物线时半长轴恒为inf）
 *         轨道离心率(e)：物体轨道的离心率，表示轨道的椭圆程度。
 *         轨道倾角(i)：物体轨道与赤道面的夹角。
 *         升交点赤经(Ω)：物体升交点在赤道面上的投影(位置)。
 *         近地点幅角(ω)：物体近地点(最近点)的投影(位置)。
 *         平近点角(M)：物体在升交点处的当地平交角。
 *
 *    编码过程简述：
 *      1.先计算当前状态下物体的平均角速度，用于后续计算当前时刻下物体的
 *        平近点角。具体可以解万有引力方程得到：
 *          G * ((M * m) / R^2) = m * R * ω^2 [a]
 *        其中G为万有引力常数，M为中心天体质量，m为物体质量，R为两个天体
 *        之间的距离，ω为物体的角速度。化简以后可以得到如下两个式：
 *        角速度与周期的转换：
 *          ω = 360 / T (T为周期) [b]
 *        联立[a]和[b]，得：
 *          ω = sqrt((G * M) / R^3)
 *        其中椭圆和双曲线轨道中R = a，抛物线轨道中R = p
 *        (注：这个数据只是在没有任何干扰的情况下得到的，实际上一个物体
 *        的轨道可能会受到其他物体的影响而发生变化)
 *      2.轨道六根数中唯一一个随时间变化的量是平近点角(M)，但是真正确定
 *        物体位置需要用到真近点角，所以此处需要进行一些转换，公式如下：
 *        [1][3][4]
 *        平近点角计算偏近点角(E)：
 *          M = E - e * sin(E)          (椭圆轨道) [c1]
 *          M = (1/2) * E + (1/6) * E^3 (抛物线轨道) [c2]
 *          M = e * sinh(E) - E         (双曲线轨道) [c3]
 *        偏近点角计算真近点角(φ)：
 *          φ = arctan((sqrt(1 - e^2) * sin(E)) / (cos(E) - e))   (椭圆轨道) [d1]
 *          φ = 2 * arctan(E)                                     (抛物线轨道) [d2]
 *          φ = 2 * arctan(sqrt((e + 1) / (e - 1)) * tanh(E / 2)) (双曲线轨道) [d3]
 *        很显然，式[c1]和式[c3]无法直接得到精确解，此处使用数值算法以
 *        获取近似值。
 *      3.按照@蓝羽提出的模型[2]，轨道坐标系经过三次方向余弦矩阵变换即
 *        可变为中心天体惯性系。代入剩下的根数即可得到位置与速度。
 *
 *    参考文献：
 *      [1] 范伟,王贵文,刘发发. GPS卫星轨道位置计算方法的研究 [J].
 *          山西师范大学学报(自然科学版), 2015, 29 (04): 63-68.
 *          DOI:10.16207/j.cnki.1009-4490.2015.04.015.
 *      [2] 轨道六根数 - 卫星百科(灰机Wiki)
 *          https://sat.huijiwiki.com/wiki/%E8%BD%A8%E9%81%93%E5%85%AD%E6%A0%B9%E6%95%B0
 *      [3] 开普勒方程 - 卫星百科(灰机Wiki)
 *          https://sat.huijiwiki.com/wiki/%E5%BC%80%E6%99%AE%E5%8B%92%E6%96%B9%E7%A8%8B
 *      [4] 偏近点角 - 卫星百科(灰机Wiki)
 *          https://sat.huijiwiki.com/wiki/%E5%81%8F%E8%BF%91%E7%82%B9%E8%A7%92
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

KeplerianSatelliteTracker::BaseType KeplerianSatelliteTracker::CheckParams(const BaseType& InitElems)
{
    BaseType ReturnElems = InitElems;
    if (IS_NO_DATA_DBL(ReturnElems.Epoch)) {ReturnElems.Epoch = J2000;}
    if (IS_NO_DATA_DBL(ReturnElems.Eccentricity)) {ReturnElems.Eccentricity = 0;}
    if (ReturnElems.Eccentricity < 1 && !KeplerCompute(ReturnElems))
    {
        throw std::logic_error("2 parameters of Pericenter distance, Period and Gravity parameter is required");
    }
    if (ReturnElems.Eccentricity >= 1)
    {
        ReturnElems.Period = __Float64::FromBytes(POS_INF_DOUBLE);
        if (IS_NO_DATA_DBL(ReturnElems.PericenterDist) || IS_NO_DATA_DBL(ReturnElems.GravParam))
        {
            std::logic_error("Pericenter distance and Gravity parameter is required for runaway objects");
        }
    }
    AngularVelocity = PericenterDistToAngularVelocity(
        ReturnElems.Eccentricity, ReturnElems.PericenterDist, ReturnElems.GravParam);
    if (IS_NO_DATA_DBL(ReturnElems.Inclination)) {ReturnElems.Inclination = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.AscendingNode)) {ReturnElems.AscendingNode = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.ArgOfPericenter)) {ReturnElems.ArgOfPericenter = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.MeanAnomaly)) {ReturnElems.MeanAnomaly = 0;}
    //TruncateTo360(ReturnElems.MeanAnomaly);
    return ReturnElems;
}

KeplerianSatelliteTracker::KeplerianSatelliteTracker(const BaseType& InitElems)
{
    auto IElems = CheckParams(InitElems);
    InitialState = IElems;
    CurrentState = IElems;
}

KeplerianSatelliteTracker::KeplerianSatelliteTracker(const OrbitStateVectors &InitState)
{

}

void KeplerianSatelliteTracker::AddMsecs(int64 Ms)
{
    SetDate(CurrentState.Epoch + Ms / (1000.0 * SynodicDay));
}

void KeplerianSatelliteTracker::AddSeconds(int64 Sec)
{
    SetDate(CurrentState.Epoch + Sec / SynodicDay);
}

void KeplerianSatelliteTracker::AddHours(int64 Hrs)
{
    SetDate(CurrentState.Epoch + Hrs * (3600.0 / SynodicDay));
}

void KeplerianSatelliteTracker::AddDays(int64 Days)
{
    SetDate(CurrentState.Epoch + Days);
}

void KeplerianSatelliteTracker::AddYears(int64 Years)
{
    CSEDateTime CurrentDateTime = JDToDateTime(CurrentState.Epoch);
    CurrentDateTime.AddYears(Years);
    float64 NewJD;
    GetJDFromDate(&NewJD,
        CurrentDateTime.date().year(), CurrentDateTime.date().month(), CurrentDateTime.date().day(),
        CurrentDateTime.time().hour(), CurrentDateTime.time().minute(),
        CurrentDateTime.time().second() + CurrentDateTime.time().msec() / 1000.0);
    SetDate(CurrentDateTime);
}

void KeplerianSatelliteTracker::AddCenturies(int64 Centuries)
{
    AddYears(100 * Centuries);
}

void KeplerianSatelliteTracker::ToCurrentDate()
{
    SetDate(GetJDFromSystem());
}

void KeplerianSatelliteTracker::SetDate(CSEDateTime DateTime)
{
    float64 JD;
    GetJDFromDate(&JD,
        DateTime.date().year(), DateTime.date().month(), DateTime.date().day(),
        DateTime.time().hour(), DateTime.time().minute(),
        DateTime.time().second() + DateTime.time().msec() / 1000.0);
    SetDate(JD);
}

void KeplerianSatelliteTracker::SetDate(float64 JD)
{
    CurrentState.Epoch = JD;
    float64 TimeDiff = (JD - InitialState.Epoch) * SynodicDay;
    float64 MDeg = InitialState.MeanAnomaly.ToDegrees();
    float64 MPass = AngularVelocity.ToDegrees() * TimeDiff;
    CurrentState.MeanAnomaly = Angle::FromDegrees(MDeg + MPass);
}

void KeplerianSatelliteTracker::Move(Angle MeanAnomalyOffset)
{
    float64 MDeg = CurrentState.MeanAnomaly.ToDegrees();
    float64 ODeg = MeanAnomalyOffset.ToDegrees();
    CurrentState.MeanAnomaly = Angle::FromDegrees(MDeg + ODeg);
    //TruncateTo360(CurrentState.MeanAnomaly);
    float64 TimePass = ODeg / AngularVelocity.ToDegrees();
    CurrentState.Epoch += TimePass / SynodicDay;
}

void KeplerianSatelliteTracker::Reset()
{
    CurrentState = InitialState;
}

KeplerianOrbitElems KeplerianSatelliteTracker::KeplerianElems() const
{
    return CurrentState;
}

EquinoctialOrbitElems KeplerianSatelliteTracker::EquinoctialElems() const
{
    Angle AscNodePArgOfPeri = Angle::FromDegrees(
        CurrentState.AscendingNode.ToDegrees() +
        CurrentState.ArgOfPericenter.ToDegrees());
    Angle InclinationD2 = Angle::FromDegrees(
        CurrentState.Inclination.ToDegrees() / 2.);
    Angle MeanLongitude = Angle::FromDegrees(
        AscNodePArgOfPeri.ToDegrees() +
        CurrentState.MeanAnomaly.ToDegrees());
    //TruncateTo360(MeanLongitude);
    return
    {
        .RefPlane       = CurrentState.RefPlane,
        .Epoch          = CurrentState.Epoch,
        .GravParam      = CurrentState.GravParam,
        .PericenterDist = CurrentState.PericenterDist,
        .Period         = CurrentState.Period,
        .EccentricityF  = CurrentState.Eccentricity * cos(AscNodePArgOfPeri),
        .EccentricityG  = CurrentState.Eccentricity * sin(AscNodePArgOfPeri),
        .InclinationH   = tan(InclinationD2) * cos(CurrentState.AscendingNode),
        .InclinationK   = tan(InclinationD2) * sin(CurrentState.AscendingNode),
        .MeanLongitude  = MeanLongitude
    };
}

OrbitStateVectors KeplerianSatelliteTracker::StateVectors(mat3 AxisMapper)const
{
    Angle EccentricAnomaly = InverseKeplerianEquation(
        CurrentState.Eccentricity, CurrentState.MeanAnomaly);
    Angle TrueAnomaly = GetTrueAnomalyFromEccentricAnomaly(
        CurrentState.Eccentricity, EccentricAnomaly);
    float64 SemiLatusRectum = GetSemiLatusRectumFromPericenterDist(
        CurrentState.Eccentricity, CurrentState.PericenterDist);
    float64 CurrentDist = SemiLatusRectum /
        (1. + CurrentState.Eccentricity * cos(TrueAnomaly));
    Angle ArgOfLatitude = GetArgOfLatitude(
        CurrentState.ArgOfPericenter, TrueAnomaly);

    float64 SinAscNode = sin(CurrentState.AscendingNode);
    float64 CosAscNode = cos(CurrentState.AscendingNode);
    float64 SinArgOfLatitude = sin(ArgOfLatitude);
    float64 CosArgOfLatitude = cos(ArgOfLatitude);
    float64 SinInclination = sin(CurrentState.Inclination);
    float64 CosInclination = cos(CurrentState.Inclination);

    vec3 CurrentPos = (AxisMapper * matrix<1, 3>{CurrentDist * vec3(
        CosAscNode * CosArgOfLatitude -
        SinAscNode * SinArgOfLatitude * CosInclination,
        SinAscNode * CosArgOfLatitude +
        CosAscNode * SinArgOfLatitude * CosInclination,
        SinArgOfLatitude * SinInclination)})[0];

    float64 EMSinArgOfPeri = CurrentState.Eccentricity *
        sin(CurrentState.ArgOfPericenter);
    float64 EMCosArgOfPeri = CurrentState.Eccentricity *
        cos(CurrentState.ArgOfPericenter);
    float64 SinArgOfLatPEMSinArgOfPeri = SinArgOfLatitude + EMSinArgOfPeri;
    float64 CosArgOfLatPEMCosArgOfPeri = CosArgOfLatitude + EMCosArgOfPeri;

    vec3 CurrentVelocity = (AxisMapper * matrix<1, 3>{
        sqrt(CurrentState.GravParam / SemiLatusRectum) * vec3(
        -CosAscNode * SinArgOfLatPEMSinArgOfPeri -
        SinAscNode * CosArgOfLatPEMCosArgOfPeri * CosInclination,
        -SinAscNode * SinArgOfLatPEMSinArgOfPeri +
        CosAscNode * CosArgOfLatPEMCosArgOfPeri * CosInclination,
        CosArgOfLatPEMCosArgOfPeri * SinInclination)})[0];

    return
    {
        .RefPlane  = CurrentState.RefPlane,
        .GravParam = CurrentState.GravParam,
        .Time      = CurrentState.Epoch,
        .Position  = CurrentPos,
        .Velocity  = CurrentVelocity
    };
}

_ORBIT_END
_CSE_END
