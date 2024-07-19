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
        SubClass1  = 4,
        SClsUncrt  = 5,
        SubClass2  = 6,
        MainLumCl1 = 9,
        SubLumCls1 = 10,
        LClsUncrt  = 11,
        MainLumCl2 = 13,
        SubLumCls2 = 14,
        PecSymbols = 15,
        ElmSymbols = 17
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
    if (!Match[SubClass1].str().empty())
    {
        Cls.SubClass[0] = std::stod(Match[SubClass1]);
        Cls.SubClsState = __Stellar_Class_Type::Single;
    }
    BindLumType(&Cls.SpecClass[0], ustring(Match[MainLumCl1].str()), ustring(Match[SubLumCls1].str()));

    if (!Match[SClsUncrt].str().empty())
    {
        if (!Match[SubClass2].str().empty())
        {
            Cls.SubClass[1] = std::stod(Match[SubClass2]);
            Cls.SubClsState = __Stellar_Class_Type::Range;
        }
        else
        {
            switch (Match[SClsUncrt].str().front())
            {
            case '+':
                Cls.SubClsState = __Stellar_Class_Type::Greater;
                break;
            case '-':
                Cls.SubClsState = __Stellar_Class_Type::Less;
                break;
            case ':':
                Cls.SubClsState = __Stellar_Class_Type::Uncertain;
                break;
            default:
                break;
            }
        }
    }

    if (!Match[LClsUncrt].str().empty())
    {
        if (!Match[MainLumCl2].str().empty())
        {
            BindSpecType(&Cls.SpecClass[1], Match[SpecClass].str().front());
            BindLumType(&Cls.SpecClass[1], ustring(Match[MainLumCl2].str()), ustring(Match[SubLumCls2].str()));
            Cls.SpClsState = __Stellar_Class_Type::Range;
        }
        else
        {
            switch (Match[LClsUncrt].str().front())
            {
            case '+':
                Cls.SpClsState = __Stellar_Class_Type::Greater;
                break;
            case '-':
                Cls.SpClsState = __Stellar_Class_Type::Less;
                break;
            case ':':
                Cls.SpClsState = __Stellar_Class_Type::Uncertain;
                break;
            default:
                break;
            }
        }
    }

    Cls.Pecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[PecSymbols]);
    Cls.ChemSymbols = ParseSymbols(__Element_Symbols_Regex, Match[ElmSymbols]);

    Classification.Elems[0] = Cls;
    return Classification;
}

_OPTICS_END _CSE_END
