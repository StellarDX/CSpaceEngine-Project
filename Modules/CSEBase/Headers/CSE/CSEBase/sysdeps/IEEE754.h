// The IEEE754 defines taken from glibc
/*
   Copyright (C) 1992-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _IEEE754_H
#define _IEEE754_H 1

#ifndef _CSE
#error This file must be included in namespace "CSE"
#endif

#include <cstdint>

// Values only for development.(Be careful to use!!!)
// -------------------------------------------------------------------------
//      Name                 Value                 Note
// The following defines are pecular values in double format.
#define MIN_SNORM_POS_DOUBLE 0x0000000000000001 // Min. subnormal positive double
#define MAX_SNORM_DOUBLE     0x000FFFFFFFFFFFFF // Max. subnormal double
#define MIN_NORM_POS_DOUBLE  0x0010000000000000 // Min. normal positive double
#define MAX_DOUBLE           0x7FEFFFFFFFFFFFFF // Max. Double
#define POS_ZERO_DOUBLE      0x0000000000000000 // +0
#define NEG_ZERO_DOUBLE      0x8000000000000000 // -0
#define POS_INF_DOUBLE       0x7FF0000000000000 // +inf
#define NEG_INF_DOUBLE       0xFFF0000000000000 // -inf
#define S_NAN_DOUBLE         0x7FF0000000000001 // Singaling NAN
#define Q_NAN_DOUBLE         0x7FF8000000000001 // Quiet NAN
#define BIG_NAN_DOUBLE       0x7FFFFFFFFFFFFFFF // an alternative encoding of NaN

// Partisions for IEEE754 standard double-precision floating point number.
// It is constructs of 1-bit for sign, 11-bit for exponential and 52-bit for fractional.
// CSpaceEngine only use 64-bit precision for calculation.
union IEEE754_Dbl64
{
    double x;
    unsigned long long Bytes;
    struct
    {
        unsigned int Mantissa1 : 32; // Lower-32 bits of fractional
        unsigned int Mantissa0 : 20; // Higher-20 bits of fractional
        unsigned int Exponent  : 11; // Exponential bits
        unsigned int Negative  : 1;  // Sign bit
    };

    struct
    {
        uint32_t lsw;
        uint32_t msw;
    } parts;

    // Constructors
    IEEE754_Dbl64() {}
    IEEE754_Dbl64(double d) : x(d) {}
    operator double() { return x; }

    static IEEE754_Dbl64 FromBytes(unsigned long long _By);

    // Generate unary operator
    #define IEEE754_UNARY_OPERATOR(operation) \
    constexpr IEEE754_Dbl64& operator operation (const IEEE754_Dbl64& scaler) \
    { \
        this->x operation scaler.x; \
        return *this; \
    } \

    IEEE754_UNARY_OPERATOR(+=)
    IEEE754_UNARY_OPERATOR(-=)
    IEEE754_UNARY_OPERATOR(*=)
    IEEE754_UNARY_OPERATOR(/=)

    #undef IEEE754_UNARY_OPERATOR
};

#endif
