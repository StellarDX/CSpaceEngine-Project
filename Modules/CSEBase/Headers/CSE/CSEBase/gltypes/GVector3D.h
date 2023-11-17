#pragma once
// 3-component vector from glsl
#ifndef __GLVECTOR3__
#define __GLVECTOR3__

#include "CSE/CSEBase/gltypes/GLBase.h"
#include "CSE/CSEBase/gltypes/GVector2D.h"

_CSE_BEGIN
_GL_BEGIN

template<typename genType>
class GVector3D
{
public:
    union { genType x, s, r; };
    union { genType y, t, g; };
    union { genType z, u, b; };

    // -- Component accesses --

    /// Return the count of components of the vector
    static constexpr size_t size() { return 3; }

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
        case 2:
            return z;
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
        case 2:
            return z;
        }
    }

    // -- Implicit basic constructors --

    constexpr GVector3D() = default;
    constexpr GVector3D(GVector3D const& v) = default;

    // -- Explicit basic constructors --

    constexpr explicit GVector3D(genType scalar) : x(scalar), y(scalar), z(scalar) {};
    constexpr GVector3D(genType _x, genType _y, genType _z) : x(_x), y(_y), z(_z) {};

    // -- Conversion vector constructors --
    /// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
    template<typename genTypeA, typename genTypeB>
    constexpr GVector3D(GVector2D<genTypeA> const& _xy, genTypeB _z)
        : x(static_cast<genType>(_xy.x))
        , y(static_cast<genType>(_xy.y))
        , z(static_cast<genType>(_z)){}

    template<typename genTypeA, typename genTypeB>
    constexpr GVector3D(genTypeA _x, GVector2D<genTypeB> const& _yz)
        : x(static_cast<genType>(_x))
        , y(static_cast<genType>(_yz.x))
        , z(static_cast<genType>(_yz.y)){}

    // -- Unary arithmetic operators --

    #define _GVEC3_UNARY_OPERATOR(op)\
    inline constexpr GVector3D& operator op(const genType& scalar)\
    {\
        this->x op scalar;\
        this->y op scalar;\
        this->z op scalar;\
        return *this;\
    }\
    \
    inline constexpr GVector3D& operator op(const GVector3D& v)\
    {\
        this->x op v.x;\
        this->y op v.y;\
        this->z op v.z;\
        return *this;\
    }

    _GVEC3_UNARY_OPERATOR(+=)
    _GVEC3_UNARY_OPERATOR(-=)
    _GVEC3_UNARY_OPERATOR(*=)
    _GVEC3_UNARY_OPERATOR(/=)

    #undef _GVEC3_UNARY_OPERATOR

    // --STL Compatbility--

    // std::array
    constexpr explicit GVector3D(std::array<genType, 3> _xyz)
    {
        x = _xyz[0];
        y = _xyz[1];
        z = _xyz[2];
    }

    operator std::array<genType, 3>()
    {
        return std::array<genType, 3>({ x, y, z });
    }

    // std::vector
    constexpr explicit GVector3D(std::vector<genType> _xyz)
    {
        x = _xyz[0];
        y = _xyz[1];
        z = _xyz[2];
    }

    operator std::vector<genType>()
    {
        return std::vector<genType>({ x, y, z });
    }

    // std::initializer_list
    operator std::initializer_list<genType>()
    {
        return { x, y, z };
    }
};

// -- Unary operators --

template<typename genType>
constexpr std::ostream& operator<<(std::ostream& os, const GVector3D<genType>& c)
{
    return os << '(' << c.x << ", " << c.y << ", " << c.z << ')';
}

template<typename genType>
constexpr GVector3D<genType> operator+(GVector3D<genType> const& v)
{
    return v;
}

template<typename genType>
constexpr GVector3D<genType> operator-(GVector3D<genType> const& v)
{
    return GVector3D<genType>(-v.x, -v.y, -v.z);
}

// -- Binary operators --

#define _GVEC3_BINARY_OPERATOR(op)\
template<typename genType>\
constexpr GVector3D<genType> operator op(GVector3D<genType> const& v, genType scalar)\
{\
    return GVector3D<genType>(v.x op scalar, v.y op scalar, v.z op scalar);\
}\
\
template<typename genType>\
constexpr GVector3D<genType> operator op(genType scalar, GVector3D<genType> const& v)\
{\
    return GVector3D<genType>(scalar op v.x , scalar op v.y, scalar op v.z);\
}\
\
template<typename genType>\
constexpr GVector3D<genType> operator op(GVector3D<genType> const& v1, GVector3D<genType> const& v2)\
{\
    return GVector3D<genType>(v1.x op v2.x, v1.y op v2.y, v1.z op v2.z);\
}

_GVEC3_BINARY_OPERATOR(+)
_GVEC3_BINARY_OPERATOR(-)
_GVEC3_BINARY_OPERATOR(*)
_GVEC3_BINARY_OPERATOR(/)

#undef _GVEC3_BINARY_OPERATOR

_GL_END

using vec3  = _GL GVector3D<float64>;
using ivec3 = _GL GVector3D<int64>;
using bvec3 = _GL GVector3D<bool>;
using uvec3 = _GL GVector3D<uint64>;

_CSE_END

#endif
