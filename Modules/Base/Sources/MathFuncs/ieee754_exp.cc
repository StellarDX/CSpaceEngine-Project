// Exponential function e^x for double and quaduple-precision floating point
// Algorithm is taken from GLibC, made by FSF (LGPL License)
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

// Unremarkable code. (平平无奇的代码)

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/ConstLists.h"
#include <cfloat>

_CSE_BEGIN

float64 __cdecl exp(float64 _X) { return __IEEE754_EXP64F(_X); };
complex64 __cdecl expc(complex64 _X) { return __GLIBCT_EXP64C(_X); }

_EXTERN_C

// --------------------DOUBLE-PRECISION-------------------- //

const unsigned long long __Exp64f_table[]
{
    0x0000000000000000, 0x3FF0000000000000,
    0x3C9B3B4F1A88BF6E, 0x3FEFF63DA9FB3335,
    0xBC7160139CD8DC5D, 0x3FEFEC9A3E778061,
    0xBC905E7A108766D1, 0x3FEFE315E86E7F85,
    0x3C8CD2523567F613, 0x3FEFD9B0D3158574,
    0xBC8BCE8023F98EFA, 0x3FEFD06B29DDF6DE,
    0x3C60F74E61E6C861, 0x3FEFC74518759BC8,
    0x3C90A3E45B33D399, 0x3FEFBE3ECAC6F383,
    0x3C979AA65D837B6D, 0x3FEFB5586CF9890F,
    0x3C8EB51A92FDEFFC, 0x3FEFAC922B7247F7,
    0x3C3EBE3D702F9CD1, 0x3FEFA3EC32D3D1A2,
    0xBC6A033489906E0B, 0x3FEF9B66AFFED31B,
    0xBC9556522A2FBD0E, 0x3FEF9301D0125B51,
    0xBC5080EF8C4EEA55, 0x3FEF8ABDC06C31CC,
    0xBC91C923B9D5F416, 0x3FEF829AAEA92DE0,
    0x3C80D3E3E95C55AF, 0x3FEF7A98C8A58E51,
    0xBC801B15EAA59348, 0x3FEF72B83C7D517B,
    0xBC8F1FF055DE323D, 0x3FEF6AF9388C8DEA,
    0x3C8B898C3F1353BF, 0x3FEF635BEB6FCB75,
    0xBC96D99C7611EB26, 0x3FEF5BE084045CD4,
    0x3C9AECF73E3A2F60, 0x3FEF54873168B9AA,
    0xBC8FE782CB86389D, 0x3FEF4D5022FCD91D,
    0x3C8A6F4144A6C38D, 0x3FEF463B88628CD6,
    0x3C807A05B0E4047D, 0x3FEF3F49917DDC96,
    0x3C968EFDE3A8A894, 0x3FEF387A6E756238,
    0x3C875E18F274487D, 0x3FEF31CE4FB2A63F,
    0x3C80472B981FE7F2, 0x3FEF2B4565E27CDD,
    0xBC96B87B3F71085E, 0x3FEF24DFE1F56381,
    0x3C82F7E16D09AB31, 0x3FEF1E9DF51FDEE1,
    0xBC3D219B1A6FBFFA, 0x3FEF187FD0DAD990,
    0x3C8B3782720C0AB4, 0x3FEF1285A6E4030B,
    0x3C6E149289CECB8F, 0x3FEF0CAFA93E2F56,
    0x3C834D754DB0ABB6, 0x3FEF06FE0A31B715,
    0x3C864201E2AC744C, 0x3FEF0170FC4CD831,
    0x3C8FDD395DD3F84A, 0x3FEEFC08B26416FF,
    0xBC86A3803B8E5B04, 0x3FEEF6C55F929FF1,
    0xBC924AEDCC4B5068, 0x3FEEF1A7373AA9CB,
    0xBC9907F81B512D8E, 0x3FEEECAE6D05D866,
    0xBC71D1E83E9436D2, 0x3FEEE7DB34E59FF7,
    0xBC991919B3CE1B15, 0x3FEEE32DC313A8E5,
    0x3C859F48A72A4C6D, 0x3FEEDEA64C123422,
    0xBC9312607A28698A, 0x3FEEDA4504AC801C,
    0xBC58A78F4817895B, 0x3FEED60A21F72E2A,
    0xBC7C2C9B67499A1B, 0x3FEED1F5D950A897,
    0x3C4363ED60C2AC11, 0x3FEECE086061892D,
    0x3C9666093B0664EF, 0x3FEECA41ED1D0057,
    0x3C6ECCE1DAA10379, 0x3FEEC6A2B5C13CD0,
    0x3C93FF8E3F0F1230, 0x3FEEC32AF0D7D3DE,
    0x3C7690CEBB7AAFB0, 0x3FEEBFDAD5362A27,
    0x3C931DBDEB54E077, 0x3FEEBCB299FDDD0D,
    0xBC8F94340071A38E, 0x3FEEB9B2769D2CA7,
    0xBC87DECCDC93A349, 0x3FEEB6DAA2CF6642,
    0xBC78DEC6BD0F385F, 0x3FEEB42B569D4F82,
    0xBC861246EC7B5CF6, 0x3FEEB1A4CA5D920F,
    0x3C93350518FDD78E, 0x3FEEAF4736B527DA,
    0x3C7B98B72F8A9B05, 0x3FEEAD12D497C7FD,
    0x3C9063E1E21C5409, 0x3FEEAB07DD485429,
    0x3C34C7855019C6EA, 0x3FEEA9268A5946B7,
    0x3C9432E62B64C035, 0x3FEEA76F15AD2148,
    0xBC8CE44A6199769F, 0x3FEEA5E1B976DC09,
    0xBC8C33C53BEF4DA8, 0x3FEEA47EB03A5585,
    0xBC845378892BE9AE, 0x3FEEA34634CCC320,
    0xBC93CEDD78565858, 0x3FEEA23882552225,
    0x3C5710AA807E1964, 0x3FEEA155D44CA973,
    0xBC93B3EFBF5E2228, 0x3FEEA09E667F3BCD,
    0xBC6A12AD8734B982, 0x3FEEA012750BDABF,
    0xBC6367EFB86DA9EE, 0x3FEE9FB23C651A2F,
    0xBC80DC3D54E08851, 0x3FEE9F7DF9519484,
    0xBC781F647E5A3ECF, 0x3FEE9F75E8EC5F74,
    0xBC86EE4AC08B7DB0, 0x3FEE9F9A48A58174,
    0xBC8619321E55E68A, 0x3FEE9FEB564267C9,
    0x3C909CCB5E09D4D3, 0x3FEEA0694FDE5D3F,
    0xBC7B32DCB94DA51D, 0x3FEEA11473EB0187,
    0x3C94ECFD5467C06B, 0x3FEEA1ED0130C132,
    0x3C65EBE1ABD66C55, 0x3FEEA2F336CF4E62,
    0xBC88A1C52FB3CF42, 0x3FEEA427543E1A12,
    0xBC9369B6F13B3734, 0x3FEEA589994CCE13,
    0xBC805E843A19FF1E, 0x3FEEA71A4623C7AD,
    0xBC94D450D872576E, 0x3FEEA8D99B4492ED,
    0x3C90AD675B0E8A00, 0x3FEEAAC7D98A6699,
    0x3C8DB72FC1F0EAB4, 0x3FEEACE5422AA0DB,
    0xBC65B6609CC5E7FF, 0x3FEEAF3216B5448C,
    0x3C7BF68359F35F44, 0x3FEEB1AE99157736,
    0xBC93091FA71E3D83, 0x3FEEB45B0B91FFC6,
    0xBC5DA9B88B6C1E29, 0x3FEEB737B0CDC5E5,
    0xBC6C23F97C90B959, 0x3FEEBA44CBC8520F,
    0xBC92434322F4F9AA, 0x3FEEBD829FDE4E50,
    0xBC85CA6CD7668E4B, 0x3FEEC0F170CA07BA,
    0x3C71AFFC2B91CE27, 0x3FEEC49182A3F090,
    0x3C6DD235E10A73BB, 0x3FEEC86319E32323,
    0xBC87C50422622263, 0x3FEECC667B5DE565,
    0x3C8B1C86E3E231D5, 0x3FEED09BEC4A2D33,
    0xBC91BBD1D3BCBB15, 0x3FEED503B23E255D,
    0x3C90CC319CEE31D2, 0x3FEED99E1330B358,
    0x3C8469846E735AB3, 0x3FEEDE6B5579FDBF,
    0xBC82DFCD978E9DB4, 0x3FEEE36BBFD3F37A,
    0x3C8C1A7792CB3387, 0x3FEEE89F995AD3AD,
    0xBC907B8F4AD1D9FA, 0x3FEEEE07298DB666,
    0xBC55C3D956DCAEBA, 0x3FEEF3A2B84F15FB,
    0xBC90A40E3DA6F640, 0x3FEEF9728DE5593A,
    0xBC68D6F438AD9334, 0x3FEEFF76F2FB5E47,
    0xBC91EEE26B588A35, 0x3FEF05B030A1064A,
    0x3C74FFD70A5FDDCD, 0x3FEF0C1E904BC1D2,
    0xBC91BDFBFA9298AC, 0x3FEF12C25BD71E09,
    0x3C736EAE30AF0CB3, 0x3FEF199BDD85529C,
    0x3C8EE3325C9FFD94, 0x3FEF20AB5FFFD07A,
    0x3C84E08FD10959AC, 0x3FEF27F12E57D14B,
    0x3C63CDAF384E1A67, 0x3FEF2F6D9406E7B5,
    0x3C676B2C6C921968, 0x3FEF3720DCEF9069,
    0xBC808A1883CCB5D2, 0x3FEF3F0B555DC3FA,
    0xBC8FAD5D3FFFFA6F, 0x3FEF472D4A07897C,
    0xBC900DAE3875A949, 0x3FEF4F87080D89F2,
    0x3C74A385A63D07A7, 0x3FEF5818DCFBA487,
    0xBC82919E2040220F, 0x3FEF60E316C98398,
    0x3C8E5A50D5C192AC, 0x3FEF69E603DB3285,
    0x3C843A59AC016B4B, 0x3FEF7321F301B460,
    0xBC82D52107B43E1F, 0x3FEF7C97337B9B5F,
    0xBC892AB93B470DC9, 0x3FEF864614F5A129,
    0x3C74B604603A88D3, 0x3FEF902EE78B3FF6,
    0x3C83C5EC519D7271, 0x3FEF9A51FBC74C83,
    0xBC8FF7128FD391F0, 0x3FEFA4AFA2A490DA,
    0xBC8DAE98E223747D, 0x3FEFAF482D8E67F1,
    0x3C8EC3BC41AA2008, 0x3FEFBA1BEE615A27,
    0x3C842B94C3A9EB32, 0x3FEFC52B376BBA97,
    0x3C8A64A931D185EE, 0x3FEFD0765B6E4540,
    0xBC8E37BAE43BE3ED, 0x3FEFDBFDAD9CBE14,
    0x3C77893B4D91CD9D, 0x3FEFE7C1819E90D8,
    0x3C5305C14160CC89, 0x3FEFF3C22B8F71F1,
};

