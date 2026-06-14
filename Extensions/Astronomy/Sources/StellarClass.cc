#include "CSE/Astronomy/StellarClass.h"
#include "CSE/Base/CSEBase.h"
#include <variant>
#include <ranges>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
using namespace std;
#endif

_CSE_BEGIN
_SPCLS_BEGIN

// ---------------------------------------- 工具函数 ---------------------------------------- //

_EXTERN_C

ustring __Escape_Special_Chars_Fast(const ustring& input)
{
    // 使用查找表，128个ASCII字符
    /*static const bool special_lookup[128] =
    {
        // 只设置需要转义的字符为true
        [L'\\'] = true, [L'.'] = true, [L'*'] = true,
        [L'+'] = true, [L'?'] = true, [L'{'] = true,
        [L'}'] = true, [L'^'] = true, [L'$'] = true,
        [L'('] = true, [L')'] = true, [L'|'] = true,
        [L'['] = true, [L']'] = true
    };*/ // GCC不支持这种写法

    static auto special_lookup = [](wchar_t c)
    {
        return c == L'\\' || c == L'.' || c == L'*' ||
           c == L'+' || c == L'?' || c == L'{' || c == L'}' ||
           c == L'^' || c == L'$' || c == L'(' || c == L')' ||
           c == L'|' || c == L'[' || c == L']';
    };

    ustring result;
    result.reserve(input.size() * 2);

    for (auto c : input)
    {
        // 检查是否在ASCII范围内且是需要转义的字符
        if (c < 128 && special_lookup(c))
        {
            result.push_back('\\');
        }
        result.push_back(c);
    }

    return result;
}

ustring __List_To_Pattern(ustringlist List)
{
    // 如果需要匹配包含重复字符的字符串（比如 "cat" 和 "category"），
    // 一定要把长的字符串放在前面（即 category|cat），否则永远匹配不到长的那个。
    // 此处进行一个倒序排序以尽量得到最符合条件的结果
    ustringlist SortedTable(List);
    std::sort(SortedTable.begin(), SortedTable.end(), 
        [](ustring S1, ustring S2){return S2 < S1;});
    ustring Table;
    Table += L"(";
    for (int i = 0; i < SortedTable.size(); ++i)
    {
        Table += __Escape_Special_Chars_Fast(ustring(SortedTable[i]));
        if (i < SortedTable.size() - 1) {Table += L"|";}
        else {Table += L")";}
    }
    return Table;
}

ustring __Regex_Str_Str(ustring Source, const wregex& Pattern, ustring* Remain = nullptr)
{
    wsmatch Match;
    if (regex_search((const std::wstring&)(Source), Match, Pattern))
    {
        ustring Prefix(Match.str(0));
        if (Source.substr(0, Prefix.size()) != Prefix)
        {
            if (Remain) {*Remain = Source;}
            return ustring();
        }
        if (Remain) {*Remain = ustring(Match.suffix().str());}
        return Prefix;
    }
    else 
    {
        if (Remain) {*Remain = Source;}
        return ustring();
    }
}

_END_EXTERN_C



// --------------------------------------- NormalStar --------------------------------------- //

ustring NormalStar::Description()const
{
    static const ustring Descr = L"Star";
    return Descr;
}

const wregex NormalStar::UncertaintyType::UncertaintyPattern(LR"(^(:|\+|-|\(|\)|/))");

void NormalStar::UncertaintyType::BracketStart(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State)
{
    *Data = UncertaintyType::UncertaintySymbols(uint16_t(*Data) | uint16_t(BrakStart));
    *State = ParserStateType(int(*State) | int(PBracket));
    *Remain = Source.erase(0, 1);
}

void NormalStar::UncertaintyType::BracketEnd(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State)
{
    *Data = UncertaintyType::UncertaintySymbols(uint16_t(*Data) | uint16_t(BrakEnd));
    *State = ParserStateType(int(*State) & (~int(PBracket)));
    *Remain = Source.erase(0, 1);
}

bool NormalStar::UncertaintyType::CheckBracketEnd(const UncertaintySymbols& Symbol)
{
    return (Symbol & UncertaintyType::BrakIn) && ((Symbol & UncertaintyType::BrakEnd) != UncertaintyType::BrakEnd);
}

