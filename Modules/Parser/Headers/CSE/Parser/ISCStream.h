///////////////////////////////////////////////////////
//                     SC Parser                     //
// ------------------------------------------------- //
//                                                   //
//         A loader for Space Engine sc file         //
//                                                   //
///////////////////////////////////////////////////////

/*
    CSpaceEngine SC Parser.
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

#include <CSE/Parser/SCSBase.h>
#include <CSE/Parser/StelCXXRes/LRParser.h>
#include <queue>
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

#define LALR1 0
#define LR1   1
#define IELR1 2

#if (SC_PARSER_LR == LALR1)

#define SCFINAL    14  // State number of the termination state.
#define SCLAST     69  // Last index in SCTABLE. (Unused)
#define SCNTOKENS  14  // Number of terminals.
#define SCNNTS     9   // Number of nonterminals.
#define SCNRULES   24  // Number of rules.
#define SCNSTATES  39  // Number of states.

#elif (SC_PARSER_LR == LR1)

// Definations from YACC Parser
#define SCFINAL    14  // State number of the termination state.
#define SCLAST     481 // Last index in TABLE. (Unused)
#define SCNTOKENS  14  // Number of terminals.
#define SCNNTS     9   // Number of nonterminals.
#define SCNRULES   24  // Number of rules.
#define SCNSTATES  85  // Number of states.

#elif (SC_PARSER_LR == IELR1)
#error IELR(1) algorithom is deprecated because its parsing table is same as LALR(1), use LALR(1) instead.
#else
#error Invalid parser option.
#endif

// Tables
extern const __LR_Parser_Base<ucs2_t>::GrammaTableType __SE_Grammar_Production_Table;
extern const __LR_Parser_Base<ucs2_t>::StatesType __SE_State_Table;

// Pre-processing
void SkipComments(ustring& Input);

// SC Lex
class __SE_Lexicality
{
public:
    ustringlist VariableList; // Identifiers match element in this list will be marked as variable.

    __SE_Lexicality() {}

    void AddVariable(ustringlist Variable);

    static int IsPunctuator(ucs2_t _C);
    static int IsOperator(ucs2_t _C);

    static void ConsumeWhiteSpace(ustring::const_iterator& it, const ustring::const_iterator& end,
        uint64* line, uint64* column);
    static void ParseOperator(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);
    static void ParseIdentifier(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);
    static void ParseNumber(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column, int* Base);
    static void ParseString(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);

    void Run(const ustring& Input, TokenArrayType* Output);
};

// SC Parser
class __SE_General_Parser : public __StelCXX_LR_Parser<ucs2_t>
{
public:
    using _Mybase = __StelCXX_LR_Parser<ucs2_t>;

    __SE_General_Parser() : _Mybase(__SE_Grammar_Production_Table, __SE_State_Table) {}

    ustring TokenToString(TokenArrayType Tokens);
    SharedPointer<SCSTable> MakeTable(std::stack<SCSTable::SCKeyValue>& SubTableTempStack);
    void MakeSubMatrix(ValueType& ExpressionBuffer, ValueType SubMatrix);
    void MoveSubMateix(ValueType& ExpressionBuffer);
    void ThrowError(size_t CurrentState, ivec2 Pos, std::string Msg);

    SharedPointer<SCSTable> Run(TokenArrayType Tokens) noexcept(0);
};

_SC_END

/**
 * @brief SC Parser constructed from streams.
 */
class ISCStream
{
public:
    __StelCXX_UString_Codec_65001 _DefDecoder = __StelCXX_UString_Codec_65001();
    __StelCXX_UString_Codec& Decoder = _DefDecoder;
    ustringlist VariableList;

    std::istream& input;
    ISCStream(std::istream& is) : input(is){}
    ISCStream& operator=(const ISCStream& is) = delete;

    void SetDecoder(__StelCXX_UString_Codec& NewDecoder) {Decoder = NewDecoder;}

    _SC SharedTablePointer Parse()const;
};

_SC SharedTablePointer ParseFile(std::filesystem::path FileName)noexcept(false);

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
