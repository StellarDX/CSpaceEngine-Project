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
#include <stdexcept>

#ifndef _EXTERN_C
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#endif

#if !defined(_MSC_VER) && !defined(__interface)
#define __interface struct
#endif

#if !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define USTR_OS_WIN32
#define USTR_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#define USTR_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#define USTR_OS_LINUX
#else
#error "This module has not been ported to this OS."
#endif

#if defined(USTR_OS_WIN32) || defined(USTR_OS_WIN64)
#define USTR_OS_WIN
#endif

#if defined(USTR_OS_WIN)
#undef USTR_OS_UNIX
#elif !defined(USTR_OS_UNIX)
#define USTR_OS_UNIX
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
#if defined(USTR_OS_WIN32)
using byte_t = char;     // Single byte, for UTF-8 and other encodings
using ucs2_t = wchar_t;  // UCS2, for UTF-16
using ucs4_t = char32_t; // UCS4, for UTF-32
#elif defined(USTR_OS_LINUX)
using byte_t = char;     // Single byte, for UTF-8 and other encodings
using ucs2_t = char16_t; // UCS2, for UTF-16
using ucs4_t = wchar_t;  // UCS4, for UTF-32
#endif

struct __StelCXX_UniString;
struct __StelCXX_UniString_Big;

__interface __StelCXX_UString_Codec
{
    virtual uint16_t CodePage()const = 0;
    virtual __StelCXX_UniString Decode(std::string str)const = 0;
    virtual std::string Encode(__StelCXX_UniString str)const = 0;
};

struct __StelCXX_UString_Codec_65001 : public __StelCXX_UString_Codec
{
    uint16_t CodePage()const {return 65001;}
    __StelCXX_UniString Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString str)const;
};

#if defined(USTR_OS_WIN32)
struct __StelCXX_UniString : public std::wstring
#elif defined(USTR_OS_LINUX)
struct __StelCXX_UniString : public std::u16string
#endif
{
    #if defined(USTR_OS_WIN32)
    using _Mybase = std::wstring;
    #elif defined(USTR_OS_LINUX)
    using _Mybase = std::u16string;
    #endif

    using _CharT  = ucs2_t;
    using _Traits = std::char_traits<_CharT>;
    using _Alloc  = std::allocator<_CharT>;

    constexpr static const __StelCXX_UString_Codec_65001 DefEncode = __StelCXX_UString_Codec_65001();
    static const uint16_t CodePage = 1200;

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

    std::wstring ToStdWString()const
    {
        #if defined(USTR_OS_WIN32)
        return *this;
        #elif defined(USTR_OS_LINUX)
        throw std::runtime_error("ToStdWString is not implemented in Linux.");
        #endif
    }
    operator std::wstring() {return ToStdWString();}

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

    //__StelCXX_UniString(const _Mybase& __str) {*this = __str;}
    __StelCXX_UniString& operator=(const _CharT* __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString& operator=(const _Mybase& __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const _CharT& __c)
    {
        auto str = _Mybase();
        str.push_back(__c);
        this->append(str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const _CharT* __str)
    {
        this->append(__str);
        return *this;
    }
    __StelCXX_UniString& operator+=(const _Mybase& __str)
    {
        this->append(__str);
        return *this;
    }
};

__StelCXX_UniString operator+(const std::string& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString::_Mybase& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_Mybase& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const std::string& __rhs);
__StelCXX_UniString operator+(const char& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString::_CharT& __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT& __rhs);
__StelCXX_UniString operator+(const char* __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString::_CharT* __lhs, const __StelCXX_UniString& __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const char* __rhs);
__StelCXX_UniString operator+(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs);

bool operator==(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs);
bool operator==(const __StelCXX_UniString& __lhs, const char* __rhs);
bool operator==(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs);
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const __StelCXX_UniString& __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const char* __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString& __lhs, const __StelCXX_UniString::_CharT* __rhs) noexcept;


__interface __StelCXX_UString_Codec_Big
{
    virtual uint16_t CodePage()const = 0;
    virtual __StelCXX_UniString_Big Decode(std::string str)const = 0;
    virtual std::string Encode(__StelCXX_UniString_Big str)const = 0;
};

struct __StelCXX_UString_Codec_Big_65001 : public __StelCXX_UString_Codec_Big
{
    uint16_t CodePage()const {return 65001;}
    __StelCXX_UniString_Big Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString_Big str)const;
};

struct __StelCXX_UString_Codec_Big_1200 : public __StelCXX_UString_Codec_Big
{
    bool BigEndian = 0;

    uint16_t CodePage()const {return 1200 + BigEndian;}
    __StelCXX_UniString_Big Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString_Big str)const;
};

