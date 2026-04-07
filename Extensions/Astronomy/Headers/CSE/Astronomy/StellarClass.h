/*
    恒星光谱分析工具（近乎全覆盖）
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

#include <cstdint>
#ifndef __STELCLASS__
#define __STELCLASS__

#include <CSE/Base.h>
#include <CSE/Object.h>
#include <CSE/Parser.h>
#include <set>

#if _USE_BOOST_REGEX
#include <boost/regex.hpp>
using boost::wregex;
#else
#include <regex>
using std::wregex;
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

#define _SPCLS_BEGIN namespace StelCls {
#define _SPCLS_END }
#define _SPCLS StelCls::

/*
    丹霞：由于不同类型的恒星光谱字符串采用了不同的结构，同种恒星的光谱型也可能会出现不同的写法，所以
    就导致恒星光谱字符串的分析变得难上加难，甚至分析的逻辑为了最大兼容不得不肆无忌惮的“拧麻花”。为增
    加灵活性，此处使用一种基于工厂函数的方式。也就是先将恒星分类，每一类对应一种继承自同一个接口的数
    据结构，构造时就可以用工厂函数进行统一管理。分析字符串时先大致把字符串扫描一遍以确定它可能属于哪
    一类恒星，在调用对应数据结构的分析函数分析字符串，完成后的数据结构以接口指针的形式返回。分析出来
    的数据字段用一堆Getter访问，这样就可以无视数据结构的具体实现，而且历史兼容性和扩展性也强。
*/

__interface StellarClassData;

typedef class StellarClassificationFactoryClass
{
public:
    using ValueType    = StellarClassificationFactoryClass;
    using Pointer      = std::shared_ptr<StellarClassData>;
    using DetectorType = uint8_t(ustring);
    using ParserType   = Pointer(ustring);

    enum StarTypes
    {
        Auto, Normal
    };

    struct MetaDataType
    {
        DetectorType* DFunc;
        ParserType*   PFunc;
    };

private:
    static std::map<StarTypes, MetaDataType> MetaData;
    static ValueType AutoDetect(ustring Class);

    Pointer Data;

public:
    static void Register(StarTypes Type, ParserType* Ctor, DetectorType* Detector);

    static ValueType CreateFromString(ustring Class, StarTypes Type = Auto);
    static ustring ExportToString(ValueType Class);

    Pointer Get()const;
    StellarClassificationFactoryClass(ustring SpType);
    operator ustring()const;
}StellarClassification;

__interface StellarClassData
{
    virtual ustring ToString()const = 0;

    virtual char SpectralClass(size_t Index = 0)const = 0;
    virtual uint16_t SpectralClassUncertained(size_t Index = 0)const = 0;
    virtual uint16_t SpecializedClass(size_t Index = 0)const = 0;
    virtual uint16_t SpecializedClassUncertained(size_t Index = 0)const = 0;
    virtual ustring LuminosityClass(size_t Index = 0)const = 0;
    virtual uint16_t LuminosityClassUncertained(size_t Index = 0)const = 0;

    virtual std::set<ustring> SpectralPeculiarities()const = 0;
    virtual std::set<ustring> ElementSymbols()const = 0;
    virtual std::map<ustring, std::variant<int16_t, ustring>> BandPecularities()const = 0;

    virtual ustring UnanalyzedString()const = 0;
};

_SPCLS_BEGIN

/**
 * @details
 * 最常见的光谱型，而正式因为它最常见从而导致了最五花八门的写法。
 * 为了最大限度的提高容错率和解决各种历史遗留问题，搞得整个分析的
 * 程序如同水多加面面多加水一般，逻辑也是肆无忌惮的拧麻花。当然这
 * 也意味着后期维护的难度会被提高到丧心病狂的程度。
 */
class NormalStar : public StellarClassData
{
public:
    enum ParserStateType
    {
        PStart   = 0b000000, 
        PSpec    = 0b000001, 
        PCarbon  = 0b000010, 
        PSub     = 0b000011, 
        PLum     = 0b000100, 
        PSLum    = 0b000101, 
        PPec     = 0b000110, 
        PChem    = 0b000111, 
        PBar     = 0b001000,
        POpMask  = 0b110000,
        PRange   = 0b010000,
        PBracket = 0b100000
    };

    using EventQueueType  = std::vector<std::function<void(NormalStar*, ustring, ustring*, ParserStateType*)>>;

    struct UncertaintyType
    {
        static const wregex UncertaintyPattern;

        enum UncertaintySymbols : uint16_t
        {
            None        = 0b000000000,
            Uncertained = 0b000000011, // :
            More        = 0b000000010, // +
            Less        = 0b000000001, // -
            Bracket     = 0b000011100, // ()
            BrakStart   = 0b000001100, // (
            BrakIn      = 0b000000100, // (...)
            BrakEnd     = 0b000010100, // )

            RangeMask   = 0b011100000,
            Ranged      = 0b000100000, // 用于范围
            Slash       = 0b001000000, // /
            Simplified  = 0b010000000, // like M1-2, Ia-ab

            Ques        = 0b100000011  // ? （不确定符号的另一写法，如HD 224906的光谱为B9IIIp?(HgMn?)）
        };

        static void BracketStart(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State);
        static void BracketEnd(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State);
        static bool CheckBracketEnd(const UncertaintySymbols& Symbol);
        static void UncertaintyHandler(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
        static void AddUncertainty(ustring* In, UncertaintyType::UncertaintySymbols Symbol);
    };

    struct ValueType
    {
        // Patterns
        static const wregex SpecClassPattern;
        static const wregex NumberPattern;
        static const wregex LuminosityClassPattern;
        static const wregex LuminosityClassPattern2;
        static const wregex SubLumPattern;

