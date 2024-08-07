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

// ------------------------- Tables ------------------------- //

const std::map<StellarClassification::__Spectral_Classification_Types, ustring>
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
    "f+", "f?", "(f)", "(f+)", "((f))", "((f*))", "h", "ha", "E", "L",
    "Hewk", "k", "m", "n", "nn", "neb", "p", "pq", "q", "s",
    "ss", "sh", "var", "wl", "z",
};

// const ustringlist StellarClassification::__Absorption_Pecularities // For pecular reg/yellow giants such as barium star
// {

// };

const std::map<StellarClassification::__Load_Type, StellarClassification::__Parse_Function_Type>
StellarClassification::__Load_Methods
{
    {MorganKeenan, StellarClassification::__Morgan_Keenan_Classification_Parse}
};


// ---------------------- Regex Strings --------------------- //

const std::wstring StellarClassification::__Spectal_Class_RegexStr = LR"(O|B|A|F|G|K|M|L|T|Y)";
const std::wstring StellarClassification::__Sub_Class_RegexStr = LR"([0-9]|[0-9]\.[0-9])";
const std::wstring StellarClassification::__Luminosity_Class_RegexStr = LR"(VI|(V|IV|III|II|I)(a|ab|b)?|Ia\+|0)";

const std::wstring StellarClassification::__Morgan_Keenan_Classification_RegexStr =
[]()->const std::wstring
{
    std::wstring Str = L'(' + __Spectal_Class_RegexStr + L')';
    RegexAppend(&Str, __Sub_Class_RegexStr, '?');
    RegexAppend(&Str, __Luminosity_Class_RegexStr, '?');
    return Str;
}();

const std::wstring StellarClassification::__Spectral_Pecularities_RegexStr
    = GenerateListMatchRegexString(__Spectral_Pecularities);

const std::wstring StellarClassification::__Element_Symbols_RegexStr =
    LR"((A[cglmrstu]|B[aehikr]?|C[adeflmnorsu]?|D[bsy]|E[rsu]|F[elmr]?|G[ade]|H[efgos]?|I[nr]?|Kr?|L[airuv]|M[cdgnot]|N[abdehiop]?|O[gs]?|P[abdmortu]?|R[abefghnu]|S[bcegimnr]?|T[abcehilms]|U|V|W|Xe|Yb?|Z[nr])+)";


// ------------------------- Regexes ------------------------ //

const _REGEX_NS wregex StellarClassification::__Spectral_Pecularities_Regex(__Spectral_Pecularities_RegexStr);
const _REGEX_NS wregex StellarClassification::__Element_Symbols_Regex(__Element_Symbols_RegexStr);
const _REGEX_NS wregex StellarClassification::__Morgan_Keenan_Classification_Regex(__Morgan_Keenan_Classification_RegexStr);


// --------------------- Regex Functions -------------------- //

const void StellarClassification::RegexAppend(std::wstring *Dst, std::wstring Src, ucs2_t Suffix)
{
    Dst->append(L"(" + Src + L")" + Suffix);
}

const std::wstring StellarClassification::GenerateListMatchRegexString(ustringlist Li)
{
    static const ucs2_t POSIXMetaCharacters[16] =
    {
        '^', '.', '[', ']', '$', '(', ')', '*', ',', '{', '}', '?', '+', '|', '-', '\\'
    };

    ustringlist __List_Regex_Raw;
    for (auto i : Li)
    {
        ustring ustr;
        auto first = std::begin(i);
        auto last = std::end(i);
        while (first != last)
        {
            if (first != last &&
                std::find(std::begin(POSIXMetaCharacters), std::end(POSIXMetaCharacters), *first)
                    != std::end(POSIXMetaCharacters))
            {
                ustr.push_back('\\');
            }
            ustr.push_back(*first);
            ++first;
        }
        __List_Regex_Raw.push_back(ustr);
    }

    ustring FinalStr;
    FinalStr.push_back('(');
    FinalStr += __List_Regex_Raw[0];
    for (int i = 1; i < __List_Regex_Raw.size(); ++i)
    {
        FinalStr += '|' + __List_Regex_Raw[i];
    }
    FinalStr.append(L")+");
    return FinalStr.ToStdWString();
}

ustringlist StellarClassification::ParseSymbols(const _REGEX_NS wregex &Reg, std::wstring str)
{
    ustringlist FinalList;
    auto it = str.end();
    auto end = str.begin();
    while (it != end)
    {
        _REGEX_NS wsregex_iterator regmatch(end, it, Reg);
        _REGEX_NS wsregex_iterator end2;
        ustring SStr = ustring((*regmatch)[1].str());
        FinalList.insert(FinalList.begin(), SStr);
        it -= SStr.size();
    }
    return FinalList;
}

void StellarClassification::BindSpecType(StelClassFlags *Dst, ucs2_t Src)
{
    switch (Src)
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
        *Dst = StelClassFlags(*Dst | ms);
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

_OPTICS_END _CSE_END
