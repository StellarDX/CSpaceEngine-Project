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

using namespace std;

_CSE_BEGIN _OPTICS_BEGIN

StellarClassification __Parse_Spec_String(ustring Str, vector<StellarClassification(*)(ustring)> FuncList)
{
    StellarClassification Classification;
    bool OK = false;
    for (auto ParseFunc : FuncList)
    {
        try{Classification = ParseFunc(Str);}
        catch (...) {continue;}
        OK = true;
        break;
    }
    if (!OK) {throw logic_error("Parse spectral type failed, is it invalid?");}
    return Classification;
}

StellarClassification StellarClassification::FromString(ustring Str, bool DisableMetalLineCheck)
{
    // Remove all spaces and '_'
    Str.erase(remove_if(Str.begin(), Str.end(), [](ucs2_t Ch){return isspace(Ch) || '_' == Ch;}), Str.end());

    // Detect MK string
    const ucs2_t MKList[11] = L"OBAFGKMLTY";
    const ustring MetalLineList[5] = {"k", "h", "m", "g", "He"};
    if (find(begin(MKList), end(MKList), Str[0]) != end(MKList))
    {
        return __Parse_Spec_String(Str,
        {
            __Morgan_Keenan_Classification_Parse,
            __MK_Ranged_Classification_Parse,
            __MK_Cyanogen_Classification_Parse,
            __MK_Classification_Bruteforce_Parse
        });
    }

    if ('(' == Str[0] && find(begin(MKList), end(MKList), Str[1]) != end(MKList))
    {
        return __Parse_Spec_String(Str,
        {
            __MK_Classification_Bruteforce_Parse
        });
    }

    for (int i = 0; i < 5; ++i)
    {
        if (Str.find(MetalLineList[i]) != Str.npos)
        {
            // TODO: Metallic pecular star parse
        }
    }

    if (Str.substr(0, 2) == L"sd" || Str.substr(1, 2) == L"sd")
    {
        return __Parse_Spec_String(Str,
        {
            __Subdwarf_Parse
        });
    }

    if (Str[0] == 'W')
    {
        if (Str[1] == 'D')
        {
            return __Parse_Spec_String(Str,
            {
                __White_Dwarf_Parse
            });
        }
        return __Parse_Spec_String(Str,
        {
            __Wolf_Rayet_Parse
        });
    }

    if (Str[0] == '[')
    {
        if (Str[0] == 'W')
        {
            return __Parse_Spec_String(Str,
            {
                __Wolf_Rayet_Parse
            });
        }
    }

    if (Str[0] == 'C')
    {
        return __Parse_Spec_String(Str,
        {
            __Carbon_Star_1D_Parse,
            __Carbon_Star_2D_Parse
        });
    }

    if (Str[0] == 'R' || Str[0] == 'S' || Str[0] == 'N')
    {
        return __Parse_Spec_String(Str,
        {
            __Carbon_Star_2D_Parse
        });
    }

    if (Str[0] == 'D')
    {
        return __Parse_Spec_String(Str,
        {
            __White_Dwarf_Parse
        });
    }

    if (Str == "Q" || Str == "Neutron" || Str == "Pulsar")
    {
        StellarClassification Classification;
        Classification.Data[0].SpecClass[0] = NS;
        Classification.Data[0].SpClsState = StellarClassificationDataType::Single;
        Classification.Data[0].SubClass[0] = 0;
        Classification.Data[0].SubClass[1] = 0;
        Classification.Data[0].SubClsState = StellarClassificationDataType::Null;
        return Classification;
    }

    if (Str == "X" || Str == "Blackhole")
    {
        StellarClassification Classification;
        Classification.Data[0].SpecClass[0] = BH;
        Classification.Data[0].SpClsState = StellarClassificationDataType::Single;
        Classification.Data[0].SubClass[0] = 0;
        Classification.Data[0].SubClass[1] = 0;
        Classification.Data[0].SubClsState = StellarClassificationDataType::Null;
        return Classification;
    }

    throw logic_error("Parse spectral type failed, is it invalid?");
}

_OPTICS_END _CSE_END