struct __StelCXX_UString_Codec_Big_936 : public __StelCXX_UString_Codec_Big
{
    uint16_t CodePage()const {return 936;}
    __StelCXX_UniString_Big Decode(std::string str)const;
    std::string Encode(__StelCXX_UniString_Big str)const;
};

#if defined(USTR_OS_WIN32)
struct __StelCXX_UniString_Big : public std::u32string
#elif defined(USTR_OS_LINUX)
struct __StelCXX_UniString_Big : public std::wstring
#endif
{
    #if defined(USTR_OS_WIN32)
    using _Mybase = std::u32string;
    #elif defined(USTR_OS_LINUX)
    using _Mybase = std::wstring;
    #endif

    using _CharT  = ucs4_t;
    using _Traits = std::char_traits<_CharT>;
    using _Alloc  = std::allocator<_CharT>;

    constexpr static const __StelCXX_UString_Codec_Big_65001 DefEncode = __StelCXX_UString_Codec_Big_65001();
    static const uint16_t CodePage = 12000;

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
    //__StelCXX_UniString_Big(__StelCXX_UniString_Big&& __str) : _Mybase(__str) {}
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

    static __StelCXX_UniString_Big Decode(const std::string& Str, const __StelCXX_UString_Codec_Big& Decoder = DefEncode)
    {
        return Decoder.Decode(Str);
    }

    std::string ToStdString() {return Encode();}
    operator std::string() {return ToStdString();}

    std::wstring ToStdWString()
    {
        #if defined(USTR_OS_WIN32)
        throw std::runtime_error("ToStdWString is not implemented in Windows.");
        #elif defined(USTR_OS_LINUX)
        return *this;
        #endif
    }
    operator std::wstring() {return ToStdWString();}

    //std::u8string ToU8String();
    //std::u16string ToU16String();
    //std::u32string ToU32String() {return *this;}

    __StelCXX_UniString_Big(const char* __str) : __StelCXX_UniString_Big(Decode(__str)) {}
    __StelCXX_UniString_Big(const std::string& __str) : __StelCXX_UniString_Big(Decode(__str)) {}
    __StelCXX_UniString_Big& operator=(const char* __str)
    {
        *this = Decode(__str);
        return *this;
    }
    __StelCXX_UniString_Big& operator=(const std::string& __str)
    {
        *this = Decode(__str);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const char& __c)
    {
        auto str = std::string();
        str.push_back(__c);
        auto ustr = Decode(str);
        this->append(ustr);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const char* __str)
    {
        auto ustr = Decode(__str);
        this->append(ustr);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const std::string& __str)
    {
        auto ustr = Decode(__str);
        this->append(ustr);
        return *this;
    }

    //__StelCXX_UniString(const _Mybase& __str) {*this = __str;}
    __StelCXX_UniString_Big& operator=(const _CharT* __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString_Big& operator=(const _Mybase& __str)
    {
        this->assign(__str);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const _CharT& __c)
    {
        auto str = _Mybase();
        str.push_back(__c);
        this->append(str);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const _CharT* __str)
    {
        this->append(__str);
        return *this;
    }
    __StelCXX_UniString_Big& operator+=(const _Mybase& __str)
    {
        this->append(__str);
        return *this;
    }
};

__StelCXX_UniString_Big operator+(const std::string& __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_Mybase& __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_Mybase& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const std::string& __rhs);
__StelCXX_UniString_Big operator+(const char& __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_CharT& __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const char& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT& __rhs);
__StelCXX_UniString_Big operator+(const char* __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big::_CharT* __lhs, const __StelCXX_UniString_Big& __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const char* __rhs);
__StelCXX_UniString_Big operator+(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs);

bool operator==(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs);
bool operator==(const __StelCXX_UniString_Big& __lhs, const char* __rhs);
bool operator==(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs);
std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big& __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const char* __rhs) noexcept;
std::weak_ordering operator<=>(const __StelCXX_UniString_Big& __lhs, const __StelCXX_UniString_Big::_CharT* __rhs) noexcept;

#if defined(USTR_OS_WIN32)
#define __stelcxx_codec_type __StelCXX_UString_Codec
#define __stelcxx_codec(cp) __StelCXX_UString_Codec_##cp
#elif defined(USTR_OS_LINUX)
#define __stelcxx_codec_type __StelCXX_UString_Codec_Big
#define __stelcxx_codec(cp) __StelCXX_UString_Codec_Big_##cp
#endif

_EXTERN_C
int __Decoder_65001_16(const char* istr, ucs2_t* ostr, size_t size);
int __Encoder_65001_16(const ucs2_t* istr, char* ostr, size_t size);
int __Decoder_65001_32(const char* istr, ucs4_t* ostr, size_t size);
int __Encoder_65001_32(const ucs4_t* istr, char* ostr, size_t size);
_END_EXTERN_C

#endif
