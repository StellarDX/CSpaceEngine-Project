/*
    NOTICE: Math functions in this file and related source files is taken
    from serval math libraries, like glibc's math functions (licensed under
    LGPL-2.1 license), or IBM Accurate Mathematical Library (licensed under
    GPL-2.0 or LGPL-2.1? license) or other open-sourse project. In either
    event, it is copyrighted by the authors. What you see either here or
    related source files maybe used freely but it comes with no support or
    guarantee.
*/

#pragma once

#ifndef _CSE_MATH_BASE
#define _CSE_MATH_BASE

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/MathFuncs.h>

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