void NormalStar::UncertaintyHandler(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    if (Source.empty()) {return;}
    // 不确定字符可能会出现在任何位置，此处涉及到一个巨大的分类讨论
    switch ((*State) & (~POpMask))
    {
    case PStart: // 若一开始就出现不确定字符，只有可能是'('
        if (Source[0] == '(')
        {
            UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
            UncertaintyType::BracketStart(Source, Remain, &Data, State);
            Output->Data.DetailedData.SpecU = Data;
        }
        break;
    case PSpec: 
        // 若不确定字符出现在光谱后面，可能是'('，')'，':'和'/'，不会或很少出现'+'和'-'
        // 但后续发现有一例M-0.14的（BD+16 1895B），还有5529例O-rich的，不知道是归类到此还是单独归类
        {
            switch (Source[0])
            {
            default:
                break;
            case '(':
                {
                    UncertaintyType::UncertaintySymbols TempData;
                    UncertaintyType::BracketStart(Source, Remain, &TempData, State);
                }
                break;
            case ')':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
                    UncertaintyType::BracketEnd(Source, Remain, &Data, State);
                    Output->Data.DetailedData.SpecU = Data;
                }
                break;
            case '?': // 这里不break
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ques));
                    Output->Data.DetailedData.SpecU = Data;
                }
            case ':':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Uncertained));
                    Output->Data.DetailedData.SpecU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '/':
                {
                    *State = ParserStateType(int(*State) | int(PRange));
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Slash));
                    Output->Data.DetailedData.SpecU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PSub:
        // 若不确定字符出现在光谱后面，可能是'('，')'，':'，'/'，'+'和'-'
        {
            switch (Source[0])
            {
            default:
                break;
            case '(':
                {
                    UncertaintyType::UncertaintySymbols TempData;
                    UncertaintyType::BracketStart(Source, Remain, &TempData, State);
                }
                break;
            case ')': // ')'后面若还有不确定符号，必为'('
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                    UncertaintyType::BracketEnd(Source, Remain, &Data, State);
                    Output->Data.DetailedData.SubU = Data;
                }
                break;
            case '?': // 这里不break
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ques));
                    Output->Data.DetailedData.SubU = Data;
                }
            case ':':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Uncertained));
                    Output->Data.DetailedData.SubU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '+':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::More));
                    Output->Data.DetailedData.SubU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '-': // 此时会有2种情况，若它的后面是OBAFGKM之一，或者跟了一个数字，就需要把PRange拉起来
                {
                    if (ustring(L"OBAFGKM").find(Source[1]) != ustring::npos || isdigit(Source[1]))
                    {
                        *State = ParserStateType(int(*State) | int(PRange));
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ranged));
                        Output->Data.DetailedData.SubU = Data;
                    }
                    else
                    {
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Less));
                        Output->Data.DetailedData.SubU = Data;
                    }
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '/':
                {
                    *State = ParserStateType(int(*State) | int(PRange));
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Slash));
                    Output->Data.DetailedData.SubU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PLum: 
        // 若不确定字符出现在光度级后面，可能是'('，')'，':'，'/'，'+'和'-'
        { 
            switch (Source[0])
            {
            default:
                break;
            case '(': // 若出现，则可能会是特殊谱线标志中的符号，也可能是不确定符号
                {
                    if (ustring(L"ab").find(Source[1]) != ustring::npos) // 原以为像Ia，Iab，IIIb这类不会把ab放在括号里面，但后续又发现一例HD 54786的光谱为B1/2I(b)
                    {
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                        UncertaintyType::BracketStart(Source, Remain, &Data, State);
                        Output->Data.DetailedData.SLumU = Data;
                        *Remain = Source.erase(0, 1);
                    }
                    else if (std::find_if(PecularityType::SpectralPecularitiesTable.begin(),
                        PecularityType::SpectralPecularitiesTable.end(),
                        [Source](ustring Match) {return Source.substr(0, Match.size()) == Match;})
                        == PecularityType::SpectralPecularitiesTable.end())
                    {
                        UncertaintyType::UncertaintySymbols TempData;
                        UncertaintyType::BracketStart(Source, Remain, &TempData, State); // 由于特殊谱线，化学元素这些是动态数组，这里只改变状态而不动数据，后续设置检测
                    }
                    // 如果检测结果是特殊谱线的字符串，不进行任何操作
                }
                break;
            case ')':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                    UncertaintyType::BracketEnd(Source, Remain, &Data, State);
                    Output->Data.DetailedData.LumU = Data;
                }
                break;
            case '?': // 这里不break
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ques));
                    Output->Data.DetailedData.LumU = Data;
                }
            case ':':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Uncertained));
                    Output->Data.DetailedData.LumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '+': // 这后面可能会跟另一个光谱型，但双星单独分类，此处不考虑。
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::More));
                    Output->Data.DetailedData.LumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '-': // 此时会有2种情况，若它的后面是另一个光度级，就需要把PRange拉起来
                {
                    if (ustring(L"IV").find(Source[1]) != ustring::npos)
                    {
                        *State = ParserStateType(int(*State) | int(PRange));
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ranged));
                        Output->Data.DetailedData.LumU = Data;
                    }
                    else
                    {
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Less));
                        Output->Data.DetailedData.LumU = Data;
                    }
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '/':
                {
                    *State = ParserStateType(int(*State) | int(PRange));
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Slash));
                    Output->Data.DetailedData.LumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PSLum:
        {
            switch (Source[0])
            {
            default:
                break;
            case '(': // 若出现，则可能会是特殊谱线标志中的符号，也可能是不确定符号
                {
                    if (std::find_if(PecularityType::SpectralPecularitiesTable.begin(),
                        PecularityType::SpectralPecularitiesTable.end(),
                        [Source](ustring Match) {return Source.substr(0, Match.size()) == Match;})
                        == PecularityType::SpectralPecularitiesTable.end())
                    {
                        UncertaintyType::UncertaintySymbols TempData;
                        UncertaintyType::BracketStart(Source, Remain, &TempData, State); // 由于特殊谱线，化学元素这些是动态数组，这里只改变状态而不动数据，后续设置检测
                    }
                    // 如果检测结果是特殊谱线的字符串，不进行任何操作
                }
                break;
            case ')':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                    UncertaintyType::BracketEnd(Source, Remain, &Data, State);
                    Output->Data.DetailedData.SLumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '?': // 这里不break
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ques));
                    Output->Data.DetailedData.SLumU = Data;
                }
            case ':':
                {
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Uncertained));
                    Output->Data.DetailedData.SLumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '+': // 出现了Ia+的情况需要特殊处理
                {
                    if (Output->Data.DetailedData.Lum == ValueType::Details::sg
                        && Output->Data.DetailedData.SLum == ValueType::Details::a)
                    {
                        Output->Data.DetailedData.Lum = ValueType::Details::hg;
                        Output->Data.DetailedData.SLum = decltype(Output->Data.DetailedData.SLum)(0);
                    }
                    else
                    {
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::More));
                        Output->Data.DetailedData.SLumU = Data;
                    }
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '0': // Ia+还有一种写法是Ia0
                if (Output->Data.DetailedData.Lum == ValueType::Details::sg
                    && Output->Data.DetailedData.SLum == ValueType::Details::a)
                {
                    Output->Data.DetailedData.Lum = ValueType::Details::hg;
                    Output->Data.DetailedData.SLum = decltype(Output->Data.DetailedData.SLum)(0);
                    *Remain = Source.erase(0, 1);
                }
                // 其余情况无操作
                break;
            case '-': // 此时会有2种情况，若它的后面是另一个光度级，就需要把PRange拉起来
                {
                    if (ustring(L"IVab0").find(Source[1]) != ustring::npos)
                    {
                        *State = ParserStateType(int(*State) | int(PRange));
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Ranged));
                        Output->Data.DetailedData.SLumU = Data;
                    }
                    else
                    {
                        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Less));
                        Output->Data.DetailedData.SLumU = Data;
                    }
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '/':
                {
                    *State = ParserStateType(int(*State) | int(PRange));
                    UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
                    Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Slash));
                    Output->Data.DetailedData.SLumU = Data;
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PPec: // 从此处以后改为操作前修改状态
        // 若不确定字符出现在特殊谱线后面，只有可能是'('和')'
        {
            switch (Source[0])
            {
            case '(': // 若出现，则可能会是特殊谱线标志中的符号，也可能是不确定符号
                {
                    if (std::find_if(PecularityType::SpectralPecularitiesTable.begin(),
                        PecularityType::SpectralPecularitiesTable.end(),
                        [Source](ustring Match) {return Source.substr(0, Match.size()) == Match;})
                        == PecularityType::SpectralPecularitiesTable.end())
                    {
                        UncertaintyType::UncertaintySymbols TempData;
                        UncertaintyType::BracketStart(Source, Remain, &TempData, State); // 由于特殊谱线，化学元素这些是动态数组，这里只改变状态而不动数据，后续设置检测
                    }
                    // 如果检测结果是特殊谱线的字符串，不进行任何操作
                }
                break;
            case ')':
                {
                    UncertaintyType::BracketEnd(Source, Remain, &Output->Pecularities.back().Uncertainty, State);
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PChem:
        // 若不确定字符出现在后面，有化学元素可能是':'，'('和')'
        {
            switch (Source[0])
            {
            case '(':
                {
                    UncertaintyType::UncertaintySymbols TempData;
                    UncertaintyType::BracketStart(Source, Remain, &TempData, State); // 由于特殊谱线，化学元素这些是动态数组，这里只改变状态而不动数据，后续设置检测
                }
                break;
            case ')':
                {
                    UncertaintyType::BracketEnd(Source, Remain, &Output->ChemicalElems.back().Uncertainty, State);
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '?': // 这里不break
                {
                    Output->ChemicalElems.back().Uncertainty = UncertaintyType::UncertaintySymbols(
                        uint16_t(Output->ChemicalElems.back().Uncertainty) | uint16_t(UncertaintyType::Ques));
                }
            case ':':
                {
                    Output->ChemicalElems.back().Uncertainty = UncertaintyType::UncertaintySymbols(
                        uint16_t(Output->ChemicalElems.back().Uncertainty) | uint16_t(UncertaintyType::Uncertained));
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    case PBar:
        // 若不确定字符出现在后面，有化学元素可能是':'，'('和')'
        {
            switch (Source[0])
            {
            case '(':
                {
                    UncertaintyType::UncertaintySymbols TempData;
                    UncertaintyType::BracketStart(Source, Remain, &TempData, State); // 由于特殊谱线，化学元素这些是动态数组，这里只改变状态而不动数据，后续设置检测
                }
                break;
            case ')':
                {
                    UncertaintyType::BracketEnd(Source, Remain, &Output->BandPecs.back().Uncertainty, State);
                    *Remain = Source.erase(0, 1);
                }
                break;
            case '?': // 这里不break
                {
                    Output->BandPecs.back().Uncertainty = UncertaintyType::UncertaintySymbols(
                        uint16_t(Output->BandPecs.back().Uncertainty) | uint16_t(UncertaintyType::Ques));
                }
            case ':':
                {
                    Output->BandPecs.back().Uncertainty = UncertaintyType::UncertaintySymbols(
                        uint16_t(Output->BandPecs.back().Uncertainty) | uint16_t(UncertaintyType::Uncertained));
                    *Remain = Source.erase(0, 1);
                }
                break;
            }
        }
        break;
    default:
        break;
    }
}

void NormalStar::UncertaintyType::AddUncertainty(ustring* In, UncertaintyType::UncertaintySymbols Symbol)
{
    if (!Symbol) {return;}

    bool InBracket = 0;
    if (Symbol & UncertaintyType::BrakIn)
    {
        Symbol = UncertaintyType::UncertaintySymbols(Symbol & (~UncertaintyType::BrakIn));
        InBracket = 1;
    }

    if (InBracket && (Symbol & UncertaintyType::BrakStart))
    {
        In->insert(In->begin(), '(');
    }

    UncertaintyType::UncertaintySymbols Symbol2 = UncertaintyType::UncertaintySymbols(
        Symbol & (~UncertaintyType::Bracket) & (~UncertaintyType::RangeMask));

    switch (Symbol2)
    {
    default:
        break;
    case UncertaintyType::Ques:
        In->push_back('?');
        break;
    case UncertaintyType::Uncertained:
        In->push_back(':');
        break;
    case UncertaintyType::More:
        In->push_back('+');
        break;
    case UncertaintyType::Less:
        In->push_back('-');
        break;
    }

    if (InBracket && (Symbol & UncertaintyType::BrakEnd)) {In->insert(In->end(), ')');}
}


const wregex NormalStar::ValueType::SpecClassPattern(LR"(^(O|B|A|F|G|K|M))", regex_constants::optimize);
const wregex NormalStar::CarbonType::CarbonOrSTypePattern(LR"(^(C|N))", regex_constants::optimize);
const wregex NormalStar::ValueType::NumberPattern(LR"(^((1?[0-9])((\.[1-9])[1-9]?)?))", regex_constants::optimize);
const wregex NormalStar::ValueType::LuminosityClassPattern(LR"(^(V|IV|III|II|I|_0))", regex_constants::optimize);
const wregex NormalStar::ValueType::LuminosityClassPattern2(LR"(^(V|IV|III|II|I|0))", regex_constants::optimize);
const wregex NormalStar::ValueType::SubLumPattern(LR"(^(ab|a|b))", regex_constants::optimize);

void NormalStar::ValueType::LoadSpecImpl(ustring SpecString, ValueType* Destination)
{
    switch (SpecString[0])
    {
        case 'O': Destination->DetailedData.Spec = ValueType::Details::O; break;
        case 'B': Destination->DetailedData.Spec = ValueType::Details::B; break;
        case 'A': Destination->DetailedData.Spec = ValueType::Details::A; break;
        case 'F': Destination->DetailedData.Spec = ValueType::Details::F; break;
        case 'G': Destination->DetailedData.Spec = ValueType::Details::G; break;
        case 'K': Destination->DetailedData.Spec = ValueType::Details::K; break;
        case 'M': Destination->DetailedData.Spec = ValueType::Details::M; break;
    }
}

void NormalStar::ValueType::LoadNumber(ustring SpecString, uint16_t* Destination)
{
    // 这里要尽量避免浮点数转化（计算机算不出0.1+0.2=0.3）
    auto DotPos = SpecString.find('.');
    uint16_t Integer = 0, Fractional = 0, Final = 0;
    if (DotPos == SpecString.npos) {Integer = stoi(SpecString);}
    else
    {
        wchar_t* FracStart;
        Integer = ::wcstol(SpecString.c_str(), &FracStart, 10);
        Fractional = ::wcstol(FracStart + 1, nullptr, 10);
    }
    Final = Integer * 100 + (Fractional >= 10 ? Fractional : Fractional * 10);
    *Destination = Final;
}

void NormalStar::ValueType::LoadLumImpl(ustring SpecString, ValueType* Destination)
{
    if (SpecString == "V") {Destination->DetailedData.Lum = ValueType::Details::ms;}
    else if (SpecString == "IV") {Destination->DetailedData.Lum = ValueType::Details::subg;}
    else if (SpecString == "III") {Destination->DetailedData.Lum = ValueType::Details::g;}
    else if (SpecString == "II") {Destination->DetailedData.Lum = ValueType::Details::bg;}
    else if (SpecString == "I") {Destination->DetailedData.Lum = ValueType::Details::sg;}
    else if (SpecString == "_0" || SpecString == "0") {Destination->DetailedData.Lum = ValueType::Details::hg;} // 特超巨星有三种表示方法，分别为Ia+，Ia0和_0（可能是为了与次型区分开所以在0前加一下划线，如HD 271182的光谱型为F8_0）
}

void NormalStar::ValueType::LoadSubLumImpl(ustring SpecString, ValueType* Destination)
{
    if (SpecString == "ab") {Destination->DetailedData.SLum = ValueType::Details::ab;}
    else if (SpecString == "a") {Destination->DetailedData.SLum = ValueType::Details::a;}
    else if (SpecString == "b") {Destination->DetailedData.SLum = ValueType::Details::b;}
}

bool NormalStar::ValueType::PecularitiesBrakStartPrecheck(ValueType* Destination, int Depth)
{
    switch (Depth)
    {
    default:
    case 4:
        if (Destination->DetailedData.SLum)
        {
            if (UncertaintyType::CheckBracketEnd(Destination->DetailedData.SLumU)) {return 0;}
        }
    case 3:
        if (Destination->DetailedData.Lum)
        {
            if (UncertaintyType::CheckBracketEnd(Destination->DetailedData.LumU)) {return 0;}
        }
    case 2:
        if (Destination->DetailedData.Sub != ValueType::U16Npos)
        {
            if (UncertaintyType::CheckBracketEnd(Destination->DetailedData.SubU)) {return 0;}
        }
    case 1:
        if (Destination->DetailedData.Spec)
        {
            if (UncertaintyType::CheckBracketEnd(Destination->DetailedData.SpecU)) {return 0;}
        }
    }

    return 1;
}

char NormalStar::ValueType::SpecToChar(decltype(DetailedData.Spec) Data)
{
    if (!Data) {return 0;}
    switch(Data)
    {
        case ValueType::Details::O: return 'O';
        case ValueType::Details::B: return 'B';
        case ValueType::Details::A: return 'A';
        case ValueType::Details::F: return 'F';
        case ValueType::Details::G: return 'G';
        case ValueType::Details::K: return 'K';
        case ValueType::Details::M: return 'M';
    }
}

ustring NormalStar::ValueType::SubToString(decltype(DetailedData.Sub) Data)
{
    if (Data == U16Npos) {return ustring();}
    uint32_t Intg = Data / 100ul;
    uint32_t Frac = Data % 100ul;
    if (!Frac) {return ustring(format("{}", Intg));}
    if (!(Frac % 10)) {Frac /= 10ul;}
    return ustring(format("{}.{}", Intg, Frac));
}

ustring NormalStar::ValueType::LumToString(decltype(DetailedData.Lum) Data, bool __2)
{
    ustring Result;
    switch (Data)
    {
        case ValueType::Details::hg: Result = __2 ? L"0" : L"_0"; break;
        case ValueType::Details::sg: Result = L"I"; break;
        case ValueType::Details::bg: Result = L"II"; break;
        case ValueType::Details::g: Result = L"III"; break;
        case ValueType::Details::subg: Result = L"IV"; break;
        case ValueType::Details::ms: Result = L"V"; break;
        default: break;
    }
    return Result;
}

ustring NormalStar::ValueType::SLumToString(decltype(DetailedData.SLum) Data)
{
    ustring Result;
    switch (Data)
    {
        case ValueType::Details::a: Result += L"a"; break;
        case ValueType::Details::b: Result += L"b"; break;
        case ValueType::Details::ab: Result += L"ab"; break;
        default: break;
    }
    return Result;
}

ustringlist NormalStar::PecularityType::SpectralPecularitiesTable
{
    L":", L"...", L"!", L"comp", L"[e]", L"er", L"eq", L"e*", L"e", L"f*",
    L"f+", L"f?", L"f", L"(f)", L"(f+)", L"((f))", L"((f*))", //"h", L"ha", L"E", L"L",
    L"Hewk", L"k", L"m", L"nn", L"neb", L"n", L"pq", L"p", L"q", L"(p)",
    L"ss", L"sh", L"s", L"var", L"wl", L"z",

    L"w", L"?", L"lB", L"v", L"lambdaBoo", L"HIwk", L"(n)", L"shell", L"[n]",
    L"pec", L"He-n", L"Si4200", L"mdD", L"kmet", L"Ssh", L"(m)", L"Hd", L"CaIIem",
    L"CaIwk", L"c", L"[Swk]", L"[S]", L"d", L"CaII", L"metalweak", L"NWk"
};

wregex NormalStar::PecularityType::Pattern(L'^' + __List_To_Pattern(PecularityType::SpectralPecularitiesTable));

const ustringlist NormalStar::ChemicalPecularitySpec::ChemicalElementsTable
{
    L"H",                                                                                                                                                                                                                    L"He",
    L"Li", L"Be",                                                                                                                                                                         L"B",  L"C",  L"N",  L"O",  L"F",  L"Ne",
    L"Na", L"Mg",                                                                                                                                                                         L"Al", L"Si", L"P",  L"S",  L"Cl", L"Ar",
    L"K",  L"Ca", L"Sc",                                                                                                   L"Ti", L"V",  L"Cr", L"Mn", L"Fe", L"Co", L"Ni", L"Cu", L"Zn", L"Ga", L"Ge", L"As", L"Se", L"Br", L"Kr",
    L"Rb", L"Sr", L"Y",                                                                                                    L"Zr", L"Nb", L"Mo", L"Tc", L"Ru", L"Rh", L"Pd", L"Ag", L"Cd", L"In", L"Sn", L"Sb", L"Te", L"I",  L"Xe",
    L"Cs", L"Ba", L"La", L"Ce", L"Pr", L"Nd", L"Pm", L"Sm", L"Eu", L"Gd", L"Tb", L"Dy", L"Ho", L"Er", L"Tm", L"Yb", L"Lu", L"Hf", L"Ta", L"W",  L"Re", L"Os", L"Ir", L"Pt", L"Au", L"Hg", L"Tl", L"Pb", L"Bi",// L"Po", L"At", L"Rn",
    //L"Fr", L"Ra", L"Ac", L"Th", L"Pa", L"U",  L"Np", L"Pu", L"Am", L"Cm", L"Bk", L"Cf", L"Es", L"Fm", L"Md", L"No", L"Lr", L"Rf", L"Db", L"Sg", L"Bh", L"Hs", L"Mt", L"Ds", L"Rg", L"Cn", L"Nh", L"Fl", L"Mc", L"Lv", L"Ts", L"Og" // 放射性元素不会出现
};

wregex NormalStar::ChemicalPecularitySpec::Pattern(L'^' + __List_To_Pattern(ChemicalPecularitySpec::ChemicalElementsTable));

ustringlist NormalStar::BandPecularitiesType::AbsorptionPecularityTable
{
    "Fe", "m", "CH", "Hdel", "Ba", "Ca", "CN", "HK", "Sr", "C2", "He", "MS", "K"
};

// 这个表达式原本可以不这么长的，但后续看到一例HD 12834的光谱型为G(6)IIICNIV/Vp，搞得不得不夹带一些私货
const std::string NormalStar::BandPecularitiesType::PatternSkeleton(
    R"(^({}(([+-]?[0-9](\.[1-9][1-9]?)?)|((V|IV|III|II|I)/(V|IV|III|II|I))|(I(ab|a|b)/((ab|a|b)|(V|IV|III|II|I)))|((V|IV|III|II|I)(ab|a|b)?))))");
std::string NormalStar::BandPecularitiesType::AbsorptionPecularityPatternString = __List_To_Pattern(AbsorptionPecularityTable).ToStdString();
wregex NormalStar::BandPecularitiesType::Pattern(ustring(vformat(BandPecularitiesType::PatternSkeleton, 
    make_format_args(AbsorptionPecularityPatternString))));

NormalStar::BandPecularitiesType NormalStar::BandPecularitiesType::Load(ustring Source)
{
    BandPecularitiesType AbsData;
    auto Result = std::find_if(AbsorptionPecularityTable.begin(),
        AbsorptionPecularityTable.end(), [Source](ustring S)
    {
        return Source.substr(0, S.size()) == S;
    });
    AbsData.Key = *Result;

    static const wregex NumReg(LR"([+-]?[0-9](\.[1-9][1-9]?)?)", regex_constants::optimize);
    ustring Val = Source;
    Val.erase(0, Result->size());
    if (regex_match(Val, NumReg))
    {
        wchar_t Sign;
        if (!::isdigit(Val[0]))
        {
            Sign = Val[0];
            Val.erase(0, 1);
        }
        auto DotPos = Val.find('.');
        int16_t Integer = 0, Fractional = 0, Final = 0;
        if (DotPos == Val.npos) {Integer = stoi(Val);}
        else
        {
            wchar_t* FracStart;
            Integer = ::wcstol(Val.c_str(), &FracStart, 10);
            Fractional = ::wcstol(FracStart + 1, nullptr, 10);
        }
        Final = (Sign == '-' ? -1 : 1) * (Integer * 100 + (Fractional >= 10 ? Fractional : Fractional * 10));
        AbsData.Value = Final;
    }
    else {AbsData.Value = Val;}

    return AbsData;
}

const NormalStar::EventQueueType NormalStar::ParserEventQueue
{
    LoadSpec, 
    LoadCarbonType, 
    LoadSub, 
    LoadSType,
    LoadLum, 
    LoadSubLum, 
    LoadPecularitiesStage1,
    LoadPecularitiesStage2, 
    LoadPecularitiesStage3
};

void NormalStar::SetState(ParserStateType* State, ParserStateType Value)
{
    *State = ParserStateType((*State & POpMask) | (Value & (~POpMask)));
}

void NormalStar::ConsumeSpace(ustring Source, ustring* Remain)
{
    auto StrippedRange = Source | std::views::drop_while([](wchar_t c) { return c == L'_'; });
    *Remain = ustring(StrippedRange.begin(), StrippedRange.end());
}

void NormalStar::LoadSpec(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Source1 = Source;
    UncertaintyHandler(Output, Source1, &Source1, State);
    ustring Spec = __Regex_Str_Str(Source1, ValueType::SpecClassPattern, &Source1);
    if (Spec.empty()) {return;}
    ValueType::LoadSpecImpl(Spec, &(Output->Data));
    if (*State & PBracket)
    {
        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SpecU;
        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::BrakIn));
        Output->Data.DetailedData.SpecU = Data;
    }
    //std::wcout << Spec << '\n';
    SetState(State, PSpec);

    UncertaintyHandler(Output, Source1, &Source1, State);
    if (*State & PRange)
    {
        if (!Output->FloatData.has_value()) {Output->FloatData = Output->Data;}
        ustring Spec2 = __Regex_Str_Str(Source1, ValueType::SpecClassPattern, &Source1);
        if (!Spec2.empty())
        {
            ValueType::LoadSpecImpl(Spec2, &(Output->FloatData.value()));
            //std::wcout << Spec2 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }
    }

    *Remain = Source1;
}

void NormalStar::LoadCarbonType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Spec = __Regex_Str_Str(Source, CarbonType::CarbonOrSTypePattern, Remain);
    if (Spec.empty()) {return;}
    Output->CData = CarbonType();
    switch (Spec[0])
    {
        case 'C': Output->CData->Data = CarbonType::C; break;
        case 'N': Output->CData->Data = CarbonType::N; break;
    }
    //std::wcout << Spec << '\n';
}

void NormalStar::LoadSub(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Source1 = Source;
    UncertaintyHandler(Output, Source1, &Source1, State);
    ustring Spec = __Regex_Str_Str(Source1, ValueType::NumberPattern, &Source1);
    if (Spec.empty()) {return;}
    if (*State & PBracket && ValueType::PecularitiesBrakStartPrecheck(&Output->Data, 1))
    {
        Output->Data.DetailedData.SubU = UncertaintyType::UncertaintySymbols(
            Output->Data.DetailedData.SubU | UncertaintyType::BrakStart);
    }
    uint16_t Result;
    ValueType::LoadNumber(Spec, &(Result));
    Output->Data.DetailedData.Sub = Result;
    if (*State & PBracket)
    {
        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SubU;
        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::BrakIn));
        Output->Data.DetailedData.SubU = Data;
    }
    //std::wcout << Spec << '\n';
    SetState(State, PSub);

    // 这里我也不知道为什么HD 126399的光谱K(3/5)III竟然能分析成功
    UncertaintyHandler(Output, Source1, &Source1, State);
    if (*State & PRange)
    {
        if (!Output->FloatData.has_value())
        {
            Output->FloatData = Output->Data;
            Output->FloatData->DetailedData.Spec = decltype(Output->FloatData->DetailedData.Spec)(0);
            Output->FloatData->DetailedData.Sub = ValueType::U16Npos;
        }
        
        ustring Spec2 = __Regex_Str_Str(Source1, ValueType::SpecClassPattern, &Source1);
        if (!Spec2.empty())
        {
            ValueType::LoadSpecImpl(Spec2, &(Output->FloatData.value()));
            //std::wcout << Spec2 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        ustring Spec3 = __Regex_Str_Str(Source1, ValueType::NumberPattern, &Source1);
        if (!Spec3.empty())
        {
            if (Spec2.empty()) 
            {
                auto Data = Output->Data.DetailedData.SubU;
                Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Simplified));
                Output->Data.DetailedData.SubU = Data;
            }
            uint16_t Result1;
            ValueType::LoadNumber(Spec3, &(Result1));
            Output->FloatData->DetailedData.Sub = Result1;
            //std::wcout << Spec3 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        UncertaintyHandler(Output, Source1, &Source1, State); // 保险起见在此加一句
    }

    *Remain = Source1;
}

void NormalStar::LoadSType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    if (Source[0] == 'S')
    {
        Output->CData = CarbonType();
        Output->CData->Data = CarbonType::S;
        //std::wcout << 'S' << '\n';
        *Remain = Source.erase(0, 1);
    }
}

