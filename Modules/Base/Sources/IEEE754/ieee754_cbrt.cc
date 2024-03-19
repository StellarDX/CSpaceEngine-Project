// Fast cube root function for double and quaduple precision.
/*
   Copyright (C) 2018-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library. Adapted for CSpaceEngine

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

// Unpretentious code. (朴实无华的代码)

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN

float64 __cdecl cbrt(float64 _X)
{
    return __IEEE754_CBRT64F(_X);
}

std::array<complex64, 3> __cdecl cbrtc(complex64 _X)
{
    float64 r = abs(_X);
    float64 tet = atan2(_X.imag(), _X.real());
    float64 ri = _CSE cbrt(r);
    return
    {
        ri * _CSE expc((1i * float64(tet + 0. * 2. * CSE_PI)) / 3.),
        ri * _CSE expc((1i * float64(tet + 1. * 2. * CSE_PI)) / 3.),
        ri * _CSE expc((1i * float64(tet + 2. * 2. * CSE_PI)) / 3.)
    };
}

_EXTERN_C

/*
 *	Cube root for double precision
 */

__Float64 __cdecl __IEEE754_CBRT64F(__Float64 _X)
{
    __Float64 x = _X;
    __Float64 xm, ym, u, t2;
    int xe;

    static const __Float64 factor[5] =
    {
        0.62996052494743658238360530363912, // 1 / cbrt(2)^2
        0.79370052598409973737585281963615, // 1 / cbrt(2)
        1.00000000000000000000000000000000,
        1.25992104989487316476721060727820, // cbrt(2)
        1.58740105196819947475170563927230, // cbrt(2)^2
    };

    /* Reduce X.  XM now is an range 1.0 to 0.5.  */
    xm = frexp(fabs(x), &xe);

    /* If X is not finite or is null return it (with raising exceptions
       if necessary.
       Note: *Our* version of `frexp' sets XE to zero if the argument is
       Inf or NaN.  This is not portable but faster.  */
    if (xe == 0 && std::fpclassify(x) <= FP_ZERO) { return x; }

    u = (0.354895765043919860 + ((1.50819193781584896 + ((-2.11499494167371287 + ((2.44693122563534430 + ((-1.83469277483613086 + (0.784932344976639262 - 0.145263899385486377 * xm) * xm) * xm)) * xm)) * xm)) * xm));
    t2 = u * u * u;
    ym = u * (t2 + 2.0 * xm) / (2.0 * t2 + xm) * factor[2 + xe % 3].x;

    return ldexp(x > 0.0 ? ym.x : -ym.x, xe / 3);
}

_END_EXTERN_C
_CSE_END
