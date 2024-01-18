#pragma once

#ifndef _SCS_BASE
#define _SCS_BASE

#include <vector>
#include <string>
#include <memory>

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/gltypes/GVector2D.h>
#include <CSE/SCStream/LRParser.h>

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

#define _SC_BEGIN namespace SCS {
#define _SC_END }
#define _SC SCS::

_CSE_BEGIN
_SC_BEGIN

#if (SC_PARSER_LR == LALR1)

#define SCFINAL    4  // State number of the termination state.
#define SCLAST     36 // Last index in SCTABLE. (Unused)
#define SCNTOKENS  12 // Number of terminals.
#define SCNNTS     10 // Number of nonterminals.
#define SCNRULES   20 // Number of rules.
#define SCNSTATES  30 // Number of states.

#elif (SC_PARSER_LR == LR1)

// Definations from YACC Parser
#define SCFINAL    4   // State number of the termination state.
#define SCLAST     315 // Last index in TABLE. (Unused)
#define SCNTOKENS  12  // Number of terminals.
#define SCNNTS     10  // Number of nonterminals.
#define SCNRULES   20  // Number of rules.
#define SCNSTATES  61  // Number of states.

#elif (SC_PARSER_LR == IELR1)
#error IELR(1) algorithom is deprecated because its parsing table is same as LALR(1), use LALR(1) instead.
#else
#error Invalid parser option.
#endif

extern CSEDebugger CSECatDebug;

class ParseException : public std::runtime_error // output errors
{
public:
    using _Mybase = std::runtime_error;
    ParseException(const std::string& Error) : _Mybase{ Error } {}
    ParseException(const std::string& err, size_t line_number) : _Mybase{ err + " at line " + std::to_string(line_number) } {}
};

enum __Token_Types
{
    Identifier = 258,
    Number     = 259,
    VString    = 260,
    Boolean    = 261,
    UnaryPunctuator,
    LeftPunctuator,
    RightPunctuator,
    eof,
    Others
};

template<typename _CharT>
using String          = std::basic_string<_CharT>;
template<typename _CharT>
using StringList      = std::vector<String<_CharT>>;
template<typename _CharT>
struct TokenType;
template<typename _CharT>
using TokenArrayType  = std::vector<TokenType<_CharT>>;
template<typename _Ty>
using SharedPointer   = std::shared_ptr<_Ty>;

template<typename _CharT>
struct TokenType
{
    __Token_Types  Type;
    String<_CharT> Value;
    ivec2          Posiston;
};

template<typename _CharT>
int IsIdentifier(TokenType<_CharT> Tok)
{
    return Tok.Type == Identifier ? int(Tok.Type) : 0;
}

template<typename _CharT>
int IsExpression(TokenType<_CharT> Tok)
{
    return (Tok.Type == Number || Tok.Type == VString || Tok.Type == Boolean) ? int(Tok.Type) : 0;
}

template<typename _CharT>
int IsPunctuator(TokenType<_CharT> Tok)
{
    return (Tok.Type == UnaryPunctuator || Tok.Type == LeftPunctuator || Tok.Type == RightPunctuator) ? int(Tok.Type) : 0;
}

template<typename _CharT>
struct SCSTable
{
    using CatalogType     = std::vector<struct SCKeyValue>;
    using ValueType       = struct SCKeyValue;
    using ReferType       = struct SCKeyValue;
    using IterType        = CatalogType::iterator;
    using ConstIter       = CatalogType::const_iterator;

    using SubTablePointer = SharedPointer<SCSTable<_CharT>>;

    struct SCKeyValue
    {
        String<_CharT>     Key;
        StringList<_CharT> Value;
        SubTablePointer    SubTable = nullptr;
    };

    constexpr static const char ArrayBegin[]  = "#ArrayBegin";
    constexpr static const char ArrayEnd[]    = "#ArrayEnd";
    constexpr static const char MatrixBegin[] = "#MatBegin";
    constexpr static const char MatrixEnd[]   = "#MatEnd";

    CatalogType _M_Elems;

    CatalogType& Get() {return _M_Elems;}
};

using SharedTablePointer = SharedPointer<SCSTable<char>>;
using WSharedTablePointer = SharedPointer<SCSTable<wchar_t>>;

// Tables
extern const __LR_Parser_Base<char>::GrammaTableType __SC_Grammar_Production_Table;
extern const __LR_Parser_Base<char>::StatesType __SC_State_Table;
extern const __LR_Parser_Base<wchar_t>::GrammaTableType __SC_Grammar_Production_Table_WCH;
extern const __LR_Parser_Base<wchar_t>::StatesType __SC_State_Table_WCH;

_SC_END
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