        struct Details
        {
            enum : uint8_t 
            {
                O     = 0b001,
                B     = 0b010,
                A     = 0b011,
                F     = 0b100,
                G     = 0b101,
                K     = 0b110,
                M     = 0b111
            } Spec = decltype(Spec)(0);

            UncertaintyType::UncertaintySymbols SpecU = UncertaintyType::None;

            uint16_t Sub = 0b11111111111; // *100后存储，问就是先前见过一例O9.75的和380例M10甚至M10.5的（O9.75后续未能查到对应恒星，推测可能为上世纪的论文中出现过的但后来不再分这么细了。M10均为温度只有2000K出头的红超巨星，最低可低至1800K以下，如葫芦星云的中心恒星OH 231.8+04.2(QX Pup)主星的光谱型为M10III，伴星是一颗A型恒星）

            UncertaintyType::UncertaintySymbols SubU = UncertaintyType::None;

            enum : uint8_t
            {
                hg    = 0b111,
                sg    = 0b110,
                bg    = 0b101,
                g     = 0b100,
                gmask = 0b100,
                subg  = 0b011,
                ms    = 0b001
            } Lum = decltype(Lum)(0);

            UncertaintyType::UncertaintySymbols LumU = UncertaintyType::None;

            enum : uint8_t
            {
                a     = 0b01,
                b     = 0b10,
                ab    = 0b11
            } SLum = decltype(SLum)(0);

            UncertaintyType::UncertaintySymbols SLumU = UncertaintyType::None;;
        }DetailedData;

        static void LoadSpecImpl(ustring SpecString, ValueType* Destination);
        static void LoadNumber(ustring SpecString, uint16_t* Destination);
        static void LoadLumImpl(ustring SpecString, ValueType* Destination);
        static void LoadSubLumImpl(ustring SpecString, ValueType* Destination);

        static bool PecularitiesBrakStartPrecheck(ValueType* Destination);

        static char SpecToChar(decltype(DetailedData.Spec) Data);
        static ustring SubToString(decltype(DetailedData.Sub) Data);
        static ustring LumToString(decltype(DetailedData.Lum) Data, bool __2 = 0);
        static ustring SLumToString(decltype(DetailedData.SLum) Data);
    };

    struct CarbonType
    {
        static const wregex CarbonOrSTypePattern;
        enum {C, N, S} Data;
    };

    struct PecularityType
    {
        static ustringlist SpectralPecularitiesTable;
        static wregex Pattern;

        ustring Element;
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None; // 基本用不到，但也可能会出现，如HD 210030为G8/K0II/III(pBa)

        static void LoadPecularities(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    };
    
    struct BandPecularitiesType
    {
        static const std::string PatternSkeleton;
        static ustringlist AbsorptionPecularityTable;
        static wregex Pattern;

        ustring Key;
        std::variant<int16_t, ustring> Value; // 数字会乘以100后储存，问就是发现了一例HD 10607的光谱为G0Vm-2.25
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None;

        static BandPecularitiesType Load(ustring Source);
        static void LoadBariumOrCN(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    };

    struct ChemicalPecularitySpec
    {
        static const ustringlist ChemicalElementsTable;
        static wregex Pattern;

        ustring Element;
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None;

        static void LoadChemElems(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    };

protected:
    ValueType Data; // Ex: G2V, K3III, A2Ia, ...（至少有一个SpecType）

    // 以下两种数据也不是互斥关系
    std::optional<CarbonType> CData; // Ex: M3SIII, OC9.7Ib
    std::optional<ValueType> FloatData; // Ex: G2Ib-II, F2/3IV/V, B8Ia/ab

    // 原以为以下三种字段有互斥关系，但后来发现一例三种字段同时出现的：辇道四 B8pSi(FeII)
    std::vector<PecularityType> Pecularities; // Ex: (n)fp, e, ...
    std::vector<ChemicalPecularitySpec> ChemicalElems; // Ex: SiSrCr, ...
    std::vector<BandPecularitiesType> BandPecs; // Ex: Ba1，Fe-0.5, CNIV, ...

    ustring RemainString; // 剩余无法解析的字符串信息

    // Event queue in ParseFunc
    static const EventQueueType ParserEventQueue;
    
    static void SetState(ParserStateType* State, ParserStateType Value);
    static void ConsumeSpace(ustring Source, ustring* Remain);
    static void LoadSpec(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadCarbonType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadSub(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadSType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadSubLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadPecularitiesStage1(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadPecularitiesStage2(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
    static void LoadPecularitiesStage3(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    // Sub-routines in ToString()
    ustring ExportSpec()const;
    ustring ExportSub()const;
    ustring ExportSpecSubRange()const;
    ustring ExportLum()const;
    ustring ExportLumRange()const;
    ustring ExportPec()const;
    ustring ExportChem()const;
    ustring ExportBand()const;
    void ExportFullPecString(ustring* MainString)const;

public:
    static std::shared_ptr<StellarClassData> ParseFunc(ustring StelClassString);
    ustring ToString()const override;

    const ValueType& GetRawDataByIndex(size_t Index)const;

    char SpectralClass(size_t Index)const override;
    uint16_t SpectralClassUncertained(size_t Index)const override;
    uint16_t SpecializedClass(size_t Index)const override;
    uint16_t SpecializedClassUncertained(size_t Index)const override;
    ustring LuminosityClass(size_t Index)const override;
    uint16_t LuminosityClassUncertained(size_t Index)const override;

    std::set<ustring> SpectralPeculiarities()const override;
    std::set<ustring> ElementSymbols()const override;
    std::map<ustring, std::variant<int16_t, ustring>> BandPecularities()const override;

    ustring UnanalyzedString()const override; // 剩余未能分析成功的字符串，如果为空表示识别成功
};

_SPCLS_END

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
