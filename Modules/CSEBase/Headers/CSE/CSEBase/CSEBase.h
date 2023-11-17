// -*- C++ -*- definitions for internal modules and values
/***
*       CSpaceEngine Astronomy Library
*
*       Copyright (C) StellarDX Astronomy.
*
*       This program is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 2 of the License, or
*       (at your option) any later version.
*
*       This program is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*       GNU General Public License for more details.
*
*       You should have received a copy of the GNU General Public License along
*       with this program; If not, see <https://www.gnu.org/licenses/>.
*
****/

#pragma once

#ifndef __CSE_BASE__
#define __CSE_BASE__

#define WIN32_LEAN_AND_MEAN
#include <version>
#include <string>
#include <ostream>

// Compiler version definations
#if defined _MSC_VER
#define COMPILER_VERSION std::string("MSC " + std::to_string(_MSC_FULL_VER) + "(" + std::to_string(_MSC_BUILD) + ")")
#elif defined __GNUG__
#if defined __clang__
#define COMPILER_VERSION std::string("Clang " + (std::string)__clang_version__)
#else
#define COMPILER_VERSION std::string("GPP " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__))
#endif
#elif defined __ARMCC_VERSION
#define COMPILER_VERSION std::string("ARMCC " + std::to_string(__ARMCC_VERSION))
#else
#define COMPILER_VERSION std::string("Unknown")
#endif

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

// CMake boolean definations
#define ON true
#define OFF false

// CSE Namespace difinations
#define _CSE_BEGIN namespace cse {
#define _CSE_END   }
#define _CSE       cse::

#ifndef _CONSTEXPR20
#define _CONSTEXPR20 constexpr
#endif

#ifndef _EXTERN_C
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#endif

_CSE_BEGIN

#include <CSE/CSEBase/sysdeps/IEEE754.h>

// CSE Type Declearations
using int64              = long long;          // int
using float64            = double;             // real
using uint64             = unsigned long long; // unsigned int

// Write bytes of buffer into return value with another type
template<typename genTypeA = uint64, typename genTypeB = float64>
inline constexpr genTypeB modtype(genTypeA Value)
{
    union _Buf{genTypeA Src; genTypeB Dst;} _Buffer;
    _Buffer.Src = Value;
    return _Buffer.Dst;
}

// CSE Debugger
class CSEDebugger
{
    std::ostream& Output;
    std::string gettime();

public:
    bool __LogTimeStamp;
    bool __LogThreadStamp;
    int __LogLevel;

    enum DebugLevel{INFO, PRINT, WARNING, ERROR, FATAL};

    CSEDebugger(std::ostream& os, bool LTime = 1, bool LThread = 1, bool LLevel = 2)
        : Output(os), __LogTimeStamp(LTime), __LogThreadStamp(LThread), __LogLevel(LLevel){}

    CSEDebugger& operator=(const CSEDebugger&) = delete;

    // Print Debug information
    template<typename genType>
    void operator()(std::string Thread, DebugLevel Type, genType Details)
    {
        if (Type == PRINT){ Output << Details; }
        else if (__LogLevel && (__LogLevel == 2 || Type))
        {
            if (__LogTimeStamp) {Output << gettime() << ' ';}
            Output << '[';
            if (__LogThreadStamp) {Output << Thread << '/' ;}
            switch(Type)
            {
            case INFO:
                Output << "INFO";
                break;
            case WARNING:
                Output << "WARNING";
                break;
            case ERROR:
                Output << "ERROR";
                break;
            case FATAL:
                Output << "FATAL";
                break;
            case PRINT:
                Output << "TEXT"; // Unreachable, make the compiler happy.
                break;
            }
            Output << "] ";
            Output << Details << '\n';
        }
    }

    // StellarDX: I think "print", "println" and "printf" functions
    // can be added in this scope, Isn't it, StellarDLy?
};

extern CSEDebugger CSESysDebug;

// Output like ostream
template<typename genType>
CSEDebugger& operator<<(CSEDebugger& os, const genType& Val)
{
    os("PRINT_STREAM", CSEDebugger::PRINT, Val);
    return os;
}

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