void NormalStar::LoadLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Source1 = Source;
    UncertaintyHandler(Output, Source1, &Source1, State);
    ustring Spec = __Regex_Str_Str(Source1, ValueType::LuminosityClassPattern, &Source1);
    if (Spec.empty()) {return;}
    if (*State & PBracket && ValueType::PecularitiesBrakStartPrecheck(&Output->Data, 2))
    {
        Output->Data.DetailedData.LumU = UncertaintyType::UncertaintySymbols(
            Output->Data.DetailedData.LumU | UncertaintyType::BrakStart);
    }
    ValueType::LoadLumImpl(Spec, &(Output->Data));
    if (*State & PBracket)
    {
        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.LumU;
        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::BrakIn));
        Output->Data.DetailedData.LumU = Data;
    }
    //std::wcout << Spec << '\n';
    SetState(State, PLum);

    UncertaintyHandler(Output, Source1, &Source1, State);
    if (*State & PRange)
    {
        if (!Output->FloatData.has_value())
        {
            Output->FloatData = Output->Data;
            Output->FloatData->DetailedData.Lum = decltype(Output->FloatData->DetailedData.Lum)(0);
        }
        ustring Spec2 = __Regex_Str_Str(Source1, ValueType::LuminosityClassPattern2, &Source1);
        if (!Spec2.empty())
        {
            ValueType::LoadLumImpl(Spec2, &(Output->FloatData.value()));
            //std::wcout << Spec2 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        ustring Spec3 = __Regex_Str_Str(Source1, ValueType::SubLumPattern, &Source1);
        if (!Spec3.empty())
        {
            ValueType::LoadSubLumImpl(Spec3, &(Output->FloatData.value()));
            //std::wcout << Spec3 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        UncertaintyHandler(Output, Source1, &Source1, State); // 保险起见加上
    }

    *Remain = Source1;
}

void NormalStar::LoadSubLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Source1 = Source;
    UncertaintyHandler(Output, Source1, &Source1, State);
    ustring Spec = __Regex_Str_Str(Source1, ValueType::SubLumPattern, &Source1);
    if (Spec.empty()) {return;}
    if (*State & PBracket && ValueType::PecularitiesBrakStartPrecheck(&Output->Data, 3))
    {
        Output->Data.DetailedData.SLumU = UncertaintyType::UncertaintySymbols(
            Output->Data.DetailedData.SLumU | UncertaintyType::BrakStart);
    }
    ValueType::LoadSubLumImpl(Spec, &(Output->Data));
    if (*State & PBracket)
    {
        UncertaintyType::UncertaintySymbols Data = Output->Data.DetailedData.SLumU;
        Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::BrakIn));
        Output->Data.DetailedData.SLumU = Data;
    }
    //std::wcout << Spec << '\n';
    SetState(State, PSLum);

    UncertaintyHandler(Output, Source1, &Source1, State);
    if (*State & PRange)
    {
        if (!Output->FloatData.has_value())
        {
            Output->FloatData = Output->Data;
            Output->FloatData->DetailedData.Lum = decltype(Output->FloatData->DetailedData.Lum)(0);
            Output->FloatData->DetailedData.SLum = decltype(Output->FloatData->DetailedData.SLum)(0);
        }
        
        ustring Spec2 = __Regex_Str_Str(Source1, ValueType::LuminosityClassPattern2, &Source1);
        if (!Spec2.empty())
        {
            ValueType::LoadLumImpl(Spec2, &(Output->FloatData.value()));
            //std::wcout << Spec2 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        ustring Spec3 = __Regex_Str_Str(Source1, ValueType::SubLumPattern, &Source1);
        if (!Spec3.empty())
        {
            if (Spec2.empty()) 
            {
                auto Data = Output->Data.DetailedData.SLumU;
                Data = UncertaintyType::UncertaintySymbols(uint16_t(Data) | uint16_t(UncertaintyType::Simplified));
                Output->Data.DetailedData.SLumU = Data;
            }
            ValueType::LoadSubLumImpl(Spec3, &(Output->FloatData.value()));
            //std::wcout << Spec3 << '\n';
            *State = ParserStateType(int(*State) & (~int(PRange)));
        }

        UncertaintyHandler(Output, Source1, &Source1, State); // 这里不加一句，分析HD 144695的光谱O8/9.5(Ib/II)时会报错。
        // 补充：原以为范围和不确定符号不会同时存在，但后来发现一例HD 214714的光谱为G3Ib-II:CN-1CH2Fe-1
    }

    *Remain = Source1;
}

