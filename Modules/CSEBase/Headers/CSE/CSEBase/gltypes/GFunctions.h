#include "CSE/CSEBase/gltypes/GVector2D.h"
#include "CSE/CSEBase/gltypes/GVector3D.h"
#include "CSE/CSEBase/gltypes/GVector4D.h"

#ifndef __GLTYPES_INL__
#define __GLTYPES_INL__

_CSE_BEGIN

#if 0
template<typename genType>struct vecType;
template<>struct vecType<vec2> { typedef vec2 type; };
template<>struct vecType<vec3> { typedef vec3 type; };
template<>struct vecType<vec4> { typedef vec4 type; };
#endif

template<typename genType> // Only used in C++20
concept vecType =
(
    std::is_same_v<genType, vec2> ||
    std::is_same_v<genType, vec3> ||
    std::is_same_v<genType, vec4>
);

template<typename genIType> // Only used in C++20
concept vecIType =
(
    std::is_same_v<genIType, ivec2> ||
    std::is_same_v<genIType, ivec3> ||
    std::is_same_v<genIType, ivec4>
);

template<typename genUType> // Only used in C++20
concept vecUType =
(
    std::is_same_v<genUType, uvec2> ||
    std::is_same_v<genUType, uvec3> ||
    std::is_same_v<genUType, uvec4>
);

template<typename genBType> // Only used in C++20
concept vecBType =
(
    std::is_same_v<genBType, bvec2> ||
    std::is_same_v<genBType, bvec3> ||
    std::is_same_v<genBType, bvec4>
);

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 LessThan(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] < y[0], x[1] < y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 LessThan(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] < y[0], x[1] < y[1], x[2] < y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 LessThan(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] < y[0], x[1] < y[1], x[2] < y[2], x[3] < y[3]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 LessThanEqual(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] <= y[0], x[1] <= y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 LessThanEqual(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] <= y[0], x[1] <= y[1], x[2] <= y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 LessThanEqual(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] <= y[0], x[1] <= y[1], x[2] <= y[2], x[3] <= y[3]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 GreaterThan(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] > y[0], x[1] > y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 GreaterThan(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] > y[0], x[1] > y[1], x[2] > y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 GreaterThan(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] > y[0], x[1] > y[1], x[2] > y[2], x[3] > y[3]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 GreaterThanEqual(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] >= y[0], x[1] >= y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 GreaterThanEqual(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] >= y[0], x[1] >= y[1], x[2] >= y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 GreaterThanEqual(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] >= y[0], x[1] >= y[1], x[2] >= y[2], x[3] >= y[3]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 Equal(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] == y[0], x[1] == y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 Equal(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] == y[0], x[1] == y[1], x[2] == y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 Equal(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] == y[0], x[1] == y[1], x[2] == y[2], x[3] == y[3]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 NotEqual(_GL GVector2D<genType> x, _GL GVector2D<genType> y)
{
    return bvec2(x[0] != y[0], x[1] != y[1]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 NotEqual(_GL GVector3D<genType> x, _GL GVector3D<genType> y)
{
    return bvec3(x[0] != y[0], x[1] != y[1], x[2] != y[2]);
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 NotEqual(_GL GVector4D<genType> x, _GL GVector4D<genType> y)
{
    return bvec4(x[0] != y[0], x[1] != y[1], x[2] != y[2], x[3] != y[3]);
}

template<typename bvec> requires vecBType<bvec>
bool any(bvec x)
{     // bvec can be bvec2, bvec3 or bvec4
    bool result = false;
    int i;
    for (i = 0; i < x.size(); ++i)
    {
        result |= x[i];
    }
    return result;
}

template<typename bvec> requires vecBType<bvec>
bool all(bvec x)       // bvec can be bvec2, bvec3 or bvec4
{
    bool result = true;
    int i;
    for (i = 0; i < x.size(); ++i)
    {
        result &= x[i];
    }
    return result;
}

template<typename bvec> requires vecBType<bvec>
bvec operator!(bvec x)
{
    bvec result;
    for (size_t i = 0; i < x.size(); ++i)
    {
        result[i] = !x[i];
    }
    return result;
}

//template<typename bvec>
//bvec operator not(bvec x)
//{
//	return !x;
//}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 isinf(_GL GVector2D<genType> x)
{
    return bvec2(std::isinf(x.x), std::isinf(x.y));
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 isinf(_GL GVector3D<genType> x)
{
    return bvec3(std::isinf(x.x), std::isinf(x.y), std::isinf(x.z));
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 isinf(_GL GVector4D<genType> x)
{
    return bvec4(std::isinf(x.x), std::isinf(x.y), std::isinf(x.z), std::isinf(x.w));
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec2 isnan(_GL GVector2D<genType> x)
{
    return bvec2(std::isnan(x.x), std::isnan(x.y));
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec3 isnan(_GL GVector3D<genType> x)
{
    return bvec3(std::isnan(x.x), std::isnan(x.y), std::isnan(x.z));
}

template<typename genType>
requires std::convertible_to<genType, float64> || std::convertible_to<genType, int64> || std::convertible_to<genType, uint64>
bvec4 isnan(_GL GVector4D<genType> x)
{
    return bvec4(std::isnan(x.x), std::isnan(x.y), std::isnan(x.z), std::isnan(x.w));
}

_CSE_END

#endif
