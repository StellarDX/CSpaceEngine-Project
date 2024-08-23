/*
    Planetary System data structure
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

#ifndef __PLANSYSTEM__
#define __PLANSYSTEM__

#include "CSE/Base.h"
#include "CSE/Object.h"
#include "CSE/Sexagesimal.h"

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

using ObjectPointer = std::shared_ptr<Object>;
using PlanetarySystemPointer = std::shared_ptr<class PlanetarySystem>;

/**
 * @brief Planetary system node type
 */
struct PlanetarySystem
{
    ObjectPointer PObject;
    std::vector<PlanetarySystemPointer> PSubSystem;
};

/**
 * @brief Planetary System builder
 */
namespace SystemBuilder
{
    using IndexTreeType   = std::multimap<uint64, uint64>;

    IndexTreeType __Sort_Indices(std::vector<Object> List, uint64* Barycenter);
    PlanetarySystemPointer __BFS_BuildSystem(std::vector<Object> List, IndexTreeType Indices, uint64 Barycenter);
}

PlanetarySystemPointer MakeSystem(std::vector<Object> List);

class StarBarycenter : public SEObject
{
public:
    ustring       Type        = _NoDataStr;
    ustringlist   Name;
    _TIME CSEDate DateUpdated;
    ustring       CenterOf    = _NoDataStr;
    Sexagesimal   RA;
    Sexagesimal   Dec;
    float64       AppMagn     = _NoDataDbl;
    float64       Parallax    = _NoDataDbl; // in mas

    PlanetarySystemPointer System;
};

// IOSCStream Compatibility
#ifdef _CSE_SCSTREAM
#ifdef GetObject
#undef GetObject
#endif

StarBarycenter GetStarBarycenterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> StarBarycenter GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(StarBarycenter Bar, int Fl, std::streamsize Prec);

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
