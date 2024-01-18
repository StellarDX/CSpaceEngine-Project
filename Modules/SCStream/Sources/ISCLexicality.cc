#include "CSE/SCStream/SCSBase.h"
#include "CSE/SCStream/ISCStream.h"
#include <regex>
#include <cctype>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN
_SC_BEGIN

namespace Tokens
{
    // IBM-standard Regular Expression for all types of tokens
    // Reference: https://www.ibm.com/docs/en/zos/3.1.0?topic=elements-tokens
    #define REGEX_DEFINE(name, reg)\
    const regex name(reg);\
    const wregex name##W(L##reg);

    // IBM only gives DFA in their documents and they
    // need to be converted to regular expressions in
    // this situation.

    // StellarDX: Some items in these regular expressions
    // below can be replaced by POSIX standard character
    // classes but I don't know which compilers support.
    REGEX_DEFINE(IdentRegex,    "^[A-Za-z_][A-Za-z0-9_]*$")
    REGEX_DEFINE(DecIntRegex,   "^[+-]?[1-9][0-9]*$")
    REGEX_DEFINE(HexIntRegex,   "^[+-]?0[Xx][A-Fa-f0-9]*$")
    REGEX_DEFINE(OctIntRegex,   "^[+-]?0[0-7]*$")
    REGEX_DEFINE(BinIntRegex,   "^[+-]?0[Bb][01]*$")
    REGEX_DEFINE(BoolRegex,     "^(true)|(false)$")
    REGEX_DEFINE(FloatRegex,    "^[+-]?([0-9]*\\.[0-9]+([Ee][+-]?[0-9]+)?)|([0-9]+\\.([Ee][+-]?[0-9]+)?)|([0-9]+([Ee][+-]?[0-9]+))$")
    REGEX_DEFINE(HexFloatRegex, "^[+-]?0[Xx]([A-Fa-f0-9]*\\.[A-Fa-f0-9]+[Pp][+-]?[0-9]+)|([A-Fa-f0-9]+\\.[Pp][+-]?[0-9]+)|([A-Fa-f0-9]+[Pp][+-]?[0-9]+)$")
    REGEX_DEFINE(StringRegex,   "\".*\"")

    #undef REGEX_DEFINE

    string UnaryPunctuators    = ",";
    string BinaryPunctuators   = "()[]{}";

    wstring WUnaryPunctuators  = L",";
    wstring WBinaryPunctuators = L"()[]{}";
}

template<typename _Iterator> requires std::random_access_iterator<_Iterator>
void ConsumeWhiteSpace(_Iterator& it, const _Iterator& end, uint64* line, uint64* column)
{
    while (it != end && isspace(*it))
    {
        if ('\n' == *it)
        {
            ++(*line);
            *column = 1;
            ++it;
            continue;
        }
        ++(*column);
        ++it;
    }
}

template<typename _Iterator> requires std::random_access_iterator<_Iterator>
void ConsumeWhiteSpaceW(_Iterator& it, const _Iterator& end, uint64* line, uint64* column)
{
    while (it != end && isspace(*it))
    {
        if (L'\n' == *it)
        {
            ++(*line);
            *column = 1;
            ++it;
            continue;
        }
        ++(*column);
        ++it;
    }
}

template<typename _CharT>
int IsUnaryPunctuator(_CharT _C)
{
    return find(Tokens::UnaryPunctuators.begin(), Tokens::UnaryPunctuators.end(), _C) !=
           Tokens::UnaryPunctuators.end();
}

template<typename _CharT>
int IsBinaryPunctuator(_CharT _C)
{
    return find(Tokens::BinaryPunctuators.begin(), Tokens::BinaryPunctuators.end(), _C) !=
           Tokens::BinaryPunctuators.end();
}

template<typename _CharT>
int IsPunctuator(_CharT _C)
{
    return IsUnaryPunctuator(_C) || IsBinaryPunctuator(_C);
}

void ParseIdentifier(string::const_iterator& it, const string::const_iterator& end, string* Output, uint64* line, uint64* column)
{
    //uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && '\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }
    //if (!regex_match(*Output, Tokens::IdentRegex)) {return;}
}

void ParseIdentifierW(wstring::const_iterator& it, const wstring::const_iterator& end, wstring* Output, uint64* line, uint64* column)
{
    //uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && L'\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }
    //if (!regex_match(*Output, Tokens::IdentRegex)) {return;}
}

