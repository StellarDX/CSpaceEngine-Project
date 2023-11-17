// Power function for double and quaduple-precision floating point
// StellarDX: power function, aka. x^y, can be written as exp(y*ln(x))
// But in this case, using this method directly maybe cause precision
// loss, so a specially modified routine is needed in this calculation.

/* Double-precision x^y function.
   Copyright (C) 2018-2023 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

float64 __cdecl pow(float64 _X, float64 _Power)
{
    return __IEEE754_POWF64(_X, _Power);
}

_EXTERN_C

// StellarDX: This implemention is made by FSF, taken from GLibC.
// But GLibC was using IBM's inplemention before the apperence
// of this version. And the former is now stored in IBM Accurate
// Mathematical Library(GPLv2 License). IBM's implemention of power
// function is very, very long and unstable because it maybe cause
// a huge delay whith is over 8000 times than normal when inputing
// some special numbers.
// For details, see http://entropymine.com/imageworsener/slowpow/

const struct __GNU_Table_Powlnt {double invc, pad, lnc, lnctail;} __Powf64_ln_table[]
{
    #define A(a,b,c) {a,0,b,c},
    A(0x1.6a00000000000p+0, -0x1.62c82f2b9c800p-2, +0x1.ab42428375680p-48)
    A(0x1.6800000000000p+0, -0x1.5d1bdbf580800p-2, -0x1.ca508d8e0f720p-46)
    A(0x1.6600000000000p+0, -0x1.5767717455800p-2, -0x1.362a4d5b6506dp-45)
    A(0x1.6400000000000p+0, -0x1.51aad872df800p-2, -0x1.684e49eb067d5p-49)
    A(0x1.6200000000000p+0, -0x1.4be5f95777800p-2, -0x1.41b6993293ee0p-47)
    A(0x1.6000000000000p+0, -0x1.4618bc21c6000p-2, +0x1.3d82f484c84ccp-46)
    A(0x1.5e00000000000p+0, -0x1.404308686a800p-2, +0x1.c42f3ed820b3ap-50)
    A(0x1.5c00000000000p+0, -0x1.3a64c55694800p-2, +0x1.0b1c686519460p-45)
    A(0x1.5a00000000000p+0, -0x1.347dd9a988000p-2, +0x1.5594dd4c58092p-45)
    A(0x1.5800000000000p+0, -0x1.2e8e2bae12000p-2, +0x1.67b1e99b72bd8p-45)
    A(0x1.5600000000000p+0, -0x1.2895a13de8800p-2, +0x1.5ca14b6cfb03fp-46)
    A(0x1.5600000000000p+0, -0x1.2895a13de8800p-2, +0x1.5ca14b6cfb03fp-46)
    A(0x1.5400000000000p+0, -0x1.22941fbcf7800p-2, -0x1.65a242853da76p-46)
    A(0x1.5200000000000p+0, -0x1.1c898c1699800p-2, -0x1.fafbc68e75404p-46)
    A(0x1.5000000000000p+0, -0x1.1675cababa800p-2, +0x1.f1fc63382a8f0p-46)
    A(0x1.4e00000000000p+0, -0x1.1058bf9ae4800p-2, -0x1.6a8c4fd055a66p-45)
    A(0x1.4c00000000000p+0, -0x1.0a324e2739000p-2, -0x1.c6bee7ef4030ep-47)
    A(0x1.4a00000000000p+0, -0x1.0402594b4d000p-2, -0x1.036b89ef42d7fp-48)
    A(0x1.4a00000000000p+0, -0x1.0402594b4d000p-2, -0x1.036b89ef42d7fp-48)
    A(0x1.4800000000000p+0, -0x1.fb9186d5e4000p-3, +0x1.d572aab993c87p-47)
    A(0x1.4600000000000p+0, -0x1.ef0adcbdc6000p-3, +0x1.b26b79c86af24p-45)
    A(0x1.4400000000000p+0, -0x1.e27076e2af000p-3, -0x1.72f4f543fff10p-46)
    A(0x1.4200000000000p+0, -0x1.d5c216b4fc000p-3, +0x1.1ba91bbca681bp-45)
    A(0x1.4000000000000p+0, -0x1.c8ff7c79aa000p-3, +0x1.7794f689f8434p-45)
    A(0x1.4000000000000p+0, -0x1.c8ff7c79aa000p-3, +0x1.7794f689f8434p-45)
    A(0x1.3e00000000000p+0, -0x1.bc286742d9000p-3, +0x1.94eb0318bb78fp-46)
    A(0x1.3c00000000000p+0, -0x1.af3c94e80c000p-3, +0x1.a4e633fcd9066p-52)
    A(0x1.3a00000000000p+0, -0x1.a23bc1fe2b000p-3, -0x1.58c64dc46c1eap-45)
    A(0x1.3a00000000000p+0, -0x1.a23bc1fe2b000p-3, -0x1.58c64dc46c1eap-45)
    A(0x1.3800000000000p+0, -0x1.9525a9cf45000p-3, -0x1.ad1d904c1d4e3p-45)
    A(0x1.3600000000000p+0, -0x1.87fa06520d000p-3, +0x1.bbdbf7fdbfa09p-45)
    A(0x1.3400000000000p+0, -0x1.7ab890210e000p-3, +0x1.bdb9072534a58p-45)
    A(0x1.3400000000000p+0, -0x1.7ab890210e000p-3, +0x1.bdb9072534a58p-45)
    A(0x1.3200000000000p+0, -0x1.6d60fe719d000p-3, -0x1.0e46aa3b2e266p-46)
    A(0x1.3000000000000p+0, -0x1.5ff3070a79000p-3, -0x1.e9e439f105039p-46)
    A(0x1.3000000000000p+0, -0x1.5ff3070a79000p-3, -0x1.e9e439f105039p-46)
    A(0x1.2e00000000000p+0, -0x1.526e5e3a1b000p-3, -0x1.0de8b90075b8fp-45)
    A(0x1.2c00000000000p+0, -0x1.44d2b6ccb8000p-3, +0x1.70cc16135783cp-46)
    A(0x1.2c00000000000p+0, -0x1.44d2b6ccb8000p-3, +0x1.70cc16135783cp-46)
    A(0x1.2a00000000000p+0, -0x1.371fc201e9000p-3, +0x1.178864d27543ap-48)
    A(0x1.2800000000000p+0, -0x1.29552f81ff000p-3, -0x1.48d301771c408p-45)
    A(0x1.2600000000000p+0, -0x1.1b72ad52f6000p-3, -0x1.e80a41811a396p-45)
    A(0x1.2600000000000p+0, -0x1.1b72ad52f6000p-3, -0x1.e80a41811a396p-45)
    A(0x1.2400000000000p+0, -0x1.0d77e7cd09000p-3, +0x1.a699688e85bf4p-47)
    A(0x1.2400000000000p+0, -0x1.0d77e7cd09000p-3, +0x1.a699688e85bf4p-47)
    A(0x1.2200000000000p+0, -0x1.fec9131dbe000p-4, -0x1.575545ca333f2p-45)
    A(0x1.2000000000000p+0, -0x1.e27076e2b0000p-4, +0x1.a342c2af0003cp-45)
    A(0x1.2000000000000p+0, -0x1.e27076e2b0000p-4, +0x1.a342c2af0003cp-45)
    A(0x1.1e00000000000p+0, -0x1.c5e548f5bc000p-4, -0x1.d0c57585fbe06p-46)
    A(0x1.1c00000000000p+0, -0x1.a926d3a4ae000p-4, +0x1.53935e85baac8p-45)
    A(0x1.1c00000000000p+0, -0x1.a926d3a4ae000p-4, +0x1.53935e85baac8p-45)
    A(0x1.1a00000000000p+0, -0x1.8c345d631a000p-4, +0x1.37c294d2f5668p-46)
    A(0x1.1a00000000000p+0, -0x1.8c345d631a000p-4, +0x1.37c294d2f5668p-46)
    A(0x1.1800000000000p+0, -0x1.6f0d28ae56000p-4, -0x1.69737c93373dap-45)
    A(0x1.1600000000000p+0, -0x1.51b073f062000p-4, +0x1.f025b61c65e57p-46)
    A(0x1.1600000000000p+0, -0x1.51b073f062000p-4, +0x1.f025b61c65e57p-46)
    A(0x1.1400000000000p+0, -0x1.341d7961be000p-4, +0x1.c5edaccf913dfp-45)
    A(0x1.1400000000000p+0, -0x1.341d7961be000p-4, +0x1.c5edaccf913dfp-45)
    A(0x1.1200000000000p+0, -0x1.16536eea38000p-4, +0x1.47c5e768fa309p-46)
    A(0x1.1000000000000p+0, -0x1.f0a30c0118000p-5, +0x1.d599e83368e91p-45)
    A(0x1.1000000000000p+0, -0x1.f0a30c0118000p-5, +0x1.d599e83368e91p-45)
    A(0x1.0e00000000000p+0, -0x1.b42dd71198000p-5, +0x1.c827ae5d6704cp-46)
    A(0x1.0e00000000000p+0, -0x1.b42dd71198000p-5, +0x1.c827ae5d6704cp-46)
    A(0x1.0c00000000000p+0, -0x1.77458f632c000p-5, -0x1.cfc4634f2a1eep-45)
    A(0x1.0c00000000000p+0, -0x1.77458f632c000p-5, -0x1.cfc4634f2a1eep-45)
    A(0x1.0a00000000000p+0, -0x1.39e87b9fec000p-5, +0x1.502b7f526feaap-48)
    A(0x1.0a00000000000p+0, -0x1.39e87b9fec000p-5, +0x1.502b7f526feaap-48)
    A(0x1.0800000000000p+0, -0x1.f829b0e780000p-6, -0x1.980267c7e09e4p-45)
    A(0x1.0800000000000p+0, -0x1.f829b0e780000p-6, -0x1.980267c7e09e4p-45)
    A(0x1.0600000000000p+0, -0x1.7b91b07d58000p-6, -0x1.88d5493faa639p-45)
    A(0x1.0400000000000p+0, -0x1.fc0a8b0fc0000p-7, -0x1.f1e7cf6d3a69cp-50)
    A(0x1.0400000000000p+0, -0x1.fc0a8b0fc0000p-7, -0x1.f1e7cf6d3a69cp-50)
    A(0x1.0200000000000p+0, -0x1.fe02a6b100000p-8, -0x1.9e23f0dda40e4p-46)
    A(0x1.0200000000000p+0, -0x1.fe02a6b100000p-8, -0x1.9e23f0dda40e4p-46)
    A(0x1.0000000000000p+0, +0x0.0000000000000p+0, +0x0.0000000000000p+00)
    A(0x1.0000000000000p+0, +0x0.0000000000000p+0, +0x0.0000000000000p+00)
    A(0x1.fc00000000000p-1, +0x1.0101575890000p-7, -0x1.0c76b999d2be8p-46)
    A(0x1.f800000000000p-1, +0x1.0205658938000p-6, -0x1.3dc5b06e2f7d2p-45)
    A(0x1.f400000000000p-1, +0x1.8492528c90000p-6, -0x1.aa0ba325a0c34p-45)
    A(0x1.f000000000000p-1, +0x1.0415d89e74000p-5, +0x1.111c05cf1d753p-47)
    A(0x1.ec00000000000p-1, +0x1.466aed42e0000p-5, -0x1.c167375bdfd28p-45)
    A(0x1.e800000000000p-1, +0x1.894aa149fc000p-5, -0x1.97995d05a267dp-46)
    A(0x1.e400000000000p-1, +0x1.ccb73cdddc000p-5, -0x1.a68f247d82807p-46)
    A(0x1.e200000000000p-1, +0x1.eea31c006c000p-5, -0x1.e113e4fc93b7bp-47)
    A(0x1.de00000000000p-1, +0x1.1973bd1466000p-4, -0x1.5325d560d9e9bp-45)
    A(0x1.da00000000000p-1, +0x1.3bdf5a7d1e000p-4, +0x1.cc85ea5db4ed7p-45)
    A(0x1.d600000000000p-1, +0x1.5e95a4d97a000p-4, -0x1.c69063c5d1d1ep-45)
    A(0x1.d400000000000p-1, +0x1.700d30aeac000p-4, +0x1.c1e8da99ded32p-49)
    A(0x1.d000000000000p-1, +0x1.9335e5d594000p-4, +0x1.3115c3abd47dap-45)
    A(0x1.cc00000000000p-1, +0x1.b6ac88dad6000p-4, -0x1.390802bf768e5p-46)
    A(0x1.ca00000000000p-1, +0x1.c885801bc4000p-4, +0x1.646d1c65aacd3p-45)
    A(0x1.c600000000000p-1, +0x1.ec739830a2000p-4, -0x1.dc068afe645e0p-45)
    A(0x1.c400000000000p-1, +0x1.fe89139dbe000p-4, -0x1.534d64fa10afdp-45)
    A(0x1.c000000000000p-1, +0x1.1178e8227e000p-3, +0x1.1ef78ce2d07f2p-45)
    A(0x1.be00000000000p-1, +0x1.1aa2b7e23f000p-3, +0x1.ca78e44389934p-45)
    A(0x1.ba00000000000p-1, +0x1.2d1610c868000p-3, +0x1.39d6ccb81b4a1p-47)
    A(0x1.b800000000000p-1, +0x1.365fcb0159000p-3, +0x1.62fa8234b7289p-51)
    A(0x1.b400000000000p-1, +0x1.4913d8333b000p-3, +0x1.5837954fdb678p-45)
    A(0x1.b200000000000p-1, +0x1.527e5e4a1b000p-3, +0x1.633e8e5697dc7p-45)
    A(0x1.ae00000000000p-1, +0x1.6574ebe8c1000p-3, +0x1.9cf8b2c3c2e78p-46)
    A(0x1.ac00000000000p-1, +0x1.6f0128b757000p-3, -0x1.5118de59c21e1p-45)
    A(0x1.aa00000000000p-1, +0x1.7898d85445000p-3, -0x1.c661070914305p-46)
    A(0x1.a600000000000p-1, +0x1.8beafeb390000p-3, -0x1.73d54aae92cd1p-47)
    A(0x1.a400000000000p-1, +0x1.95a5adcf70000p-3, +0x1.7f22858a0ff6fp-47)
    A(0x1.a000000000000p-1, +0x1.a93ed3c8ae000p-3, -0x1.8724350562169p-45)
    A(0x1.9e00000000000p-1, +0x1.b31d8575bd000p-3, -0x1.c358d4eace1aap-47)
    A(0x1.9c00000000000p-1, +0x1.bd087383be000p-3, -0x1.d4bc4595412b6p-45)
    A(0x1.9a00000000000p-1, +0x1.c6ffbc6f01000p-3, -0x1.1ec72c5962bd2p-48)
    A(0x1.9600000000000p-1, +0x1.db13db0d49000p-3, -0x1.aff2af715b035p-45)
    A(0x1.9400000000000p-1, +0x1.e530effe71000p-3, +0x1.212276041f430p-51)
    A(0x1.9200000000000p-1, +0x1.ef5ade4dd0000p-3, -0x1.a211565bb8e11p-51)
    A(0x1.9000000000000p-1, +0x1.f991c6cb3b000p-3, +0x1.bcbecca0cdf30p-46)
    A(0x1.8c00000000000p-1, +0x1.07138604d5800p-2, +0x1.89cdb16ed4e91p-48)
    A(0x1.8a00000000000p-1, +0x1.0c42d67616000p-2, +0x1.7188b163ceae9p-45)
    A(0x1.8800000000000p-1, +0x1.1178e8227e800p-2, -0x1.c210e63a5f01cp-45)
    A(0x1.8600000000000p-1, +0x1.16b5ccbacf800p-2, +0x1.b9acdf7a51681p-45)
    A(0x1.8400000000000p-1, +0x1.1bf99635a6800p-2, +0x1.ca6ed5147bdb7p-45)
    A(0x1.8200000000000p-1, +0x1.214456d0eb800p-2, +0x1.a87deba46baeap-47)
    A(0x1.7e00000000000p-1, +0x1.2bef07cdc9000p-2, +0x1.a9cfa4a5004f4p-45)
    A(0x1.7c00000000000p-1, +0x1.314f1e1d36000p-2, -0x1.8e27ad3213cb8p-45)
    A(0x1.7a00000000000p-1, +0x1.36b6776be1000p-2, +0x1.16ecdb0f177c8p-46)
    A(0x1.7800000000000p-1, +0x1.3c25277333000p-2, +0x1.83b54b606bd5cp-46)
    A(0x1.7600000000000p-1, +0x1.419b423d5e800p-2, +0x1.8e436ec90e09dp-47)
    A(0x1.7400000000000p-1, +0x1.4718dc271c800p-2, -0x1.f27ce0967d675p-45)
    A(0x1.7200000000000p-1, +0x1.4c9e09e173000p-2, -0x1.e20891b0ad8a4p-45)
    A(0x1.7000000000000p-1, +0x1.522ae0738a000p-2, +0x1.ebe708164c759p-45)
    A(0x1.6e00000000000p-1, +0x1.57bf753c8d000p-2, +0x1.fadedee5d40efp-46)
    A(0x1.6c00000000000p-1, +0x1.5d5bddf596000p-2, -0x1.a0b2a08a465dcp-47)
    #undef A
};

/* Returns 0 if not int, 1 if odd int, 2 if even int.  The argument is
   the bit representation of a non-zero finite floating-point value.  */
