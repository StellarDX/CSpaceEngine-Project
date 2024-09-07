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
#define __SPEC_REGEX_NOT_MATCHED(RStr) RStr.str().empty()
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
    Classification.LoadType = MKRanged;
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
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass2]),
                        ustring(Match[LumCls2Main]), ustring(Match[LumCls2Sub]));
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
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub1]));
            BindLumType(&Data.SpecClass[1], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub2]));
        }
    }

    Classification.Data[0] = Data;

    Classification.SpectralPecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities]);

    return Classification;
}

StellarClassification StellarClassification::__MK_Cyanogen_Classification_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = MKCyanogen;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        SpecClsFullStr,
        SingleSpecCls,
        SpecClass1,
        FullSubClass,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
        SubCls1IsUncertain,
        SubClass2,
        SubCls2IsInt,
        SubCls2IsFract,
        RangedSpecCls,
        SpecClass2,
        SubClass3,
        SubCls3IsInt,
        SubCls3IsFract,
        SpecClass3,
        SubClass4,
        SubCls4IsInt,
        SubCls4IsFract,
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
        CyanogenFullStr,
        CyanogenType1,
        CyanogenCls1Full,
        CyanogenClass1,
        CyanogenCls1Main,
        CyanogenCls1Sub,
        CyanogenClsIsUncertain,
        CyanogenClass2,
        CyanogenCls2Main,
        CyanogenCls2Sub,
        CyanogenType2,
        CyanogenClass3,
        CyanogenCls3Sub1,
        CyanogenCls3Sub2,
        SpectralPecularities,
        SpecPecLastElem
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __MK_Class_Cyanogen_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (__SPEC_REGEX_MATCHED(Match[SingleSpecCls]))
    {
        BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass1].str()));
        Data.SpClsState = StellarClassificationDataType::Single;

        if (__SPEC_REGEX_MATCHED(Match[FullSubClass]))
        {
            Data.SubClass[0] = stod(Match[SubClass1]);
            Data.SubClsState = StellarClassificationDataType::Single;

            if (__SPEC_REGEX_MATCHED(Match[SubCls1IsUncertain]))
            {
                if (__SPEC_REGEX_MATCHED(Match[SubClass2]))
                {
                    Data.SubClass[1] = stod(Match[SubClass2]);
                    Data.SubClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SubClsState, ustring(Match[SubCls1IsUncertain]));
                }
            }
        }
    }
    else if (__SPEC_REGEX_MATCHED(Match[RangedSpecCls]))
    {
        BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass2].str()));
        BindSpecType(&Data.SpecClass[1], ustring(Match[SpecClass3].str()));
        Data.SpClsState = StellarClassificationDataType::Range;

        if (__SPEC_REGEX_MATCHED(Match[SubClass3]))
        {
            Data.SubClass[0] = stod(Match[SubClass3]);
        }
        if (__SPEC_REGEX_MATCHED(Match[SubClass4]))
        {
            Data.SubClass[1] = stod(Match[SubClass4]);
        }
        Data.SubClsState = StellarClassificationDataType::Range;
    }

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
                    if (Data.SpClsState == StellarClassificationDataType::Single)
                    {
                        Data.SpecClass[1] = CurrentSpType;
                    }
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass2]),
                        ustring(Match[LumCls2Main]), ustring(Match[LumCls2Sub]));
                    Data.SpClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SpClsState, ustring(Match[LumClsMainIsUncertain]));
                    if (__SPEC_REGEX_MATCHED(Match[RangedSpecCls]))
                    {
                        BindLumType(&Data.SpecClass[1], ustring(Match[LumClass1]),
                            ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
                    }
                }
            }
            else
            {
                if (__SPEC_REGEX_MATCHED(Match[RangedSpecCls]))
                {
                    BindLumType(&Data.SpecClass[1], ustring(Match[LumClass1]),
                        ustring(Match[LumCls1Main]), ustring(Match[LumCls1Sub]));
                }
            }
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClsType2]))
        {
            if (Data.SpClsState == StellarClassificationDataType::Single)
            {
                Data.SpecClass[1] = CurrentSpType;
            }
            BindLumType(&Data.SpecClass[0], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub1]));
            BindLumType(&Data.SpecClass[1], ustring(Match[LumClsType2]),
                ustring(Match[LumClass3]), ustring(Match[LumCls3Sub2]));
            Data.SpClsState = StellarClassificationDataType::Range;
        }
    }

    Classification.Data[0] = Data;

    Classification.CyanogenType.Enable = 1;
    if (__SPEC_REGEX_MATCHED(Match[CyanogenType1]))
    {
        Classification.CyanogenType.Strength[0][0] = Match[CyanogenCls1Main];
        Classification.CyanogenType.Strength[0][1] = Match[CyanogenCls1Sub];
        if (__SPEC_REGEX_MATCHED(Match[CyanogenClsIsUncertain]))
        {
            if (__SPEC_REGEX_MATCHED(Match[CyanogenClass2]))
            {
                Classification.CyanogenType.Strength[1][0] = Match[CyanogenCls2Main];
                Classification.CyanogenType.Strength[1][1] = Match[CyanogenCls2Sub];
                Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Range;
            }
            else
            {
                switch (Match[CyanogenClsIsUncertain].str().front())
                {
                case '+':
                    Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Greater;
                    break;
                case '-':
                    Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Less;
                    break;
                case ':':
                case '?':
                    Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Uncertain;
                    break;
                default:
                    break;
                }
            }
        }
    }
    else if (__SPEC_REGEX_MATCHED(Match[CyanogenType2]))
    {
        Classification.CyanogenType.Strength[0][0] = Match[CyanogenClass3];
        Classification.CyanogenType.Strength[0][1] = Match[CyanogenCls3Sub1];
        Classification.CyanogenType.Strength[1][0] = Match[CyanogenClass3];
        Classification.CyanogenType.Strength[1][1] = Match[CyanogenCls3Sub2];
        Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Range;
    }

    Classification.SpectralPecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities]);

    return Classification;
}

