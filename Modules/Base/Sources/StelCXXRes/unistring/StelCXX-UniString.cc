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
    ucs2_t* wc = (ucs2_t*)malloc(size * 6);
    memset(wc, 0, size * 6);
    __Decoder_65001_16(istr, wc, size);
    __StelCXX_UniString wstr(wc);
    free(wc);
    return wstr;
}

std::string __StelCXX_UString_Codec_65001::Encode(__StelCXX_UniString str)const
{
    auto istr = str.c_str();
    auto size = str.size();
    char* cstr = (char*)malloc(size * 6);
    memset(cstr, 0, size * 6);
    __Encoder_65001_16(istr, cstr, size);
    std::string sstr(cstr);
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

__StelCXX_UniString operator+(const std::wstring& __lhs, const __StelCXX_UniString& __rhs)
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

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const std::wstring& __rhs)
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

__StelCXX_UniString operator+(const ucs2_t& __lhs, const __StelCXX_UniString& __rhs)
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

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const ucs2_t& __rhs)
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

__StelCXX_UniString operator+(const ucs2_t* __lhs, const __StelCXX_UniString& __rhs)
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

__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs)
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

bool operator==(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs)
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

std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs) noexcept
{
    return static_cast<std::weak_ordering>(__lhs.compare(__rhs) <=> 0);
}
