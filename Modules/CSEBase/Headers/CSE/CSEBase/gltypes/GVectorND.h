#pragma once
// n-component vector based on STD array
#ifndef __GLVECTORN__
#define __GLVECTORN__

#include "CSE/CSEBase/gltypes/GLBase.h"
#include <array>

namespace std { // This module is essentially a addon of std::array...

/****************************************************************************************\
*                                    Array Operators                                     *
\****************************************************************************************/

template<typename _Tp, std::size_t _Nm>
constexpr std::ostream& operator<<(std::ostream& os, const std::array<_Tp, _Nm>& c)
{
    os << '[';
    for (int i = 0; i < _Nm; ++i)
    {
        os << c[i];
        if (i < _Nm - 1) {os << ", ";}
    }
    os << ']';
    return os;
}

template<typename _Tp, std::size_t _Nm>
constexpr std::array<_Tp, _Nm> operator+(std::array<_Tp, _Nm> const& v)
{
    return v;
}

template<typename _Tp, std::size_t _Nm>
constexpr std::array<_Tp, _Nm> operator-(std::array<_Tp, _Nm> const& v)
{
    std::array<_Tp, _Nm> Result;
    for (size_t i = 0; i < _Nm; ++i)
    {
        Result[i] = -v[i];
    }
    return Result;
}

#define _ARRAY_BINARY_OPERATOR(op)\
template<typename _Tp, std::size_t _Nm>\
constexpr std::array<_Tp, _Nm> operator op(std::array<_Tp, _Nm> const& _Left, _Tp _Right)\
{\
    std::array<_Tp, _Nm> Result;\
    for (size_t i = 0; i < _Nm; ++i)\
    {\
        Result[i] = _Left[i] op _Right;\
    }\
    return Result;\
}\
\
template<typename _Tp, std::size_t _Nm>\
constexpr std::array<_Tp, _Nm> operator op(std::array<_Tp, _Nm> const& _Left, std::array<_Tp, _Nm> const& _Right)\
{\
    std::array<_Tp, _Nm> Result;\
    for (size_t i = 0; i < _Nm; ++i)\
    {\
            Result[i] = _Left[i] op _Right[i];\
    }\
    return Result;\
}\
\
template<typename _Tp, std::size_t _Nm>\
constexpr std::array<_Tp, _Nm> operator op(_Tp _Left, std::array<_Tp, _Nm> const& _Right)\
{\
    std::array<_Tp, _Nm> Result;\
    for (size_t i = 0; i < _Nm; ++i)\
    {\
        Result[i] = _Left op _Right[i];\
    }\
    return Result;\
}

_ARRAY_BINARY_OPERATOR(+)
_ARRAY_BINARY_OPERATOR(-)
_ARRAY_BINARY_OPERATOR(*)
_ARRAY_BINARY_OPERATOR(/)

#undef _ARRAY_BINARY_OPERATOR

}

#endif