template<typename Tp>
auto __Merge_Vector(std::vector<Tp>& Dst, std::vector<Tp> Src)
{
    Dst.insert(Dst.end(), Src.begin(), Src.end());
};

StellarClassification StellarClassification::__MK_Classification_Bruteforce_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = MKFull;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        SpecClassFullString,
        SpecClassType1,
        SpecClass1Full,
        SpecClass1,
        SpecClass1Uncertain1,
        SpecClass1Uncertain2FullString,
        SpecClass1Uncertain2,
        CarbonType,
        SubClassFullString,
        SubClassType1,
        SubClass1,
        SubClass1IsInt,
        SubClass1IsFract,
        SubClass1Uncertainty,
        SubClass2,
        SubClass2IsInt,
        SubClass2IsFract,
        SubClassType2,
        SubClass3,
        SubClass3IsInt,
        SubClass3IsFract,
        SubClass3Uncertainty,
        SubClass4,
        SubClass4IsInt,
        SubClass4IsFract,
        SType1,
        SpecClassType2,
        SpecClass2,
        SubClass5,
        SubClass5IsInt,
        SubClass5IsFract,
        SType2,
        SpecClass3,
        SubClass6,
        SubClass6IsInt,
        SubClass6IsFract,
        SType3,
        LumClassFullString,
        LumClassType1,
        LumClass1FullString,
        LumClass1,
        LumClass1Main,
        LumClass1Sub,
        LumClass1Uncertainty,
        LumClass2,
        LumClass2Main,
        LumClass2Sub,
        LumClassType2,
        LumClass3Main,
        LumClass3Sub1,
        LumClass3Sub2,
        LumClassType3,
        LumClass4FullString,
        LumClass4,
        LumClass4Main,
        LumClass4Sub,
        LumClass4Uncertainty,
        LumClass5,
        LumClass5Main,
        LumClass5Sub,
        LumClassType4,
        LumClass6Main,
        LumClass6Sub1,
        LumClass6Sub2,
        SpectralPecularities1,
        SpectralPecularities1LastItem,
        LumClassWithCyanogen1,
        LumClassCNType1,
        LumClass7FullString,
        LumClass7,
        LumClass7Main,
        LumClass7Sub,
        LumClass7Uncertainty,
        LumClass8,
        LumClass8Main,
        LumClass8Sub,
        LumClassCNType2,
        LumClass9Main,
        LumClass9Sub1,
        LumClass9Sub2,
        CyanogenType1,
        CyanogenClass1Full,
        CyanogenClass1,
        CyanogenClass1Main,
        CyanogenClass1Sub,
        CyanogenClass1IsUncertain,
        CyanogenClass2,
        CyanogenClass2Main,
        CyanogenClass2Sub,
        CyanogenType2,
        CyanogenClass3,
        CyanogenClass3Sub1,
        CyanogenClass3Sub2,
        LumClassWithCyanogen2,
        LumClassCNType3,
        LumClass10FullString,
        LumClass10,
        LumClass10Main,
        LumClass10Sub,
        LumClass10Uncertainty,
        LumClass11,
        LumClass11Main,
        LumClass11Sub,
        LumClassCNType4,
        LumClass12Main,
        LumClass12Sub1,
        LumClass12Sub2,
        CyanogenType3,
        CyanogenCls4Full,
        CyanogenClass4,
        CyanogenClass4Main,
        CyanogenClass4Sub,
        CyanogenClass4IsUncertain,
        CyanogenClass5,
        CyanogenClass5Main,
        CyanogenClass5Sub,
        CyanogenType4,
        CyanogenClass6,
        CyanogenClass6Sub1,
        CyanogenClass6Sub2,
        LumClassWithCyanogen3,
        LumClassCNType5,
        LumClass13FullString,
        LumClass13,
        LumClass13Main,
        LumClass13Sub,
        LumClass13Uncertainty,
        LumClass14,
        LumClass14Main,
        LumClass14Sub,
        LumClassCNType6,
        LumClass15Main,
        LumClass15Sub1,
        LumClass15Sub2,
        CyanogenType5,
        CyanogenCls7Full,
        CyanogenClass7,
        CyanogenClass7Main,
        CyanogenClass7Sub,
        CyanogenClass7IsUncertain,
        CyanogenClass8,
        CyanogenClass8Main,
        CyanogenClass8Sub,
        CyanogenType6,
        CyanogenClass9,
        CyanogenClass9Sub1,
        CyanogenClass9Sub2,
        PecularitiesFullString,
        SpectralPecularities2,
        SpectralPecularities2LastItem,
        AbsorptionPecularities,
        AbsorpPecularitiesLastElem,
        AbsorpPecularitiesCompound,
        AbsorpPecularitiesSign,
        AbsorpPecularitiesSub,
        AbsorpPecularitiesLastInt,
        AbsorpPecularitiesLastFract,
        AbsorpPecularitiesUncertainty,
        ChemicalPecularities1,
        ChemicalPecularities1LastElem,
        ChemicalPecularities1LastSymbol,
        ChemicalPecularities1Uncertain,
        SpectralPecularitiesUncertainFullString,
        SpectralPecularitiesUncertainty,
        SpectralPecularities3,
        SpectralPecularities3LastItem,
        ChemicalPecularities2,
        ChemicalPecularities2LastElem,
        ChemicalPecularities2LastSymbol,
        ChemicalPecularities2Uncertain,
        SpectralPecularities4,
        SpectralPecularities4LastItem,
        ChemicalPecularities3,
        ChemicalPecularities3LastElem,
        ChemicalPecularities3LastSymbol,
        ChemicalPecularities3Uncertain,
        AdditionalPecularity
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __MK_Class_Full_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (__SPEC_REGEX_MATCHED(Match[SpecClassType1]))
    {
        if (__SPEC_REGEX_MATCHED(Match[SpecClass1]))
        {
            BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass1].str()));
            Data.SpClsState = StellarClassificationDataType::Single;
        }
        else if (__SPEC_REGEX_MATCHED(Match[SpecClass1Uncertain1]))
        {
            BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass1Uncertain1].str()));
            Data.SpClsState = StellarClassificationDataType::Single;
        }
        else if (__SPEC_REGEX_MATCHED(Match[SpecClass1Uncertain2FullString]))
        {
            BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass1Uncertain1].str()));
            BindUncertainty(&Data.SpClsState, ustring(Match[SpecClass1Uncertain2FullString].str().substr(1, 1)));
        }

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

        if (__SPEC_REGEX_MATCHED(Match[SubClassFullString]))
        {
            uint64 SubClass1Position, SubClassUncertainty1Position, SubClass2Position;

            if (__SPEC_REGEX_MATCHED(Match[SubClassType1]))
            {
                SubClass1Position = SubClass1;
                SubClassUncertainty1Position = SubClass1Uncertainty;
                SubClass2Position = SubClass2;
            }
            else if (__SPEC_REGEX_MATCHED(Match[SubClassType2]))
            {
                SubClass1Position = SubClass3;
                SubClassUncertainty1Position = SubClass3Uncertainty;
                SubClass2Position = SubClass4;
            }

            Data.SubClass[0] = stod(Match[SubClass1Position]);
            Data.SubClsState = StellarClassificationDataType::Single;

            if (__SPEC_REGEX_MATCHED(Match[SubClassUncertainty1Position]))
            {
                if (__SPEC_REGEX_MATCHED(Match[SubClass2Position]))
                {
                    Data.SubClass[1] = stod(Match[SubClass2Position]);
                    Data.SubClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SubClsState, ustring(Match[SubClassUncertainty1Position]));
                }
            }
        }

        if (__SPEC_REGEX_MATCHED(Match[SType1]))
        {
            __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], S);
        }
    }
    else if (__SPEC_REGEX_MATCHED(Match[SpecClassType2]))
    {
        BindSpecType(&Data.SpecClass[0], ustring(Match[SpecClass2].str()));
        BindSpecType(&Data.SpecClass[1], ustring(Match[SpecClass3].str()));
        Data.SpClsState = StellarClassificationDataType::Range;

        if (__SPEC_REGEX_MATCHED(Match[SubClass5]))
        {
            Data.SubClass[0] = stod(Match[SubClass5]);
        }
        if (__SPEC_REGEX_MATCHED(Match[SubClass6]))
        {
            Data.SubClass[1] = stod(Match[SubClass6]);
        }
        Data.SubClsState = StellarClassificationDataType::Range;

        if (__SPEC_REGEX_MATCHED(Match[SType2]))
        {
            __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], S);
        }
        if (__SPEC_REGEX_MATCHED(Match[SType3]))
        {
            __SPEC_BIND_ENUM_DATA(Data.SpecClass[1], S);
        }
    }

    if (__SPEC_REGEX_MATCHED(Match[LumClassFullString]))
    {
        bool HasPecularType = 0, HasCyanogenType = 0;
        uint64 LumClsTy1, LCls1, LCls1Main, LCls1Sub,
            LCls1U, LCls2, LCls2Main, LCls2Sub,
            LClsTy2, LCls3, LCls3Sub1, LCls3Sub2,
            PecularTy,
            CNClsTy1, CNCls1, CNCls1Main, CNCls1Sub,
            CNCls1U, CNCls2, CNCls2Main, CNCls2Sub,
            CNClsTy2, CNCls3, CNCls3Sub1, CNCls3Sub2;

        if (__SPEC_REGEX_MATCHED(Match[LumClassType1]))
        {
            LumClsTy1 = LumClass1FullString;
            LCls1 = LumClass1;
            LCls1Main = LumClass1Main;
            LCls1Sub = LumClass1Sub;
            LCls1U = LumClass1Uncertainty;
            LCls2 = LumClass2;
            LCls2Main = LumClass2Main;
            LCls2Sub = LumClass2Sub;
            LClsTy2 = LumClassType2;
            LCls3 = LumClass3Main;
            LCls3Sub1 = LumClass3Sub1;
            LCls3Sub2 = LumClass3Sub2;
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClassType3]))
        {
            HasPecularType = 1;
            LumClsTy1 = LumClass4FullString;
            LCls1 = LumClass4;
            LCls1Main = LumClass4Main;
            LCls1Sub = LumClass4Sub;
            LCls1U = LumClass4Uncertainty;
            LCls2 = LumClass5;
            LCls2Main = LumClass5Main;
            LCls2Sub = LumClass5Sub;
            LClsTy2 = LumClassType4;
            LCls3 = LumClass6Main;
            LCls3Sub1 = LumClass6Sub1;
            LCls3Sub2 = LumClass6Sub2;
            PecularTy = SpectralPecularities1;
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClassCNType1]))
        {
            HasCyanogenType = 1;
            LumClsTy1 = LumClass7FullString;
            LCls1 = LumClass7;
            LCls1Main = LumClass7Main;
            LCls1Sub = LumClass7Sub;
            LCls1U = LumClass7Uncertainty;
            LCls2 = LumClass8;
            LCls2Main = LumClass8Main;
            LCls2Sub = LumClass8Sub;
            LClsTy2 = LumClassCNType2;
            LCls3 = LumClass9Main;
            LCls3Sub1 = LumClass9Sub1;
            LCls3Sub2 = LumClass9Sub2;
            CNClsTy1 = CyanogenType1;
            CNCls1 = CyanogenClass1;
            CNCls1Main = CyanogenClass1Main;
            CNCls1Sub = CyanogenClass1Sub;
            CNCls1U = CyanogenClass1IsUncertain;
            CNCls2 = CyanogenClass2;
            CNCls2Main = CyanogenClass2Main;
            CNCls2Sub = CyanogenClass2Sub;
            CNClsTy2 = CyanogenType2;
            CNCls3 = CyanogenClass3;
            CNCls3Sub1 = CyanogenClass3Sub1;
            CNCls3Sub2 = CyanogenClass3Sub2;
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClassCNType3]))
        {
            HasCyanogenType = 1;
            LumClsTy1 = LumClass10FullString;
            LCls1 = LumClass10;
            LCls1Main = LumClass10Main;
            LCls1Sub = LumClass10Sub;
            LCls1U = LumClass10Uncertainty;
            LCls2 = LumClass11;
            LCls2Main = LumClass11Main;
            LCls2Sub = LumClass11Sub;
            LClsTy2 = LumClassCNType4;
            LCls3 = LumClass12Main;
            LCls3Sub1 = LumClass12Sub1;
            LCls3Sub2 = LumClass12Sub2;
            CNClsTy1 = CyanogenType3;
            CNCls1 = CyanogenClass4;
            CNCls1Main = CyanogenClass4Main;
            CNCls1Sub = CyanogenClass4Sub;
            CNCls1U = CyanogenClass4IsUncertain;
            CNCls2 = CyanogenClass5;
            CNCls2Main = CyanogenClass5Main;
            CNCls2Sub = CyanogenClass5Sub;
            CNClsTy2 = CyanogenType4;
            CNCls3 = CyanogenClass6;
            CNCls3Sub1 = CyanogenClass6Sub1;
            CNCls3Sub2 = CyanogenClass6Sub2;
        }
        else if (__SPEC_REGEX_MATCHED(Match[LumClassCNType5]))
        {
            HasCyanogenType = 1;
            LumClsTy1 = LumClass13FullString;
            LCls1 = LumClass13;
            LCls1Main = LumClass13Main;
            LCls1Sub = LumClass13Sub;
            LCls1U = LumClass13Uncertainty;
            LCls2 = LumClass14;
            LCls2Main = LumClass14Main;
            LCls2Sub = LumClass14Sub;
            LClsTy2 = LumClassCNType6;
            LCls3 = LumClass15Main;
            LCls3Sub1 = LumClass15Sub1;
            LCls3Sub2 = LumClass15Sub2;
            CNClsTy1 = CyanogenType5;
            CNCls1 = CyanogenClass7;
            CNCls1Main = CyanogenClass7Main;
            CNCls1Sub = CyanogenClass7Sub;
            CNCls1U = CyanogenClass7IsUncertain;
            CNCls2 = CyanogenClass8;
            CNCls2Main = CyanogenClass8Main;
            CNCls2Sub = CyanogenClass8Sub;
            CNClsTy2 = CyanogenType6;
            CNCls3 = CyanogenClass9;
            CNCls3Sub1 = CyanogenClass9Sub1;
            CNCls3Sub2 = CyanogenClass9Sub2;
        }

        auto CurrentSpType = Data.SpecClass[0];
        if (__SPEC_REGEX_MATCHED(Match[LumClsTy1]))
        {
            BindLumType(&Data.SpecClass[0], ustring(Match[LCls1]),
                ustring(Match[LCls1Main]), ustring(Match[LCls1Main]));
            if (__SPEC_REGEX_MATCHED(Match[LCls1U]))
            {
                if (__SPEC_REGEX_MATCHED(Match[LCls2]))
                {
                    if (Data.SpClsState == StellarClassificationDataType::Single)
                    {
                        Data.SpecClass[1] = CurrentSpType;
                    }
                    BindLumType(&Data.SpecClass[1], ustring(Match[LCls2]),
                        ustring(Match[LCls2Main]), ustring(Match[LCls2Sub]));
                    Data.SpClsState = StellarClassificationDataType::Range;
                }
                else
                {
                    BindUncertainty(&Data.SpClsState, ustring(Match[LCls1U]));
                    if (__SPEC_REGEX_MATCHED(Match[LClsTy2]))
                    {
                        BindLumType(&Data.SpecClass[1], ustring(Match[LCls1]),
                            ustring(Match[LCls1Main]), ustring(Match[LCls1Sub]));
                    }
                }
            }
            else
            {
                if (__SPEC_REGEX_MATCHED(Match[LClsTy2]))
                {
                    BindLumType(&Data.SpecClass[1], ustring(Match[LCls1]),
                        ustring(Match[LCls1Main]), ustring(Match[LCls1Sub]));
                }
            }
        }
        else if (__SPEC_REGEX_MATCHED(Match[LClsTy2]))
        {
            if (Data.SpClsState == StellarClassificationDataType::Single)
            {
                Data.SpecClass[1] = CurrentSpType;
            }
            BindLumType(&Data.SpecClass[0], ustring(Match[LClsTy2]),
                ustring(Match[LCls3]), ustring(Match[LCls3Sub1]));
            BindLumType(&Data.SpecClass[1], ustring(Match[LClsTy2]),
                ustring(Match[LCls3]), ustring(Match[LCls3Sub2]));
            Data.SpClsState = StellarClassificationDataType::Range;
        }

        if (HasPecularType)
        {
            __Merge_Vector(Classification.SpectralPecularities, ParseSymbols(__Spectral_Pecularities_Regex, Match[PecularTy]));
        }

        if (HasCyanogenType)
        {
            Classification.CyanogenType.Enable = 1;
            if (__SPEC_REGEX_MATCHED(Match[CNCls1]))
            {
                Classification.CyanogenType.Strength[0][0] = Match[CNCls1Main];
                Classification.CyanogenType.Strength[0][1] = Match[CNCls1Sub];
                if (__SPEC_REGEX_MATCHED(Match[CNCls1U]))
                {
                    if (__SPEC_REGEX_MATCHED(Match[CNCls2]))
                    {
                        Classification.CyanogenType.Strength[1][0] = Match[CNCls2Main];
                        Classification.CyanogenType.Strength[1][1] = Match[CNCls2Sub];
                        Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Range;
                    }
                    else
                    {
                        switch (Match[CNCls1U].str().front())
                        {
                        case '+':
                            Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Greater;
                            break;
                        case '-':
                            Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Less;
                            break;
                        case ':':
                        case '?':
                            Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Uncertain;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            else if (__SPEC_REGEX_MATCHED(Match[CNClsTy2]))
            {
                Classification.CyanogenType.Strength[0][0] = Match[CNCls3];
                Classification.CyanogenType.Strength[0][1] = Match[CNCls3Sub1];
                Classification.CyanogenType.Strength[1][0] = Match[CNCls3];
                Classification.CyanogenType.Strength[1][1] = Match[CNCls3Sub2];
                Classification.CyanogenType.Uncertainty = Classification.CyanogenType.Range;
            }
        }
    }

    Classification.Data[0] = Data;

    __Merge_Vector(Classification.SpectralPecularities,
        ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities2]));
    __Merge_Vector(Classification.AbsorptionPecularities,
        ParseAbsorptionPecularities(ustring(Match[AbsorptionPecularities])));
    __Merge_Vector(Classification.ChemicalPecularities,
        ParseChemicallyPecularities(ustring(Match[ChemicalPecularities1])));

    __Merge_Vector(Classification.SpectralPecularities,
        ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities3]));
    __Merge_Vector(Classification.ChemicalPecularities,
        ParseChemicallyPecularities(ustring(Match[ChemicalPecularities2])));

    __Merge_Vector(Classification.SpectralPecularities,
        ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities4]));
    __Merge_Vector(Classification.ChemicalPecularities,
        ParseChemicallyPecularities(ustring(Match[ChemicalPecularities3])));

    if (__SPEC_REGEX_MATCHED(Match[AdditionalPecularity]))
    {
        Classification.SpectralPecularities.push_back(ustring(Match[AdditionalPecularity]));
    }

    return Classification;
}

