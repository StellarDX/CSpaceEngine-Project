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

SatelliteTracker::OrbitElemType SatelliteTracker::CheckParams(const OrbitElemType& InitElems)
{
    OrbitElemType ReturnElems = InitElems;
    if (IS_NO_DATA_DBL(ReturnElems.Epoch)) {ReturnElems.Epoch = J2000;}
    if (KeplerCompute(ReturnElems))
    {throw std::logic_error("2 parameters of Pericenter distance, Period and Gravity Parameter is required");}
    if (IS_NO_DATA_DBL(ReturnElems.Eccentricity)) {ReturnElems.Eccentricity = 0;}
    if (ReturnElems.Eccentricity >= 1)
    {
        //throw std::logic_error("Runaway objects tracking is not supported.");
        ReturnElems.Period = __Float64::FromBytes(POS_INF_DOUBLE);
    }
    if (IS_NO_DATA_DBL(ReturnElems.Inclination)) {ReturnElems.Inclination = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.AscendingNode)) {ReturnElems.AscendingNode = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.ArgOfPericenter)) {ReturnElems.ArgOfPericenter = 0;}
    if (IS_NO_DATA_DBL(ReturnElems.MeanAnomaly)) {ReturnElems.MeanAnomaly = 0;}
    return ReturnElems;
}

bool SatelliteTracker::KeplerCompute(OrbitElemType &InitElems)
{

}

SatelliteTracker::SatelliteTracker(const OrbitElemType& InitElems)
{

}

_ORBIT_END
_CSE_END
