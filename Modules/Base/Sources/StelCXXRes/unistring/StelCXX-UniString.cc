/*
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

#include "CSE/Base/StelCXXRes/StelCXX-UniString.h"
#include <cstring>

// Member functions for Unicode encoders

_EXTERN_C

__StelCXX_UniString __StelCXX_UString_Codec_65001::Decode(std::string str)const
{
    auto istr = str.c_str();
    auto size = str.size();
    ucs2_t* wc = (ucs2_t*)malloc(size * 6 + 4);
    memset(wc, 0, size * 6 + 4);
    __Decoder_65001_16(istr, wc, size);
    __StelCXX_UniString wstr(wc);
    free(wc);
    return wstr;
}

std::string __StelCXX_UString_Codec_65001::Encode(__StelCXX_UniString str)const
{
    auto istr = str.c_str();
    auto size = str.size();
    char* cstr = (char*)malloc(size * 6 + 4);
    memset(cstr, 0, size * 6 + 4);
    __Encoder_65001_16(istr, cstr, size);
    std::string sstr(cstr);
    free(cstr);
    return sstr;
}

__StelCXX_UniString_Big __StelCXX_UString_Codec_Big_65001::Decode(std::string str) const
{
    auto istr = str.c_str();
    auto size = str.size();
    ucs4_t* wc = (ucs4_t*)malloc(size * 6 + 4);
    memset(wc, 0, size * 6 + 4);
    __Decoder_65001_32(istr, wc, size);
    __StelCXX_UniString_Big wstr(wc);
    free(wc);
    return wstr;
}

std::string __StelCXX_UString_Codec_Big_65001::Encode(__StelCXX_UniString_Big str) const
{
    auto istr = str.c_str();
    auto size = str.size();
    char* cstr = (char*)malloc(size * 6 + 4);
    memset(cstr, 0, size * 6 + 4);
    __Encoder_65001_32(istr, cstr, size);
    std::string sstr(cstr);
    free(cstr);
    return sstr;
}

__StelCXX_UniString_Big __StelCXX_UString_Codec_Big_936::Decode(std::string str) const
{
    auto istr = str.c_str();
    auto size = str.size();
    ucs4_t* wc = (ucs4_t*)malloc(size * 4 + 4);
    memset(wc, 0, size * 4 + 4);
    __Decoder_936_32((const unsigned char*)istr, wc, size);
    __StelCXX_UniString_Big wstr(wc);
    free(wc);
    return wstr;
}

std::string __StelCXX_UString_Codec_Big_936::Encode(__StelCXX_UniString_Big str) const
{
    auto istr = str.c_str();
    auto size = str.size();
    unsigned char* cstr = (unsigned char*)malloc(size * 4 + 4);
    memset(cstr, 0, size * 4 + 4);
    __Encoder_936_32(istr, cstr, size);
    std::string sstr((char*)cstr);
    free(cstr);
    return sstr;
}

_END_EXTERN_C

// String literals
__StelCXX_UniString operator+(const std::string& __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString::_Mybase& __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __str.append(__rhs);
    return __str;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_Mybase& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __StelCXX_UniString __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const std::string& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __StelCXX_UniString __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString operator+(const char& __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str;
    __str.push_back(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString::_CharT& __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str;
    __str.push_back(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __str.push_back(__rhs);
    return __str;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __str.push_back(__rhs);
    return __str;
}

__StelCXX_UniString operator+(const char* __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString::_CharT* __lhs, const __StelCXX_UniString& __rhs)
{
    __StelCXX_UniString __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char* __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __StelCXX_UniString __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs)
{
    __StelCXX_UniString __str(__lhs);
    __StelCXX_UniString __str2(__rhs);
    return __str + __str2;
}

bool operator==(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs)
{
    return !__lhs.compare(__rhs);
}

bool operator==(const __StelCXX_UniString& __lhs, const char* __rhs)
{
    return __lhs == __StelCXX_UniString(__rhs);
}

bool operator==(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs)
{
    return __lhs == __StelCXX_UniString(__rhs);
}

std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs) noexcept
{
    return static_cast<std::weak_ordering>(__lhs.compare(__rhs) <=> 0);
}

std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const char* __rhs) noexcept
{
    __StelCXX_UniString ustr(__rhs);
    return static_cast<std::weak_ordering>(__lhs.compare(ustr) <=> 0);
}

std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs) noexcept
{
    return static_cast<std::weak_ordering>(__lhs.compare(__rhs) <=> 0);
}

__StelCXX_UniString_Big operator+(const std::string& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_Mybase& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __str.append(__rhs);
    return __str;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_Mybase& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __StelCXX_UniString_Big __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const std::string& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __StelCXX_UniString_Big __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString_Big operator+(const char& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str;
    __str.push_back(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_CharT& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str;
    __str.push_back(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const char& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __str.push_back(__rhs);
    return __str;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __str.push_back(__rhs);
    return __str;
}

__StelCXX_UniString_Big operator+(const char* __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_CharT* __lhs, const __StelCXX_UniString_Big& __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    return __str + __rhs;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const char* __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __StelCXX_UniString_Big __str2(__rhs);
    return __str + __str2;
}

__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs)
{
    __StelCXX_UniString_Big __str(__lhs);
    __StelCXX_UniString_Big __str2(__rhs);
    return __str + __str2;
}

bool operator==(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs)
{
    return !__lhs.compare(__rhs);
}

bool operator==(const __StelCXX_UniString_Big& __lhs, const char* __rhs)
{
    return __lhs == __StelCXX_UniString_Big(__rhs);
}

bool operator==(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs)
{
    return __lhs == __StelCXX_UniString_Big(__rhs);
}

std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs) noexcept
{
    return static_cast<std::weak_ordering>(__lhs.compare(__rhs) <=> 0);
}

std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const char* __rhs) noexcept
{
    __StelCXX_UniString_Big ustr(__rhs);
    return static_cast<std::weak_ordering>(__lhs.compare(ustr) <=> 0);
}

std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs) noexcept
{
    return static_cast<std::weak_ordering>(__lhs.compare(__rhs) <=> 0);
}