StellarClassification StellarClassification::__Metallic_Line_Parser(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = Metallic;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString = 0,
        MainType   = 1,
        LineTypes  = 48,
        FinalLine  = 90
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __Metallic_Line_Stars_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    return Classification;
}

StellarClassification StellarClassification::__Subdwarf_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = Subdwarf;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        SubdwarfPrefix,
        SubdwarfUncertain,
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
    if (!_REGEX_NS regex_match(str, Match, __Subdwarfs_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (L"sd" == Match[SubdwarfPrefix].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], sd);
    }
    else if (L"esd" == Match[SubdwarfPrefix].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], esd);
    }
    else if (L"usd" == Match[SubdwarfPrefix].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], usd);
    }

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

StellarClassification StellarClassification::__Wolf_Rayet_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = WolfRayet;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        WRClass,
        FullSubClass,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
        SubClsIsUncertain,
        SubClass2,
        SubCls2IsInt,
        SubCls2IsFract,
        SpectralPecularities,
        SpecPecLastElem,
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __Wolf_Rayet_Star_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (L"R" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WR);
    }
    else if (L"N" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WN);
    }
    else if (L"C" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WC);
    }
    else if (L"NC" == Match[WRClass].str() || L"N/C" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WNC);
    }
    else if (L"O" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WO);
    }
    Data.SpClsState = StellarClassificationDataType::Range;

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

    Classification.Data[0] = Data;

    Classification.SpectralPecularities = ParseSymbols(__WR_Spectral_Pecularities_Regex, Match[SpectralPecularities]);

    return Classification;
}

