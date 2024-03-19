/*
    Skeleton implementation of Unicode strings by StellarDX
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

#ifndef _UNISTRING_
#define _UNISTRING_

#include <string>
#include <cstdint>

#ifndef _EXTERN_C
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#endif

#if !defined(_MSC_VER) && !defined(__interface)
#define __interface struct
#endif

// Definations of return value of decoders

// Return code if invalid input after a shift sequence of n bytes was read.
#define DCRET_SHIFT_ILSEQ(n)   (-1-2*(n))
// Return code if invalid.
#define DCRET_ILSEQ            DCRET_SHIFT_ILSEQ(0)
// Return code if only a shift sequence of n bytes was read.
#define DCRET_TOOFEW(n)        (-2-2*(n))
// Retrieve the n from the encoded RET_... value.
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(DCRET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(DCRET_TOOFEW(0) - (r)) / 2)
// Maximum value of n that may be used as argument to RET_SHIFT_ILSEQ or RET_TOOFEW.
#define DCRET_COUNT_MAX        ((INT_MAX / 2) - 1)
// Return code if invalid. (xxx_wctomb)
#define DCRET_ILUNI            -1

// Universal Coded Character Set
using byte_t = char;     // Single byte, for UTF-8 and other encodings
using ucs2_t = wchar_t;  // UCS2, for UTF-16
using ucs4_t = char32_t; // UCS4, for UTF-32

struct __StelCXX_UniString;
struct __StelCXX_UniString_Big;

struct __StelCXX_UString_Codec
{
    virtual uint16_t CodePage() = 0;
    virtual __StelCXX_UniString Decode(std::string str)const = 0;
    virtual std::string Encode(__StelCXX_UniString str)const = 0;
};

struct __StelCXX_UString_Codec_65001 : public __StelCXX_UString_Codec
{
    uint16_t CodePage() {return 65001;}
    __StelCXX_UniString Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString str)const;
};

struct __StelCXX_UniString : public std::wstring
{
    using _Mybase = std::wstring;
    using _CharT  = ucs2_t;
    using _Traits = std::char_traits<_CharT>;
    using _Alloc  = std::allocator<_CharT>;

    constexpr static const __StelCXX_UString_Codec_65001 DefEncode = __StelCXX_UString_Codec_65001();

    // Constructors

    __StelCXX_UniString() : _Mybase() {}
    explicit __StelCXX_UniString(const _Alloc& __a)  : _Mybase(__a) {}
    __StelCXX_UniString(const __StelCXX_UniString& __str) : _Mybase(__str) {}
    __StelCXX_UniString(const __StelCXX_UniString& __str, size_type __pos,
        const _Alloc& __a = _Alloc()) : _Mybase(__str, __pos, __a) {}
    __StelCXX_UniString(const __StelCXX_UniString& __str, size_type __pos,
        size_type __n) :  _Mybase(__str, __pos, __n) {}
    __StelCXX_UniString(const __StelCXX_UniString& __str, size_type __pos,
        size_type __n, const _Alloc& __a) : _Mybase(__str, __pos, __n, __a) {}
    __StelCXX_UniString(const _CharT* __s, size_type __n,
        const _Alloc& __a = _Alloc()) : _Mybase(__s, __n, __a) {}
    __StelCXX_UniString(const _CharT* __s, const _Alloc& __a = _Alloc())
        :  _Mybase(__s, __a) {}
    __StelCXX_UniString(size_type __n, _CharT __c, const _Alloc& __a = _Alloc())
        : _Mybase(__n, __c, __a) {}
    //__StelCXX_UniString(__StelCXX_UniString&& __str) : _Mybase(__str) {}
    __StelCXX_UniString(std::initializer_list<_CharT> __l, const _Alloc& __a = _Alloc())
        : _Mybase(__l, __a) {}
    __StelCXX_UniString(const __StelCXX_UniString& __str, const _Alloc& __a)
        : _Mybase(__str, __a) {}
    __StelCXX_UniString(__StelCXX_UniString&& __str, const _Alloc& __a)
        : _Mybase(__str, __a) {}
    template<typename _InputIterator>
    __StelCXX_UniString(_InputIterator __beg, _InputIterator __end,
        const _Alloc& __a = _Alloc()) : _Mybase(__beg, __end, __a) {}
    template<typename _Tp>
    __StelCXX_UniString(const _Tp& __t, size_type __pos, size_type __n,
        const _Alloc& __a = _Alloc()) : _Mybase(__t, __pos, __n, __a) {}
    template<typename _Tp>
    explicit __StelCXX_UniString(const _Tp& __t, const _Alloc& __a = _Alloc())
        : _Mybase(__t, __a) {}

    // Encoder and decoder

    std::string Encode(const __StelCXX_UString_Codec& Encoder = DefEncode)const
    {
        return Encoder.Encode(*this);
    }

    static __StelCXX_UniString Decode(const std::string& Str, const __StelCXX_UString_Codec& Decoder = DefEncode)
    {
        return Decoder.Decode(Str);
    }

    // STD string compatibilities

    std::string ToStdString()const {return Encode();}
    operator std::string() {return ToStdString();}
    std::wstring ToStdWString()const {return *this;}
    //operator std::wstring() {return *this;}
    //std::u8string ToU8String();
    //std::u16string ToU16String();
    //std::u32string ToU32String();

    __StelCXX_UniString(const char* __str) : __StelCXX_UniString(Decode(__str)) {}
    __StelCXX_UniString(const std::string& __str) : __StelCXX_UniString(Decode(__str)) {}
    __StelCXX_UniString& operator=(const char* __str)
    {
        *this = Decode(__str);
        return *this;
    }
    __StelCXX_UniString& operator=(const std::string& __str)
    {
        *this = Decode(__str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const char& __c)
    {
        auto str = std::string();
        str.push_back(__c);
        auto ustr = Decode(str);
        this->append(ustr);
        return *this;
    }
    __StelCXX_UniString& operator+=(const char* __str)
    {
        auto ustr = Decode(__str);
        this->append(ustr);
        return *this;
    }
    __StelCXX_UniString& operator+=(const std::string& __str)
    {
        auto ustr = Decode(__str);
        this->append(ustr);
        return *this;
    }

    //__StelCXX_UniString(const std::wstring& __str) {*this = __str;}
    __StelCXX_UniString& operator=(const ucs2_t* __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString& operator=(const std::wstring& __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const ucs2_t& __c)
    {
        auto str = std::wstring();
        str.push_back(__c);
        this->append(str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const ucs2_t* __str)
    {
        this->append(__str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const std::wstring& __str)
    {
        this->append(__str);
        return *this;
    }
};

__StelCXX_UniString operator+(const std::string& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const std::wstring& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const std::wstring& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const std::string& __rhs);
__StelCXX_UniString operator+(const char& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const ucs2_t& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const ucs2_t& __rhs);
__StelCXX_UniString operator+(const char* __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const ucs2_t* __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char* __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs);

bool operator==(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs);
bool operator==(const __StelCXX_UniString& __lhs, const char* __rhs);
bool operator==(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs);
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const char* __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const ucs2_t* __rhs) noexcept;

/*__interface __StelCXX_UString_Codec_Big
{
    virtual uint16_t CodePage() = 0;
    virtual __StelCXX_UniString_Big Decode(std::string str)const = 0;
    virtual std::string Encode(__StelCXX_UniString_Big str)const = 0;
};*/

