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

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN _OPTICS_BEGIN

// ------------------------- Tables ------------------------- //

const map<StellarClassification::__Spectral_Classification_Types, ustring>
StellarClassification::__Spectral_Classification_Table
{
    {ms, "V"}, {O, "O"}, {B, "B"}, {A, "A"}, {F, "F"}, {G, "G"}, {K, "K"}, {M, "M"},
    {L, "L"}, {T, "T"}, {Y, "Y"},
    {sd, "sd"}, {esd, "esd"}, {usd, "usd"},
    {subg, "IV"}, {g, "III"}, {bg, "II"}, {sg, "I"}, {hg, "Ia+"}, {a, "a"}, {ab, "ab"}, {b, "b"},
    {WR, "W"}, {WN, "N"}, {WC, "C"}, {WO, "O"},
    {C, "C-"}, {N, "N"}, {CJ, "J"}, {CH, "H"}, {CHd, "Hd"}, {S, "S"}, {SC, "SC"}, {MS, "MS"},
    {WD, "D"}, {WDA, "A"}, {WDB, "B"}, {WDO, "O"}, {WDQ, "Q"}, {WDZ, "Z"}, {WDC, "C"}, {WDX, "X"},
    {NS, "Neutron"}, {BH, "Blackhole"}, {PEC, "Pecular"}
};

const ustringlist StellarClassification::__Spectral_Pecularities
{
    ":", "...", "!", "comp", "e", "[e]", "er", "eq", "f", "f*",
    "f+", "f?", "(f)", "(f+)", "((f))", "((f*))", //"h", "ha", "E", "L",
    "Hewk", "k", "m", "n", "nn", "neb", "p", "pq", "q", "s",
    "ss", "sh", "var", "wl", "z",

    "w", "?", "lB", "v", "lambdaBoo", "HIwk", "(n)", "shell", "[n]",
    "pec", "He-n", "Si4200", "e*", "mdD", "kmet", "Ssh", "(m)", "Hd", "CaIIem",
    "CaIwk", "c", "[Swk]", "[S]", "d", "CaII", "metalweak"
};

const ustringlist StellarClassification::__WR_Spectral_Pecularities
{
    "h", "(h)", "ha", "E", "L", "d", "p", "-w", "-s", "b"
};

const ustringlist StellarClassification::__C_Spectral_Pecularities
{
    "p", "CH", "J", "e", ":", "m", "p", "pec", "-no", "-s",
    "V", "..."
};

const ustringlist StellarClassification::__Absorption_Pecularities // For pecular reg/yellow giants such as barium star
{
    "Fe", "m", "CH", "Hdel", "Ba", "Ca", "CN", "HK", "Sr", "C2", "He", "MS"
};

const map<StellarClassification::__Load_Type, StellarClassification::__Export_Function_Type>
StellarClassification::__Export_Methods
{

};


// ---------------------- Regex Strings --------------------- //

const string StellarClassification::__Spectal_Class_RegexStr = R"((O|B|A|F|G|K|M|L|T|Y))";
const string StellarClassification::__Sub_Class_RegexStr = R"((([0-9])|([0-9]\.[0-9])))";
const string StellarClassification::__Sub_Class_Dbl_RegexStr = R"((([0-9]?[0-9])|([0-9]?[0-9]\.[0-9][0-9]?)))";
const string StellarClassification::__Main_Lum_Class_RegexStr = R"((V|IV|III|II|I))";
const string StellarClassification::__Sub_Lum_Class_RegexStr = R"((a|ab|b))";
const string StellarClassification::__Luminosity_Class_RegexStr = vformat(R"((VIII|VII|VI|{0}{1}?|Ia\+|Ia0|0))",
    make_format_args(__Main_Lum_Class_RegexStr, __Sub_Lum_Class_RegexStr));
const string StellarClassification::__Lum_Class_SubUncertain_RegexStr = vformat(R"(({0}{1}[\-/]{1}))",
    make_format_args(__Main_Lum_Class_RegexStr, __Sub_Lum_Class_RegexStr));
const string StellarClassification::__Uncertainty_Template_RegexStr = R"(({0}([\+\-\:\?]|[/\-]{0})?))";
const string StellarClassification::__Sub_Class_Full_RegexStr = vformat(__Uncertainty_Template_RegexStr,
    make_format_args(__Sub_Class_Dbl_RegexStr));
