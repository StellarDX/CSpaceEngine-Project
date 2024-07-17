/*
    IAU WGCCRE Rotation Tracker member functions
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

#include "CSE/Base/ConstLists.h"
#include "CSE/Physics/Rotation.h"

_CSE_BEGIN
_ROT_BEGIN

void __IAU_WGCCRE_Complex_Rotation_Model::_Check_Params()
{
    if (IS_NO_DATA_DBL(_M_Data.Epoch)) {_M_Data.Epoch = J2000;}
    if (IS_NO_DATA_DBL(_M_Data.PoleRA)) {_M_Data.PoleRA = 0;}
    if (IS_NO_DATA_DBL(_M_Data.PoleRARate)) {_M_Data.PoleRARate = 0;}
    if (IS_NO_DATA_DBL(_M_Data.PoleDec)) {_M_Data.PoleDec = 90;}
    if (IS_NO_DATA_DBL(_M_Data.PoleDecRate)) {_M_Data.PoleDecRate = 0;}
    if (IS_NO_DATA_DBL(_M_Data.PrimeMeridian)) {_M_Data.PrimeMeridian = 0;}
    if (IS_NO_DATA_DBL(_M_Data.RotationRate)) {throw RotationCalculateException("Rotation Rate is required");}
    if (IS_NO_DATA_DBL(_M_Data.RotationAccel)) {_M_Data.RotationAccel = 0;}
}

vec3 __IAU_WGCCRE_Complex_Rotation_Model::_Fix_Data(vec3 State)const
{
    if (abs(State.x) >= 360) {State.x = mod(State.x, 360);}
    if (State.x < 0) {State.x += 360;}
    auto DecPos = Quadrant(State.y);
    if (DecPos > 2 && DecPos < 6)
    {
        State.y = 180 - State.y;
    }
    if (abs(State.z) >= 360) {State.z = mod(State.z, 360);}
    if (State.z < 0) {State.z += 360;}
    return State;
}

__IAU_WGCCRE_Complex_Rotation_Model::__IAU_WGCCRE_Complex_Rotation_Model(const RotationData &_NewData)
    : _M_Data(_NewData), _M_Epoch(_NewData.Epoch) {_Check_Params();}

void __IAU_WGCCRE_Complex_Rotation_Model::AddMsecs(int64 Ms)
{
    _M_Epoch += Ms / 86400000.;
}

void __IAU_WGCCRE_Complex_Rotation_Model::AddSeconds(int64 Sec)
{
    _M_Epoch += Sec / 86400.;
}

void __IAU_WGCCRE_Complex_Rotation_Model::AddHours(int64 Hrs)
{
    _M_Epoch += Hrs / 24.;
}

void __IAU_WGCCRE_Complex_Rotation_Model::AddDays(int64 Days)
{
    _M_Epoch += Days;
}

void __IAU_WGCCRE_Complex_Rotation_Model::AddYears(int64 Years)
{
    _M_Epoch += Years * 365.25;
}

void __IAU_WGCCRE_Complex_Rotation_Model::AddCenturies(int64 Centuries)
{
    _M_Epoch += Centuries * 36525;
}

void __IAU_WGCCRE_Complex_Rotation_Model::ToCurrentDate()
{
    _M_Epoch = GetJDFromSystem();
}

void __IAU_WGCCRE_Complex_Rotation_Model::SetDate(CSEDateTime DateTime)
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

void __IAU_WGCCRE_Complex_Rotation_Model::SetDateJD(float64 JD)
{
    _M_Epoch = JD;
}

void __IAU_WGCCRE_Complex_Rotation_Model::Reset()
{
    _M_Epoch = _M_Data.Epoch;
}

vec3 __IAU_WGCCRE_Complex_Rotation_Model::State() const
{
    return operator()(0);
}

// SimpleRotation __IAU_WGCCRE_Complex_Rotation_Model::ToSimple() const
// {
//     // TODO
// }

vec3 __IAU_WGCCRE_Complex_Rotation_Model::CurrentState() const
{
    return operator()(_M_Epoch - _M_Data.Epoch);
}

// SimpleRotation __IAU_WGCCRE_Complex_Rotation_Model::CurrentStateToSimple() const
// {
//     // TODO
// }

vec3 __IAU_WGCCRE_Complex_Rotation_Model::operator()(float64 AddTime)const
{
    float64 TDays = AddTime;
    float64 TCenturies = AddTime / 36525.;

    float64 RA = _M_Data.PoleRA + _M_Data.PoleRARate * TCenturies;
    float64 Dec = _M_Data.PoleDec + _M_Data.PoleDecRate * TCenturies;
    float64 Meridian = _M_Data.PrimeMeridian + _M_Data.RotationRate * TDays + _M_Data.RotationAccel * TCenturies * TCenturies;

    if (!_M_Data.PeriodicTerms.empty())
    {
        float64 T = _M_Data.UsingSecular ? TCenturies : TDays;
        for (auto i : _M_Data.PeriodicTerms)
        {
            RA += i[0] * sin(Angle::FromDegrees(i[3] + i[4] * T + i[5] * T * T));
            Dec += i[1] * cos(Angle::FromDegrees(i[3] + i[4] * T + i[5] * T * T));
            Meridian += i[2] * sin(Angle::FromDegrees(i[3] + i[4] * T + i[5] * T * T));
        }
    }

    return _Fix_Data(vec3(RA, Dec, Meridian));
}

_ROT_END
_CSE_END
