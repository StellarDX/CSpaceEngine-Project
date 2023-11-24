/*
* Common logarithm, for double and quaduple precision
*/

/*
   Cephes Math Library Release 2.2:  January, 1991
   Copyright 1984, 1991 by Stephen L. Moshier
   Adapted for glibc November, 2001
   Adapted for CSpaceEngine, 2023

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

// Unspectacular code. (貌不惊人的代码)

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

float64 __cdecl log(float64 _X) { return __IEEE754_LOGF128_CF64(_X); };

float64 __cdecl log(float64 _X, float64 _Base)
{
    return _CSE log(_X) / _CSE log(_Base);
}

_EXTERN_C

/*
 * SYNOPSIS:
 *
 * float64/float128 x, y, log();
 * y = log(x);
 *
 * DESCRIPTION:
 *
 * Returns the base 10 logarithm of x.
 *
 * The argument is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x^2 + x^3 P(x)/Q(x).
 *
 * Otherwise, setting  z = 2(x-1)/x+1),
 *
 *     log(x) = z + z^3 P(z)/Q(z).
 *
 * ACCURACY:
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0.5, 2.0     30000      2.3e-34     4.9e-35
 *    IEEE     exp(+-10000)  30000      1.0e-34     4.1e-35
 *
 * In the tests over the interval exp(+-10000), the logarithms
 * of the random arguments were uniformly distributed over
 * [-10000, +10000].
 */

__Float64 __cdecl __IEEE754_LOGF128_CF64(__Float64 _X)
{
    __Float64 x = _X;
    int64 hx;
    #if 0 // _QUAD_PRECISION
    hx = x.parts64.msw;
    int64 lx = x.parts64.lsw;
    if (((hx & 0x7FFFFFFFFFFFFFFF) | lx) == 0) { return __Float64::FromBytes(CSE_NEGATIVE_INF); }
    if (hx >= 0x7FFF000000000000) { return __Float64::FromBytes(CSE_POSITIVE_INF); }
    #else
    hx = x.Bytes;
    if ((hx & 0x7FFFFFFFFFFFFFFF) == 0) { return __Float64::FromBytes(CSE_NEGATIVE_INF); }
    if (hx >= 0x7FF0000000000000) { return __Float64::FromBytes(CSE_POSITIVE_INF); }
    #endif
    if (hx < 0) { return __Float64::FromBytes(CSE_NAN); }
    if (x == 1.0L) { return 0.0L; }

    int e;
    x = frexp(x, &e);

    auto neval = [](__Float64 x, const __Float64* p, int n)
    {
        __Float64 y;
        p += n;
        y = *p;
        p -= 1;
        do
        {
            y = y * x + (*p).x;
            p -= 1;
        } while (--n > 0);
        return y;
    };

    auto deval = [](__Float64 x, const __Float64* p, int n)
    {
        __Float64 y;
        p += n;
        y = x.x + (*p).x;
        p -= 1;
        do
        {
            y = y * x + (*p).x;
            p -= 1;
        } while (--n > 0);
        return y;
    };

    __Float64
        L102A = 0.3125L,
        L102B = -1.14700043360188047862611052755069732318101185E-2L,
        L10EA = 0.5L,
        L10EB = -6.570551809674817234887108108339491770560299E-2L,
        SQRTH = 7.071067811865475244008443621048490392848359E-1L;
    __Float64 z, y;
    if ((e > 2) || (e < -2))
    {
        if (x < SQRTH)
        {
            e -= 1;
            z = x - 0.5L;
            y = 0.5L * z + 0.5L;
        }
        else
        {
            z = x - 0.5L;
            z -= 0.5L;
            y = 0.5L * x + 0.5L;
        }
        x = z / y;
        z = x * x;
        static const __Float64 R[6] =
        {
            +1.418134209872192732479751274970992665513E+5L,
            -8.977257995689735303686582344659576526998E+4L,
            +2.048819892795278657810231591630928516206E+4L,
            -2.024301798136027039250415126250455056397E+3L,
            +8.057002716646055371965756206836056074715E+1L,
            -8.828896441624934385266096344596648080902E-1L
        };
        static const __Float64 S[6] =
        {
            +1.701761051846631278975701529965589676574E+6L,
            -1.332535117259762928288745111081235577029E+6L,
            +4.001557694070773974936904547424676279307E+5L,
            -5.748542087379434595104154610899551484314E+4L,
            +3.998526750980007367835804959888064681098E+3L,
            -1.186359407982897997337150403816839480438E+2L
        };
        y = x * (z * neval(z, R, 5) / deval(z, S, 5));
        return y * L10EB + x * L10EB + e * L102B + y * L10EA + x * L10EA + e * L102A;
    }

    if (x < SQRTH)
    {
        e -= 1;
        x = 2.0 * x - 1.0L;	/*  2x - 1  */
    }
    else
    {
        x = x - 1.0L;
    }
    z = x * x;
    static const __Float64 P[13] =
    {
        1.313572404063446165910279910527789794488E4L,
        7.771154681358524243729929227226708890930E4L,
        2.014652742082537582487669938141683759923E5L,
        3.007007295140399532324943111654767187848E5L,
        2.854829159639697837788887080758954924001E5L,
        1.797628303815655343403735250238293741397E5L,
        7.594356839258970405033155585486712125861E4L,
        2.128857716871515081352991964243375186031E4L,
        3.824952356185897735160588078446136783779E3L,
        4.114517881637811823002128927449878962058E2L,
        2.321125933898420063925789532045674660756E1L,
        4.998469661968096229986658302195402690910E-1L,
        1.538612243596254322971797716843006400388E-6L
    };
    static const __Float64 Q[12] =
    {
        3.940717212190338497730839731583397586124E4L,
        2.626900195321832660448791748036714883242E5L,
        7.777690340007566932935753241556479363645E5L,
        1.347518538384329112529391120390701166528E6L,
        1.514882452993549494932585972882995548426E6L,
        1.158019977462989115839826904108208787040E6L,
        6.132189329546557743179177159925690841200E5L,
        2.248234257620569139969141618556349415120E5L,
        5.605842085972455027590989944010492125825E4L,
        9.147150349299596453976674231612674085381E3L,
        9.104928120962988414618126155557301584078E2L,
        4.839208193348159620282142911143429644326E1L
    };
    y = x * (z * neval(x, P, 12) / deval(x, Q, 11));
    y = y - 0.5 * z;
    return y * L10EB + x * L10EB + e * L102B + y * L10EA + x * L10EA + e * L102A;
}

_END_EXTERN_C
_CSE_END