void NormalStar::LoadPecularities(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State, bool __2)
{
    ustring Src = Source;
    ConsumeSpace(Src, &Src);
    UncertaintyHandler(Output, Src, &Src, State);
    bool IsStartingBracket = (*State & PBracket) && 
        (Output->BandPecs.empty() ?
            ValueType::PecularitiesBrakStartPrecheck(&(Output->Data)) :
            !UncertaintyType::CheckBracketEnd(Output->BandPecs.back().Uncertainty));
    ustring Spec = __Regex_Str_Str(Src, PecularityType::Pattern, &Src);
    while (!Spec.empty())
    {
        if (__2 && !Output->PecularitiesBehindBandPecs) {Output->PecularitiesBehindBandPecs = 1;}
        Output->Pecularities.push_back({Spec});
        if ((Output->ChemicalElems.size() == 1) && (*State & PBracket))
        {
            Output->Pecularities.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->Pecularities.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakIn));
        }
        UncertaintyHandler(Output, Src, &Src, State);
        if (IsStartingBracket)
        {
            Output->Pecularities.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->Pecularities.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakStart));
            IsStartingBracket = 0;
        }
        if (*State & PBracket)
        {
            if (!UncertaintyType::CheckBracketEnd(Output->Pecularities.back().Uncertainty))
            {
                IsStartingBracket = 1;
            }
            else
            {
                Output->Pecularities.back().Uncertainty = 
                    UncertaintyType::UncertaintySymbols(uint16_t(Output->Pecularities.back().Uncertainty) 
                    | uint16_t(UncertaintyType::BrakIn));
            }
        }
        //std::wcout << Spec << '\n';
        ConsumeSpace(Src, &Src);
        UncertaintyHandler(Output, Src, &Src, State);
        Spec = __Regex_Str_Str(Src, PecularityType::Pattern, &Src);
    }
    *Remain = Src;
}

void NormalStar::LoadChemElems(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Src = Source;
    ConsumeSpace(Src, &Src);
    UncertaintyHandler(Output, Src, &Src, State);
    bool IsStartingBracket = (*State & PBracket) && 
        (Output->Pecularities.empty() ?
            ValueType::PecularitiesBrakStartPrecheck(&(Output->Data)) :
            !UncertaintyType::CheckBracketEnd(Output->Pecularities.back().Uncertainty));
    ustring Spec = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Src);
    while (!Spec.empty())
    {
        Output->ChemicalElems.push_back({Spec});
        if ((Output->ChemicalElems.size() == 1) && (*State & PBracket))
        {
            Output->ChemicalElems.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->ChemicalElems.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakIn));
        }
        UncertaintyHandler(Output, Src, &Src, State);
        if (IsStartingBracket)
        {
            Output->ChemicalElems.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->ChemicalElems.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakStart));
            IsStartingBracket = 0;
        }
        if (*State & PBracket)
        {
            if (!UncertaintyType::CheckBracketEnd(Output->ChemicalElems.back().Uncertainty))
            {
                IsStartingBracket = 1;
            }
            else
            {
                Output->ChemicalElems.back().Uncertainty = 
                    UncertaintyType::UncertaintySymbols(uint16_t(Output->ChemicalElems.back().Uncertainty) 
                    | uint16_t(UncertaintyType::BrakIn));
            }
        }
        //std::wcout << Spec << '\n';
        ConsumeSpace(Src, &Src);
        UncertaintyHandler(Output, Src, &Src, State);
        ustring Source1 = Src, Remain1;
        if (!__Regex_Str_Str(Source1, BandPecularitiesType::Pattern, &Remain1).empty()) {break;}
        Spec = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Src);
    }
    *Remain = Src;
}

void NormalStar::LoadBandPecs(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Src = Source;
    ConsumeSpace(Src, &Src);
    UncertaintyHandler(Output, Src, &Src, State);
    bool IsStartingBracketPreCheck = ValueType::PecularitiesBrakStartPrecheck(&(Output->Data));
    bool IsStartingBracket = (*State & PBracket) && 
        ((Output->Pecularities.empty() ? IsStartingBracketPreCheck :
            !UncertaintyType::CheckBracketEnd(Output->Pecularities.back().Uncertainty)) ||
        (Output->ChemicalElems.empty() ? IsStartingBracketPreCheck :
            !UncertaintyType::CheckBracketEnd(Output->ChemicalElems.back().Uncertainty)));
    ustring Spec = __Regex_Str_Str(Src, BandPecularitiesType::Pattern, &Src);
    while (!Spec.empty())
    {
        Output->BandPecs.push_back(BandPecularitiesType::Load(Spec));
        if ((Output->ChemicalElems.size() == 1) && (*State & PBracket))
        {
            Output->BandPecs.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->BandPecs.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakIn));
        }
        UncertaintyHandler(Output, Src, &Src, State);
        if (IsStartingBracket)
        {
            Output->BandPecs.back().Uncertainty = 
                UncertaintyType::UncertaintySymbols(uint16_t(Output->BandPecs.back().Uncertainty) 
                | uint16_t(UncertaintyType::BrakStart));
            IsStartingBracket = 0;
        }
        if (*State & PBracket)
        {
            if (!UncertaintyType::CheckBracketEnd(Output->BandPecs.back().Uncertainty))
            {
                IsStartingBracket = 1;
            }
            else
            {
                Output->BandPecs.back().Uncertainty = 
                    UncertaintyType::UncertaintySymbols(uint16_t(Output->BandPecs.back().Uncertainty) 
                    | uint16_t(UncertaintyType::BrakIn));
            }
        }
        ConsumeSpace(Src, &Src);
        //std::wcout << Spec << '\n';
        UncertaintyHandler(Output, Src, &Src, State);
        Spec = __Regex_Str_Str(Src, BandPecularitiesType::Pattern, &Src);
    }
    *Remain = Src;
}

void NormalStar::LoadPecularitiesStage1(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    SetState(State, PPec);
    LoadPecularities(Output, Source, Remain, State);
}

void NormalStar::LoadPecularitiesStage2(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    ustring Source1 = Source;
    ustring Remain1;
    ustring TestStr = __Regex_Str_Str(ustring(Source1.back() == '(' ? Source1.erase(0, 1) : Source1), 
        BandPecularitiesType::Pattern, &Remain1);
    if (TestStr.empty())
    {
        SetState(State, PChem);
        LoadChemElems(Output, Source, Remain, State);
    }
    else 
    {
        SetState(State, PBar);
        LoadBandPecs(Output, Source, Remain, State);
    }
}

void NormalStar::LoadPecularitiesStage3(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    if ((*State & ~(POpMask)) == PBar && Output->Pecularities.empty()) // 特殊谱线若出现在前面，后续就不会出现了
    {
        SetState(State, PPec);
        LoadPecularities(Output, Source, Remain, State, 1);
    }
    else if ((*State & ~(POpMask)) != PBar)
    {
        SetState(State, PBar);
        LoadBandPecs(Output, Source, Remain, State);
    }
}