__Float64 __cdecl __IEEE754_EXP64F(__Float64 _X)
{
    __Float64 x = _X;
    uint32_t abstop = x.Exponent;
    uint64 ki, idx, top, sbits;
    __Float64 kd, z, r, scale, tail, tmp;

    if (abstop - 0x3C9 >= 0x408 - 0x3C9) // 0x063
    {
        if (abstop - 0x3C9 >= 0x80000000)
        {
            // Avoid spurious underflow for tiny x.
            // Note: 0 is common input.
            return 1.0 + x;
        }

        if (abstop >= 0x409) // 1024
        {
            if (x.Bytes == NEG_INF_DOUBLE) { return 0.0; }
            if (abstop >= 0x7FF) { return 1.0 + x; }             // negative inf
            if (x.Bytes >> 63) { return __Float64::FromBytes(0); } // e^(-inf) = 0
            else { return __Float64::FromBytes(POS_INF_DOUBLE); }  // e^(+inf) = +inf
        }
        // Large x is special cased below.
        abstop = 0;
    }

    // exp(x) = 2^(k/N) * exp(r), with exp(r) in [2^(-1/2N),2^(1/2N)].
    // x = ln2/N*k + r, with int k and r in [-ln2/2N, ln2/2N].
    int64 N = 1 << 7;
    __Float64 InvLn2N = 1.4426950408889634073599246810019 * N; // N / ln2
    z = InvLn2N * x;

    // z - kd is in [-1, 1] in non-nearest rounding modes.
    __Float64 Shift = 0x1.8p52;
    kd = z + Shift;
    ki = kd.Bytes;
    kd -= Shift; // Round z to integer

    __Float64 NegLn2N = -0.69314718055994530941723212145818 / N;                    // 0xBF762E42FEFA39EF (-ln2 / N)
    __Float64 NegLn2hiN = __Float64::FromBytes(NegLn2N.Bytes & 0xFFFFFFFFFFFF0000); // 0xBF762E42FEFA0000 (extract higher 48 bits from NegLn2N)
    __Float64 NegLn2loN = __Float64::FromBytes(0xBD0CF79ABC9E3B3A);                 // NegLn2hiN - NegLn2N
    r = x + kd * NegLn2hiN + kd * NegLn2loN;

    // 2^(k/N) ~= scale * (1 + tail).
    idx = 2 * (ki % N);
    top = ki << (52 - 7);
    tail = __Float64::FromBytes(__Exp64f_table[idx]);
    sbits = __Exp64f_table[idx + 1] + top; // This is only a valid scale when -1023*N < k < 1024*N.

    // exp(x) = 2^(k/N) * exp(r) ~= scale + scale * (tail + exp(r) - 1).
    // Talor series of exp(x), and using Qin Jiushao's method to simplify.
    // Without this step, the worst case error is 0.25/N ulp larger.
    // and the Worst case error is less than 0.5+1.11/N+(abs poly error * 2^53) ulp after this step.
    __Float64 C1 = 1.00000000000000000000000000000000000E+00; // 1 / 1!
    __Float64 C2 = 5.00000000000000000000000000000000000E-01; // 1 / 2!
    __Float64 C3 = 1.66666666666666666666666666666666666E-01; // 1 / 3!
    __Float64 C4 = 4.16666666666666666666666666666666666E-02; // 1 / 4!
    __Float64 C5 = 8.33333333333333333333333333333333333E-03; // 1 / 5!
    // exp(x) = SUM(int n : [1, inf]){pow(x, n) / n!}
    tmp = tail + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));

    // Note: tmp == 0 or |tmp | > 2 ^ -65 and scale > 2 ^ -739, so there
    // is no spurious underflow here even without fma.
    if (abstop == 0)
    {
        // Handle cases that may overflow or underflow when computing the result that
        // is scale * (1 + TMP) without intermediate rounding.The bit representation of
        // scale is in SBITS, however it has a computed exponent that may have
        // overflown into the sign bit so that needs to be adjusted before using it as
        // a double.  (int32_t)KI is the k used in the argument reduction and exponent
        // adjustment of scale, positive k here means the result may overflow and
        // negative k means the result may underflow
        static auto specialcase = [](double_t tmp, uint64_t sbits, uint64_t ki)
        {
            __Float64 scale, y;

            // k > 0, the exponent of scale might have overflowed by <= 460.
            if ((ki & 0x80000000) == 0)
            {
                sbits -= 1009ull << 52;
                scale = __Float64::FromBytes(sbits);
                return 0x1p1009 * (scale + scale * tmp);
            }

            // k < 0, need special care in the subnormal range.
            sbits += 1022ull << 52;
            scale = __Float64::FromBytes(sbits);
            y = scale + scale * tmp;

            // Round y to the right precision before scaling it into the subnormal
            // range to avoid double rounding that can cause 0.5 + E / 2 ulp error where
            // E is the worst - case ulp error outside the subnormal range.So this
            // is only useful if the goal is better than 1 ulp worst - case error.
            if (y < 1.0)
            {
                __Float64 hi, lo;
                lo = scale - y + scale * tmp;
                hi = 1.0 + y;
                lo = 1.0 - hi + y + lo;
                y = (hi + lo) - 1.0;
                // Avoid -0.0 with downward rounding. (results of exp(x) are impossible to approach negative infinitesimal.)
                if (y == 0.0) { y = 0.0; }
            }
            return 0x1p-1022 * y;
        };
        return specialcase(tmp, sbits, ki);
    }
    else
    {
        scale = __Float64::FromBytes(sbits);
        return scale + scale * tmp;
    }
}

