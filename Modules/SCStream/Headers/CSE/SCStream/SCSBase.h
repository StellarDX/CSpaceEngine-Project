#pragma once

#ifndef _SCS_BASE
#define _SCS_BASE

#include <array>
#include <vector>
#include <string>
#include <memory>

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/gltypes/GVector2D.h>
#include <CSE/CSEBase/Algorithms.h>
#include <CSE/SCStream/StelCXXRes/LRParser.h>
#include <CSE/SCStream/StelCXXRes/CodeCvt.h>

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

#define LALR1 0
#define LR1   1
#define IELR1 2

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

using ucs2_t = wchar_t;

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

// String to booolean
inline bool stob(const std::string& __str)
{
    return __str == "true" ? 1 : 0;
}

inline bool stob(const std::wstring& __str)
{
    return __str == L"true" ? 1 : 0;
}

inline ustring __Matrix_To_String(struct ValueType Val);

struct ValueType
{
    using SubMatrixType = std::map<size_t, ValueType>;

    enum TypeID
    {
        Number  = 0b0001,
        VString = 0b0010,
        Boolean = 0b0011,
        Array   = 0b0100,
        Matrix  = 0b1100,
        Others  = 0b0000,

        Mask    = 0b0011
    }Type;

    StringList<ucs2_t> Value;
    SharedPointer<SubMatrixType> SubMatrices = nullptr;

    template<typename _Tp>
    TypeID ToTypeID()
    {
        if (std::convertible_to<_Tp, float64>) {return Number;}
        else if (std::convertible_to<_Tp, ustring>) {return VString;}
        else if (std::is_same_v<_Tp, bool>) {return Boolean;}
        else {return Others;}
    }

    void GetQualified(void* Dst, size_t Idx = 0)
    {
        switch (Type)
        {
        case Number:
            *((float64*)Dst) = std::stod(Value.front());
            break;

        case VString:
            *((ustring*)Dst) = Value.front();
            ((ustring*)Dst)->erase(((ustring*)Dst)->begin());
            ((ustring*)Dst)->pop_back();
            break;

        case Boolean:
            *((bool*)Dst) = stob(Value.front());
            break;

        default:
            throw ParseException("Value is not convertable to single value.");
            break;
        }
    }

    bool GetTypeID() {return (Type & Mask);}

    template<typename _Tp, std::size_t _Nm>
    void GetAsArray(std::array<_Tp, _Nm>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        size_t Capacity = max(Value.size(), _Nm);
        for (int i = 0; i < Capacity; ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    template<typename _Tp>
    void GetAsArray(std::vector<_Tp>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        if (Dst->size() < Value.size()) {Dst->resize(Value.size());}
        for (int i = 0; i < Value.size(); ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    ustring ToString()const
    {
        if ((Type & (~Mask)) == Array)
        {
            ustring Str;
            Str.push_back(L'(');
            for (int i = 0; i < Value.size(); ++i)
            {
                Str += Value[i];
                if (i < Value.size() - 1) {Str += L", ";}
            }
            Str.push_back(L')');
            return Str;
        }
        if ((Type & (~Mask)) == Matrix)
        {
            return __Matrix_To_String(*this);
        }
        else {return Value.front();}
    }
};

inline ustring __Matrix_To_String(ValueType Val)
{
    ustring Str;
    Str += L"{ ";
    for (size_t i = 0; i <= Val.Value.size(); ++i)
    {
        if (Val.SubMatrices)
        {
            size_t j = i;
            while (Val.SubMatrices->contains(j))
            {
                Str += Val.SubMatrices->at(j).ToString();
                Str += L" ";
                ++j;
            }
        }
        if (i < Val.Value.size())
        {
            Str += Val.Value[i];
            Str += L" ";
        }
    }
    Str.push_back(L'}');
    return Str;
}

using ValueList = std::vector<ValueType>;

struct SCSTable
{
    using SubTablePointer = SharedPointer<SCSTable>;

    struct SCKeyValue
    {
        ustring            Key;
        ValueList          Value;
        SubTablePointer    SubTable = nullptr;
    };

    using CatalogType     = std::vector<SCKeyValue>;
    using ValueType       = SCKeyValue;
    using ReferType       = SCKeyValue&;
    using IterType        = CatalogType::iterator;
    using ConstIter       = CatalogType::const_iterator;

    CatalogType _M_Elems;

    CatalogType& Get() {return _M_Elems;}
};

using SharedTablePointer = SharedPointer<SCSTable>;

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