StellarClassification StellarClassification::__WR_Class_Uncertain_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = WolfRayet;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        WRClass,
        FullSubClass,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
        SubClsIsUncertain,
        SubClass2,
        SubCls2IsInt,
        SubCls2IsFract
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __Wolf_Rayet_Star_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (L"R" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WR);
    }
    else if (L"N" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WN);
    }
    else if (L"C" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WC);
    }
    else if (L"NC" == Match[WRClass].str() || L"N/C" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WNC);
    }
    else if (L"O" == Match[WRClass].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WO);
    }
    Data.SpClsState = StellarClassificationDataType::Range;

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

    Classification.Data[0] = Data;

    return Classification;
}

StellarClassification StellarClassification::__Carbon_Star_1D_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = Carbon1D;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        CTypeFullString,
        CarbonType,
        CTypeUncertain,
        FullSubClass,
        SubClass1,
        SubCls1IsInt,
        SubCls1IsFract,
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
    if (!_REGEX_NS regex_match(str, Match, __Carbon_Star_1D_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (L"R" == Match[CarbonType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], C);
    }
    else if (L"N" == Match[CarbonType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], N);
    }
    else if (L"J" == Match[CarbonType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], CJ);
    }
    else if (L"H" == Match[CarbonType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], CH);
    }
    else if (L"Hd" == Match[CarbonType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], CHd);
    }
    Data.SpClsState = StellarClassificationDataType::Range;

    if (__SPEC_REGEX_MATCHED(Match[FullSubClass]))
    {
        Data.SubClass[0] = stod(Match[SubClass1]);
        Data.SubClsState = StellarClassificationDataType::Single;
    }

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

    Classification.SpectralPecularities = ParseSymbols(__WR_Spectral_Pecularities_Regex, Match[SpectralPecularities]);

    return Classification;
}