/*							expm1l.c
 *
 *	Exponential function, minus 1
 *      128-bit quaduple precision
 *
 *
 *
 * SYNOPSIS:
 *
 * quaduple x, y, expm1l();
 *
 * y = exp( x ) - 1;
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns e (2.71828...) raised to the x power, minus one.
 *
 * Range reduction is accomplished by separating the argument
 * into an integer k and fraction f such that
 *
 *     x    k  f
 *    e  = 2  e.
 *
 * An expansion x + .5 x^2 + x^3 R(x) approximates exp(f) - 1
 * in the basic range [-0.5 ln 2, 0.5 ln 2].
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE    -79,+MAXLOG    100,000     1.7e-34     4.5e-35
 *
 */

/*
    Copyright 2001 by Stephen L. Moshier

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <https://www.gnu.org/licenses/>.
*/
__Float64 __cdecl __IEEE854_EXPXM1128F_C64F(__Float64 _X)
{
    static const __Float64 // ln(2^(-114))
        ln2pm114 = -7.9018778583833765273564461846232128760607E1L;

    __Float64 x = _X;
    __Float64 px, qx, xx;
    int32_t ix, lx, sign;
    int k;
    __Float64 xhi;

    // Detect infinity and NaN.
    xhi = x;
    ix = x.parts.msw;
    lx = x.parts.lsw;
    sign = ix & 0x80000000;
    ix &= 0x7fffffff;
    if (!sign && ix >= 0x40600000) {return __IEEE754_EXP64F(x);}
    if (ix >= 0x7ff00000)
    {
        // Infinity (which must be negative infinity).
        if (((ix - 0x7ff00000) | lx) == 0) {return -1.0L;}
        // NaN.  Invalid exception if signaling.
        return __Float64::FromBytes(S_NAN_DOUBLE);
    }

    // exp(+- 0) - 1 = +- 0.
    if ((ix | lx) == 0) {return x;}

    /* Minimum value.  */
    if (x < ln2pm114.x) {return -1;}

    // Express x = ln 2 (k + remainder), remainder not exceeding 1/2.
    static const __Float64
        C1 = 6.93145751953125E-1L, // 0x3fe62e4000000000, Higher 28-bit of ln(2)
        C2 = 1.428606820309417232121458176568075500134E-6L; // ln(2) - C1
    xx = C1.x + C2.x; // ln 2.
    px = cse::floor(0.5 + x / xx);
    k = px;
    // remainder times ln 2
    x -= px * C1.x;
    x -= px * C2.x;

    // Pade 8/8 Approximate exp(remainder ln 2).
    __Float64
        P0 = 2.943520915569954073888921213330863757240E8L,
        P1 = -5.722847283900608941516165725053359168840E7L,
        P2 = 8.944630806357575461578107295909719817253E6L,
        P3 = -7.212432713558031519943281748462837065308E5L,
        P4 = 4.578962475841642634225390068461943438441E4L,
        P5 = -1.716772506388927649032068540558788106762E3L,
        P6 = 4.401308817383362136048032038528753151144E1L,
        P7 = -4.888737542888633647784737721812546636240E-1L,
        Q0 = 1.766112549341972444333352727998584753865E9L,
        Q1 = -7.848989743695296475743081255027098295771E8L,
        Q2 = 1.615869009634292424463780387327037251069E8L,
        Q3 = -2.019684072836541751428967854947019415698E7L,
        Q4 = 1.682912729190313538934190635536631941751E6L,
        Q5 = -9.615511549171441430850103489315371768998E4L,
        Q6 = 3.697714952261803935521187272204485251835E3L,
        Q7 = -8.802340681794263968892934703309274564037E1L;
    // Q8 = 1.000000000000000000000000000000000000000E0
    px = (((((((P7 * x
        + P6) * x
        + P5) * x + P4) * x + P3) * x + P2) * x + P1) * x + P0) * x;
    qx = (((((((x
        + Q7) * x
        + Q6) * x + Q5) * x + Q4) * x + Q3) * x + Q2) * x + Q1) * x + Q0;
    xx = x * x;
    qx = x + (0.5 * xx + xx * px / qx);

    // exp(x) = exp(k ln 2) exp(remainder ln 2) = 2^k exp(remainder ln 2).
    // We have qx = exp(remainder ln 2) - 1, so
    // exp(x) - 1 = 2^k (qx + 1) - 1 => 2^k qx + 2^k - 1.
    px = ::ldexp(1.0L, k);
    x = px * qx + (px - 1.0);
    return x;
}

