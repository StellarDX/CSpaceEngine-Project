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

#pragma once

#ifndef __SPECTROSCOPY__
#define __SPECTROSCOPY__

#include "CSE/Base.h"

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

#define _OPTICS_BEGIN namespace Illuminants {
#define _OPTICS_END }
#define _OPTICS Illuminants::

_OPTICS_BEGIN

/****************************************************************************************\
*                                       Magnitudes                                       *
\****************************************************************************************/

/**
 * @brief Calculate absolute magnitude from visual magnitude and distance
 * @note where it is assumed that extinction from gas and dust is negligible.
 * Typical extinction rates within the Milky Way galaxy are 1 to 2 magnitudes
 * per kiloparsec, when dark clouds are taken into account.
 * @param AppMagn - visual magnitude
 * @param Dist - distance in parsec
 * @return absolute magnitude
 */
float64 GetAbsMagnFromAppMagnDist(float64 AppMagn, float64 Dist);

/**
 * @brief Calculate absolute magnitude from visual magnitude and parallax
 * @param AppMagn - visual magnitude
 * @param Dist - parallax in mas
 * @return absolute magnitude
 */
float64 GetAbsMagnFromAppMagnParallax(float64 AppMagn, float64 Parallax);

/**
 * @brief Calculate absolute bolometric magnitude from bolometric luminosity
 * @param LumBol - Bolometric luminosity in watts
 * @return absolute bolometric magnitude
 */
float64 GetAbsMagnBolFromLumBol(float64 LumBol);

/**
 * @brief Calculate bolometric luminosity from absolute bolometric magnitude
 * @param absolute bolometric magnitude
 * @return bolometric luminosity
 */
float64 GetLumBolFromAbsMagnBol(float64 AbsMagnBol);


/****************************************************************************************\
*                                 Stellar Classification                                 *
\****************************************************************************************/

struct StellarClassification
{
    typedef enum __Spectral_Classification_Types
    {
        ms      = 0b10000000000000000000000000000000, // Main-Sequence (V)
        O       = 0b00000000000000000000000000000001, // O-type
        B       = 0b00000000000000000000000000000010, // B-type
        A       = 0b00000000000000000000000000000011, // A-type
        F       = 0b00000000000000000000000000000100, // F-type
        G       = 0b00000000000000000000000000000101, // G-type
        K       = 0b00000000000000000000000000000110, // K-type
        M       = 0b00000000000000000000000000000111, // M-type

        L       = 0b00000000000000000000000000001000, // L-type
        T       = 0b00000000000000000000000000001001, // T-type
        Y       = 0b00000000000000000000000000001010, // Y-type

        sd      = 0b01000000000000000000000000000000, // Subdwarf
        esd     = 0b01100000000000000000000000000000, // Extreme-Subdwarf
        usd     = 0b01010000000000000000000000000000, // Ultra-Subdwarf

        subg    = 0b00001100000000000000000000000000, // Subgiant (IV)
        g       = 0b00000100000000000000000000000000, // Giant (III)
        bg      = 0b00000110000000000000000000000000, // Bright-Giant (II)
        sg      = 0b00000101000000000000000000000000, // Supergiant (I)
        hg      = 0b00000111000000000000000000000000, // Hypergiant (I+)
        a       = 0b00000100010000000000000000000000, // a (I-Va)
        ab      = 0b00000100110000000000000000000000, // ab (I-Vab)
        b       = 0b00000100100000000000000000000000, // b (I-Vb)

        WR      = 0b00000000001000000000000000000000, // WR-Stars
        WN      = 0b00000000001100000000000000000000, // WN
        WC      = 0b00000000001010000000000000000000, // WC
        WNC     = 0b00000000001110000000000000000000, // WNC
        WO      = 0b00000000001001000000000000000000, // WO

        C       = 0b00000000000000100000000000000000, // Carbon-Stars
        N       = 0b00000000000000101000000000000000, // N-Stars
        CH      = 0b00000000000000100100000000000000, // CH-Stars
        CHd     = 0b00000000000000101100000000000000, // CH-Stars
        CJ      = 0b00000000000000100010000000000000, // CJ-Stars
        S       = 0b00000000000000010000000000000000, // S-type-Stars
        MS      = 0b00000000000000011110000000000000, // MS-Stars
        SC      = 0b00000000000000110000000000000000, // SC-Stars

        WD      = 0b00000000000000000001000000000000, // White-dwarf
        WDA     = 0b00000000000000000001100000000000, // DA
        WDB     = 0b00000000000000000001010000000000, // DB
        WDO     = 0b00000000000000000001001000000000, // DO
        WDQ     = 0b00000000000000000001000100000000, // DQ
        WDZ     = 0b00000000000000000001000010000000, // DZ
        WDC     = 0b00000000000000000001000001000000, // DC
        WDX     = 0b00000000000000000001000000100000, // DX

        NS      = 0b00000000000000000000000000010000, // Neutron-star
        BH      = 0b00000000000000000000000000000000, // Black-Hole
        PEC     = 0b11111111111111111111111111111111, // Pecilar-star

        TyMask  = 0b00000000000000000000000000001111,
        sdMask  = 0b01110000000000000000000000000000,
        gMask   = 0b00000111110000000000000000000000,
        WRMask  = 0b00000000001111000000000000000000,
        CMask   = 0b00000000000000111110000000000000,
        WDMask  = 0b00000000000000000001111111100000,
    }StelClassFlags;

    static const std::map<__Spectral_Classification_Types, ustring> __Spectral_Classification_Table;
    static const ustringlist __Spectral_Pecularities;

    static const _REGEX_NS wregex __Spectral_Pecularities_Regex;
    static const _REGEX_NS wregex __Element_Symbols_Regex;

    static const _REGEX_NS wregex __Morgan_Keenan_Classification_Regex;

protected:
    struct __Stellar_Class_Type
    {
        enum __Sub_Cls_State
        {
            Null, Single, Range
        }SubClsState = Null;
        bool IsDoubleLCls = 0;

        StelClassFlags SpecClass[2]; // Main-class, include spectal and luminosity class
        float64        SubClass[2];  // Sub-class
        ustringlist    Pecularities; // Pecularities
        ustringlist    ChemSymbols;  // For Chemically pecular star
    }Elems[3] = {{.SpecClass = {StelClassFlags(M | ms)}, .SubClass = {2}}};

    int Size = 1;

    enum __Load_Type
    {
        MorganKeenan, // Normal method
    }LoadType = MorganKeenan;

    ustring SrcString;

    using __Parse_Function_Type = std::function<StellarClassification(ustring)>;
    static const std::map<__Load_Type, __Parse_Function_Type> __Load_Methods;

    static const void RegexAppend(std::wstring* Dst, std::wstring Src);
    static const std::wstring __Spectral_Pecularities_RegexStr;
    static const std::wstring GeneratePecularitiesRegexString();
    static const std::wstring __Element_Symbols_RegexStr;

    static ustringlist ParseSymbols(const _REGEX_NS wregex& Reg, std::wstring str);

    static void BindSpecType(StelClassFlags* Dst, ucs2_t Src);
    static void BindLumType(StelClassFlags* Dst, ustring Src, ustring SubLC);

public:
    StellarClassification() {}
    StellarClassification(ustring);

    static StellarClassification __Morgan_Keenan_Classification_Parse(ustring str);
};

_OPTICS_END

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