const string StellarClassification::__LCls_Uncertainty = vformat(__Uncertainty_Template_RegexStr,
    make_format_args(__Luminosity_Class_RegexStr));
const string StellarClassification::__Luminosity_Class_Full_RegexStr = vformat(R"(({0}|{1}))",
    make_format_args(__LCls_Uncertainty, __Lum_Class_SubUncertain_RegexStr));
const string StellarClassification::__WR_Class_RegexStr = R"((R|N|C|NC|N/C|O))";
const string StellarClassification::__Carbon_Star_Class_RegexStr = R"((R|N|H|Hd|J))";
const string StellarClassification::__White_Dwarf_Class_RegexStr = R"((A|B|O|Q|Z|C|X))";
const string StellarClassification::__Metallic_Line_RegexStr = R"((k|h|m|g|He))";
const string StellarClassification::__Metallic_Line_Single_RegexStr = []()->string
{
    std::string SingleStr = vformat(R"(({0}{1}?({2}|\({2}\))?))",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr));
    return vformat(R"((({0}{1})+)?)", make_format_args(__Metallic_Line_RegexStr, SingleStr));
}();

const string StellarClassification::__Spectral_Pecularities_RegexStr
    = GenerateListMatchRegexString(__Spectral_Pecularities, "");
const string StellarClassification::__WR_Spectral_Pecularities_RegexStr
    = GenerateListMatchRegexString(__WR_Spectral_Pecularities, "");
const string StellarClassification::__C_Spectral_Pecularities_RegexStr
    = GenerateListMatchRegexString(__C_Spectral_Pecularities, "");
const string StellarClassification::__Element_Symbols_RegexStr =
    R"((((A[cglmrstu]|B[aehikr]|C[adeflmnorsu]?|D[bsy]|E[rsu]|F[elmr]|G[ade]|H[efgos]?|I[nr]|Kr|L[airuv]|M[cdgnot]|N[abdehiop]?|O[gs]|P[abdmortu]?|R[abefghnu]|S[bcegimnr]?|T[abcehilms]|U|W|Xe|Yb?|Z[nr])([\:\?])?)+))";
const string StellarClassification::__Absorption_Pecularities_RegexStr
    = GenerateListMatchRegexString(__Absorption_Pecularities, R"(([\+\-])?(([0-9]?[0-9])|([0-9]\.[0-9][0-9]?))([\+\-:\?])?)");

