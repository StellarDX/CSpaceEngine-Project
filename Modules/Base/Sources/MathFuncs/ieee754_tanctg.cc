/*
    角度tan/ctg函数

    此设计基于GlibC提供的四倍精度正切函数整理而来，原本由Sun公司设计，Stephen L. 
    Moshier将精度扩展到四倍精度。角度正切函数以180为一个周期，且tan(-x) = tan(x)，
    tan(x) = ctg(90 - x)，这就意味着实际计算时只需要计算0 ~ 45之间的值就行了，其他角
    度可以使用诱导公式间接求得。0 ~ 45之间的算法是，先判断角度是不是小于~38.6，因为大于
    这个值时正切函数的曲率将会变得非常离谱且无法用多项式去拟合了，此时就需要再次使用诱导
    公式反转到38.6以内再去计算。剩下的用帕德逼近就可以得到一个足够精确的值。
*/

// Sun提供的许可证：
/*
    Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

    Developed at SunPro, a Sun Microsystems, Inc. business.
    Permission to use, copy, modify, and distribute this software is freely 
    granted, provided that this notice is preserved.
*/

// GLibC提供的许可证：
/*
    Quaduple precision expansions are
    Copyright (C) 2001 Stephen L. Moshier <moshier@na-net.ornl.gov>
    and are incorporated herein by permission of the author.  The author
    reserves the right to distribute this material elsewhere under different
    copying permissions.  These modifications are distributed here under
    the following terms:

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

#include "CSE/Base/Algorithms.h"
#include "CSE/Base/MathFuncs.h"

_CSE_BEGIN

__Float64 __cdecl __IEEE754_PADE_TANCTG(__Float64 _X, bool _Inv)
{
    if (abs(_X) > 45)
    {
        throw std::logic_error("argument out of range.");
    }

    __Float64 T, U, R;
    int32_t ix, sign, hx, lx;

    __Float64 x = _X, xhi = x;
    hx = xhi.parts.msw;
    lx = xhi.parts.lsw;
    ix = hx & 0x7fffffff;

    if (ix < 0x3c600000) // x < 2**-57
    {
        if ((int)x == 0) // generate inexact
        {
            if ((ix | lx) == 0 && _Inv) {return 1. / abs(x);}
            else if (_Inv) {return 1. / x;}
            else {return x;}
        }
    }

    /*
        正切函数tan(x)在x接近45时，其曲率会显著增大。如果直接用多项式逼近
        （如泰勒展开或帕德逼近），在x较大时可能需要更高阶的项才能保证精度，
        但这样会增加计算量。故此处以38.6为分解点，大于此分解点的x使用正切
        恒等式反转后再拟合。设y = 45 - x，则有：
        tan(x) = tan(45 - y) = (1 - tan(y)) / (1 + tan(y))
              => 1 - 2 * (tan(y) - (tan(y)^2) / (1+tan(y)))
    */
    static const auto TANCTG_BOUNDARY = 38.6;
    if (_X >= TANCTG_BOUNDARY)
    {
        if (std::signbit(x))
        {
            x = -x;
            sign = -1;
        }
        else {sign = 1;}
        x = 45 - x;
    }

    // 角度帕德逼近
    // tan x = x + x^3 / 3 + x^5 T(x^2)/U(x^2)
    // 0 <= x <= 0.6743316650390625 rad (~38.5)
    // 等价于23阶泰勒展开，理论误差约O(x^25)
    static const __Float64
        H1 = 0.0174532925199432957692369076848861271344L,
        H2 = 1.7721923114025960319773842630945013261446E-06,

        /*
            分子多项式的系数可以由以下公式计算得到（需要先计算分母多项式的系数）：

            T0 ​= a0 * ​U0
            ​T1 ​= a0 * ​U1 ​+ a1 * ​U0
            ​T2 ​= a0 * ​U2 ​+ a1 * ​U1 ​+ a2 * ​U0
            ​T3 ​= a0 * ​U3 ​+ a1 * ​U2 ​+ a2 * ​U1 ​+ a3 * ​U0
            ​T4 ​= a0 * ​U4 ​+ a1 * ​U3 ​+ a2 * ​U2 ​+ a3 * ​U1 ​+ a4 * ​U0​​
        */
        T0 = -18170460.L,
        T1 = +403.1733397845002995793889573449407738775646L, // +1323540,
        T2 = -0.0024418154916931242720973625921194130111L, // -26315,
        T3 = +4.9936582858652166901917019121888113559763E-09L, // 530/3
        T4 = -2.8700481236962312460967797548144015600900E-15L, // -51050/153153
        // T4 = -1208/17 + 30396/7 -824600/9 +19037050/33 -19091656/39
        //   -> 134262418/231 - 1156052392/1989 = -51050/153153

        /*
            分母多项式的系数由以下线性系统求解得到，a_n为tan(x)的泰勒展开系数：

            U_n =>
            [​a5, ​a6, ​a7, ​a8, ​a9,   [U0,   [-a0,
            ​​ a4, ​a5, ​a6, ​a7, ​a8,    U1,    -a1,
            ​​ a3, ​a4, ​a5, ​a6, ​a7, *  U2, =  -a2,
            ​ ​a2, ​a3, ​a4, ​a5, ​a6,    U3,    -a3,
             ​​a1, ​a2, ​a3, ​a4, ​a5​​]    U4]    -a4]

            a0 ​= 2/15​, a1 ​= 17/315​, a2 ​= 62/2835​, a3 ​= 1382/155925​,
            a4 ​= 21844/6081075​, a5 ​= 929569/638512875​, a6 ​= 6404582/1854718875​,
            a7 ​= 443861162/1856156927625​, a8 ​= 18888466084/194896477400625​,
            a9 ​= 113927491862/2900518163668125
        */
        // 以下系数是把五次项倍率叠加到分母多项式的结果，看起来非常夸张，
        // 但经实验发现对精度影响不是很大，所以无伤大雅
        U0 = -84147331368660132.7731490376686610712032315067236332318146L, // -136278450
        U1 = +12242252074115.5150472234450882856473075025679713617898L, // +65086875
        U2 = -240040668.2700583833133458695936936111076127268987L, // -4189500
        U3 = +1404.2919161546375775928015923259377892361347L, // +80460
        U4 = -0.0028334227661130917734791355453475615320L, // -9060/17
        U5 = +1.6195219477959060127115896863380180362935E-09L; // 1
        // 以下系数是没有叠加五次项倍率的系数
        // US = +1.6195219477959060127115896863380180362935E-09L,
        // U0 = -136278450L, // -136278450
        // U1 = +19826.5959244800292671065790554803723588046555L, // +65086875
        // U2 = -0.3887511306269558859187497845215383169303L, // -4189500
        // U3 = +2.2742815793248037824122132218191231794444E-06L, // +80460
        // U4 = -4.5887903571047382272779265940993901961400E-12L, // -9060/17
        // U5 = +2.6228513393926453203048273376369937038919E-18L; // 1

    // 由于系数大小非常夸张，这里不能用秦九韶写法，要不然会出现“大吃小”导致额外的误差。
    // 丹霞：用了些额外的计算换取接近64位浮点极限的精度，所以不亏。
    __Float64
        x2 = x * x,
        x3 = x2 * x,
        x4 = x2 * x2,
        x5 = x3 * x2,
        x6 = x3 * x3,
        x8 = x6 * x2,
        x10 = x8 * x2;
    //T = T0.x + x2 * (T1.x + x2 * (T2.x + x2 * (T3.x + x2 * T4.x)));
    T = T4.x * x8 + T3.x * x6 + T2.x * x4 + T1.x * x2 + T0.x;
    //U = U0.x + x2 * (U1.x + x2 * (U2.x + x2 * (U3.x + x2 * (U4.x + x2))));
    U = U5.x * x10 + U4.x * x8 + U3.x * x6 + U2.x * x4 + U1.x * x2 + U0.x;
    R = T / U;

    __Float64 r = x2 * (x3 * R); // * US.x;
    r += H2.x * x3;
    __Float64 w = H1.x * x + r;

    if (_X >= TANCTG_BOUNDARY)
    {
        w = 1.0 - 2.0 * (H1.x * x - ((w * w) / (w + 1.0) - r));
        return (sign < 0) ? -w.x : w.x;
    }

    if (_Inv) // Inv置1时返回ctg(x)
    {
        // 如果允许的误差可以到2 ulp，直接返回1.0/(x+r)就行了
        // 这里用浮点截断修正以提高计算精度至64位极限。
        __Float64 x1 = H1.x * x;
        __Float64 u1(w);
        u1.parts.lsw = 0;
        __Float64 v = r - (u1 - x1);	// u1+v = r+x
        __Float64 z = 1.0 / w;
        __Float64 z1(z);
        z1.parts.lsw = 0;
        __Float64 s = 1.0 - z1 * u1;
        return z1 + z * (s - z1 * v);
    }

    return w; // 返回tan(x)
}

