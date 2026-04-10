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

#include <unordered_map>
#include <variant>
#ifndef __STELCLASS__
#define __STELCLASS__

#include <CSE/Base.h>
#include <CSE/Object.h>
#include <CSE/Parser.h>
#include <set>
#include <cstdint>

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

__interface StellarClassData
{
    virtual ustring Description()const = 0;
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

_SPCLS_BEGIN

/**
 * @brief 最常见的光谱型，而正式因为它最常见从而导致了最五花八门的写法。
 * @note 为了最大限度的提高容错率和解决各种历史遗留问题，搞得整个分析的程序如同水多加面面多加水一般。
 * 当然这也意味着后期维护的难度会被提高到丧心病狂的程度。
 */
class NormalStar : public StellarClassData
{
public:
    enum ParserStateType
    {
        PStart   = 0b00000, 
        PSpec    = 0b00001, 
        PSub     = 0b00010, 
        PLum     = 0b00011, 
        PSLum    = 0b00100, 
        PPec     = 0b00101, 
        PChem    = 0b00110, 
        PBar     = 0b00111,
        POpMask  = 0b11000,
        PRange   = 0b01000,
        PBracket = 0b10000
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

        static const uint16_t U16Npos = ~(uint16_t(0));

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

            uint16_t Sub = U16Npos; // *100后存储，问就是先前见过一例O9.75的和380例M10甚至M10.5的，不过O9.75后续未能查到对应恒星，推测可能为上世纪的论文中出现过的但后来不再分这么细了。M10及以下的均为温度只有2000K出头的红超巨星，最低可低至1800K以下，如葫芦星云的中心恒星OH 231.8+04.2(QX Pup)主星的光谱型为M10III，伴星是一颗A型恒星。著名的V* V838 Mon的光谱也是M10（现修正为M5.5I）。

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

            UncertaintyType::UncertaintySymbols SLumU = UncertaintyType::None;
        }DetailedData;

        static void LoadSpecImpl(ustring SpecString, ValueType* Destination);
        static void LoadNumber(ustring SpecString, uint16_t* Destination);
        static void LoadLumImpl(ustring SpecString, ValueType* Destination);
        static void LoadSubLumImpl(ustring SpecString, ValueType* Destination);

        static bool PecularitiesBrakStartPrecheck(ValueType* Destination, int Depth = 4);

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

        static void LoadPecularities(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State, bool __2 = 0);
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
        static void LoadBandPecs(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);
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
    bool PecularitiesBehindBandPecs = 0; // 后续发现一例HD 146850的光谱为K3IIICNVp

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
    ustring Description()const override;
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

    friend class AmStar;
};

/**
 * @details 一类缺钙，钪的恒星。这种异常的相对丰度导致根据钙线（即1价钙离子的393.366nm）测定的光谱
 * 类型系统性地比根据其他金属线测定的光谱类型更早。通常情况下，仅根据氢谱线判断的光谱类型属于中间型。
 * 因此，此类恒星通常会给出两到三种光谱类型并写作k...h...m...的形式。
 * 
 * 例如天狼的完整光谱型为kA0hA0VmA1，意思是其在钙线的光谱型为A0，氢线的光谱型为A0，金属线的光谱型为
 * A1，不过天狼可能是因为不同的谱线下测得的光谱型差距不大，所以一般简写为A0mA1Va。
 *
 * 详见：https://en.wikipedia.org/wiki/Am_star
 *
 * @note Am恒星和天弁二变星在赫罗图上大致位于相同的位置，但是这是两种不同的恒星。并且一个恒星既是Am
 * 恒星又是天弁二变星的情况十分罕见，例如瓠瓜三和弧矢增卅二。
 */
class AmStar : public StellarClassData
{
public:
    using VirtualBase            = NormalStar;
    using KeyType                = ustring;
    using PecularityType         = VirtualBase::PecularityType;
    using ChemicalPecularitySpec = VirtualBase::ChemicalPecularitySpec;

    struct ValueType
    {
        VirtualBase::ValueType Value;
        std::optional<VirtualBase::ValueType> FloatValue;
    };
    
    enum SubFmts // 两种子类型中唯一的区别可能也就是有没有主光谱
    {
        /**
         * @details 第一种表示方法类似瓠瓜三：kA7hF1VmF1pSrEuCr:，
         * 由多个段组成且没有明确的主光谱。
         */
        DeltaDelphini,

        /**
         * @details 第二种表示方法类似弧矢增卅二：F5IIkF2IImF5II，
         * 有明确的主光谱。这类Am恒星通常被认为演化的更晚且更亮，位
         * 于主序带上方。
         */
        RhoPuppis
    };

    struct DDelValueType
    {
        std::unordered_map<KeyType, ValueType> Segments;
    };

    struct RPupValueType
    {
        ValueType MainSegment;
        std::vector<PecularityType> MainSegPecs;
        std::unordered_map<KeyType, ValueType> Segments;
    };

    static ustringlist LinesTable;
    static wregex LinesPattern;

protected:
    std::variant<DDelValueType, RPupValueType> Data;
    bool AddSuffix = 0; // 部分案例会有"Am"后缀
    std::vector<PecularityType> Pecularities; // 部分恒星会有Am星与玄戈变星双重身份，位于零龄主序区附近。除Am星的缺钙特征以外还缺铁。见：https://en.wikipedia.org/wiki/Lambda_Bo%C3%B6tis_star
    std::vector<ChemicalPecularitySpec> ChemElems;

public:
    ustring Description()const override;
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