int CheckInt(uint64_t iy)
{
    int e = iy >> 52 & 0x7ff;
    if (e < 0x3ff) {return 0;}
    if (e > 0x3ff + 52) {return 2;}
    if (iy & ((1ULL << (0x3ff + 52 - e)) - 1)) {return 0;}
    if (iy & (1ULL << (0x3ff + 52 - e))) {return 1;}
    return 2;
}

__Float64 __cdecl __IEEE754_POWF64(__Float64 _X, __Float64 _Power)
{
    __Float64 x = _X, y = _Power;

    uint32_t sign_bias = 0;
    uint64_t ix, iy;
    uint32_t topx, topy;

    ix = x.Bytes;
    iy = y.Bytes;
    topx = x.Bytes >> 52;
    topy = y.Bytes >> 52;

    static auto zeroinfnan = [](uint64_t i)->int
    {
        return 2 * i - 1 >= 2 * uint64(CSE_POSITIVE_INF) - 1;
    };

    static auto issignaling_inline = [](__Float64 x)->int
    {
        return 2 * (x.Bytes ^ 0x0008000000000000) > 2 * 0x7ff8000000000000ULL;
    };

    if (topx - 0x001 >= 0x7ff - 0x001 || (topy & 0x7ff) - 0x3be >= 0x43e - 0x3be)
    {
        // Note: if |y| > 1075 * ln2 * 2^53 ~= 0x1.749p62 then pow(x,y) = inf/0
        // and if |y| < 2^-54 / 1075 ~= 0x1.e7b6p-65 then pow(x,y) = +-1.
        // Special cases: (x < 0x1p-126 or inf or nan) or
        // (|y| < 0x1p-65 or |y| >= 0x1p63 or nan).
        if (zeroinfnan(iy))
        {
            if (2 * iy == 0) {return issignaling_inline(x) ? x + y : 1.0;}
            if (ix == __Float64(1.0).Bytes) {return issignaling_inline(y) ? x + y : 1.0;}
            if (2 * ix > 2 * uint64(CSE_POSITIVE_INF) || 2 * iy > 2 * uint64(CSE_POSITIVE_INF)) {return x + y;}
            if (2 * ix == 2 * __Float64(1.0).Bytes) {return 1.0;}
            if ((2 * ix < 2 * __Float64(1.0).Bytes) == !(iy >> 63)) {return 0.0;} // |x|<1 && y==inf or |x|>1 && y==-inf.
            return y * y;
        }
        if (zeroinfnan(ix))
        {
            double_t x2 = x * x;
            if (ix >> 63 && CheckInt(iy) == 1)
            {
                x2 = -x2;
                sign_bias = 1;
            }
            if (2 * ix == 0 && iy >> 63) {return sign_bias ? __Float64::FromBytes(CSE_NEGATIVE_INF) : __Float64::FromBytes(CSE_POSITIVE_INF);}
            // Without the barrier some versions of clang hoist the 1/x2 and
            // thus division by zero exception can be signaled spuriously.
            return iy >> 63 ? (1 / x2) : x2;
        }
        // Here x and y are non-zero finite.
        if (ix >> 63)
        {
            // Finite x < 0.
            int yint = CheckInt(iy);
            if (yint == 0) {return __Float64::FromBytes(CSE_NAN);}
            if (yint == 1) {sign_bias = (0x800 << 7);}
            ix &= 0x7FFFFFFFFFFFFFFF;
            topx &= 0x7ff;
        }
        if ((topy & 0x7ff) - 0x3be >= 0x43e - 0x3be)
        {
            // Note: sign_bias == 0 here because y is not odd.
            if (ix == __Float64(1.0).Bytes) {return 1.0;}
            if ((topy & 0x7ff) < 0x3be)
            {
                // |y| < 2^-65, x^y ~= 1 + y*log(x).
                return ix > __Float64(1.0).Bytes ? 1.0 + y : 1.0 - y;
            }
            return (ix > __Float64(1.0).Bytes) == (topy < 0x800)
                 ? 0x1.0p-767 * 0x1.0p-767  // Overflow
                 : 0x1.8p-538 * 0x1.8p-538; // Underflow
        }
        if (topx == 0)
        {
            // Normalize subnormal x so exponent becomes negative.
            ix = __Float64(x * 0x1p52).Bytes;
            ix &= 0x7fffffffffffffff;
            ix -= 52ULL << 52;
        }
    }

    // Compute y+TAIL = ln(x) where the rounded result is y and TAIL has about
    // additional 15 bits precision.  IX is the bit representation of x, but
    // normalized in the subnormal range using the sign bit for the exponent.
    static auto ln_inline = [](uint64_t ix, __Float64* tail)
    {
        /* Algorithm:
         *
            x = 2^k z
            ln(x) = k ln2 + ln(c) + ln(z/c)
            ln(z/c) = poly(z/c - 1)

        where z is in [0x1.69555p-1; 0x1.69555p0] which is split into N subintervals
        and z falls into the ith one, then table entries are computed as

            tab[i].invc = 1/c
            tab[i].lnc = round(0x1p43*ln(c))/0x1p43
            tab[i].lnctail = (double)(ln(c) - lnc)

        where c is chosen near the center of the subinterval such that 1/c has only a
        few precision bits so z/c - 1 is exactly representible as double:

            1/c = center < 1 ? round(N/center)/N : round(2*N/center)/N/2

        Note: |z/c - 1| < 1/N for the chosen c, |log(c) - logc - logctail| < 0x1p-97,
        the last few bits of logc are rounded away so k*ln2hi + logc has no rounding
        error and the interval for z is selected such that near x == 1, where log(x)
        is tiny, large cancellation error is avoided in logc + poly(z/c - 1).  */

        static const auto OFFSET = 0x3fe6955500000000;
        static const auto Ln2hi  = 0x1.62e42fefa3800p-01;
        static const auto Ln2lo  = 0x1.ef35793c76730p-45;

        // Coefficents of taylor series
        static const __Float64 _Taylor_Ln[]
        {
            // relative error: 0x1.11922ap-70
            // in -0x1.6bp-8 0x1.6bp-8
            // Coefficients are scaled to match the scaling during evaluation.
            -0.50000000000000000000000000000000 * +1,
            +0.33333333333333333333333333333333 * -2,
            -0.25000000000000000000000000000000 * -2,
            +0.20000000000000000000000000000000 * +4,
            -0.16666666666666666666666666666667 * +4,
            +0.14285714285714285714285714285714 * -8,
            -0.12500000000000000000000000000000 * -8,
        };

        // double_t for better performance on targets with FLT_EVAL_METHOD==2.
        __Float64 z, r, y, invc, logc, logctail, kd, hi, t1, t2, lo, lo1, lo2, p;
        uint64_t iz, tmp;
        int k, i;

        // x = 2^k z; where z is in range [OFF,2*OFF) and exact.
        // The range is split into N subintervals.
        // The ith subinterval contains z and c is near its center.
        tmp = ix - OFFSET;
        i = (tmp >> (52 - 7)) % (1 << 7);
        k = (int64_t)tmp >> 52; // arithmetic shift
        iz = ix - (tmp & 0xfffULL << 52);
        z = __Float64::FromBytes(iz);
        kd = __Float64(k);

        // log(x) = k*Ln2 + ln(c) + ln1p(z/c-1).
        invc = __Powf64_ln_table[i].invc;
        logc = __Powf64_ln_table[i].lnc;
        logctail = __Powf64_ln_table[i].lnctail;

        // Note: 1/c is j/N or j/N/2 where j is an integer in [N,2N) and
        // |z/c - 1| < 1/N, so r = z/c - 1 is exactly representible.
        // Split z such that rhi, rlo and rhi*rhi are exact and |rlo| <= |r|.
        double_t zhi = __Float64::FromBytes((iz + (1ULL << 31)) & (-1LL << 32));
        double_t zlo = z - zhi;
        double_t rhi = zhi * invc - 1.0;
        double_t rlo = zlo * invc;
        r = rhi + rlo;

        // k*Ln2 + ln(c) + r.
        t1 = kd * Ln2hi + logc;
        t2 = t1 + r;
        lo1 = kd * Ln2lo + logctail;
        lo2 = t1 - t2 + r;

        // Evaluation is optimized assuming superscalar pipelined execution.
        double_t ar, ar2, ar3, lo3, lo4;
        ar = _Taylor_Ln[0].x * r; /* A[0] = -0.5.  */
        ar2 = r * ar;
        ar3 = r * ar2;
        // k*Ln2 + ln(c) + r + A[0]*r*r.
        double_t arhi = _Taylor_Ln[0].x * rhi;
        double_t arhi2 = rhi * arhi;
        hi = t2 + arhi2;
        lo3 = rlo * (ar + arhi);
        lo4 = t2 - hi + arhi2;
        // p = ln1p(r) - r - A[0]*r*r.
        p = (ar3 * (_Taylor_Ln[1].x + r * _Taylor_Ln[2].x + ar2 * (_Taylor_Ln[3].x + r * _Taylor_Ln[4].x + ar2 * (_Taylor_Ln[5].x + r * _Taylor_Ln[6].x))));
        lo = lo1 + lo2 + lo3 + lo4 + p;
        y = hi + lo;
        *tail = hi - y + lo;
        return y;
    };

    __Float64 lo;
    __Float64 hi = ln_inline(ix, &lo);
    __Float64 ehi, elo;

    __Float64 yhi = __Float64::FromBytes(iy & -1LL << 27);
    __Float64 ylo = y - yhi;
    __Float64 lhi = __Float64::FromBytes(__Float64(hi).Bytes & -1LL << 27);
    __Float64 llo = hi - lhi + lo;
    ehi = yhi * lhi;
    elo = ylo * lhi + y * llo; // |elo| < |ehi| * 2^-25.

    // Computes sign*exp(x+xtail) where |xtail| < 2^-8/N and |xtail| <= |x|.
    // The sign_bias argument is SIGN_BIAS or 0 and sets the sign to -1 or 1.
    static auto exp_inline = [](__Float64 x, __Float64 xtail, uint32_t sign_bias)
    {
        static const __Float64 InvLn2N   = 0x1.71547652b82fep0 * (1 << 7);
        static const __Float64 Shift     = 0x1.8p52;
        static const __Float64 NegLn2hiN = -0x1.62e42fefa0000p-8;
        static const __Float64 NegLn2loN = -0x1.cf79abc9e3b3ap-47;

        // Coefficients of taylor series
        static const __Float64 C2 = 0.50000000000000000000000000000000;
        static const __Float64 C3 = 0.16666666666666666666666666666667;
        static const __Float64 C4 = 0.04166666666666666666666666666667;
        static const __Float64 C5 = 0.00833333333333333333333333333333;

        uint32_t abstop;
        uint64_t ki, idx, top, sbits;
        // double_t for better performance on targets with FLT_EVAL_METHOD==2.
        __Float64 kd, z, r, r2, scale, tail, tmp;

        static auto top12 = [](__Float64 x)
        {
            return (x).Bytes >> 52;
        };

        abstop = top12(x) & 0x7ff;
        if ((abstop - top12(0x1p-54) >= top12(512.0) - top12(0x1p-54)))
        {
            if (abstop - top12(0x1p-54) >= 0x80000000)
            {
                // Avoid spurious underflow for tiny x.
                // Note: 0 is common input.
                __Float64 one = 1.0 + x;
                return sign_bias ? -one.x : one.x;
            }
            if (abstop >= top12(1024.0))
            {
                // Note: inf and nan are already handled.
                if ((x).Bytes >> 63) {return (sign_bias ? -0x1p-767 : 0x1p-767) * 0x1p-767;}
                else {return (sign_bias ? -0x1.8p-538 : 0x1.8p-538) * 0x1.8p-538;}
            }
            // Large x is special cased below.
            abstop = 0;
        }

        // exp(x) = 2^(k/N) * exp(r), with exp(r) in [2^(-1/2N),2^(1/2N)].
        // x = ln2/N*k + r, with int k and r in [-ln2/2N, ln2/2N].
        z = InvLn2N.x * x;
        // z - kd is in [-1, 1] in non-nearest rounding modes.
        kd = z + Shift.x;
        ki = kd.Bytes;
        kd -= Shift;

        r = x + kd * NegLn2hiN.x + kd * NegLn2loN.x;
        // The code assumes 2^-200 < |xtail| < 2^-8/N.
        r += xtail;
        // 2^(k/N) ~= scale * (1 + tail).
        idx = 2 * (ki % (1 << 7));
        top = (ki + sign_bias) << (52 - 7);
        tail = __Float64::FromBytes(__Expf64_table[idx]);
        // This is only a valid scale when -1023*N < k < 1024*N.
        sbits = __Expf64_table[idx + 1] + top;
        // exp(x) = 2^(k/N) * exp(r) ~= scale + scale * (tail + exp(r) - 1).
        // Evaluation is optimized assuming superscalar pipelined execution.
        r2 = r * r;
        // Without fma the worst case error is 0.25/N ulp larger.
        // Worst case error is less than 0.5+1.11/N+(abs poly error * 2^53) ulp.
        tmp = tail + r + r2 * (C2.x + r * C3.x) + r2 * r2 * (C4.x + r * C5.x);
        if (abstop == 0)
        {
            /* Handle cases that may overflow or underflow when computing the result that
               is scale*(1+TMP) without intermediate rounding.  The bit representation of
               scale is in SBITS, however it has a computed exponent that may have
               overflown into the sign bit so that needs to be adjusted before using it as
               a double.  (int32_t)KI is the k used in the argument reduction and exponent
               adjustment of scale, positive k here means the result may overflow and
               negative k means the result may underflow.  */
            static auto specialcase = [](__Float64 tmp, uint64_t sbits, uint64_t ki)
            {
                double_t scale, y;

                if ((ki & 0x80000000) == 0)
                {
                    // k > 0, the exponent of scale might have overflowed by <= 460.
                    sbits -= 1009ull << 52;
                    scale = __Float64::FromBytes(sbits);
                    y = 0x1p1009 * (scale + scale * tmp);
                    return y;
                }
                // k < 0, need special care in the subnormal range.
                sbits += 1022ull << 52;
                // Note: sbits is signed scale.
                scale = __Float64::FromBytes(sbits);
                y = scale + scale * tmp;
                if (fabs (y) < 1.0)
                {
                    /* Round y to the right precision before scaling it into the subnormal
                     range to avoid double rounding that can cause 0.5+E/2 ulp error where
                     E is the worst-case ulp error outside the subnormal range.  So this
                     is only useful if the goal is better than 1 ulp worst-case error.  */
                    __Float64 hi, lo, one = 1.0;
                    if (y < 0.0) {one = -1.0;}
                    lo = scale - y + scale * tmp;
                    hi = one + y;
                    lo = one - hi + y + lo;
                    y = (hi + lo) - one;
                    // Fix the sign of 0.
                    if (y == 0.0) {y = __Float64::FromBytes(sbits & 0x8000000000000000);}
                }
                y = 0x1p-1022 * y;
                return y;
            };

            return specialcase(tmp, sbits, ki);
        }
        scale = __Float64::FromBytes(sbits);
        /* Note: tmp == 0 or |tmp| > 2^-200 and scale > 2^-739, so there
     is no spurious underflow here even without fma.  */
        return scale + scale * tmp;
    };

    return exp_inline(ehi, elo, sign_bias);
}

_END_EXTERN_C
_CSE_END
