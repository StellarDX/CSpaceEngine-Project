///////////////////////////////////////////////////////
//                     SC Parser                     //
// ------------------------------------------------- //
//                                                   //
//         A loader for Space Engine sc file         //
//                                                   //
///////////////////////////////////////////////////////

/**
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

/**
 * @brief SC Parser constructed from streams.
 */
class ISCStream
{
public:
    std::istream& input;
    ISCStream(std::istream& is) : input(is){}
    ISCStream& operator=(const ISCStream& is) = delete;
    SharedTablePointer Parse()const;
};

void SkipComments(std::string& Input);
SharedPointer<TokenArrayType<char>> Tokenizer(const std::string& Input);
SharedTablePointer Parser(SharedPointer<TokenArrayType<char>> Input);

/**
 * @brief SC Parser wide-char version.
 */
class WISCStream
{
public:
    std::wistream& input;
    WISCStream(std::wistream& is) : input(is){}
    WISCStream& operator=(const WISCStream& is) = delete;
    WSharedTablePointer Parse()const;
};

void SkipCommentsW(std::wstring& Input);
SharedPointer<TokenArrayType<wchar_t>> TokenizerW(const std::wstring& Input);
SharedTablePointer ParserW(SharedPointer<TokenArrayType<wchar_t>> Input);

_SC_END

_SC SharedTablePointer ParseFile(std::filesystem::path FileName)noexcept(false);
_SC WSharedTablePointer ParseFileW(std::filesystem::path FileName)noexcept(false);

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