std::shared_ptr<StellarClassData> NormalStar::ParseFunc(ustring StelClassString)
{
    NormalStar Result;
    ustring Remain = StelClassString;
    ParserStateType PrevState = PStart;

    for (uint64 i = 0; (i < ParserEventQueue.size()) && (!Remain.empty()); ++i)
    {
        ParserEventQueue[i](&Result, Remain, &Remain, &PrevState);
    }

    Result.RemainString = Remain;
    //std::wcout << Remain << '\n';
    return std::make_shared<NormalStar>(Result);
}

const NormalStar::ValueType& NormalStar::GetRawDataByIndex(size_t Index)const
{
    return Index ? (FloatData.has_value() ? FloatData.value() : Data) : Data;
}

char NormalStar::SpectralClass(size_t Index)const
{
    auto& IData = GetRawDataByIndex(Index);
    auto Spec = Index && IData.DetailedData.Spec ? IData.DetailedData.Spec : Data.DetailedData.Spec;
    return ValueType::SpecToChar(Spec);
}

uint16_t NormalStar::SpectralClassUncertained(size_t Index)const
{
    return Data.DetailedData.SpecU;
}

uint16_t NormalStar::SpecializedClass(size_t Index)const
{
    auto& IData = GetRawDataByIndex(Index);
    return Index && ~(IData.DetailedData.Sub) ? IData.DetailedData.Sub : Data.DetailedData.Sub;
}

uint16_t NormalStar::SpecializedClassUncertained(size_t Index)const
{
    return Data.DetailedData.SubU;
}

ustring NormalStar::LuminosityClass(size_t Index)const
{
    auto& IData = GetRawDataByIndex(Index);
    ustring Result;
    Result.append(ValueType::LumToString(IData.DetailedData.Lum));
    Result.append(ValueType::SLumToString(IData.DetailedData.SLum));
    if (Result.empty())
    {
        Result.append(ValueType::LumToString(Data.DetailedData.Lum));
        Result.append(ValueType::SLumToString(Data.DetailedData.SLum));
    }
    return Result;
}

uint16_t NormalStar::LuminosityClassUncertained(size_t Index)const
{
    return Data.DetailedData.LumU == UncertaintyType::None ? 
        Data.DetailedData.SLumU : Data.DetailedData.LumU;
}

std::set<ustring> NormalStar::SpectralPeculiarities()const
{
    std::set<ustring> Pec;
    for (const auto& i : Pecularities)
    {
        Pec.insert(i.Element);
    }
    return Pec;
}

std::set<ustring> NormalStar::ElementSymbols()const
{
    std::set<ustring> Chem;
    for (const auto& i : ChemicalElems)
    {
        Chem.insert(i.Element);
    }
    return Chem;
}

std::map<ustring, std::variant<int16_t, ustring>> NormalStar::BandPecularities()const
{
    std::map<ustring, std::variant<int16_t, ustring>> Bands;
    for (const auto& i : BandPecs)
    {
        Bands.insert({i.Key, i.Value});
    }
    return Bands;
}

ustring NormalStar::UnanalyzedString()const
{
    return RemainString;
}

ustring NormalStar::ExportSpec()const
{
    auto ST = ValueType::SpecToChar(Data.DetailedData.Spec);
    if (!ST) {return ustring();}
    ustring Result;
    Result.push_back(ST);
    if (CData.has_value())
    {
        switch (CData->Data)
        {
            case CarbonType::C: Result.push_back('C'); break;
            case CarbonType::N: Result.push_back('N'); break;
            default: break;
        }
    }
    if (!FloatData.has_value()) {UncertaintyType::AddUncertainty(&Result, Data.DetailedData.SpecU);}
    return Result;
}

ustring NormalStar::ExportSub()const
{
    ustring Result = ValueType::SubToString(Data.DetailedData.Sub);
    if (FloatData.has_value()) {Result += ExportSpecSubRange();}
    if (!FloatData.has_value() || (FloatData.has_value() && 
        ((Data.DetailedData.SubU & UncertaintyType::BrakIn) || 
        (FloatData->DetailedData.Sub == ValueType::U16Npos) ||
        (Data.DetailedData.Sub == FloatData->DetailedData.Sub))))
    {
        UncertaintyType::AddUncertainty(&Result, Data.DetailedData.SubU);
    }
    if (CData.has_value() && CData->Data == CarbonType::S)
    {
        Result.push_back('S');
    }
    return Result;
}

ustring NormalStar::ExportSpecSubRange()const
{
    if (!FloatData.has_value() || (FloatData.has_value() && 
        ((!FloatData->DetailedData.Spec && (FloatData->DetailedData.Sub == ValueType::U16Npos)) ||
        (FloatData->DetailedData.Spec == Data.DetailedData.Spec &&
        FloatData->DetailedData.Sub == Data.DetailedData.Sub)))) 
    {
        return ustring();
    }
    ustring Result;
    if (Data.DetailedData.SpecU & UncertaintyType::Slash || 
        Data.DetailedData.SubU & UncertaintyType::Slash)
    {
        Result.push_back('/');
    }
    else {Result.push_back('-');}

    if (!(Data.DetailedData.SubU & UncertaintyType::Simplified) || 
        FloatData->DetailedData.Spec != Data.DetailedData.Spec)
    {
        Result.push_back(ValueType::SpecToChar(FloatData->DetailedData.Spec));
    }
    
    Result.append(ValueType::SubToString(FloatData->DetailedData.Sub));

    return Result;
}

ustring NormalStar::ExportLum()const
{
    // 此处逻辑过于复杂，不得不转成一个大型分类讨论
    // 以下为两个操作分支：
    auto Op1 = [this]()
    {
        ustring Result;
        Result = ValueType::LumToString(Data.DetailedData.Lum);
        UncertaintyType::AddUncertainty(&Result, Data.DetailedData.LumU);
        if (Data.DetailedData.SLum)
        {
            ustring Result2 = ValueType::SLumToString(Data.DetailedData.SLum);
            UncertaintyType::AddUncertainty(&Result2, Data.DetailedData.SLumU);
            Result += Result2;
        }
        return Result;
    };
    
    auto Op2 = [this]() // 这种情况下若出现"()"，只会出现整个在内部的情况
    {
        ustring Result;
        UncertaintyType::UncertaintySymbols LU = 
            UncertaintyType::UncertaintySymbols(Data.DetailedData.LumU | Data.DetailedData.SLumU);
        Result = ValueType::LumToString(Data.DetailedData.Lum);
        if (Data.DetailedData.SLum)
        {
            Result += ValueType::SLumToString(Data.DetailedData.SLum);
        }
        Result += ExportLumRange();
        if (LU)
        {
            UncertaintyType::AddUncertainty(&Result, LU);
        }
        return Result;
    };

    if (!FloatData.has_value()) {return Op1();}
    else
    {
        if (!FloatData->DetailedData.Lum || (FloatData->DetailedData.Lum == Data.DetailedData.Lum))
        {
            if (FloatData->DetailedData.SLum && 
                (FloatData->DetailedData.SLum != Data.DetailedData.SLum))
            {
                return Op2();
            }
            else {return Op1();}
        }
        else {return Op2();}
    }
}

ustring NormalStar::ExportLumRange()const
{
    if (!FloatData.has_value() || (FloatData.has_value() && 
        ((!FloatData->DetailedData.Lum && !FloatData->DetailedData.SLum) || 
        (FloatData->DetailedData.Lum == Data.DetailedData.Lum &&
        FloatData->DetailedData.SLum == Data.DetailedData.SLum)))) 
    {
        return ustring();
    }
    ustring Result;
    if (Data.DetailedData.LumU & UncertaintyType::Slash || 
        Data.DetailedData.SLumU & UncertaintyType::Slash)
    {
        Result.push_back('/');
    }
    else {Result.push_back('-');}

    if (!(Data.DetailedData.SLumU & UncertaintyType::Simplified) || 
        FloatData->DetailedData.Lum != Data.DetailedData.Lum)
    {
        Result.append(ValueType::LumToString(FloatData->DetailedData.Lum, 1));
    }

    Result.append(ValueType::SLumToString(FloatData->DetailedData.SLum));

    return Result;
}

ustring NormalStar::ExportPec()const
{
    ustring Result;
    for (const auto& i : Pecularities)
    {
        ustring Pec = i.Element;
        UncertaintyType::AddUncertainty(&Pec, i.Uncertainty);
        Result.append(Pec);
    }
    return Result;
}

ustring NormalStar::ExportChem()const
{
    ustring Result;
    for (const auto& i : ChemicalElems)
    {
        ustring Pec = i.Element;
        UncertaintyType::AddUncertainty(&Pec, i.Uncertainty);
        Result.append(Pec);
    }
    return Result;
}

ustring NormalStar::ExportBand()const
{
    ustring Result;
    for (const auto& i : BandPecs)
    {
        ustring Pec = i.Key;
        if (std::holds_alternative<int16_t>(i.Value))
        {
            int16_t IValue = std::get<int16_t>(i.Value);
            Pec.append(IValue < 0 ? L"-" : L"");
            Pec.append(ValueType::SubToString(abs(int64(IValue))));
        }
        else {Pec.append(std::get<ustring>(i.Value));}
        UncertaintyType::AddUncertainty(&Pec, i.Uncertainty);
        Result.append(Pec);
    }
    return Result;
}

void NormalStar::ExportFullPecString(ustring* MainString)const
{
    auto CountBrackets = [](ustring s)
    {
        auto Left = std::count(s.begin(), s.end(), L'(');
        auto Right = std::count(s.begin(), s.end(), L')');
        return Left - Right;
    };
    
    ustring PecString = ExportPec();
    ustring ChemString = ExportChem();
    ustring BandString = ExportBand();

    ustring Combination1 = *MainString + PecString + ChemString + BandString;
    ustring Combination2 = *MainString + BandString + PecString;

    if (PecularitiesBehindBandPecs) {*MainString = Combination2;}
    else {*MainString = Combination1;}
}

ustring NormalStar::ToString()const
{
    ustring Result;
    Result.append(ExportSpec());
    Result.append(ExportSub());
    Result.append(ExportLum());
    ExportFullPecString(&Result);
    return Result;
}



// ----------------------------------------- AmStar ----------------------------------------- //

ustring AmStar::Description()const
{
    static const ustring Descr = L"Am Star";
    return Descr;
}

std::flat_map<ustring, AmStar::KeyType> AmStar::LinesTable
{
    {L"k", {.Line = "k", .Element = "Ca+", .WaveLength = 393.366}}, // 1价钙离子K段
    {L"h", {.Line = "h", .Element = "Hδ", .WaveLength = 410.175}}, // 氢巴耳末系，以氢δ线为例
    {L"g", {.Line = "g", .Element = "CH", .WaveLength = 430.774}}, // 次甲基G段（如HD 48565的光谱为hF8gF8mF3）
    {L"m", {.Line = "m", .Element = "Fe", .WaveLength = 495.761}}, // 金属线，以铁c线为例
    {L"He", {.Line = "He", .Element = "He", .WaveLength = 587.5618}} // 氦线，以氦D3线为例（如青丘一的光谱为kB8hB8HeA0VSi）
};

wregex AmStar::LinesPattern(L'^' + __List_To_Pattern(LinesTable.keys()));

const AmStar::EventQueueType AmStar::ParserEventQueue
{
    {PLine, LoadLine},
    {PSpec, LoadSpec},
    {PSub, LoadSub},
    {PLum, LoadLum},
    {PSLum, LoadSLum},
    {PPec, LoadPec},
    {PChem, LoadChem},
};

