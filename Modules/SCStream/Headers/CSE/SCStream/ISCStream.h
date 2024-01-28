///////////////////////////////////////////////////////
//                     SC Parser                     //
// ------------------------------------------------- //
//                                                   //
//         A loader for Space Engine sc file         //
//                                                   //
///////////////////////////////////////////////////////

/*
    CSpaceEngine SC Stream.
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

#ifndef _ISCSTREAM
#define _ISCSTREAM

#include <CSE/SCStream/SCSBase.h>
#include <istream>
#include <filesystem>

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
_SC_BEGIN

// Tables
extern const __LR_Parser_Base<char>::GrammaTableType __SC_Grammar_Production_Table;
extern const __LR_Parser_Base<char>::StatesType __SC_State_Table;
extern const __LR_Parser_Base<wchar_t>::GrammaTableType __SC_Grammar_Production_Table_WCH;
extern const __LR_Parser_Base<wchar_t>::StatesType __SC_State_Table_WCH;

// SC Parser
class SCParser : public LRParser
{
public:
    using _Mybase = LRParser;

    __StelCXX_Text_Codecvt_65001 _DefDecoder = __StelCXX_Text_Codecvt_65001();
    __StelCXX_Text_Codecvt_Base& Decoder = _DefDecoder;

    SCParser() : _Mybase(__SC_Grammar_Production_Table, __SC_State_Table) {}

    std::string TokenToString(SharedPointer<TokenArrayType<char>> Tokens);
    SharedPointer<SCSTable> MakeTable(std::stack<SCSTable::SCKeyValue>& SubTableTempStack);
    void MakeSubMatrix(ValueType& ExpressionBuffer, ValueType SubMatrix);
    void MoveSubMateix(ValueType& ExpressionBuffer);
    void ThrowError(size_t CurrentState, ivec2 Pos);

    SharedPointer<SCSTable> Run(SharedPointer<TokenArrayType<char>> Tokens) noexcept(0);
};

class WSCParser : public WLRParser
{
public:
    using _Mybase = WLRParser;

    WSCParser() : _Mybase(__SC_Grammar_Production_Table_WCH, __SC_State_Table_WCH) {}

    std::wstring TokenToString(SharedPointer<TokenArrayType<wchar_t>> Tokens);
    SharedPointer<SCSTable> MakeTable(std::stack<SCSTable::SCKeyValue>& SubTableTempStack);
    void MakeSubMatrix(ValueType& ExpressionBuffer, ValueType SubMatrix);
    void MoveSubMateix(ValueType& ExpressionBuffer);
    void ThrowError(size_t CurrentState, ivec2 Pos);

    SharedPointer<SCSTable> Run(SharedPointer<TokenArrayType<wchar_t>> Tokens) noexcept(0);
};

void SkipComments(std::string& Input);
SharedPointer<TokenArrayType<char>> Tokenizer(const std::string& Input);
SharedTablePointer Parser(SharedPointer<TokenArrayType<char>> Input, __StelCXX_Text_Codecvt_Base& Decoder);

void SkipCommentsW(std::wstring& Input);
SharedPointer<TokenArrayType<wchar_t>> TokenizerW(const std::wstring& Input);
SharedTablePointer ParserW(SharedPointer<TokenArrayType<wchar_t>> Input);

_SC_END

/**
 * @brief SC Parser constructed from streams.
 */
class ISCStream
{
public:
    __StelCXX_Text_Codecvt_65001 _DefDecoder = __StelCXX_Text_Codecvt_65001();
    __StelCXX_Text_Codecvt_Base& Decoder = _DefDecoder;

    std::istream& input;
    ISCStream(std::istream& is) : input(is){}
    ISCStream& operator=(const ISCStream& is) = delete;

    void SetDecoder(__StelCXX_Text_Codecvt_Base& NewDecoder) {Decoder = NewDecoder;}

    _SC SharedTablePointer Parse()const;
};

/**
 * @brief SC Parser wide-char version.
 */
class WISCStream
{
public:
    std::wistream& input;
    WISCStream(std::wistream& is) : input(is){}
    WISCStream& operator=(const WISCStream& is) = delete;
    _SC SharedTablePointer Parse()const;
};

_SC SharedTablePointer ParseFile(std::filesystem::path FileName)noexcept(false);
_SC SharedTablePointer ParseFileW(std::filesystem::path FileName)noexcept(false);

#ifdef GetObject
#undef GetObject
#endif

template<typename _SEObject> requires std::is_base_of_v<SEObject, _SEObject>
_SEObject GetObject(_SC SharedTablePointer Table, ustring Name);

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