StellarClassification StellarClassification::__Carbon_Star_2D_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = Carbon2D;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        MainType,
        FullSubType,
        SingleSubClass1,
        SingleSubCls1IsInt,
        SingleSubCls1IsFract,
        BinarySubClass,
        BinarySubCls1FullStr,
        BinarySubClass1,
        BinarySubCls1IsInt,
        BinarySubCls1IsFract,
        BinarySubCls1Uncertain,
        BinarySubCls2FullStr,
        BinarySubClass2,
        BinarySubCls2IsInt,
        BinarySubCls2IsFract,
        BinarySubCls2Uncertain,
        QuadrupleSubClass,
        QuadrupleSubClassP1,
        QuadrupleSubClass1,
        QuadrupleSubCls1IsInt,
        QuadrupleSubCls1IsFract,
        QuadrupleSubCls1Uncertain,
        QuadrupleSubClass2,
        QuadrupleSubCls2IsInt,
        QuadrupleSubCls2IsFract,
        QuadrupleSubClassP2Full,
        QuadrupleSubClassP2,
        QuadrupleSubClass3,
        QuadrupleSubCls3IsInt,
        QuadrupleSubCls3IsFract,
        QuadrupleSubCls3Uncertain,
        QuadrupleSubClass4,
        QuadrupleSubCls4IsInt,
        QuadrupleSubCls4IsFract,
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
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __Supplementary_Class_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data1);
    __CREATE_EMPTY_SPEC_DATA(Data2);

    if (L"C" == Match[MainType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data1.SpecClass[0], C);
    }
    else if (L"SC" == Match[MainType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data1.SpecClass[0], SC);
    }
    else if (L"S" == Match[MainType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data1.SpecClass[0], S);
    }
    else if (L"R" == Match[MainType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data1.SpecClass[0], C);
    }
    else if (L"N" == Match[MainType].str())
    {
        __SPEC_BIND_ENUM_DATA(Data1.SpecClass[0], N);
    }
    Data1.SpClsState = StellarClassificationDataType::Range;

    if (__SPEC_REGEX_MATCHED(Match[SingleSubClass1]))
    {
        Data1.SubClass[0] = stod(Match[SingleSubClass1]);
        Data1.SubClsState = StellarClassificationDataType::Single;
    }
    else if (__SPEC_REGEX_MATCHED(Match[BinarySubClass]))
    {
        Data1.SubClass[0] = stod(Match[BinarySubClass1]);
        Data1.SubClsState = StellarClassificationDataType::Single;

        Data1.SubClass[1] = stod(Match[BinarySubClass2]);
        Data1.SubClsState = StellarClassificationDataType::Range;
    }
    else if (__SPEC_REGEX_MATCHED(Match[QuadrupleSubClass]))
    {
        Data2 = Data1;
        Data1.SubClass[0] = stod(Match[QuadrupleSubClass1]);
        Data1.SubClsState = StellarClassificationDataType::Single;

        if (__SPEC_REGEX_MATCHED(Match[QuadrupleSubCls1Uncertain]))
        {
            Data1.SubClass[1] = stod(Match[QuadrupleSubClass2]);
            Data1.SubClsState = StellarClassificationDataType::Range;
        }

        Data2.SubClass[0] = stod(Match[QuadrupleSubClass3]);
        Data2.SubClsState = StellarClassificationDataType::Single;

        if (__SPEC_REGEX_MATCHED(Match[QuadrupleSubCls3Uncertain]))
        {
            Data2.SubClass[1] = stod(Match[QuadrupleSubClass4]);
            Data2.SubClsState = StellarClassificationDataType::Range;
        }
    }

    Classification.Data[0] = Data1;
    if (__SPEC_REGEX_MATCHED(Match[QuadrupleSubClass]))
    {
        Classification.Data[1] = Data2;
    }

    Classification.SpectralPecularities = ParseSymbols(__Spectral_Pecularities_Regex, Match[SpectralPecularities]);
    Classification.AbsorptionPecularities = ParseAbsorptionPecularities(ustring(Match[AbsorptionPecularities]));

    return Classification;
}