void AmStar::SetState(ParserStateType* State, ParserStateType Value)
{
    *State = ParserStateType((*State & POpMask) | (Value & (~POpMask)));
}

void AmStar::LoadLine(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    bool IsBracketStart = 0;
    if (!Source.empty() && !(*State & PBracket) && Source.front() == '(')
    {
        IsBracketStart = 1;
        *State = ParserStateType(*State | PBracket);
        Source.erase(Source.begin());
    }
    
    ustring Key = __Regex_Str_Str(Source, LinesPattern, Remain);
    if (Key.empty())
    {
        *State = PEnd;
        return;
    }

    if (Output->FmtFlag == DeltaDelphini)
    {
        DDelValueType RData = std::get<DeltaDelphini>(Output->Data);
        auto LineKey = LinesTable.at(Key);

        if (*State & PBracket)
        {
            if (IsBracketStart) 
            {
                LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                    LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakStart);
                IsBracketStart = 0;
            }
            else
            {
                LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                    LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakIn);
            }
        }

        if (!Remain->empty() && (*State & PBracket) && Remain->front() == ')')
        {
            LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakEnd);
            *State = ParserStateType(*State & (~PBracket));
            Remain->erase(Remain->begin());
        }

        RData.Segments.insert({LineKey, AmStar::ValueType()});
        Output->Data = RData;
    }
    else if (Output->FmtFlag == RhoPuppis)
    {
        RPupValueType RData = std::get<RhoPuppis>(Output->Data);
        auto LineKey = LinesTable.at(Key);

        if (*State & PBracket)
        {
            if (IsBracketStart) 
            {
                LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                    LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakStart);
                IsBracketStart = 0;
            }
            else
            {
                LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                    LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakIn);
            }
        }

        if (!Remain->empty() && (*State & PBracket) && Remain->front() == ')')
        {
            LineKey.UncertaintyFlags = ImportBase::UncertaintyType::UncertaintySymbols(
                LineKey.UncertaintyFlags | ImportBase::UncertaintyType::BrakEnd);
            *State = ParserStateType(*State & (~PBracket));
            Remain->erase(Remain->begin());
        }

        RData.Segments.insert({LineKey, AmStar::ValueType()});
        Output->Data = RData;
    }

    //std::wcout << Key << '\n';
    SetState(State, PSpec);
    *Line = Key;
}

void AmStar::LoadSpec(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    ustring Spec = __Regex_Str_Str(Source, ImportBase::ValueType::SpecClassPattern, Remain);
    if (Spec.empty())
    {
        *State = PEnd;
        return;
    }
    auto RLine = Output->FmtFlag == KappaOctantis ? L"m2" : *Line;
    auto RData = Output->GetDataByLine(RLine);
    ImportBase::ValueType::LoadSpecImpl(Spec, &RData.Value);
    Output->SetDataByLine(RLine, RData);
    //std::wcout << Spec << '\n';
    SetState(State, PSub);
}

void AmStar::LoadSub(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    bool IsBracketStart = 0;
    if (!Source.empty() && !(*State & PBracket) && Source.front() == '(')
    {
        IsBracketStart = 1;
        *State = ParserStateType(*State | PBracket);
        Source.erase(Source.begin());
    }

    ustring Sub = __Regex_Str_Str(Source, ImportBase::ValueType::NumberPattern, Remain);
    if (Sub.empty())
    {
        *State = PEnd;
        return;
    }
    auto RLine = Output->FmtFlag == KappaOctantis ? L"m2" : *Line;
    auto RData = Output->GetDataByLine(RLine);
    if (*State & PBracket)
    {
        if (IsBracketStart) 
        {
            RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::BrakStart);
            IsBracketStart = 0;
        }
        else
        {
            RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::BrakIn);
        }
    }
    ImportBase::ValueType::LoadNumber(Sub, &RData.Value.DetailedData.Sub);

    bool NextTransferToKOct = 0;
    if (!Remain->empty() && (Remain->front() == '+' || Remain->front() == '-' || Remain->front() == ':'))
    {
        switch (Remain->front())
        {
        case '+':
            RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::More);
            break;
        case '-':
            if (Remain->size() >= 3 && ustring(L"OBAFGKM").find((*Remain)[1]) != ustring::npos && isdigit((*Remain)[2]))
            {
                NextTransferToKOct = 1;
            }
            else
            {
                RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
                    RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::Less);
            }
            break;
        case ':':
            RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::Uncertained);
            break;
        }
        Remain->erase(Remain->begin());
    }
    else if (!Remain->empty() && Remain->front() == L'/') // 此处不使用单独的处理函数统一管理了，只会引入一堆不必要的状态量增加复杂度
    {
        RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
            RData.Value.DetailedData.SubU | 
                (ImportBase::UncertaintyType::Ranged | 
                ImportBase::UncertaintyType::Slash | 
                ImportBase::UncertaintyType::Simplified));
        if (!RData.FloatValue.has_value()) {RData.FloatValue = RData.Value;}
        Remain->erase(Remain->begin());

        if (ustring(L"OBAFGKM").find(Remain->front()) != ustring::npos)
        {
            ustring Spec1 = __Regex_Str_Str((*Remain), ImportBase::ValueType::SpecClassPattern, Remain);
            ImportBase::ValueType::LoadSpecImpl(Spec1, &RData.FloatValue.value());
            //std::wcout << Spec1 << '\n';
        }
        if (isdigit(Remain->front()))
        {
            ustring Sub1 = __Regex_Str_Str((*Remain), ImportBase::ValueType::NumberPattern, Remain);
            ImportBase::ValueType::LoadNumber(Sub1, &RData.FloatValue->DetailedData.Sub);
            //std::wcout << Sub1 << '\n';
        }
    }

    if (!Remain->empty() && (*State & PBracket) && Remain->front() == ')')
    {
        RData.Value.DetailedData.SubU = ImportBase::UncertaintyType::UncertaintySymbols(
            RData.Value.DetailedData.SubU | ImportBase::UncertaintyType::BrakEnd);
        *State = ParserStateType(*State & (~PBracket));
        Remain->erase(Remain->begin());
    }

    Output->SetDataByLine(RLine, RData);
    //std::wcout << Sub << '\n';

    ustring Src1 = *Remain, Rem1;
    if (!Src1.empty() && Src1.front() == '(') {Src1.erase(Src1.begin());}
    if (Output->FmtFlag == RhoPuppis && NextTransferToKOct)
    {
        TransferToKOct(Output, State);
        SetState(State, PSpec);
    }
    else if (!__Regex_Str_Str(Src1, ImportBase::ValueType::LuminosityClassPattern, &Rem1).empty())
    {
        SetState(State, PLum);
    }
    else {NextSegmentCheck(Output, Src1, State);}
}

void AmStar::LoadLum(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    bool IsBracketStart = 0;
    if (!Source.empty() && !(*State & PBracket) && Source.front() == '(')
    {
        IsBracketStart = 1;
        *State = ParserStateType(*State | PBracket);
        Source.erase(Source.begin());
    }

    ustring Lum = __Regex_Str_Str(Source, ImportBase::ValueType::LuminosityClassPattern, Remain);
    if (Lum.empty())
    {
        *State = PEnd;
        return;
    }
    auto RData = Output->GetDataByLine(*Line);
    if (*State & PBracket)
    {
        if (IsBracketStart) 
        {
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::BrakStart);
            IsBracketStart = 0;
        }
        else
        {
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::BrakIn);
        }
    }
    ImportBase::ValueType::LoadLumImpl(Lum, &RData.Value);
    //std::wcout << Lum << '\n';

    if (!Remain->empty() && (Remain->front() == '+' || Remain->front() == '-' || Remain->front() == ':' || Remain->front() == L'/'))
    {
        switch (Remain->front())
        {
        case '+':
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::More);
            break;
        case '-':
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::Less);
            break;
        case ':':
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::Uncertained);
            break;
        case '/':
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::Ranged | 
                ImportBase::UncertaintyType::Slash | ImportBase::UncertaintyType::Simplified);
            break;
        }
        Remain->erase(Remain->begin());
    }

    ustring Lum1 = __Regex_Str_Str(*Remain, ImportBase::ValueType::LuminosityClassPattern, Remain);
    if (!Lum1.empty())
    {
        if (!(RData.Value.DetailedData.LumU & ImportBase::UncertaintyType::Ranged))
        {
            // 进到这里时若Ranged没有被拉起，说明是'-'后面出现了另一个光度级
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU & (~ImportBase::UncertaintyType::Less));
            RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
                RData.Value.DetailedData.LumU | 
                    (ImportBase::UncertaintyType::Ranged | 
                    ImportBase::UncertaintyType::Simplified));
        }

        if (!RData.FloatValue.has_value()) {RData.FloatValue = RData.Value;}
        ImportBase::ValueType::LoadLumImpl(Lum1, &RData.FloatValue.value());
        //std::wcout << Lum1 << '\n';
    }

    if (!Remain->empty() && (*State & PBracket) && Remain->front() == ')')
    {
        RData.Value.DetailedData.LumU = ImportBase::UncertaintyType::UncertaintySymbols(
            RData.Value.DetailedData.LumU | ImportBase::UncertaintyType::BrakEnd);
        *State = ParserStateType(*State & (~PBracket));
        Remain->erase(Remain->begin());
    }

    Output->SetDataByLine(*Line, RData);

    ustring Src1 = *Remain, Rem1;
    if (!Src1.empty() && Src1.front() == '(') {Src1.erase(Src1.begin());}
    if (!__Regex_Str_Str(Src1, ImportBase::ValueType::SubLumPattern, &Rem1).empty())
    {
        SetState(State, PSLum);
    }
    else {NextSegmentCheck(Output, Src1, State);}
}

void AmStar::LoadSLum(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    ustring SLum = __Regex_Str_Str(Source, ImportBase::ValueType::SubLumPattern, Remain);
    if (SLum.empty())
    {
        *State = PEnd;
        return;
    }
    auto RData = Output->GetDataByLine(*Line);
    ImportBase::ValueType::LoadSubLumImpl(SLum, &RData.Value);
    Output->SetDataByLine(*Line, RData);
    //std::wcout << SLum << '\n';

    ustring Src1 = *Remain;
    if (!Src1.empty() && Src1.front() == '(') {Src1.erase(Src1.begin());}
    NextSegmentCheck(Output, Src1, State);
}

