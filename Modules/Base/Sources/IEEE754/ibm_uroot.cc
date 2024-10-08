// Simple method and IBM Ultimate routine for solving square root
// written by International Business Machines Corp. (LGPL Licence)
// Port by StellarDX

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"

#include <algorithm>

_CSE_BEGIN

float64 __cdecl sqrt(float64 _X)
{
    return __IBM_SQRT64F(_X);
}

float64 __cdecl inversesqrt(float64 _X)
{
    return 1. / __IBM_SQRT64F(_X);
}

std::array<complex64, 2> __cdecl sqrtc(complex64 _X)
{
    complex64 _Res;
    float64 Sign = _CSE sgn(_X.imag());
    if (Sign == 0)
    {
        if (_X.real() > 0) { return { sqrt(_X.real()), -sqrt(_X.real()) }; }
        if (_X.real() < 0) { return { 1i * sqrt(-_X.real()), 1i * -sqrt(-_X.real()) }; }
    }
    _Res = _CSE sqrt((abs(_X) + _X.real()) / 2.) + 1i * Sign * _CSE sqrt((abs(_X) - _X.real()) / 2.);
    return { _Res, -_Res };
}

std::array<complex64, 2> __cdecl inversesqrtc(complex64 _X)
{
    return {1. / _CSE sqrtc(_X)[0], 1. / _CSE sqrtc(_X)[1]};
}

_EXTERN_C

// This is an IBM Function taken from GLibC, used for double precision
// and almost nothing has been modified because it is almost perfect.

/*
 * IBM Accurate Mathematical Library
 * written by International Business Machines Corp.
 * Copyright (C) 2001-2023 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

// Common code. (稀松平常的代码)

const double __IBM_inroot_table[128] =
{
    1.40872145012100,  1.39792649065766,  1.38737595123859,  1.37706074531819,
    1.36697225234682,  1.35710228748795,  1.34744307370643,  1.33798721601135,
    1.32872767765984,  1.31965775814772,  1.31077107283046,  1.30206153403386,
    1.29352333352711,  1.28515092624400,  1.27693901514820,  1.26888253714903,
    1.26097664998256,  1.25321671998073,  1.24559831065844,  1.23811717205462,
    1.23076923076923,  1.22355058064300,  1.21645747403153,  1.20948631362953,
    1.20263364480453,  1.19589614840310,  1.18927063399547,  1.18275403352732,
    1.17634339535009,  1.17003587860341,  1.16382874792529,  1.15771936846787,
    1.15170520119791,  1.14578379846309,  1.13995279980655,  1.13420992801334,
    1.12855298537376,  1.12297985014975,  1.11748847323133,  1.11207687497107,
    1.10674314218572,  1.10148542531442,  1.09630193572405,  1.09119094315276,
    1.08615077328341,  1.08117980543918,  1.07627647039410,  1.07143924829188,
    1.06666666666667,  1.06195729855996,  1.05730976072814,  1.05272271193563,
    1.04819485132867,  1.04372491688551,  1.03931168393861,  1.03495396376504,
    1.03065060224133,  1.02640047855933,  1.02220250399990,  1.01805562076124,
    1.01395880083916,  1.00991104495649,  1.00591138153909,  1.00195886573624,
    0.99611649018350,  0.98848330114434,  0.98102294317595,  0.97372899112030,
    0.96659534932828,  0.95961623024651,  0.95278613468066,  0.94609983358253,
    0.93955235122353,  0.93313894963169,  0.92685511418159,  0.92069654023750,
    0.91465912076005,  0.90873893479530,  0.90293223677296,  0.89723544654727,
    0.89164514012056,  0.88615804099474,  0.88077101210109,  0.87548104826333,
    0.87028526915267,  0.86518091269740,  0.86016532891275,  0.85523597411976,
    0.85039040552437,  0.84562627613070,  0.84094132996422,  0.83633339758291,
    0.83180039185606,  0.82734030399203,  0.82295119979782,  0.81863121615464,
    0.81437855769486,  0.81019149366693,  0.80606835497581,  0.80200753138734,
    0.79800746888611,  0.79406666717674,  0.79018367731967,  0.78635709949278,
    0.78258558087123,  0.77886781361798,  0.77520253297841,  0.77158851547266,
    0.76802457717971,  0.76450957210799,  0.76104239064719,  0.75762195809661,
    0.75424723326565,  0.75091720714229,  0.74763090162560,  0.74438736831878,
    0.74118568737933,  0.73802496642311,  0.73490433947940,  0.73182296599416,
    0.72878002987884,  0.72577473860242,  0.72280632232420,  0.71987403306536,
    0.71697714391715,  0.71411494828392,  0.71128675915902,  0.70849190843208
};

/*********************************************************************/
/* An ultimate sqrt routine. Given an IEEE double machine number x   */
/* it computes the correctly rounded (to nearest) value of square    */
/* root of x.                                                        */
/*********************************************************************/

