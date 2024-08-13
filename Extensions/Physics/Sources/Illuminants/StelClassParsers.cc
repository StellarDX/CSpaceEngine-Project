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

#define __CREATE_EMPTY_SPEC_DATA(Data) StellarClassification::StellarClassificationDataType Data\
{\
    .SpecClass = {StelClassFlags(0), StelClassFlags(0)},\
    .SubClass = {0, 0}\
}

#define __SPEC_BIND_ENUM_DATA(Dst, Data) Dst = StelClassFlags(Dst | Data)
#define __SPEC_REGEX_MATCHED(RStr) !RStr.str().empty()
#define __SPEC_TO_SINGLE_CHAR(RStr) RStr.str().front()

StellarClassification StellarClassification::__Morgan_Keenan_Classification_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = MKGeneral;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        SpecClass,
        CarbonType,
        FullSubClass,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
        SubClsIsUncertain,
        SubClass2,
        SubCls2IsInt,
        SubCls2IsFract,
        IsSType,
        FullLumClass,
        LumClsType1,
        LumClass1,
        LumCls1Main,
        LumCls1Sub,
        LumClsMainIsUncertain,
        LumClass2,
        LumCls2Main,
        LumCls2Sub,
        LumClsType2,
        LumClass3,
        LumCls3Sub1,
        LumCls3Sub2,
        SpectralPecularities,
        SpecPecLastElem,
        AbsorptionPecularities,
        AbsorpPecLastElem,
        AbsorpPecCompound,
        AbsorpPecSign,
        AbsorpPecSub,
        AbsorpPecLastInt,
        AbsorpPecLastFract,
        AbsorpPecUncertainty,
        ChemicalPecularities,
        ChemPecLastElem,
        ChemPecLastSymbol,
        ChemPecHasUncertainties
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __MK_Class_General_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    // Bind Spectral type
    BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass].str()));
    Data.SpClsState = StellarClassificationDataType::Single;

    // Bind Carbon type
    if (__SPEC_REGEX_MATCHED(Match[CarbonType]))
    {
        switch (__SPEC_TO_SINGLE_CHAR(Match[CarbonType]))
        {
        case 'C':
            __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], C);
            break;
        case 'N':
            __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], N);
            break;
        default:
            break;
        }
    }

    // Bind Sub-type
    if (__SPEC_REGEX_MATCHED(Match[FullSubClass]))
    {
        Data.SubClass[0] = stod(Match[SubClass1]);
        Data.SubClsState = StellarClassificationDataType::Single;

        if (__SPEC_REGEX_MATCHED(Match[SubClsIsUncertain]))
        {
            if (__SPEC_REGEX_MATCHED(Match[SubClass2]))
            {
                Data.SubClass[1] = stod(Match[SubClass2]);
                Data.SubClsState = StellarClassificationDataType::Range;
            }
            else
            {
                BindUncertainty(&Data.SubClsState, ustring(Match[SubClsIsUncertain]));
            }
        }
    }

    // Bind S-type
    if (__SPEC_REGEX_MATCHED(Match[IsSType]))
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], S);
    }

    // Bind Luminosity type
    if (__SPEC_REGEX_MATCHED(Match[FullLumClass]))
    {
        auto CurrentSpType = Data.SpecClass[0];
        if (__SPEC_REGEX_MATCHED(Match[LumClsType1]))
        {
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClass1]),
                ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
            if (__SPEC_REGEX_MATCHED(Match[LumClsMainIsUncertain]))
            {
                if (__SPEC_REGEX_MATCHED(Match[LumClass2]))
                {
                    Data.SpecClass[1] = CurrentSpType;
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass2]),
                        ustring(Match[LumCls2Main]), ustring(Match[LumCls2Sub]));
                    Data.SpClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SpClsState, ustring(Match[LumClsMainIsUncertain]));
                }
            }
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClsType2]))
        {
            Data.SpecClass[1] = CurrentSpType;
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub1]));
            BindLumType(&Data.SpecClass[1], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub2]));
            Data.SpClsState = StellarClassificationDataType::Range;
        }
    }

    Classification.Data[0] = Data;

    Classification.SpectralPecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities]);
    Classification.AbsorptionPecularities = ParseAbsorptionPecularities(ustring(Match[AbsorptionPecularities]));
    Classification.ChemicalPecularities = ParseChemicallyPecularities(ustring(Match[ChemicalPecularities]));

    return Classification;
}

StellarClassification StellarClassification::__MK_Ranged_Classification_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = MKGeneral;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        SpecClass1,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
        SubCls1IsSType,
        SpecClass2,
        SubClass2,
        SubCls2IsInt,
        SubCls2IsFract,
        SubCls2IsSType,
        FullLumClass,
        LumClsType1,
        LumClass1,
        LumCls1Main,
        LumCls1Sub,
        LumClsMainIsUncertain,
        LumClass2,
        LumCls2Main,
        LumCls2Sub,
        LumClsType2,
        LumClass3,
        LumCls3Sub1,
        LumCls3Sub2,
        SpectralPecularities,
        SpecPecLastElem
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __MK_Class_Ranged_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    // Bind Spectral type
    BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass1].str()));
    BindSpecType(&Data.SpecClass[1], ustring(Match[SpecClass2].str()));
    Data.SpClsState = StellarClassificationDataType::Range;

    // Bind Sub-type
    if (__SPEC_REGEX_MATCHED(Match[SubClass1]))
    {
        Data.SubClass[0] = stod(Match[SubClass1]);
    }
    if (__SPEC_REGEX_MATCHED(Match[SubClass2]))
    {
        Data.SubClass[1] = stod(Match[SubClass2]);
    }
    Data.SubClsState = StellarClassificationDataType::Range;

    // Bind S-type
    if (__SPEC_REGEX_MATCHED(Match[SubCls1IsSType]))
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], S);
    }
    if (__SPEC_REGEX_MATCHED(Match[SubCls2IsSType]))
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[1], S);
    }

    // Bind Luminosity type
    if (__SPEC_REGEX_MATCHED(Match[FullLumClass]))
    {
        auto CurrentSpType = Data.SpecClass[0];
        if (__SPEC_REGEX_MATCHED(Match[LumClsType1]))
        {
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClass1]),
                ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
            if (__SPEC_REGEX_MATCHED(Match[LumClsMainIsUncertain]))
            {
                if (__SPEC_REGEX_MATCHED(Match[LumClass2]))
                {
                    Data.SpecClass[1] = CurrentSpType;
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass2]),
                        ustring(Match[LumCls2Main]), ustring(Match[LumCls2Sub]));
                    Data.SpClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SpClsState, ustring(Match[LumClsMainIsUncertain]));
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass1]),
                        ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
                }
            }
            else
            {
                BindLumType(&Data.SpecClass[1], ustring(Match[LumClass1]),
                    ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
            }
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClsType2]))
        {
            Data.SpecClass[1] = CurrentSpType;
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub1]));
            BindLumType(&Data.SpecClass[1], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub2]));
            Data.SpClsState = StellarClassificationDataType::Range;
        }
    }

    Classification.Data[0] = Data;

    Classification.SpectralPecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities]);

    return Classification;
}

_OPTICS_END _CSE_END