void ParseNumber(string::const_iterator& it, const string::const_iterator& end, string* Output, uint64* line, uint64* column)
{
    uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && '\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }

    // Remove + and - at the begining of number
    string NumberPart = *Output;
    if ('+' == Output->front() || '-' == Output->front()) {NumberPart.erase(NumberPart.begin());}

    if ('0' == NumberPart.front())
    {
        // Binary Number
        if (regex_match(NumberPart.substr(0, 2), regex("0[Bb]")))
        {
            if (regex_match(NumberPart, Tokens::BinIntRegex)) {return;}
            else
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in binary constant.",
                make_format_args(*line, StartCol)));
            }
        }
        // Hexadecimal Number
        if (regex_match(NumberPart.substr(0, 2), regex("0[Xx]")))
        {
            if
            (
                regex_match(NumberPart, Tokens::HexFloatRegex) ||
                regex_match(NumberPart, Tokens::HexIntRegex)
            ) {return;}
            else
            {
                transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
                auto IsFound = [](string In, char Val)
                {
                    return find(In.begin(), In.end(), Val) != In.end();
                };
                // Check error
                if (IsFound(NumberPart, '.') && !IsFound(NumberPart, 'p'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Hexadecimal floating literal requires an exponent.",
                    make_format_args(*line, StartCol)));
                }
                else if (IsFound(NumberPart, 'p'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Exponent has no digits.",
                    make_format_args(*line, StartCol)));
                }
                else
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in integer constant.",
                    make_format_args(*line, StartCol)));
                }
            }
        }
        else // Octal integer or decimal floating point
        {
            if
            (
                regex_match(NumberPart, Tokens::FloatRegex) ||
                regex_match(NumberPart, Tokens::OctIntRegex)
            ){return;}
            else
            {
                transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
                auto IsFound = [](string In, char Val)
                {
                    return find(In.begin(), In.end(), Val) != In.end();
                };
                if (IsFound(NumberPart, '.') || IsFound(NumberPart, 'e'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in floating constant.",
                    make_format_args(*line, StartCol)));
                }
                else
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in octal constant.",
                    make_format_args(*line, StartCol)));
                }
            }
        }
    }
    else
    {
        if
        (
            regex_match(NumberPart, Tokens::FloatRegex) ||
            regex_match(NumberPart, Tokens::DecIntRegex)
        ){return;}
        else
        {
            transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
            auto IsFound = [](string In, char Val)
            {
                return find(In.begin(), In.end(), Val) != In.end();
            };
            if (IsFound(NumberPart, '.') || IsFound(NumberPart, 'e'))
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in floating constant.",
                make_format_args(*line, StartCol)));
            }
            else
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in integer constant.",
                make_format_args(*line, StartCol)));
            }
        }
    }
}

void ParseNumberW(wstring::const_iterator& it, const wstring::const_iterator& end, wstring* Output, uint64* line, uint64* column)
{
    uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && L'\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }

    // Remove + and - at the begining of number
    wstring NumberPart = *Output;
    if (L'+' == Output->front() || L'-' == Output->front()) {NumberPart.erase(NumberPart.begin());}

    if (L'0' == NumberPart.front())
    {
        // Binary Number
        if (regex_match(NumberPart.substr(0, 2), wregex(L"0[Bb]")))
        {
            if (regex_match(NumberPart, Tokens::BinIntRegexW)) {return;}
            else
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in binary constant.",
                make_format_args(*line, StartCol)));
            }
        }
        // Hexadecimal Number
        if (regex_match(NumberPart.substr(0, 2), wregex(L"0[Xx]")))
        {
            if
            (
                regex_match(NumberPart, Tokens::HexFloatRegexW) ||
                regex_match(NumberPart, Tokens::HexIntRegexW)
            ) {return;}
            else
            {
                transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
                auto IsFound = [](wstring In, char Val)
                {
                    return find(In.begin(), In.end(), Val) != In.end();
                };
                // Check error
                if (IsFound(NumberPart, L'.') && !IsFound(NumberPart, L'p'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Hexadecimal floating literal requires an exponent.",
                    make_format_args(*line, StartCol)));
                }
                else if (IsFound(NumberPart, L'p'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Exponent has no digits.",
                    make_format_args(*line, StartCol)));
                }
                else
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in integer constant.",
                    make_format_args(*line, StartCol)));
                }
            }
        }
        else // Octal integer or decimal floating point
        {
            if
            (
                regex_match(NumberPart, Tokens::FloatRegexW) ||
                regex_match(NumberPart, Tokens::OctIntRegexW)
            ){return;}
            else
            {
                transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
                auto IsFound = [](wstring In, char Val)
                {
                    return find(In.begin(), In.end(), Val) != In.end();
                };
                if (IsFound(NumberPart, L'.') || IsFound(NumberPart, L'e'))
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in floating constant.",
                    make_format_args(*line, StartCol)));
                }
                else
                {
                    throw ParseException(vformat
                    ("Error at ({}, {}): Invalid digit in octal constant.",
                    make_format_args(*line, StartCol)));
                }
            }
        }
    }
    else
    {
        if
        (
            regex_match(NumberPart, Tokens::FloatRegexW) ||
            regex_match(NumberPart, Tokens::DecIntRegexW)
        ){return;}
        else
        {
            transform(NumberPart.begin(), NumberPart.end(), NumberPart.begin(), ::tolower);
            auto IsFound = [](wstring In, char Val)
            {
                return find(In.begin(), In.end(), Val) != In.end();
            };
            if (IsFound(NumberPart, L'.') || IsFound(NumberPart, L'e'))
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in floating constant.",
                make_format_args(*line, StartCol)));
            }
            else
            {
                throw ParseException(vformat
                ("Error at ({}, {}): Invalid digit in integer constant.",
                make_format_args(*line, StartCol)));
            }
        }
    }
}

