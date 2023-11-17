#pragma once
// 3-component vector from glsl
#ifndef __GLVECTOR4__
#define __GLVECTOR4__

#include <CSE/CSEBase/gltypes/GLBase.h>
#include <CSE/CSEBase/gltypes/GVector2D.h>
#include <CSE/CSEBase/gltypes/GVector3D.h>

_CSE_BEGIN
_GL_BEGIN

template<typename genType>
class GVector4D
{
public:
    union { genType x, s, r; };
    union { genType y, t, g; };
    union { genType z, u, b; };
    union { genType w, v, a; };

    // -- Component accesses --

    /// Return the count of components of the vector
    static constexpr size_t size() { return 4; }

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
        case 3:
            return w;
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
        case 3:
            return w;
        }
    }

    // -- Implicit basic constructors --

    constexpr GVector4D() = default;
    constexpr GVector4D(GVector4D const& v) = default;

    // -- Explicit basic constructors --

    constexpr explicit GVector4D(genType scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {};
    constexpr GVector4D(genType _x, genType _y, genType _z, genType _w) : x(_x), y(_y), z(_z), w(_w) {};

    // -- Conversion vector constructors --
    /// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
    template<typename genTypeA, typename genTypeB, typename genTypeC>
    constexpr GVector4D(GVector2D<genTypeA> const& _xy, genTypeB _z, genTypeC _w)
        : x(static_cast<genType>(_xy.x))
        , y(static_cast<genType>(_xy.y))
        , z(static_cast<genType>(_z))
        , w(static_cast<genType>(_w)){}

    template<typename genTypeA, typename genTypeB, typename genTypeC>
    constexpr GVector4D(genTypeA _x, GVector2D<genTypeB> const& _yz, genTypeC _w)
        : x(static_cast<genType>(_x))
        , y(static_cast<genType>(_yz.x))
        , z(static_cast<genType>(_yz.y))
        , w(static_cast<genType>(_w)){}

    template<typename genTypeA, typename genTypeB, typename genTypeC>
    constexpr GVector4D(genTypeA _x, genTypeB _y, GVector2D<genTypeC> const& _zw)
        : x(static_cast<genType>(_x))
        , y(static_cast<genType>(_y))
        , z(static_cast<genType>(_zw.x))
        , w(static_cast<genType>(_zw.y)){}

    template<typename genTypeA, typename genTypeB>
    constexpr GVector4D(GVector3D<genTypeA> const& _xyz, genTypeB _w)
        : x(static_cast<genType>(_xyz.x))
        , y(static_cast<genType>(_xyz.y))
        , z(static_cast<genType>(_xyz.z))
        , w(static_cast<genType>(_w)){}

    template<typename genTypeA, typename genTypeB>
    constexpr GVector4D(GVector2D<genTypeA> const& _xy, GVector2D<genTypeB> const& _zw)
        : x(static_cast<genType>(_xy.x))
        , y(static_cast<genType>(_xy.y))
        , z(static_cast<genType>(_zw.x))
        , w(static_cast<genType>(_zw.y)){}

    template<typename genTypeA, typename genTypeB>
    constexpr GVector4D(genTypeA _x, GVector3D<genTypeB> const& _yzw)
        : x(static_cast<genType>(_x))
        , y(static_cast<genType>(_yzw.x))
        , z(static_cast<genType>(_yzw.y))
        , w(static_cast<genType>(_yzw.z)){}

    // -- Unary arithmetic operators --

    #define _GVEC4_UNARY_OPERATOR(op)\
    constexpr GVector4D& operator op(const genType& scalar)\
    {\
        this->x op scalar;\
        this->y op scalar;\
        this->z op scalar;\
        this->w op scalar;\
        return *this;\
    }\
    \
    constexpr GVector4D& operator op(const GVector4D& v)\
    {\
        this->x op v.x;\
        this->y op v.y;\
        this->z op v.z;\
        this->w op v.w;\
        return *this;\
    }

    _GVEC4_UNARY_OPERATOR(+=)
    _GVEC4_UNARY_OPERATOR(-=)
    _GVEC4_UNARY_OPERATOR(*=)
    _GVEC4_UNARY_OPERATOR(/=)

    #undef _GVEC4_UNARY_OPERATOR

    // --STL Compatbility--

    // std::array
    constexpr explicit GVector4D(std::array<genType, 4> _xyzw)
    {
        x = _xyzw[0];
        y = _xyzw[1];
        z = _xyzw[2];
        w = _xyzw[3];
    }

    operator std::array<genType, 4>()
    {
        return std::array<genType, 4>({ x, y, z, w });
    }

    // std::vector
    constexpr explicit GVector4D(std::vector<genType> _xyzw)
    {
        x = _xyzw[0];
        y = _xyzw[1];
        z = _xyzw[2];
        w = _xyzw[3];
    }

    operator std::vector<genType>()
    {
        return std::vector<genType>({ x, y, z, w });
    }

    // std::initializer_list
    operator std::initializer_list<genType>()
    {
        return { x, y, z, w };
    }
};

// -- Unary operators --

template<typename genType>
constexpr std::ostream& operator<<(std::ostream& os, const GVector4D<genType>& c)
{
    return os << '(' << c.x << ", " << c.y << ", " << c.z << ", " << c.w << ')';
}

template<typename genType>
constexpr GVector4D<genType> operator+(GVector4D<genType> const& v)
{
    return v;
}

template<typename genType>
constexpr GVector4D<genType> operator-(GVector4D<genType> const& v)
{
    return GVector4D<genType>(-v.x, -v.y, -v.z, -v.w);
}

// -- Binary operators --

#define _GVEC4_BINARY_OPERATOR(op)\
template<typename genType>\
constexpr GVector4D<genType> operator op(GVector4D<genType> const& v, genType scalar)\
{\
    return GVector4D<genType>(v.x op scalar, v.y op scalar, v.z op scalar, v.w op scalar);\
}\
\
template<typename genType>\
constexpr GVector4D<genType> operator op(genType scalar, GVector4D<genType> const& v)\
{\
    return GVector4D<genType>(scalar op v.x, scalar op v.y, scalar op v.z, scalar op v.w);\
}\
\
template<typename genType>\
constexpr GVector4D<genType> operator op(GVector4D<genType> const& v1, GVector4D<genType> const& v2)\
{\
    return GVector4D<genType>(v1.x op v2.x, v1.y op v2.y, v1.z op v2.z, v1.w op v2.w);\
}

_GVEC4_BINARY_OPERATOR(+)
_GVEC4_BINARY_OPERATOR(-)
_GVEC4_BINARY_OPERATOR(*)
_GVEC4_BINARY_OPERATOR(/)

#undef _GVEC4_BINARY_OPERATOR

_GL_END

using vec4  = _GL GVector4D<float64>;
using ivec4 = _GL GVector4D<int64>;
using bvec4 = _GL GVector4D<bool>;
using uvec4 = _GL GVector4D<uint64>;

_CSE_END

#endif