__Float64 __cdecl __IEEE754_TAN128F_C64F(__Float64 _X)
{
    __Float64 absx = abs(_X);
    int neg = std::signbit(_X) ? -1 : 1;

    // |x| ~< 45
    if (absx.x <= 45)
    {
        return __IEEE754_PADE_TANCTG(_X, 0);
    }

    /* tan(Inf or NaN) = NaN */
    else if (isinf(_X) || isnan(_X))
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE);
    }

    /* 将范围缩减到0-180 */
    __Float64 tx = mod(absx, 180);
    // tan(x)
    if (tx <= 45)
    {
        return neg * __IEEE754_PADE_TANCTG(tx, 0);
    }
    // tan(x) = ctg(90 - x)
    else if (tx > 45 && tx < 90)
    {
        return neg * __IEEE754_PADE_TANCTG(90 - tx, 1);
    }
    // tan(90) = NaN
    else if (tx == 90)
    {
        return neg * __Float64::FromBytes(BIG_NAN_DOUBLE);
    }
    // tan(x) = -ctg(x - 90)
    else if (tx > 90 && tx < 135)
    {
        return neg * -__IEEE754_PADE_TANCTG(tx - 90, 1);
    }
    // tan(x) = -tan(180 - x)
    else
    {
        return neg * -__IEEE754_PADE_TANCTG(180 - tx, 0);
    }
}