const wstring StellarClassification::__MK_Class_General_RegexStr = []()->wstring
{
    string PecStr = vformat(R"({0}?{2}?{1}?)", make_format_args(__Spectral_Pecularities_RegexStr,
        __Element_Symbols_RegexStr, __Absorption_Pecularities_RegexStr));
    ustring FinalRegex = ustring(vformat(R"(^{0}(C|N)?{1}?(S)?{2}?{3}$)",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr, PecStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__MK_Class_Ranged_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^{0}{1}?(S)?[\-/]{0}{1}?(S)?{2}?{3}?$)",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Dbl_RegexStr,
        __Luminosity_Class_Full_RegexStr, __Spectral_Pecularities_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__MK_Class_Cyanogen_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^(({0}{1}?)|({0}{4}?[\-/]{0}{4}?)){2}(CN{2}){3}?$)",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr,
        __Luminosity_Class_Full_RegexStr, __Spectral_Pecularities_RegexStr, __Sub_Class_Dbl_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__MK_Class_Full_RegexStr = []()->wstring
{
    string SpTyStr = vformat(R"(({0}|\({0}\)|({0}[\+\-\:\?])))", make_format_args(__Spectal_Class_RegexStr));
    string SubClsStr = vformat(R"(({0}|\({0}\)))", make_format_args(__Sub_Class_Full_RegexStr));
    string LClsStr = vformat(R"(({0}|\({0}{1}?\)|({0}CN{0})|(\({0}CN{0}\))|({0}\(CN{0}\))))", make_format_args(__Luminosity_Class_Full_RegexStr,
        __Spectral_Pecularities_RegexStr));
    string PecStr = vformat(R"(({0}?{2}?{1}?(\(({0}|{1}|{0}{1})\))?))", make_format_args(__Spectral_Pecularities_RegexStr,
        __Element_Symbols_RegexStr, __Absorption_Pecularities_RegexStr));
    ustring FinalRegex = ustring(vformat(R"(^(({0}(C|N)?{1}?(S)?)|({4}{5}?(S)?[\-/]{4}{5}?(S)?)){2}?{3}?(Ksn|pec)?$)",
        make_format_args(SpTyStr, SubClsStr, LClsStr, PecStr,
        __Spectal_Class_RegexStr, __Sub_Class_Dbl_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__Metallic_Line_Stars_RegexStr = []()->wstring
{
    std::string FirstTerm = vformat(R"(({0}({1}|\({1}\))?{2}?{3}?{4}?{5}?))",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr,
        __Spectral_Pecularities_RegexStr, __Absorption_Pecularities_RegexStr, __Element_Symbols_RegexStr));
    std::string SingleStr = vformat(R"(({0}{1}?({2}|\({2}\))?))",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr));
    std::string RangedStr = vformat(R"(({0}({1}|\({1}\))?[\-/]{0}({1}|\({1}\))?{2}?))",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr));
    std::string TripleStr = vformat(R"(({0}{1}?\-{0}{1}?\/{0}{1}?))",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr));
    ustring FinalRegex = ustring(vformat(R"(^({0})?{8}({1}({2}|{3}|{4}))({5}|\({5}\))?{6}?({7}|\({7}\))?$)",
        make_format_args(FirstTerm, __Metallic_Line_RegexStr, SingleStr, RangedStr, TripleStr,
        __Spectral_Pecularities_RegexStr, __Absorption_Pecularities_RegexStr,
        __Element_Symbols_RegexStr, __Metallic_Line_Single_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__Subdwarfs_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^([eu]?sd)([\:\?])?{0}(C|N)?{1}?{2}?{3}?{4}?{5}?$)",
        make_format_args(__Spectal_Class_RegexStr, __Sub_Class_Full_RegexStr, __Luminosity_Class_Full_RegexStr,
        __Spectral_Pecularities_RegexStr, __Absorption_Pecularities_RegexStr, __Element_Symbols_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__Wolf_Rayet_Star_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^W{0}{1}?{2}?$)",
        make_format_args(__WR_Class_RegexStr, __Sub_Class_Full_RegexStr, __WR_Spectral_Pecularities_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__WR_Class_Uncertain_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^\[W{0}{1}?\]$)",
        make_format_args(__WR_Class_RegexStr, __Sub_Class_Full_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__Carbon_Star_1D_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^C(-{0})(:)?({1}[\+\-:]?)?{2}?{3}?$)",
        make_format_args(__Carbon_Star_Class_RegexStr, __Sub_Class_RegexStr,
        __Luminosity_Class_Full_RegexStr, __Spectral_Pecularities_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__Supplementary_Class_RegexStr = []()->wstring
{
    string SubCls1 = vformat(R"((({0})([\+\-:])?[\,\*]({0})([\+\-:]?)))", make_format_args(__Sub_Class_RegexStr));
    string SubCls2 = vformat(R"(({0}([\+\-:]|\-{0})?))", make_format_args(__Sub_Class_RegexStr));
    string SubCls3 = vformat(R"(({0}(/{0})?))", make_format_args(SubCls2));
    ustring FinalRegex = ustring(vformat(R"(^(C|SC|S|R|N)({0}|{1}|{2})?{3}?{4}?$)",
        make_format_args(__Sub_Class_RegexStr, SubCls1, SubCls3,
        __C_Spectral_Pecularities_RegexStr, __Absorption_Pecularities_RegexStr)));
    return FinalRegex.ToStdWString();
}();

const wstring StellarClassification::__White_Dwarf_RegexStr = []()->wstring
{
    ustring FinalRegex = ustring(vformat(R"(^(WD)$|^(D({0}+){1}?)$)",
        make_format_args(__White_Dwarf_Class_RegexStr, __Sub_Class_RegexStr)));
    return FinalRegex.ToStdWString();
}();


// ------------------------- Regexes ------------------------ //

const _REGEX_NS wregex StellarClassification::__Spectral_Pecularities_Regex(ustring(__Spectral_Pecularities_RegexStr).ToStdWString());
const _REGEX_NS wregex StellarClassification::__WR_Spectral_Pecularities_Regex(ustring(__WR_Spectral_Pecularities_RegexStr).ToStdWString());
const _REGEX_NS wregex StellarClassification::__C_Spectral_Pecularities_Regex(ustring(__C_Spectral_Pecularities_RegexStr).ToStdWString());
const _REGEX_NS wregex StellarClassification::__Absorption_Pecularities_Regex(ustring(__Absorption_Pecularities_RegexStr).ToStdWString());
const _REGEX_NS wregex StellarClassification::__Element_Symbols_Regex(ustring(__Element_Symbols_RegexStr).ToStdWString());

const _REGEX_NS wregex StellarClassification::__MK_Class_General_Regex(__MK_Class_General_RegexStr);
const _REGEX_NS wregex StellarClassification::__MK_Class_Ranged_Regex(__MK_Class_Ranged_RegexStr);
const _REGEX_NS wregex StellarClassification::__MK_Class_Cyanogen_Regex(__MK_Class_Cyanogen_RegexStr);
const _REGEX_NS wregex StellarClassification::__MK_Class_Full_Regex(__MK_Class_Full_RegexStr);
const _REGEX_NS wregex StellarClassification::__Metallic_Line_Stars_Regex(__Metallic_Line_Stars_RegexStr);
const _REGEX_NS wregex StellarClassification::__Subdwarfs_Regex(__Subdwarfs_RegexStr);
const _REGEX_NS wregex StellarClassification::__Wolf_Rayet_Star_Regex(__Wolf_Rayet_Star_RegexStr);
const _REGEX_NS wregex StellarClassification::__WR_Class_Uncertain_Regex(__WR_Class_Uncertain_RegexStr);
const _REGEX_NS wregex StellarClassification::__Carbon_Star_1D_Regex(__Carbon_Star_1D_RegexStr);
const _REGEX_NS wregex StellarClassification::__Supplementary_Class_Regex(__Supplementary_Class_RegexStr);
const _REGEX_NS wregex StellarClassification::__White_Dwarf_Regex(__White_Dwarf_RegexStr);


// --------------------- Regex Functions -------------------- //

const string StellarClassification::GenerateListMatchRegexString(ustringlist Li, ustring Suffix)
{
    static const ucs2_t POSIXMetaCharacters[17] =
    {
        '^', '.', '[', ']', '$', '(', ')', '*', ',', '{', '}', '?', '+', '|', '-', '\\', ':'
    };

    ustringlist __List_Regex_Raw;
    for (auto i : Li)
    {
        ustring ustr;
        auto first = begin(i);
        auto last = end(i);
        while (first != last)
        {
            if (first != last &&
                find(begin(POSIXMetaCharacters), end(POSIXMetaCharacters), *first)
                != end(POSIXMetaCharacters))
            {
                ustr.push_back('\\');
            }
            ustr.push_back(*first);
            ++first;
        }
        __List_Regex_Raw.push_back(ustr);
    }

    ustring FinalStr;
    FinalStr.append(L"((");
    if (!Suffix.empty()) {FinalStr.push_back(L'(');}
    FinalStr += __List_Regex_Raw[0];
    for (int i = 1; i < __List_Regex_Raw.size(); ++i)
    {
        FinalStr += '|' + __List_Regex_Raw[i];
    }
    if (!Suffix.empty()) {FinalStr.append(L")" + Suffix);}
    FinalStr.append(L")+)");
    return FinalStr.ToStdString();
}

ustringlist StellarClassification::ParseSymbols(const _REGEX_NS wregex &Reg, wstring str)
{
    ustringlist FinalList;
    auto it = str.end();
    auto end = str.begin();
    while (it != end)
    {
        _REGEX_NS wsregex_iterator regmatch(end, it, Reg);
        _REGEX_NS wsregex_iterator end2;
        ustring SStr = ustring((*regmatch)[2].str());
        FinalList.insert(FinalList.begin(), SStr);
        it -= SStr.size();
    }
    return FinalList;
}

void StellarClassification::BindSpecType(StelClassFlags *Dst, ustring Src)
{
    switch (Src.front())
    {
    case 'O':
        *Dst = StelClassFlags(*Dst | O);
        break;
    case 'B':
        *Dst = StelClassFlags(*Dst | B);
        break;
    case 'A':
        *Dst = StelClassFlags(*Dst | A);
        break;
    case 'F':
        *Dst = StelClassFlags(*Dst | F);
        break;
    case 'G':
        *Dst = StelClassFlags(*Dst | G);
        break;
    case 'K':
        *Dst = StelClassFlags(*Dst | K);
        break;
    case 'M':
        *Dst = StelClassFlags(*Dst | M);
        break;
    case 'L':
        *Dst = StelClassFlags(*Dst | L);
        break;
    case 'T':
        *Dst = StelClassFlags(*Dst | T);
        break;
    case 'Y':
        *Dst = StelClassFlags(*Dst | Y);
        break;
    default:
        break;
    }
}

void StellarClassification::BindLumType(StelClassFlags *Dst, ustring FullStr, ustring Src, ustring SubLC)
{
    if (FullStr.empty()) {return;}
    bool UseSubLumClasses = 1;
    if (Src == "V") {*Dst = StelClassFlags(*Dst | ms);}
    else if (FullStr == "VI")
    {
        *Dst = StelClassFlags(*Dst | sd);
        UseSubLumClasses = 0;
    }
    else if (Src == "IV") {*Dst = StelClassFlags(*Dst | subg);}
    else if (Src == "III") {*Dst = StelClassFlags(*Dst | g);}
    else if (Src == "II") {*Dst = StelClassFlags(*Dst | bg);}
    else if (Src == "I") {*Dst = StelClassFlags(*Dst | sg);}
    else if (FullStr == "Ia+" || FullStr == "0")
    {
        *Dst = StelClassFlags(*Dst | hg);
        UseSubLumClasses = 0;
    }

    if (!SubLC.empty() && UseSubLumClasses)
    {
        if (SubLC == "a") {*Dst = StelClassFlags(*Dst | a);}
        else if (SubLC == "ab") {*Dst = StelClassFlags(*Dst | ab);}
        else if (SubLC == "b") {*Dst = StelClassFlags(*Dst | b);}
    }
}

void StellarClassification::BindUncertainty(StellarClassificationDataType::__Cls_State *Dst, ustring FullStr)
{
    switch (FullStr.front())
    {
    case '+':
        *Dst = StellarClassificationDataType::Greater;
        break;
    case '-':
        *Dst = StellarClassificationDataType::Less;
        break;
    case ':':
    case '?':
        *Dst = StellarClassificationDataType::Uncertain;
        break;
    default:
        break;
    }
}

vector<StellarClassification::AbsorptionPecularityDataType>
StellarClassification::ParseAbsorptionPecularities(const ustring& Src)
{
    enum RegPosition
    {
        FullString,
        AllTypes,
        LastElem,
        Symbol,
        Sign,
        Sub,
        LastInt,
        LastFract,
        Uncertainty
    };

    vector<AbsorptionPecularityDataType> FinalList;
    auto it = Src.end();
    auto end = Src.begin();
    while (it != end)
    {
        _REGEX_NS wsmatch Match;
        _REGEX_NS regex_match(end, it, Match, __Absorption_Pecularities_Regex);
        AbsorptionPecularityDataType Data;
        Data.Compound = ustring(Match[Symbol].str());
        ustring S = ustring(Match[Sign].str());
        ustring N = ustring(Match[Sub].str());
        Data.Strength = stod(S + N);
        switch (Match[Uncertainty].str().front())
        {
        case '+':
            Data.Uncertainty = AbsorptionPecularityDataType::Greater;
            break;
        case '-':
            Data.Uncertainty = AbsorptionPecularityDataType::Less;
            break;
        case ':':
        case '?':
            Data.Uncertainty = AbsorptionPecularityDataType::Uncertain;
            break;
        default:
            Data.Uncertainty = AbsorptionPecularityDataType::None;
            break;
        }
        FinalList.insert(FinalList.begin(), Data);
        it -= Match[LastElem].str().size();
    }
    return FinalList;
}

vector<StellarClassification::ChemicalPecularityDataType>
StellarClassification::ParseChemicallyPecularities(const ustring &Src)
{
    enum RegPosition
    {
        FullString,
        AllSymbols,
        LastElem,
        Symbol,
        Uncertainty
    };

    vector<ChemicalPecularityDataType> FinalList;
    auto it = Src.end();
    auto end = Src.begin();
    while (it != end)
    {
        _REGEX_NS wsmatch Match;
        _REGEX_NS regex_match(end, it, Match, __Element_Symbols_Regex);
        ChemicalPecularityDataType Data;
        Data.Symbol = Match[Symbol];
        if (!Match[Uncertainty].str().empty()) {Data.Uncertainty = 1;}
        else {Data.Uncertainty = 0;}
        FinalList.insert(FinalList.begin(), Data);
        it -= Match[LastElem].str().size();
    }
    return FinalList;
}

_OPTICS_END _CSE_END