StellarClassification StellarClassification::__White_Dwarf_Parse(ustring str)
{
    StellarClassification Classification;
    Classification.LoadType = WhiteDwarf;
    Classification.SrcString = str;

    enum RegPosition
    {
        FullString,
        WDOnly,
        WDDetails,
        WDTypes,
        WDTypeLastItem,
        WDSubClass,
        WDSubClsIsInt,
        WDSubClsIsFract
    };

    _REGEX_NS wsmatch Match;
    if (!_REGEX_NS regex_match(str, Match, __White_Dwarf_Regex))
    {
        throw std::logic_error("Not the correct pattern.");
    }

    __CREATE_EMPTY_SPEC_DATA(Data);

    if (__SPEC_REGEX_MATCHED(Match[WDOnly]))
    {
        __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WD);
    }
    else
    {
        for (auto i : Match[WDTypes].str())
        {
            switch (i)
            {
            case 'A':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDA);
                break;
            case 'B':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDB);
                break;
            case 'O':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDO);
                break;
            case 'Q':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDQ);
                break;
            case 'Z':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDZ);
                break;
            case 'C':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDC);
                break;
            case 'X':
                __SPEC_BIND_ENUM_DATA(Data.SpecClass[0], WDX);
                break;
            default:
                break;
            }
        }
        Data.SpClsState = StellarClassificationDataType::Single;

        if (__SPEC_REGEX_MATCHED(Match[WDSubClass]))
        {
            Data.SubClass[0] = stod(Match[WDSubClass]);
            Data.SubClsState = StellarClassificationDataType::Single;
        }
    }

    Classification.Data[0] = Data;
    return Classification;
}

_OPTICS_END _CSE_END