void AmStar::LoadPec(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    ustring Src = Source;
    bool IsBracketStart = 0;
    if (!Src.empty() && Src.front() == '(') 
    {
        ustring Tmp;
        ustring Spec1 = __Regex_Str_Str(Src, PecularityType::Pattern, &Tmp);
        if (Spec1.empty())
        {
            IsBracketStart = 1;
            *State = ParserStateType(*State | PBracket);
            Src.erase(Src.begin());
        }
    }

    ustring Spec = __Regex_Str_Str(Src, PecularityType::Pattern, &Src);
    while (!Spec.empty())
    {
        Output->Pecularities.push_back({Spec});
        //std::wcout << Spec << '\n';
        if (*State & PBracket)
        {
            if (IsBracketStart) 
            {
                Output->Pecularities.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                    Output->Pecularities.back().Uncertainty | ImportBase::UncertaintyType::BrakStart);
                IsBracketStart = 0;
            }
            else
            {
                Output->Pecularities.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                    Output->Pecularities.back().Uncertainty | ImportBase::UncertaintyType::BrakIn);
            }
        }
        if (!Src.empty() && (*State & PBracket) && Src.front() == ')')
        {
            Output->Pecularities.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                Output->Pecularities.back().Uncertainty | ImportBase::UncertaintyType::BrakEnd);
            *State = ParserStateType(*State & (~PBracket));
            Src.erase(Src.begin());
        }

        if (!Src.empty() && Src.front() == '(') 
        {
            ustring Tmp;
            ustring Spec1 = __Regex_Str_Str(Src, PecularityType::Pattern, &Tmp);
            if (Spec1.empty())
            {
                IsBracketStart = 1;
                *State = ParserStateType(*State | PBracket);
                Src.erase(Src.begin());
            }
        }

        if (__ChkSeg(Src)) {break;}
        
        Spec = __Regex_Str_Str(Src, PecularityType::Pattern, &Src);
    }
    *Remain = Src;
    
    ustring Src1 = *Remain;
    if (!Src1.empty() && Src1.front() == '(') {Src1.erase(Src1.begin());}
    NextSegmentCheck(Output, Src, State);
}

void AmStar::LoadChem(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State, ustring* Line)
{
    ustring Src = Source;
    bool IsBracketStart = 0;
    if (!Src.empty() && Src.front() == '(') 
    {
        ustring Tmp;
        ustring Spec1 = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Tmp);
        if (Spec1.empty())
        {
            IsBracketStart = 1;
            *State = ParserStateType(*State | PBracket);
            Src.erase(Src.begin());
        }
    }

    ustring Spec = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Src);
    while (!Spec.empty())
    {
        Output->ChemElems.push_back({Spec});
        //std::wcout << Spec << '\n';
        if (*State & PBracket)
        {
            if (IsBracketStart) 
            {
                Output->ChemElems.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                    Output->ChemElems.back().Uncertainty | ImportBase::UncertaintyType::BrakStart);
                IsBracketStart = 0;
            }
            else
            {
                Output->ChemElems.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                    Output->ChemElems.back().Uncertainty | ImportBase::UncertaintyType::BrakIn);
            }
        }
        if (!Src.empty() && Src.front() == ':')
        {
            Output->ChemElems.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                Output->ChemElems.back().Uncertainty | ImportBase::UncertaintyType::Uncertained);
            Src.erase(Src.begin());
        }
        if (!Src.empty() && (*State & PBracket) && Src.front() == ')')
        {
            Output->ChemElems.back().Uncertainty = ImportBase::UncertaintyType::UncertaintySymbols(
                Output->ChemElems.back().Uncertainty | ImportBase::UncertaintyType::BrakEnd);
            *State = ParserStateType(*State & (~PBracket));
            Src.erase(Src.begin());
        }

        if (!Src.empty() && Src.front() == '(') 
        {
            ustring Tmp;
            ustring Spec1 = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Tmp);
            if (Spec1.empty())
            {
                IsBracketStart = 1;
                *State = ParserStateType(*State | PBracket);
                Src.erase(Src.begin());
            }
        }
        Spec = __Regex_Str_Str(Src, ChemicalPecularitySpec::Pattern, &Src);
    }
    *Remain = Src;

    SetState(State, PLine);
}

bool AmStar::__ChkSeg(ustring Src1)
{
    auto S = Src1 | std::views::filter([](char c)  // '('和')'会干扰识别，先把它们去掉
    {
        return c != '(' && c != ')';
    });
    ustring Rem1(S.begin(), S.end());
    return !__Regex_Str_Str(Rem1, LinesPattern, &Rem1).empty() && 
        !__Regex_Str_Str(Rem1, ImportBase::ValueType::SpecClassPattern, &Rem1).empty() &&
        !__Regex_Str_Str(Rem1, ImportBase::ValueType::NumberPattern, &Rem1).empty();
}

void AmStar::NextSegmentCheck(AmStar* Output, ustring Source, ParserStateType* State)
{
    ustring Src1 = Source, Rem1;
    if (__ChkSeg(Src1))
    {
        SetState(State, PLine);
        return;
    }
    if (!__Regex_Str_Str(Src1, ImportBase::PecularityType::Pattern, &Rem1).empty())
    {
        SetState(State, PPec);
        return;
    }
    if (!__Regex_Str_Str(Src1, ImportBase::ChemicalPecularitySpec::Pattern, &Rem1).empty())
    {
        SetState(State, PChem);
        return;
    }
    *State = PEnd;
}

void AmStar::DdelRoutine(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    Output->Data = DDelValueType();
    ustring Source0 = Source;
    *State = PLine;
    Output->FmtFlag = DeltaDelphini;
    ustring CurrentLine;
    while (*State != -1)
    {
        ParserEventQueue.at(ParserStateType((*State) & (~POpMask)))(Output, Source0, &Source0, State, &CurrentLine);
    }
    *Remain = Source0;
}

void AmStar::RPupRoutine(AmStar* Output, ustring Source, ustring* Remain, ParserStateType* State)
{
    Output->Data = RPupValueType();
    ustring Source0 = Source;
    *State = PSpec;
    Output->FmtFlag = RhoPuppis;
    ustring CurrentLine;
    while (*State != -1)
    {
        ParserEventQueue.at(ParserStateType((*State) & (~POpMask)))(Output, Source0, &Source0, State, &CurrentLine);
    }
    *Remain = Source0;
}

void AmStar::TransferToKOct(AmStar* Output, ParserStateType* State)
{
    Output->FmtFlag = KappaOctantis;
    auto OldValue = std::get<RPupValueType>(Output->Data);
    KOctValueType NewValue;
    NewValue.MainType = OldValue.MainSegment;
    NewValue.LineUncertaintyFlags = OldValue.Segments.rbegin()->first.UncertaintyFlags;
    NewValue.MetallicType1 = OldValue.Segments.rbegin()->second; // 此处不会出现除“m”以外的其他线
    Output->Data = NewValue;
}

std::shared_ptr<StellarClassData> AmStar::ParseFunc(ustring StelClassString)
{
    ParserStateType State = PStart;
    AmStar Result;
    ustring Remain;
    if (!__Regex_Str_Str(StelClassString, LinesPattern).empty())
    {
        DdelRoutine(&Result, StelClassString, &Remain, &State);
    }
    else if (!__Regex_Str_Str(StelClassString, ImportBase::ValueType::SpecClassPattern).empty())
    {
        RPupRoutine(&Result, StelClassString, &Remain, &State);
    }
    Result.RemainString = Remain;
    //std::wcout << Remain << '\n';
    return std::make_shared<AmStar>(Result);
}

AmStar::ValueType AmStar::GetDataByLine(ustring Line)
{
    if (FmtFlag == DeltaDelphini)
    {
        DDelValueType RData = std::get<DeltaDelphini>(Data);
        using IType = decltype(RData.Segments.begin())::value_type;
        auto it = std::find_if(RData.Segments.begin(), RData.Segments.end(), 
            [Line](IType Target)
        {
            return Target.first.Line == Line;
        });
        if (it == RData.Segments.end()) {it = RData.Segments.begin();}
        return it->second;
    }
    else if (FmtFlag == RhoPuppis)
    {
        RPupValueType RData = std::get<RhoPuppis>(Data);
        if (Line.empty()) {return RData.MainSegment;}
        using IType = decltype(RData.Segments.begin())::value_type;
        auto it = std::find_if(RData.Segments.begin(), RData.Segments.end(), 
            [Line](IType Target)
        {
            return Target.first.Line == Line;
        });
        if (it == RData.Segments.end()) {return RData.MainSegment;}
        return it->second;
    }
    else if (FmtFlag == KappaOctantis)
    {
        KOctValueType RData = std::get<KappaOctantis>(Data);
        if (Line.empty()) {return RData.MainType;}
        if (Line == "m") {return RData.MetallicType1;}
        if (Line == "m2") {return RData.MetallicType2;}
    }
}

void AmStar::SetDataByLine(ustring Line, ValueType NewValue)
{
    if (FmtFlag == DeltaDelphini)
    {
        DDelValueType RData = std::get<DeltaDelphini>(Data);
        using IType = decltype(RData.Segments.begin())::value_type;
        auto Ptr = std::find_if(RData.Segments.begin(), RData.Segments.end(), 
            [Line](IType Target)
        {
            return Target.first.Line == Line;
        });
        Ptr->second = NewValue;
        Data = RData;
    }
    else if (FmtFlag == RhoPuppis)
    {
        RPupValueType RData = std::get<RhoPuppis>(Data);
        if (Line.empty()) {RData.MainSegment = NewValue;}
        else 
        {
            using IType = decltype(RData.Segments.begin())::value_type;
            auto Ptr = std::find_if(RData.Segments.begin(), RData.Segments.end(), 
                [Line](IType Target)
            {
                return Target.first.Line == Line;
            });
            Ptr->second = NewValue;
        }
        Data = RData;
    }
    else if (FmtFlag == KappaOctantis)
    {
        // 此状态的前两个段与弧矢增卅二共用解析流程，仅当字符串分析到“-”的时候
        // 才会切换到此类型（Am恒星的光谱型中表范围的不会出现“-”字符，只会出现
        // “/”字符），此时已是最后一个光谱段，存储到MetallicType2即可
        KOctValueType RData = std::get<KOctValueType>(Data);
        if (Line.empty()) {RData.MainType = NewValue;}
        else if (Line == L"m") {RData.MetallicType1 = NewValue;}
        else if (Line == L"m2") {RData.MetallicType2 = NewValue;}
        Data = RData;
    }
}

ustring AmStar::ExportSpec(const ValueType& Value)
{
    auto ST = ImportBase::ValueType::SpecToChar(Value.Value.DetailedData.Spec);
    if (!ST) {return ustring();}
    ustring Result;
    Result.push_back(ST);
    if (!Value.FloatValue.has_value()) 
    {
        ImportBase::UncertaintyType::AddUncertainty(&Result, Value.Value.DetailedData.SpecU);
    }
    return Result;
}

ustring AmStar::ExportSub(const ValueType& Value)
{
    ustring Result = ImportBase::ValueType::SubToString(Value.Value.DetailedData.Sub);
    if (Value.FloatValue.has_value()) {Result += ExportSpecSubRange(Value);}
    if (!Value.FloatValue.has_value() || (Value.FloatValue.has_value() && 
        ((Value.Value.DetailedData.SubU & ImportBase::UncertaintyType::BrakIn) || 
        (Value.FloatValue->DetailedData.Sub == ImportBase::ValueType::U16Npos) ||
        (Value.Value.DetailedData.Sub == Value.FloatValue->DetailedData.Sub))))
    {
        ImportBase::UncertaintyType::AddUncertainty(&Result, Value.Value.DetailedData.SubU);
    }
    return Result;
}

