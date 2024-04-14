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

class RotationCalculateException : public std::logic_error
{
public:
    RotationCalculateException(char const* e) : std::logic_error(e) {}
};

using SimpleRotation = Object::SimpleRotationModel;
using IAURotation = Object::IAURotationModel;

/****************************************************************************************\
*                                     Rotation Models                                    *
\****************************************************************************************/

/**
 * @brief IAU Rotation Tracker, results will be returned as Polar(RA, Dec, Dist) format.
 * Simple model convertion set Solar System's north ecliptic pole by default.
 * 「是非成败转头空，青山依旧在，几度夕阳红。」
 */
typedef class __IAU_WGCCRE_Complex_Rotation_Model
{
public:
    using RotationData = IAURotation;

    RotationData _M_Data;  // Data
    float64      _M_Epoch; // Current time
    vec2         _M_NorthEclipticPole = vec2(270, -66.56070833333333333333333333333333); // J2000

protected:
    void _Check_Params();
    void _To_Simple(vec3 State);
    vec3 _Fix_Data(vec3 State) const;

public:
    __IAU_WGCCRE_Complex_Rotation_Model(RotationData const& _NewData);

    void AddMsecs(int64 Ms);
    void AddSeconds(int64 Sec);
    void AddHours(int64 Hrs);
    void AddDays(int64 Days);
    void AddYears(int64 Years);
    void AddCenturies(int64 Centuries);

    void ToCurrentDate();
    void SetDate(CSEDateTime DateTime);
    void SetDateJD(float64 JD);
    void Reset();

    vec3 State()const;
    SimpleRotation ToSimple()const; // TODO

    vec3 CurrentState()const;
    SimpleRotation CurrentStateToSimple()const; // TODO

    static __IAU_WGCCRE_Complex_Rotation_Model FromSimpleModel(SimpleRotation Simple); // TODO

    vec3 operator()(float64 AddTime)const;
}IAUComplexRotationTracker, WGCCREComplexRotationTracker;



/****************************************************************************************\
*                                    Static Functions                                    *
\****************************************************************************************/

// Nothing just now...

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
