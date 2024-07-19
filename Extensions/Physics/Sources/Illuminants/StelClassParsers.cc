/*
    Stellar Classification module
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

#include "CSE/Physics/Illuminants.h"

_CSE_BEGIN _OPTICS_BEGIN

StellarClassification StellarClassification::__Morgan_Keenan_Classification_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = MorganKeenan;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString = 0,
        MainClass  = 1,
        SpecClass  = 2,
        SubClass   = 3,
        FullLumCls = 4,
        MainLumCls = 5,
        SubLumCls  = 6,
        PecSymbols = 7,
        ElmSymbols = 9
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __Morgan_Keenan_Classification_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    StellarClassification::__Stellar_Class_Type Cls
    {
        .SpecClass = {StelClassFlags(0), StelClassFlags(0)},
        .SubClass = {0, 0},
    };
    BindSpecType(&Cls.SpecClass[0], Match[SpecClass].str().front());
    if (!Match[SubClass].str().empty())
    {
        Cls.SubClass[0] = std::stod(Match[SubClass]);
        Cls.SubClsState = __Stellar_Class_Type::Single;
    }
    BindLumType(&Cls.SpecClass[0], ustring(Match[MainLumCls].str()), ustring(Match[SubLumCls].str()));

    Cls.Pecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[PecSymbols]);
    Cls.ChemSymbols = ParseSymbols(__Element_Symbols_Regex, Match[ElmSymbols]);

    Classification.Elems[0] = Cls;
    return Classification;
}

_OPTICS_END _CSE_END