complex64 __cdecl __GLIBCT_EXP64C(complex64 _X)
{
    float64 XReal = _X.real(), XImag = _X.imag();
    int RClass = std::fpclassify(XReal);
    int IClass = std::fpclassify(XImag);

    if (RClass >= FP_ZERO) // Real part is finite
    {
        if (IClass >= FP_ZERO) // Imaginary part is finite.
        {
            // exp(x) = exp(Re) * (cos(Im) + i * sin(Im))
            static const int IExpMax = int((DBL_MAX_EXP - 1) * CSE_LN2);
            float64 SinImag, CosImag;

            if (abs(XImag) > DBL_MIN)
            {
                SinImag = __IBM_SIN64F(XImag);
                CosImag = __IBM_COS64F(XImag);
            }
            else
            {
                SinImag = XImag;
                CosImag = 1;
            }

            static const float64 ExpMax = __IEEE754_EXP64F(IExpMax);
            for (int i = 0; i < 2 && XReal > IExpMax; ++i)
            {
                XReal -= IExpMax;
                SinImag *= ExpMax;
                CosImag *= ExpMax;
            }

            if (XReal > IExpMax) // Overflow (original real part of x > 3t).
            {
                return {DBL_MAX * CosImag, DBL_MAX * SinImag};
            }
            else
            {
                float64 ExpReal = __IEEE754_EXP64F(XReal);
                return {ExpReal * CosImag, ExpReal * SinImag};
            }
        }
        else
        {
            // If the imaginary part is +-inf or NaN and the real part
            // is not +-inf the result is NaN + iNaN.
            CSESysDebug("MathFunctions", CSEDebugger::ERROR,
                "Invalid argument of exp(x) (imag part is nan)");
            return {__Float64::FromBytes(BIG_NAN_DOUBLE),
                __Float64::FromBytes(BIG_NAN_DOUBLE)};
        }
    }
    else if (RClass == FP_INFINITE) // Real part is infinite.
    {
        if (IClass >= FP_ZERO) // Imaginary part is finite.
        {
            float64 RValue = std::signbit(XReal) ? 0 :
                __Float64::FromBytes(POS_INF_DOUBLE).x;

            if (IClass == FP_ZERO) // Imaginary part is 0.0.
            {
                return {RValue, 0};
            }

            float64 CosImag = 0, SinImag = XImag;
            if (abs(XImag) > DBL_MIN)
            {
                SinImag = __IBM_SIN64F(XImag);
                CosImag = __IBM_COS64F(XImag);
            }

            return {::copysign(RValue, CosImag),
                ::copysign(RValue, SinImag)};
        }
        else if (std::signbit(XReal))
        {
            return {0, ::copysign(0, XImag)};
        }
        else
        {
            return {__Float64::FromBytes(POS_INF_DOUBLE),
                XImag - XImag};
        }
    }
    else
    {
        // If the real part is NaN the result is NaN + iNaN unless the
        // imaginary part is zero.
        if (IClass == FP_ZERO)
        {
            return {__Float64::FromBytes(BIG_NAN_DOUBLE), XImag};
        }
        else
        {
            if (RClass != FP_NAN || IClass != FP_NAN)
            {
                CSESysDebug("MathFunctions", CSEDebugger::ERROR,
                    "Invalid argument of exp(x).");
            }
            return {__Float64::FromBytes(BIG_NAN_DOUBLE),
                __Float64::FromBytes(BIG_NAN_DOUBLE)};
        }
    }
}