/*struct __StelCXX_UString_Codec_Big_65001 : public __StelCXX_UString_Codec_Big
{
    uint16_t CodePage() {return 65001;}
    __StelCXX_UniString_Big Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString_Big str)const;
};*/

/*struct __StelCXX_UniString_Big : public std::u32string
{
    using _Mybase = std::u32string;
    using _CharT  = ucs4_t;
    using _Traits = std::char_traits<_CharT>;
    using _Alloc  = std::allocator<_CharT>;

    static const __StelCXX_UString_Codec_Big_65001 DefEncode;

    __StelCXX_UniString_Big() : _Mybase() {}
    explicit __StelCXX_UniString_Big(const _Alloc& __a)  : _Mybase(__a) {}
    __StelCXX_UniString_Big(const __StelCXX_UniString_Big& __str) : _Mybase(__str) {}
    __StelCXX_UniString_Big(const __StelCXX_UniString_Big& __str, size_type __pos,
        const _Alloc& __a = _Alloc()) : _Mybase(__str, __pos, __a) {}
    __StelCXX_UniString_Big(const __StelCXX_UniString_Big& __str, size_type __pos,
        size_type __n) :  _Mybase(__str, __pos, __n) {}
    __StelCXX_UniString_Big(const __StelCXX_UniString_Big& __str, size_type __pos,
        size_type __n, const _Alloc& __a) : _Mybase(__str, __pos, __n, __a) {}
    __StelCXX_UniString_Big(const _CharT* __s, size_type __n,
        const _Alloc& __a = _Alloc()) : _Mybase(__s, __n, __a) {}
    __StelCXX_UniString_Big(const _CharT* __s, const _Alloc& __a = _Alloc())
        :  _Mybase(__s, __a) {}
    __StelCXX_UniString_Big(size_type __n, _CharT __c, const _Alloc& __a = _Alloc())
        : _Mybase(__n, __c, __a) {}
    __StelCXX_UniString_Big(__StelCXX_UniString_Big&& __str) : _Mybase(__str) {}
    __StelCXX_UniString_Big(std::initializer_list<_CharT> __l, const _Alloc& __a = _Alloc())
        : _Mybase(__l, __a) {}
    __StelCXX_UniString_Big(const __StelCXX_UniString_Big& __str, const _Alloc& __a)
        : _Mybase(__str, __a) {}
    __StelCXX_UniString_Big(__StelCXX_UniString_Big&& __str, const _Alloc& __a)
        : _Mybase(__str, __a) {}
    template<typename _InputIterator>
    __StelCXX_UniString_Big(_InputIterator __beg, _InputIterator __end,
        const _Alloc& __a = _Alloc()) : _Mybase(__beg, __end, __a) {}
    template<typename _Tp>
    __StelCXX_UniString_Big(const _Tp& __t, size_type __pos, size_type __n,
        const _Alloc& __a = _Alloc()) : _Mybase(__t, __pos, __n, __a) {}
    template<typename _Tp>
    explicit __StelCXX_UniString_Big(const _Tp& __t, const _Alloc& __a = _Alloc())
        : _Mybase(__t, __a) {}

    std::string Encode(const __StelCXX_UString_Codec_Big& Encoder = DefEncode)
    {
        return Encoder.Encode(*this);
    }

    static __StelCXX_UniString_Big Decode(const std::string& Str, const __StelCXX_UString_Codec_Big& Decoder)
    {
        return Decoder.Decode(Str);
    }

    std::string ToStdString() {return Encode();}
    operator std::string() {return ToStdString();}
    std::string ToStdWString();
    operator std::wstring();
    //std::u8string ToU8String();
    //std::u16string ToU16String();
    //std::u32string ToU32String() {return *this;}
};*/

_EXTERN_C
int __Decoder_65001_16(const char* istr, ucs2_t* ostr, size_t size);
int __Encoder_65001_16(const ucs2_t* istr, char* ostr, size_t size);
_END_EXTERN_C

#endif
