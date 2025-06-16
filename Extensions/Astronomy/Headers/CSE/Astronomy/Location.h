/*
    Object Location data structures
    Copyleft (L) StellarDX Astronomy.

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

#ifndef __PLANSYSTEM__
#define __PLANSYSTEM__

#include <CSE/Base.h>
#include <CSE/Object.h>

#if __has_include(<CSE/Parser.h>)
#include <CSE/Parser.h>
#endif

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

struct Sexagesimal
{
    bool     Negative;
    uint16_t Degrees;
    uint16_t Minutes;
    float64  Seconds;

    Sexagesimal() {}
    Sexagesimal(bool Neg, uint16_t Deg, uint16_t Min, float64 Sec)
        : Negative(Neg), Degrees(Deg), Minutes(Min), Seconds(Sec) {}
    Sexagesimal(Angle Decimal);
    operator float64();
    operator Angle();
};

std::ostream& operator<<(std::ostream& os, const Sexagesimal& Angle);
std::wostream& operator<<(std::wostream& os, const Sexagesimal& Ang);

// 此处为方便计算，经度会由24h制转为360°制储存(会掉精度!!!)，然后输出时再转回24h制。

Sexagesimal __Create_Sexagesimal_From_Seconds(float64 TS);
Sexagesimal __Convert_24_to_360(bool n, float64 d, float64 m, float64 s);
Sexagesimal __Convert_360_to_24(Sexagesimal Ang);

/**
 * @brief 用极坐标表示的一个位置
 */
class Location : public SEObject
{
public:
    ustring                        Type = _NoDataStr;
    ustringlist                    Name;
    Sexagesimal                    RA;   // 经度(转成360°制)
    Sexagesimal                    Dec;  // 纬度
    float64                        Dist; // 距离(秒差距)
    std::map<char, float64>        AppMagn;
};


/**
 * @brief 行星系统结构体
 */
struct StellarSystem
{
    ObjectPtr_t PObject;
    std::vector<std::shared_ptr<StellarSystem>> PSubSystem;
};

namespace SystemBuilder
{
    using IndexTreeType   = std::multimap<uint64, uint64>;

    IndexTreeType __Sort_Indices(std::vector<Object> List, uint64* Barycenter);
    std::shared_ptr<StellarSystem> __BFS_BuildSystem(std::vector<Object> List, IndexTreeType Indices, uint64 Barycenter);
}

/**
 * @brief 以一组物体重建行星系统
 * @param List 物体数组
 * @return 系统根指针
 */
std::shared_ptr<StellarSystem> MakeSystem(std::vector<Object> List);

extern const struct __Flux_Type
{
    char    PhotometricLetter;   // 名称
    float64 EffectiveWavelength; // λeff
    float64 FWHM;                // Δλ
}__Photometric_Wavelengths_Table[9];

/**
 * @brief 恒星(系统)根结构体
 */
class StarBarycenter : public Location
{
public:
    _TIME CSEDate                  DateUpdated;
    ustring                        CenterOf       = _NoDataStr;
    bool                           ForceAppMagn   = 0;
    std::shared_ptr<StellarSystem> System;
};


/**
 * @brief 欧拉角
 */
struct EulerAnglesType
{
    float64 Yaw = _NoDataDbl;
    float64 Pitch = _NoDataDbl;
    float64 Roll = _NoDataDbl;
};

/**
 * @brief 深空天体基类
 */
class DSO : public Location
{
public:
    ustring                        Type           = _NoDataStr;
    EulerAnglesType                EulerAngles;
    vec4                           Axis           = vec4(_NoDataDbl);
    vec4                           Quat           = vec4(_NoDataDbl);
    float64                        AbsMagn        = _NoDataDbl;
    float64                        Radius         = _NoDataDbl;
    float64                        Luminosity     = _NoDataDbl;
    float64                        Age            = _NoDataDbl;
};

/**
 * @brief 星系
 */
class Galaxy : public DSO
{
public:
    float64                        ModelBright    = _NoDataDbl;
    bool                           SolFade        = 0;
};

/**
 * @brief 星团
 */
class Cluster : public DSO
{
public:
    ustring                        Galaxy         = _NoDataStr;
    ustring                        CenPow         = _NoDataStr;
    uint64                         NStars         = _NoDataInt;
    float64                        MaxStarAppMagn = _NoDataDbl;
    float64                        FeH            = _NoDataDbl;
    vec3                           Color          = vec3(_NoDataDbl);
};

/**
 * @brief 星云
 */
class Nebula : public DSO
{
public:
    ustring                        Galaxy         = _NoDataStr;
    vec3                           Randomize      = vec3(_NoDataDbl);
    bool                           GenerateStars  = 0;
};


#if __has_include(<CSE/Parser.h>)
#ifdef GetObject
#undef GetObject
#endif

Location GetLocationFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Location GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Location Loc, int Fl, std::streamsize Prec);

StarBarycenter GetStarBarycenterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> StarBarycenter GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(StarBarycenter Bar, int Fl, std::streamsize Prec);

DSO GetDSOFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> DSO GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(DSO Obj, int Fl, std::streamsize Prec);

Galaxy GetGalaxyFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Galaxy GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Galaxy Obj, int Fl, std::streamsize Prec);

Cluster GetClusterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Cluster GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Cluster Obj, int Fl, std::streamsize Prec);

Nebula GetNebulaFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Nebula GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Nebula Obj, int Fl, std::streamsize Prec);

#endif

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