_END_EXTERN_C
_CSE_END

#if 0 // exp test program generated by Deepseek
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <random>
#include <vector>
#include <string>

#include <CSE/Base.h>

void write_csv_header(std::ofstream& csv_file)
{
    csv_file << "x, Custom_Exp, Std_Exp, Absolute_Difference, ULP_Error, Relative_Error\n";
}

void write_test_case(std::ofstream& csv_file, double x, double custom, double std_exp)
{
    double diff = custom - std_exp;
    double ulp = std::nextafter(std_exp, std::numeric_limits<double>::infinity()) - std_exp;
    double ulp_error = (ulp == 0) ? 0.0 : std::abs(diff / ulp);
    double rel_error = (std_exp == 0) ? 0.0 : std::abs(diff / std_exp);

    csv_file << std::scientific << std::setprecision(15);
    csv_file << x << ",";
    csv_file << custom << ",";
    csv_file << std_exp << ",";
    csv_file << diff << ",";
    csv_file << ulp_error << ",";
    csv_file << rel_error << "\n";
}

int main()
{
    std::ofstream csv_file("exp_test_results.csv");
    if (!csv_file.is_open())
    {
        std::cerr << "Failed to open file: " << "exp_test_results.csv" << std::endl;
        return 1;
    }

    write_csv_header(csv_file);

    std::vector<double> test_cases =
    {
        // 特殊值
        0.0, -0.0, INFINITY, -INFINITY, NAN,

        // 边界值
        709.78271289338397, 709.782712893384,
        -708.3964185322641, -708.3964185322642,
        0x1p-10, -0x1p-10,

        // 常规值
        1.0, 2.0, 10.0, -1.0, -2.0, -10.0,
        50.0, 100.0, -50.0, -100.0,
        500.0, 700.0, -500.0, -700.0,

        // 精度敏感区域
        0.9999999999999999, 1.0000000000000001,
        -0.9999999999999999, -1.0000000000000001,
        1e-20, -1e-20, 1e-100, -1e-100,
        0x1p-50, -0x1p-50
    };

    // 随机测试
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-700, 700);
    for (int i = 0; i < 1000; ++i)
    {
        test_cases.push_back(dis(gen));
    }

    for (double x : test_cases)
    {
        double custom = cse::__IEEE754_EXP64F(x);
        double std_exp = std::exp(x);
        write_test_case(csv_file, x, custom, std_exp);
    }

    csv_file.close();
    std::cout << "Test results saved to: " << "exp_test_results.csv" << std::endl;
}
#endif