ustring AmStar::ExportSpecSubRange(const ValueType& Value)
{
    auto Data = Value.Value;
    auto FloatData = Value.FloatValue;
    if (!FloatData.has_value() || (FloatData.has_value() && 
        ((!FloatData->DetailedData.Spec && (FloatData->DetailedData.Sub == ImportBase::ValueType::U16Npos)) ||
        (FloatData->DetailedData.Spec == Data.DetailedData.Spec &&
        FloatData->DetailedData.Sub == Data.DetailedData.Sub)))) 
    {
        return ustring();
    }
    ustring Result;
    if (Data.DetailedData.SpecU & ImportBase::UncertaintyType::Slash || 
        Data.DetailedData.SubU & ImportBase::UncertaintyType::Slash)
    {
        Result.push_back('/');
    }

    if (!(Data.DetailedData.SubU & ImportBase::UncertaintyType::Simplified) || 
        FloatData->DetailedData.Spec != Data.DetailedData.Spec)
    {
        Result.push_back(ImportBase::ValueType::SpecToChar(FloatData->DetailedData.Spec));
    }
    
    Result.append(ImportBase::ValueType::SubToString(FloatData->DetailedData.Sub));

    return Result;
}

ustring AmStar::ExportLum(const ValueType& Value)
{
    auto Op1 = [&Value]()
    {
        ustring Result;
        Result = ImportBase::ValueType::LumToString(Value.Value.DetailedData.Lum);
        ImportBase::UncertaintyType::AddUncertainty(&Result, Value.Value.DetailedData.LumU);
        if (Value.Value.DetailedData.SLum)
        {
            ustring Result2 = ImportBase::ValueType::SLumToString(Value.Value.DetailedData.SLum);
            ImportBase::UncertaintyType::AddUncertainty(&Result2, Value.Value.DetailedData.SLumU);
            Result += Result2;
        }
        return Result;
    };
    
    auto Op2 = [&Value]() // 这种情况下若出现"()"，只会出现整个在内部的情况
    {
        ustring Result;
        ImportBase::UncertaintyType::UncertaintySymbols LU = 
            ImportBase::UncertaintyType::UncertaintySymbols(Value.Value.DetailedData.LumU | Value.Value.DetailedData.SLumU);
        Result = ImportBase::ValueType::LumToString(Value.Value.DetailedData.Lum);
        if (Value.Value.DetailedData.SLum)
        {
            Result += ImportBase::ValueType::SLumToString(Value.Value.DetailedData.SLum);
        }
        Result += ExportLumRange(Value);
        if (LU)
        {
            ImportBase::UncertaintyType::AddUncertainty(&Result, LU);
        }
        return Result;
    };

    if (!Value.FloatValue.has_value()) {return Op1();}
    else
    {
        if (!Value.FloatValue->DetailedData.Lum || (Value.FloatValue->DetailedData.Lum == Value.Value.DetailedData.Lum))
        {
            if (Value.FloatValue->DetailedData.SLum && 
                (Value.FloatValue->DetailedData.SLum != Value.Value.DetailedData.SLum))
            {
                return Op2();
            }
            else {return Op1();}
        }
        else {return Op2();}
    }
}

ustring AmStar::ExportLumRange(const ValueType& Value)
{
    if (!Value.FloatValue.has_value() || (Value.FloatValue.has_value() && 
        ((!Value.FloatValue->DetailedData.Lum && !Value.FloatValue->DetailedData.SLum) || 
        (Value.FloatValue->DetailedData.Lum == Value.Value.DetailedData.Lum &&
        Value.FloatValue->DetailedData.SLum == Value.Value.DetailedData.SLum)))) 
    {
        return ustring();
    }
    ustring Result;
    if (Value.Value.DetailedData.LumU & ImportBase::UncertaintyType::Slash || 
        Value.Value.DetailedData.SLumU & ImportBase::UncertaintyType::Slash)
    {
        Result.push_back('/');
    }
    else {Result.push_back('-');}

    if (!(Value.Value.DetailedData.SLumU & ImportBase::UncertaintyType::Simplified) || 
        Value.FloatValue->DetailedData.Lum != Value.Value.DetailedData.Lum)
    {
        Result.append(ImportBase::ValueType::LumToString(Value.FloatValue->DetailedData.Lum, 1));
    }

    Result.append(ImportBase::ValueType::SLumToString(Value.FloatValue->DetailedData.SLum));

    return Result;
}

ustring AmStar::ExportSegmentString(const SegmentType::value_type& Value)
{
    ustring Str = Value.first.Line;
    ImportBase::UncertaintyType::AddUncertainty(&Str, Value.first.UncertaintyFlags);
    Str += ExportSpec(Value.second);
    Str += ExportSub(Value.second);
    Str += ExportLum(Value.second);
    return Str;
}

ustring AmStar::ExportPec(const std::vector<PecularityType>& Table)
{
    ustring Result;
    for (const auto& i : Table)
    {
        ustring Pec = i.Element;
        ImportBase::UncertaintyType::AddUncertainty(&Pec, i.Uncertainty);
        Result.append(Pec);
    }
    return Result;
}

ustring AmStar::ExportChem(const std::vector<ChemicalPecularitySpec>& Table)
{
    ustring Result;
    for (const auto& i : Table)
    {
        ustring Pec = i.Element;
        ImportBase::UncertaintyType::AddUncertainty(&Pec, i.Uncertainty);
        Result.append(Pec);
    }
    return Result;
}

ustring AmStar::ToString()const
{
    if (FmtFlag == DeltaDelphini)
    {
        ustring Str;
        auto EData = std::get<DeltaDelphini>(Data);
        for (const auto& i : EData.Segments)
        {
            Str += ExportSegmentString(i);
        }
        Str += ExportPec(Pecularities);
        Str += ExportChem(ChemElems);
        return Str;
    }
    else if (FmtFlag == RhoPuppis)
    {
        ustring Str;
        auto EData = std::get<RhoPuppis>(Data);
        Str += ExportSpec(EData.MainSegment);
        Str += ExportSub(EData.MainSegment);
        Str += ExportLum(EData.MainSegment);
        Str += ExportPec(Pecularities);
        for (const auto& i : EData.Segments)
        {
            Str += ExportSegmentString(i);
        }
        return Str;
    }
    else if (FmtFlag == KappaOctantis)
    {
        ustring Str;
        auto EData = std::get<KappaOctantis>(Data);
        Str += ExportSpec(EData.MainType);
        Str += ExportSub(EData.MainType);
        ustring M = L"m";
        ImportBase::UncertaintyType::AddUncertainty(&M, EData.LineUncertaintyFlags);
        Str += M;
        Str += ExportSpec(EData.MetallicType1);
        Str += ExportSub(EData.MetallicType1);
        Str += "-";
        Str += ExportSpec(EData.MetallicType2);
        Str += ExportSub(EData.MetallicType2);
        return Str;
    }
}

size_t AmStar::size()const
{
    if (FmtFlag == DeltaDelphini)
    {
        return std::get<DeltaDelphini>(Data).Segments.size();
    }
    else if (FmtFlag == RhoPuppis)
    {
        return std::get<RhoPuppis>(Data).Segments.size() + 1;
    }
    else if (FmtFlag == KappaOctantis)
    {
        return 3;
    }
}

const AmStar::ValueType& AmStar::GetRawDataByIndex(size_t Index)const
{
    if (FmtFlag == DeltaDelphini)
    {
        return (std::get<DeltaDelphini>(Data).Segments.begin() + Index)->second;
    }
    else if (FmtFlag == RhoPuppis)
    {
        if (Index == 0) {return std::get<RhoPuppis>(Data).MainSegment;}
        else {return (std::get<RhoPuppis>(Data).Segments.begin() + Index - 1)->second;}
    }
    else if (FmtFlag == KappaOctantis)
    {
        if (Index == 0) {return std::get<KappaOctantis>(Data).MainType;}
        if (Index == 1) {return std::get<KappaOctantis>(Data).MetallicType1;}
        if (Index == 2) {return std::get<KappaOctantis>(Data).MetallicType2;}
    }
}

char AmStar::SpectralClass(size_t Index)const
{
    bool PickFloatData = Index >= size();
    size_t RealIndex = PickFloatData ? Index - size() : Index;
    auto& RData = GetRawDataByIndex(Index);
    auto& IData = PickFloatData ? RData.FloatValue.value() : RData.Value;
    auto Spec = Index && IData.DetailedData.Spec ? IData.DetailedData.Spec : RData.Value.DetailedData.Spec;
    return ImportBase::ValueType::SpecToChar(Spec);
}

uint16_t AmStar::SpectralClassUncertained(size_t Index)const
{
    auto& RData = GetRawDataByIndex(Index);
    return RData.Value.DetailedData.SpecU;
}

uint16_t AmStar::SpecializedClass(size_t Index)const
{
    bool PickFloatData = Index >= size();
    size_t RealIndex = PickFloatData ? Index - size() : Index;
    auto& RData = GetRawDataByIndex(Index);
    auto& IData = PickFloatData ? RData.FloatValue.value() : RData.Value;
    auto Sub = Index && ~(IData.DetailedData.Sub) ? IData.DetailedData.Sub : RData.Value.DetailedData.Sub;
    return Sub;
}

uint16_t AmStar::SpecializedClassUncertained(size_t Index)const
{
    auto& RData = GetRawDataByIndex(Index);
    return RData.Value.DetailedData.SubU;
}

ustring AmStar::LuminosityClass(size_t Index)const
{
    bool PickFloatData = Index >= size();
    size_t RealIndex = PickFloatData ? Index - size() : Index;
    auto& RData = GetRawDataByIndex(Index);
    auto& IData = PickFloatData ? RData.FloatValue.value() : RData.Value;
    
    ustring Result;
    Result.append(ImportBase::ValueType::LumToString(IData.DetailedData.Lum));
    Result.append(ImportBase::ValueType::SLumToString(IData.DetailedData.SLum));
    if (Result.empty())
    {
        Result.append(ImportBase::ValueType::LumToString(RData.Value.DetailedData.Lum));
        Result.append(ImportBase::ValueType::SLumToString(RData.Value.DetailedData.SLum));
    }
    return Result;
}

uint16_t AmStar::LuminosityClassUncertained(size_t Index)const
{
    auto& RData = GetRawDataByIndex(Index);
    return RData.Value.DetailedData.LumU == ImportBase::UncertaintyType::None ? 
        RData.Value.DetailedData.SLumU : RData.Value.DetailedData.LumU;
}

std::set<ustring> AmStar::SpectralPeculiarities()const
{
    std::set<ustring> Pec;
    for (const auto& i : Pecularities)
    {
        Pec.insert(i.Element);
    }
    return Pec;
}

std::set<ustring> AmStar::ElementSymbols()const
{
    std::set<ustring> Chem;
    for (const auto& i : ChemElems)
    {
        Chem.insert(i.Element);
    }
    return Chem;
}

std::map<ustring, std::variant<int16_t, ustring>> AmStar::BandPecularities()const
{
    std::map<ustring, std::variant<int16_t, ustring>> Bands;
    if (FmtFlag == DeltaDelphini)
    {
        auto EData = std::get<DeltaDelphini>(Data);
        for (const auto& i : EData.Segments)
        {
            Bands.insert({i.first.Line, i.first.Element});
        }
    }
    else if (FmtFlag == RhoPuppis)
    {
        auto EData = std::get<DeltaDelphini>(Data);
        for (const auto& i : EData.Segments)
        {
            Bands.insert({i.first.Line, i.first.Element});
        }
    }
    else if (FmtFlag == KappaOctantis)
    {
        Bands.insert({LinesTable.at("m").Line, LinesTable.at("m").Element});
    }
    return Bands;
}



_SPCLS_END
_CSE_END
