/************************************************************
  CSpaceEngine Geometry.
***********************************************************/

#pragma once

#ifndef _CSE_MATH_GEOMETRY
#define _CSE_MATH_GEOMETRY

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/gltypes.h"
#include "CSE/CSEBase/Algorithms.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

// These functions can only used for vecs.
// No base functions, all the definations are in templates.

/**
 * @brief Returns the length of the vector
 */
template<typename genType> requires vecType<genType>
float64 Length(genType x)
{
    float64 len = 0;
    for (size_t i = 0; i < x.size(); i++)
    {
        len += _CSE pow(x[i], 2);
    }
    return _CSE sqrt(len);
}

/**
 * @brief Returns the distance between the two points p0 and p1.
 * @param p0 - Specifies the first of two points
 * @param p1 - Specifies the second of two points
 */
template<typename genType> requires vecType<genType>
float64 Distance(genType p0, genType p1)
{
    return Length(p0 - p1);
}

inline float64 DistancePolar(vec2 p0, vec2 p1);

inline float64 AngularSeparation(vec3 p0, vec3 p1, float64 RABase = 24);

/**
 * @brief Returns the dot product of two vectors
 * @param x - Specifies the first of two vectors
 * @param y - Specifies the second of two vectors
 */
template<typename genType> requires vecType<genType>
float64 dot(genType x, genType y)
{
    float64 Result = 0;
    for (size_t i = 0; i < x.size(); i++)
    {
        Result += x[i] * y[i];
    }
    return Result;
}

/**
 * @brief Returns the cross product of two vectors
 * @param x - Specifies the first of two vectors
 * @param y - Specifies the second of two vectors
 */
inline vec3 cross(vec3 x, vec3 y);

/**
 * @brief Corss product on 4-dimension, using 3 vectors
 * @link https://www.researchgate.net/publication/318543243_Vector_Cross_Product_in_4D_Euclidean_Space_A_Maple_worksheet
 */
inline vec4 cross(vec4 u, vec4 v, vec4 t);

/**
 * @brief Returns a vector with the same direction as its parameter, v, but with length 1.
 */
template<typename genType> requires vecType<genType>
genType normalize(genType v)
{
    return v / Length(v);
}

/**
 * @brief If dot(Nref, I) less than 0 faceforward returns N, otherwise it returns -N.
 * @param N - Specifies the vector to orient.
 * @param I - Specifies the incident vector.
 * @param Nref - Specifies the reference vector.
 */
template<typename genType> requires vecType<genType>
genType faceforward(genType N, genType I, genType Nref)
{
    if (dot(Nref, I) < 0) { return N; }
    else { return vec3(0) - N; }
}

/**
 * @brief For a given incident vector I and surface normal N reflect returns the reflection direction.
 * N should be normalized in order to achieve the desired result.
 * @param I - Specifies the incident vector.
 * @param N - Specifies the normal vector.
 */
template<typename genType> requires vecType<genType>
genType reflect(genType I, genType N)
{
    // Original code in GLSL (Normal vector need to be normalized)
    /*if (Length(N) != 1.)
    {
        MathLog.Out("Math-Geometry", "WARNING", "Length of normal vector is not 1, automatically nomalized.", SysLogLevel);
        N = normalize(N);
    }
    return I - 2.0 * dot(N, I) * N;*/

    // StellarDX's Implementation (Normal vector doesn't need to be normalized)
    // Reference: https://blog.csdn.net/heyuchang666/article/details/51833893
    genType S = -(dot(I, N) / _CSE pow(Length(N), 2)) * N;
    genType P = I + S;
    return 2.0 * P - I;
}

/**
 * @brief For a given incident vector I, surface normal N and ratio of indices of refraction, eta, returns the refraction vector, R.
 * The direction of normal vector is the upper media.
 * @param I - Specifies the incident vector.
 * @param N - Specifies the normal vector.
 * @param eta - Specifies the ratio of indices of refraction. (n2 / n1)
 */
template<typename genType> requires vecType<genType>
genType refract(genType I, genType N, float64 eta)
{
    // StellarDX's Implementation (Incident vector and Normal vector will be normalized automatically)
    // Reference: https://en.wikipedia.org/wiki/Snell%27s_law
    float64 ctet1 = abs(dot(I, N) / (Length(I) * Length(N)));
    float64 ctet2 = _CSE sqrt(1.0 - _CSE pow(eta, 2.) * (1.0 - _CSE pow(ctet1, 2.)));
    if (_CSE isnan(ctet2)) { return genType(0.); }
    return eta * normalize(I) + (eta * ctet1 - ctet2) * normalize(N);
}

template<typename genType> requires vecType<genType>
genType refract(genType I, genType N, float64 UpperMediaIndex, float64 LowerMediaIndex)
{
    return refract(I, N, UpperMediaIndex / LowerMediaIndex);
}

_CSE_END

#endif