#if 0 // expm1 test program generated by Deepseek
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <fstream>

#include <CSE/Base.h>

using namespace cse;

int main()
{
    std::ofstream out("expxm1_test_report.csv");

    // CSV表头
    out << "x值,自定义函数结果,标准库结果,绝对误差,相对误差\n";

    // 测试用例数组
    double test_cases[] =
    {
        // 特殊值
        0.0,
        -0.0,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),

        // 极小值
        -7.9018778583833765273564461846232128760607E1L,  // ln2pm114
        -7.9018778583833765273564461846232128760607E1L - 1e-10,
        -7.9018778583833765273564461846232128760607E1L + 1e-10,

        // 边界值
        -1.0,
        -0.5,
        -0.1,
        0.1,
        0.5,
        1.0,

        // 正常范围值
        -1e-5,
        -1e-10,
        1e-5,
        1e-10,

        // 大负数
        -100.0,
        -50.0,
        -20.0,

        // 正数（应调用标准exp）
        1.0,
        2.0,
        10.0,
        100.0,
        700.0  // 接近double能表示的最大exp值
    };

    for (double x : test_cases)
    {
        double custom_result = __IEEE854_EXPXM1128F_C64F(x);
        double std_result = std::expm1(x);

        double abs_error = std::abs(custom_result - std_result);
        double rel_error = (std_result != 0) ? abs_error / std::abs(std_result) : 0;

        out << std::setprecision(15) << x << ","
            << custom_result << ","
            << std_result << ","
            << abs_error << ","
            << rel_error << "\n";
    }

    out.close();
    std::cout << "测试结果已保存到 expm1_test_report.csv" << std::endl;
}
#endif

