/**********************************************************************************\
 *                              Data types from GLSL                              *
 * ------------------------------------------------------------------------------ *
 * NOTE:                                                                          *
 * Classes and functions of these headers and inline files are taken from GLM     *
 * library by G-Truc licensed under The Happy Bunny License or MIT License.       *
 * Linear algebra implementation standard now uses BLAS Standard instead of       *
 * OpenGL Standard, for future alignment with C++26.                              *
 * ------------------------------------------------------------------------------ *
 * GLM by G-Truc                                                                  *
 * Copyright (c) 2005 - 2014 G-Truc Creation                                      *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in     *
 * all copies or substantial portions of the Software.                            *
 *                                                                                *
\**********************************************************************************/

#pragma once

#ifndef _STELCXX_GLM_
#define _STELCXX_GLM_

#include "StelCXX-GLM-GLVectorT.hh"

using vec2   = __StelCXX_GLM_GLVector_T<double, 2>;
using vec3   = __StelCXX_GLM_GLVector_T<double, 3>;
using vec4   = __StelCXX_GLM_GLVector_T<double, 4>;
template<std::size_t _Nm> //requires (_Nm >= 5)
using fvec   = __StelCXX_GLM_GLVector_T<double, _Nm>;

using ivec2  = __StelCXX_GLM_GLVector_T<long long, 2>;
using ivec3  = __StelCXX_GLM_GLVector_T<long long, 3>;
using ivec4  = __StelCXX_GLM_GLVector_T<long long, 4>;
template<std::size_t _Nm> //requires (_Nm >= 5)
using ivec   = __StelCXX_GLM_GLVector_T<long long, _Nm>;

using bvec2  = __StelCXX_GLM_GLVector_T<bool, 2>;
using bvec3  = __StelCXX_GLM_GLVector_T<bool, 3>;
using bvec4  = __StelCXX_GLM_GLVector_T<bool, 4>;
template<std::size_t _Nm> //requires (_Nm >= 5)
using bvec   = __StelCXX_GLM_GLVector_T<bool, _Nm>;

using uvec2  = __StelCXX_GLM_GLVector_T<unsigned long long, 2>;
using uvec3  = __StelCXX_GLM_GLVector_T<unsigned long long, 3>;
using uvec4  = __StelCXX_GLM_GLVector_T<unsigned long long, 4>;
template<std::size_t _Nm> //requires (_Nm >= 5)
using uvec   = __StelCXX_GLM_GLVector_T<unsigned long long, _Nm>;

template<typename _Tp, std::size_t _Nm>
using gvec   = __StelCXX_GLM_GLVector_T<_Tp, _Nm>;

#include "StelCXX-GLM-GenericMatrix.hh"

using mat2   = __StelCXX_GLM_Generic_Matrix<double, 2, 2>;
using mat3   = __StelCXX_GLM_Generic_Matrix<double, 3, 3>;
using mat4   = __StelCXX_GLM_Generic_Matrix<double, 4, 4>;

using mat2x2 = mat2;
using mat2x3 = __StelCXX_GLM_Generic_Matrix<double, 2, 3>;
using mat2x4 = __StelCXX_GLM_Generic_Matrix<double, 2, 4>;
using mat3x2 = __StelCXX_GLM_Generic_Matrix<double, 3, 2>;
using mat3x3 = mat3;
using mat3x4 = __StelCXX_GLM_Generic_Matrix<double, 3, 4>;
using mat4x2 = __StelCXX_GLM_Generic_Matrix<double, 4, 2>;
using mat4x3 = __StelCXX_GLM_Generic_Matrix<double, 4, 3>;
using mat4x4 = mat4;

template<std::size_t _Col, std::size_t _Row>
using matrix = __StelCXX_GLM_Generic_Matrix<double, _Col, _Row>;

#endif
