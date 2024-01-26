/*
    Skeleton implementation Unicode text encoder/decoder in C++

    Copyright (C) 2023, 2024
    StellarDX Astronomy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _SCS_CODEC
#define _SCS_CODEC

#include <string>
#include <stdexcept>

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

#if !defined(_MSC_VER) && !defined(__interface)
#define __interface struct
#endif

// Return code if invalid input after a shift sequence of n bytes was read. (xxx_mbtowc)
#define RET_SHIFT_ILSEQ(n)     (-1-2*(n))
// Return code if invalid.
#define RET_ILSEQ              RET_SHIFT_ILSEQ(0)
// Return code if only a shift sequence of n bytes was read.
#define RET_TOOFEW(n)          (-2-2*(n))
// Retrieve the n from the encoded RET_... value.
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)
// Maximum value of n that may be used as argument to RET_SHIFT_ILSEQ or RET_TOOFEW.
#define RET_COUNT_MAX          ((INT_MAX / 2) - 1)
// Return code if invalid. (xxx_wctomb)
#define RET_ILUNI              -1

// Code convertor interface
__interface __StelCXX_Text_Codecvt_Base
{
    virtual std::wstring ConvertToUnicode(std::string in, int* state) const = 0;
    virtual std::string ConvertFromUnicode(std::wstring in, int* state) const = 0;

    virtual int CodePage()const = 0;

    std::wstring ToUnicode(std::string ByteArray)
    {
        int ReturnValue;
        std::wstring Result = ConvertToUnicode(ByteArray, &ReturnValue);
        if (ReturnValue < 0) {throw std::runtime_error("Invalid characters in string.");}
        return Result;
    }

    std::string FromUnicode(std::wstring Str)
    {
        int ReturnValue;
        std::string Result = ConvertFromUnicode(Str, &ReturnValue);
        if (ReturnValue < 0) {throw std::runtime_error("Invalid characters in string.");}
        return Result;
    }
};

// CP65001 (UTF-8)
class __StelCXX_Text_Codecvt_65001 : public __StelCXX_Text_Codecvt_Base
{
public:
    int CodePage()const override {return 65001;}
    std::wstring ConvertToUnicode(std::string in, int* state) const override;
    std::string ConvertFromUnicode(std::wstring in, int* state) const override;
};

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
_STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif
