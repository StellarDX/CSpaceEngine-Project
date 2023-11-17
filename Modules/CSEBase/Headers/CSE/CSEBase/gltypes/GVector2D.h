#pragma once
// 2-component vector from glsl
#ifndef __GLVECTOR2__
#define __GLVECTOR2__

#include "CSE/CSEBase/gltypes/GLBase.h"

_CSE_BEGIN
_GL_BEGIN

template<typename genType>
class GVector2D
{
public:
    union { genType x, s, r; };
    union { genType y, t, g; };

    // -- Component accesses --

    // Return the count of components of the vector
    static constexpr size_t size() { return 2; }

    inline constexpr genType& operator[](size_t i)
    {
        assert(i >= 0 && i < this->size());
        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    inline constexpr genType const& operator[](size_t i) const
    {
        assert(i >= 0 && i < this->size());
        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    // -- Implicit basic constructors --

    constexpr GVector2D() = default;
    constexpr GVector2D(GVector2D const& v) = default;

    // -- Explicit basic constructors --

    constexpr explicit GVector2D(genType scalar) : x(scalar), y(scalar){};
    constexpr GVector2D(genType _x, genType _y) : x(_x), y(_y) {};

    // -- Unary arithmetic operators --

    // All these functions shareing a same template
    // so use a macro to define this template...
    #define _GVEC2_UNARY_OPERATOR(op)\
    inline constexpr GVector2D& operator op(const genType& scalar)\
    {\
        this->x op scalar;\
        this->y op scalar;\
        return *this;\
    }\
    \
    inline constexpr GVector2D& operator op(const GVector2D& v)\
    {\
        this->x op v.x;\
        this->y op v.y;\
        return *this;\
    }

    //...and the rest works push to the compiler.
    _GVEC2_UNARY_OPERATOR(+=)
    _GVEC2_UNARY_OPERATOR(-=)
    _GVEC2_UNARY_OPERATOR(*=)
    _GVEC2_UNARY_OPERATOR(/=)

    #undef _GVEC2_UNARY_OPERATOR

    // --STL Compatbility--

    // std::array
    constexpr explicit GVector2D(std::array<genType, 2> _xy)
    {
        x = _xy[0];
        y = _xy[1];
    }

    operator std::array<genType, 2>()
    {
        return std::array<genType, 2>({ x, y });
    }

    // std::vector
    constexpr explicit GVector2D(std::vector<genType> _xy)
    {
        x = _xy[0];
        y = _xy[1];
    }

    operator std::vector<genType>()
    {
        return std::vector<genType>({ x, y });
    }

    // std::initializer_list
    operator std::initializer_list<genType>()
    {
        return { x, y };
    }

    // complex number
    constexpr explicit GVector2D(std::complex<genType> _xy)
    {
        x = _xy.real();
        y = _xy.imag();
    }

    operator std::complex<genType>()
    {
        return std::complex<genType>(x, y);
    }
};

// -- Unary operators --

template<typename genType>
constexpr std::ostream& operator<<(std::ostream& os, const GVector2D<genType>& c)
{
    return os << '(' << c.x << ", " << c.y << ')';
}

template<typename genType>
constexpr GVector2D<genType> operator+(GVector2D<genType> const& v)
{
    return v;
}

template<typename genType>
constexpr GVector2D<genType> operator-(GVector2D<genType> const& v)
{
    return GVector2D<genType>(-v.x, -v.y);
}

// -- Binary operators --

#define _GVEC2_BINARY_OPERATOR(op)\
template<typename genType>\
constexpr GVector2D<genType> operator op(GVector2D<genType> const& v, genType scalar)\
{\
    return GVector2D<genType>(v.x op scalar, v.y op scalar);\
}\
\
template<typename genType>\
constexpr GVector2D<genType> operator op(GVector2D<genType> const& v1, GVector2D<genType> const& v2)\
{\
    return GVector2D<genType>(v1.x op v2.x, v1.y op v2.y);\
}\
\
template<typename genType>\
constexpr GVector2D<genType> operator op(genType scalar, GVector2D<genType> const& v)\
{\
    return GVector2D<genType>(scalar op v.x, scalar op v.y);\
}

_GVEC2_BINARY_OPERATOR(+)
_GVEC2_BINARY_OPERATOR(-)
_GVEC2_BINARY_OPERATOR(*)
_GVEC2_BINARY_OPERATOR(/)

#undef _GVEC2_BINARY_OPERATOR

_GL_END

using vec2  = _GL GVector2D<float64>;
using ivec2 = _GL GVector2D<int64>;
using bvec2 = _GL GVector2D<bool>;
using uvec2 = _GL GVector2D<uint64>;

_CSE_END

#endif
