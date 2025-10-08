/*
    Lexical analysing part of SC Parser.
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

#include "CSE/Parser/SCSBase.h"
#include "CSE/Parser/ISCStream.h"
#include <cwchar>
#include <cctype>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

#ifdef _USE_BOOST_REGEX
using namespace boost;
#endif

_CSE_BEGIN
_SC_BEGIN

namespace Tokens
{
    // IBM-standard Regular Expression for all types of tokens
    // Reference: https://www.ibm.com/docs/en/zos/3.1.0?topic=elements-tokens

    // IBM only gives DFA in their documents and they
    // need to be converted to regular expressions in
    // this situation.

    // StellarDX: Some items in these regular expressions
    // below can be replaced by POSIX standard character
    // classes but I don't know which compilers support.

    static const regex IdentRegex    ("^[A-Za-z_][A-Za-z0-9_]*$");
    static const regex DecIntRegex   ("^[+-]?[1-9][0-9]*$");
    static const regex HexIntRegex   ("^[+-]?0[Xx][A-Fa-f0-9]+$");
    static const regex OctIntRegex   ("^[+-]?0[0-7]*$");
    static const regex BinIntRegex   ("^[+-]?0[Bb][01]+$");
    static const regex BoolRegex     ("^(true)|(false)$");
    static const regex FloatRegex    ("^[+-]?(([0-9]*\\.[0-9]+([Ee][+-]?[0-9]+)?)|([0-9]+\\.([Ee][+-]?[0-9]+)?)|([0-9]+([Ee][+-]?[0-9]+)))$");
    static const regex HexFloatRegex ("^[+-]?0[Xx](([A-Fa-f0-9]*\\.[A-Fa-f0-9]+[Pp][+-]?[0-9]+)|([A-Fa-f0-9]+\\.[Pp][+-]?[0-9]+)|([A-Fa-f0-9]+[Pp][+-]?[0-9]+))$");
    static const regex StringRegex   ("\".*\"");

    static const wchar_t Punctuators[8] = L",()[]{}";
    static const wchar_t Operators[5]   = L"<=>!";

    static wchar_t const* ValidSingleOperators[2]
    {
        L"<", L">"
    };

    static wchar_t const* ValidBinaryOperators[4]
    {
        L"<=", L">=", L"==", L"!="
    };
}

void __SE_Lexicality::AddVariable(ustringlist Variable)
{
    VariableList.insert(VariableList.end(), Variable.begin(), Variable.end());
}

int __SE_Lexicality::IsPunctuator(wchar_t _C)
{
    return find(begin(Tokens::Punctuators), end(Tokens::Punctuators), _C)
        != end(Tokens::Punctuators);
}

int __SE_Lexicality::IsOperator(wchar_t _C)
{
    return find(begin(Tokens::Operators), end(Tokens::Operators), _C)
        != end(Tokens::Operators);
}

void __SE_Lexicality::ConsumeWhiteSpace(ustring::const_iterator& it,
    const ustring::const_iterator& end, uint64* line, uint64* column)
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

void __SE_Lexicality::ParseOperator(ustring::const_iterator& it, const ustring::const_iterator& end,
    ustring* Output, uint64* line, uint64* column)
{
    uint64 StartCol = *column;
    *Output += *it;
    if (find_if(std::begin(Tokens::ValidBinaryOperators),
        std::end(Tokens::ValidBinaryOperators),
        [it](wchar_t const* ch) {return ch[0] == *it;})) // If possible binary
    {
        if (IsOperator(*(++it))) {*Output += *it;}
        ++(*column);
        if (Output->size() > 1)
        {
            if (find_if(std::begin(Tokens::ValidBinaryOperators),
                    std::end(Tokens::ValidBinaryOperators),
                    [Output](wchar_t const* str)->bool{return !wcscmp(str, Output->c_str());})
                != std::end(Tokens::ValidBinaryOperators))
            {
                ++it;
                ++(*column);
                return;
            }
        }
    }

    if (find_if(std::begin(Tokens::ValidSingleOperators),
            std::end(Tokens::ValidSingleOperators),
            [Output](wchar_t const* str)->bool{return !wcscmp(str, Output->c_str());})
        != std::end(Tokens::ValidSingleOperators))
    {
        ++it;
        ++(*column);
        return;
    }
    else
    {
        throw ParseException(format
            ("Error at ({}, {}): Unexpected \"{}\".",
            *line, StartCol, Output->ToStdString()));
    }
}

void __SE_Lexicality::ParseIdentifier(ustring::const_iterator& it, const ustring::const_iterator& end,
    ustring* Output, uint64* line, uint64* column)
{
    //uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && !IsOperator(*it) && L'\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }
    //if (!regex_match(*Output, Tokens::IdentRegex)) {return;}
}

void __SE_Lexicality::ParseNumber(ustring::const_iterator& it, const ustring::const_iterator& end,
    ustring* Output, uint64* line, uint64* column, int* Base)
{
    uint64 StartCol = *column;
    while (it != end && !isspace(*it) && !IsPunctuator(*it) && !IsOperator(*it) && L'\"' != *it)
    {
        *Output += *it;
        ++(*column);
        ++it;
    }

    // Remove + and - at the begining of number
    ustring NumberPart = *Output;
    if (L'+' == Output->front() || L'-' == Output->front()) {NumberPart.erase(NumberPart.begin());}

    if (L'0' == NumberPart.front())
    {
        if (NumberPart == "0")
        {
            *Base = 10;
            return;
        }
        // Binary Number
        else if (regex_match(NumberPart.substr(0, 2), wregex(L"0[Bb]")))
        {
            if (regex_match(NumberPart.ToStdString(), Tokens::BinIntRegex))
            {
                *Base = 2;
                return;
            }
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
                regex_match(NumberPart.ToStdString(), Tokens::HexFloatRegex) ||
                regex_match(NumberPart.ToStdString(), Tokens::HexIntRegex)
            )
            {
                *Base = 16;
                return;
            }
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
            #if NO_OCTAL_NUMBER
            string NumberPartWithoutZero = NumberPart.ToStdString();
            NumberPartWithoutZero.erase(NumberPartWithoutZero.begin());
            if
            (
                regex_match(NumberPartWithoutZero, Tokens::FloatRegex) ||
                regex_match(NumberPartWithoutZero, regex("[0-9]*$"))
            )
            {
                *Base = 10;
                return;
            }
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
            #else
            if
            (
                regex_match(NumberPart.ToStdString(), Tokens::FloatRegex) ||
                regex_match(NumberPart.ToStdString(), Tokens::OctIntRegex)
            )
            {
                auto IsFound = [](wstring In, char Val)
                {
                    return find(In.begin(), In.end(), Val) != In.end();
                };
                if (IsFound(NumberPart, L'.') || IsFound(NumberPart, L'e')) {*Base = 10;}
                else {*Base = 8;}
                return;
            }
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
            #endif
        }
    }
    else
    {
        if
        (
            regex_match(NumberPart.ToStdString(), Tokens::FloatRegex) ||
            regex_match(NumberPart.ToStdString(), Tokens::DecIntRegex)
        )
        {
            *Base = 10;
            return;
        }
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

void __SE_Lexicality::ParseString(ustring::const_iterator& it, const ustring::const_iterator& end,
    ustring* Output, uint64* line, uint64* column)
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

void __SE_Lexicality::Run(const ustring& Input, TokenArrayType* Output)
{
    auto begin = Input.begin();
    auto it = begin;
    auto end = Input.end();
    uint64 LineNumber = 1, ColumnNumber = 1;
    ConsumeWhiteSpace(it, end, &LineNumber, &ColumnNumber);
    while (it != end)
    {
        if (L'\n' == *it)
        {
            ++LineNumber;
            ColumnNumber = 1;
            ++it;
            continue;
        }

        if (isspace(*it)) {ConsumeWhiteSpace(it, end, &LineNumber, &ColumnNumber);}

        if (it == end) {break;}

        // Parse Punctuator
        auto IsPunc = find(std::begin(Tokens::Punctuators), std::end(Tokens::Punctuators), *it);
        if (IsPunc != std::end(Tokens::Punctuators))
        {
            Output->push_back({Punctuator, ustring(L"") + *it, ivec2(LineNumber, ColumnNumber)});
            ++ColumnNumber;
            ++it;
            continue;
        }

        // Parse Operator
        if (IsOperator(*it))
        {
            ustring OpBuf;
            uint64 StartCol = ColumnNumber;
            ParseOperator(it, end, &OpBuf, &LineNumber, &ColumnNumber);
            Output->push_back({Operator, OpBuf, ivec2(LineNumber, StartCol)});
            continue;
        }

        // Parse Identifier, Variable and Boolean
        if (isalpha(*it) || L'_' == *it)
        {
            ustring IDBuf;
            uint64 StartCol = ColumnNumber;
            ParseIdentifier(it, end, &IDBuf, &LineNumber, &ColumnNumber);
            if (L"true" == IDBuf || L"false" == IDBuf)
            {
                Output->push_back({Boolean, IDBuf, ivec2(LineNumber, StartCol)});
            }
            else if (find(VariableList.begin(), VariableList.end(), IDBuf) != VariableList.end())
            {
                Output->push_back({Variable, IDBuf, ivec2(LineNumber, StartCol)});
            }
            else {Output->push_back({Identifier, IDBuf, ivec2(LineNumber, StartCol)});}
            continue;
        }

        // Parse Number
        if (isdigit(*it) || L'+' == *it || L'-' == *it)
        {
            ustring NumBuf;
            uint64 StartCol = ColumnNumber;
            int Base;
            ParseNumber(it, end, &NumBuf, &LineNumber, &ColumnNumber, &Base);
            Output->push_back({Number, NumBuf, ivec2(LineNumber, StartCol), Base});
            continue;
        }

        // Parse String
        if (L'\"' == *it)
        {
            ustring StrBuf;
            uint64 StartLine = LineNumber, StartCol = ColumnNumber;
            ParseString(it, end, &StrBuf, &LineNumber, &ColumnNumber);
            Output->push_back({String, StrBuf, ivec2(StartLine, StartCol)});
            continue;
        }

        short Chr = short(*it);
        throw ParseException(vformat("Unexpected character <{}> at ({}, {})",
            make_format_args(Chr, LineNumber, ColumnNumber)));
    }
}

_SC_END
_CSE_END