__Float64 __cdecl __IBM_SQRT64F(__Float64 _X)
{
    /* Use generic implementation.  */

    // Coefficients of Taylor series.
    // inversesqrt(1 + x) = 1 - (1/2)*x + (3/8)*x^2 - (5/16)*x^3 + (35/128)*x^4 - (63/256)*x^5 + ...
    //                      (((-1)^n*(2*n)!)/(4^n*(n!)^2))
    static const float64
        rt0 = 1.0000,
        rt1 = 0.5000,
        rt2 = 0.3750,
        rt3 = 0.3125;

    static const double big = 134217728.0;

    __Float64 y, t, del, res, res1, hy, z, zz, s;
    __Float64 a, c = 0;
    uint32_t k;

    __Float64 x = _X;
    a = x;
    k = a.parts.msw;
    a.parts.msw = (k & 0x001fffff) | 0x3fe00000;
    t = __IBM_inroot_table[(k & 0x001fffff) >> 14];
    s = a;
    /*----------------- 2^-1022  <= | x |< 2^1024  -----------------*/
    if (k > 0x000fffff && k < 0x7ff00000)
    {
        __Float64 ret;
        y = 1.0 - t * (t * s);
        t = t * (rt0 + y * (rt1 + y * (rt2 + y * rt3)));
        c.parts.msw = 0x20000000 + ((k & 0x7fe00000) >> 1);
        y = t * s;
        hy = (y + big) - big;
        del = 0.5 * t * ((s - hy * hy) - (y - hy) * (y + hy));
        res = y + del;
        if (res == (res + 1.002 * ((y - res) + del))) { ret = res * c; }
        else
        {
            res1 = res + 1.5 * ((y - res) + del);
            auto EMULV = [](__Float64& res, __Float64& res1, __Float64& z, __Float64& zz)
            {
                __Float64 __p, hx, tx, hy, ty;
                __p = 134217729.0 * (res);
                hx = ((res)-__p) + __p;
                tx = (res)-hx;
                __p = 134217729.0 * (res1);
                hy = ((res1)-__p) + __p;
                ty = (res1)-hy;
                z = (res) * (res1);
                zz = (((hx * hy - z) + hx * ty) + tx * hy) + tx * ty;
            };
            EMULV(res, res1, z, zz); /* (z+zz)=res*res1 */
            res = ((((z - s) + zz) < 0) ? std::max(res.x, res1.x) :
                       std::min(res.x, res1.x));
            ret = res * c;
        }

        float64 dret = x / ret;
        if (dret != ret)
        {
            float64 force_inexact = 1.0 / 3.0;
            /* The square root is inexact, ret is the round-to-nearest
               value which may need adjusting for other rounding
               modes.  */
            ret = (res - 0x1p-1022) * c;
        }
        /* Otherwise (x / ret == ret), either the square root was exact or
           the division was inexact.  */
        return ret;
    }
    else
    {
        if ((k & 0x7ff00000) == 0x7ff00000) {return x * x + x;} /* sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN */
        if (x == 0) {return x;} /* sqrt(+0)=+0, sqrt(-0)=-0 */
        if (k < 0) {return __Float64::FromBytes(S_NAN_DOUBLE);} /* sqrt(-ve)=sNaN */
        return 0x1p-256 * __IBM_SQRT64F(x * 0x1p512);
    }
}

_END_EXTERN_C
_CSE_END