__Float64 __cdecl __IEEE754_CTG128F_C64F(__Float64 _X)
{
    __Float64 absx = abs(_X);
    int neg = std::signbit(_X) ? -1 : 1;

    // ctg(0) = NaN
    if (_X == 0)
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE);
    }

    // |x| ~< 45
    if (absx.x <= 45)
    {
        return __IEEE754_PADE_TANCTG(_X, 1);
    }

    /* ctg(Inf or NaN) = NaN */
    else if (isinf(_X) || isnan(_X))
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE);
    }

    /* 将范围缩减到0-180 */
    __Float64 tx = mod(absx, 180);
    // ctg(0) = NaN
    if (tx == 0)
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE);
    }
    // ctg(x)
    else if (tx > 0 && tx <= 45)
    {
        return neg * __IEEE754_PADE_TANCTG(tx, 1);
    }
    // ctg(x) = tan(90 - x)
    else if (tx > 45 && tx <= 90)
    {
        return neg * __IEEE754_PADE_TANCTG(90 - tx, 0);
    }
    // ctg(x) = -tan(x - 90)
    else if (tx > 90 && tx <= 135)
    {
        return neg * -__IEEE754_PADE_TANCTG(tx - 90, 0);
    }
    // ctg(x) = -ctg(180 - x)
    else if (tx > 135 && tx < 180)
    {
        return neg * -__IEEE754_PADE_TANCTG(180 - tx, 1);
    }
    // ctg(180) = NaN
    else
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE);
    }
}

_CSE_END