void ParseString(string::const_iterator& it, const string::const_iterator& end, string* Output, uint64* line, uint64* column)
{
    *Output += *it;
    ++(*column);
    ++it;
    while (it != end && '\"' != *it)
    {
        *Output += *it;
        if ('\n' == *it)
        {
            ++(*line);
            *column = 1;
            ++it;
            continue;
        }
        ++(*column);
        ++it;
    }
    *Output += *it;
    ++(*column);
    ++it;
}

void ParseStringW(wstring::const_iterator& it, const wstring::const_iterator& end, wstring* Output, uint64* line, uint64* column)
{
    *Output += *it;
    ++(*column);
    ++it;
    while (it != end && L'\"' != *it)
    {
        *Output += *it;
        if (L'\n' == *it)
        {
            ++(*line);
            *column = 1;
            ++it;
            continue;
        }
        ++(*column);
        ++it;
    }
    *Output += *it;
    ++(*column);
    ++it;
}

void __Tokenizer(const string& Input, TokenArrayType<char>* Output)
{
    auto begin = Input.begin();
    auto it = begin;
    auto end = Input.end();
    uint64 LineNumber = 1, ColumnNumber = 1;
    ConsumeWhiteSpace(it, end, &LineNumber, &ColumnNumber);
    while (it != end)
    {
        if ('\n' == *it)
        {
            ++LineNumber;
            ColumnNumber = 1;
            ++it;
            continue;
        }

        if (isspace(*it)) {ConsumeWhiteSpace(it, end, &LineNumber, &ColumnNumber);}

        // Parse Punctuator
        auto IsUPunc = find(Tokens::UnaryPunctuators.begin(), Tokens::UnaryPunctuators.end(), *it);
        auto IsBPunc = find(Tokens::BinaryPunctuators.begin(), Tokens::BinaryPunctuators.end(), *it);
        if (IsUPunc != Tokens::UnaryPunctuators.end())
        {
            Output->push_back({UnaryPunctuator, string("") + *it, ivec2(LineNumber, ColumnNumber)});
            ++ColumnNumber;
            ++it;
            continue;
        }
        else if (IsBPunc != Tokens::BinaryPunctuators.end())
        {
            if ((IsBPunc - Tokens::BinaryPunctuators.begin()) % 2 == 0)
            {
                Output->push_back({LeftPunctuator, string("") + *it, ivec2(LineNumber, ColumnNumber)});
            }
            else {Output->push_back({RightPunctuator, string("") + *it, ivec2(LineNumber, ColumnNumber)});}
            ++ColumnNumber;
            ++it;
            continue;
        }

        // Parse Identifier and boolean
        if (isalpha(*it) || '_' == *it)
        {
            string IDBuf;
            uint64 StartCol = ColumnNumber;
            ParseIdentifier(it, end, &IDBuf, &LineNumber, &ColumnNumber);
            if ("true" == IDBuf || "false" == IDBuf)
            {
                Output->push_back({Boolean, IDBuf, ivec2(LineNumber, StartCol)});
            }
            else {Output->push_back({Identifier, IDBuf, ivec2(LineNumber, StartCol)});}
            continue;
        }

        // Parse Number
        if (isdigit(*it) || '+' == *it || '-' == *it)
        {
            string NumBuf;
            uint64 StartCol = ColumnNumber;
            ParseNumber(it, end, &NumBuf, &LineNumber, &ColumnNumber);
            Output->push_back({Number, NumBuf, ivec2(LineNumber, StartCol)});
            continue;
        }

        // Parse String
        if ('\"' == *it)
        {
            string StrBuf;
            uint64 StartLine = LineNumber, StartCol = ColumnNumber;
            ParseString(it, end, &StrBuf, &LineNumber, &ColumnNumber);
            Output->push_back({VString, StrBuf, ivec2(StartLine, StartCol)});
            continue;
        }

        throw ParseException(vformat("Unexpected character <0x{:X}> at ({}, {})",
            make_format_args(*it, LineNumber, ColumnNumber)));
    }
}