#if 0 // expc test program generated by Deepseek
#include <iostream>
#include <complex>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cfloat>

#include <CSE/Base.h>

void ExpcTest(const std::complex<double>& z)
{
    // 测试函数结果
    std::complex<double> custom_result = cse::__GLIBCT_EXP64C(z);

    // 标准库结果
    std::complex<double> std_result = std::exp(z);

    // 计算差异
    double real_diff = std::abs(custom_result.real() - std_result.real());
    double imag_diff = std::abs(custom_result.imag() - std_result.imag());

    // 输出结果
    std::cout << std::scientific << std::setprecision(15);
    std::cout << "Input: (" << z.real() << ", " << z.imag() << "i)\n";
    std::cout << "  Custom exp: (" << custom_result.real() << ", " << custom_result.imag() << "i)\n";
    std::cout << "  Std exp:    (" << std_result.real() << ", " << std_result.imag() << "i)\n";
    std::cout << "  Real diff:  " << real_diff << "\n";
    std::cout << "  Imag diff:  " << imag_diff << "\n";
    std::cout << "----------------------------------------\n";
}

int main()
{
    std::cout << "expc(x) Test Report\n";
    std::cout << "----------------------------------------\n";
    // 常规有限值测试
    ExpcTest({0.0, 0.0});
    ExpcTest({1.0, 0.0});
    ExpcTest({-1.0, 0.0});
    ExpcTest({0.0, 1.0});
    ExpcTest({1.0, 1.0});
    ExpcTest({-1.0, -1.0});
    ExpcTest({123.456, 789.012});
    ExpcTest({1e-10, 1e-10});

    // 边界条件测试
    ExpcTest({DBL_MAX, 0.0});
    ExpcTest({-DBL_MAX, 0.0});
    ExpcTest({0.0, DBL_MAX});
    ExpcTest({0.0, -DBL_MAX});
    ExpcTest({DBL_MIN, DBL_MIN});
    ExpcTest({-DBL_MIN, -DBL_MIN});

    // 特殊值测试
    ExpcTest({INFINITY, 0.0});
    ExpcTest({-INFINITY, 0.0});
    ExpcTest({0.0, INFINITY});
    ExpcTest({0.0, -INFINITY});
    ExpcTest({INFINITY, INFINITY});
    ExpcTest({NAN, 0.0});
    ExpcTest({0.0, NAN});
    ExpcTest({NAN, NAN});

    // 溢出测试
    ExpcTest({1000.0, 0.0});
    ExpcTest({1000.0, 1.0});

    return 0;
}
#endif