void __Tokenizer_WCHAR(const wstring& Input, TokenArrayType<wchar_t>* Output)
{
    auto begin = Input.begin();
    auto it = begin;
    auto end = Input.end();
    uint64 LineNumber = 1, ColumnNumber = 1;
    ConsumeWhiteSpaceW(it, end, &LineNumber, &ColumnNumber);
    while (it != end)
    {
        if (L'\n' == *it)
        {
            ++LineNumber;
            ColumnNumber = 1;
            ++it;
            continue;
        }

        if (isspace(*it)) {ConsumeWhiteSpaceW(it, end, &LineNumber, &ColumnNumber);}

        // Parse Punctuator
        auto IsUPunc = find(Tokens::WUnaryPunctuators.begin(), Tokens::WUnaryPunctuators.end(), *it);
        auto IsBPunc = find(Tokens::WBinaryPunctuators.begin(), Tokens::WBinaryPunctuators.end(), *it);
        if (IsUPunc != Tokens::WUnaryPunctuators.end())
        {
            Output->push_back({UnaryPunctuator, wstring(L"") + *it, ivec2(LineNumber, ColumnNumber)});
            ++ColumnNumber;
            ++it;
            continue;
        }
        else if (IsBPunc != Tokens::WBinaryPunctuators.end())
        {
            if ((IsBPunc - Tokens::WBinaryPunctuators.begin()) % 2 == 0)
            {
                Output->push_back({LeftPunctuator, wstring(L"") + *it, ivec2(LineNumber, ColumnNumber)});
            }
            else {Output->push_back({RightPunctuator, wstring(L"") + *it, ivec2(LineNumber, ColumnNumber)});}
            ++ColumnNumber;
            ++it;
            continue;
        }

        // Parse Identifier and boolean
        if (isalpha(*it) || L'_' == *it)
        {
            wstring IDBuf;
            uint64 StartCol = ColumnNumber;
            ParseIdentifierW(it, end, &IDBuf, &LineNumber, &ColumnNumber);
            if (L"true" == IDBuf || L"false" == IDBuf)
            {
                Output->push_back({Boolean, IDBuf, ivec2(LineNumber, StartCol)});
            }
            else {Output->push_back({Identifier, IDBuf, ivec2(LineNumber, StartCol)});}
            continue;
        }

        // Parse Number
        if (isdigit(*it) || L'+' == *it || L'-' == *it)
        {
            wstring NumBuf;
            uint64 StartCol = ColumnNumber;
            ParseNumberW(it, end, &NumBuf, &LineNumber, &ColumnNumber);
            Output->push_back({Number, NumBuf, ivec2(LineNumber, StartCol)});
            continue;
        }

        // Parse String
        if (L'\"' == *it)
        {
            wstring StrBuf;
            uint64 StartLine = LineNumber, StartCol = ColumnNumber;
            ParseStringW(it, end, &StrBuf, &LineNumber, &ColumnNumber);
            Output->push_back({VString, StrBuf, ivec2(StartLine, StartCol)});
            continue;
        }

        short Chr = short(*it);
        throw ParseException(vformat("Unexpected character <{}> at ({}, {})",
            make_format_args(Chr, LineNumber, ColumnNumber)));
    }
}

SharedPointer<TokenArrayType<char>> Tokenizer(const string& Input)
{
    TokenArrayType<char> Output;
    __Tokenizer(Input, &Output);
    return make_shared<TokenArrayType<char>>(Output);
}

SharedPointer<TokenArrayType<wchar_t>> TokenizerW(const wstring& Input)
{
    TokenArrayType<wchar_t> Output;
    __Tokenizer_WCHAR(Input, &Output);
    return make_shared<TokenArrayType<wchar_t>>(Output);
}

_SC_END
_CSE_END
