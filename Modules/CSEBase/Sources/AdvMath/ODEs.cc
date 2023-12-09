/***********************************************************************
  Implementions of Solving Ordinary Differential Equations
  Some functions taken from SciPy and converted to C++ format.
************************************************************************/

// SciPy is available under the license below:
/*
    Copyright (c) 2001-2002 Enthought, Inc. 2003-2023, SciPy Developers.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials provided
       with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CSE/CSEBase/AdvMath.h"

_CSE_BEGIN

// RK23 Tables

const uint64 __RK23_C_Table[3] =
{
    0x0000000000000000, // 0
    0x3FE0000000000000, // 1/2
    0x3FE8000000000000  // 3/4
};

const uint64 __RK23_A_Table[9] =
{
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x3FE0000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x3FE8000000000000, 0x0000000000000000
};

const uint64 __RK23_B_Table[3] =
{
    0x3FCC71C71C71C71C, // 2/9
    0x3FD5555555555555, // 1/3
    0x3FDC71C71C71C71C  // 4/9
};

const uint64 __RK23_E_Table[4] =
{
    0x3FB1C71C71C71C72, // 5/72
    0xBFB5555555555555, // -1/12
    0xBFBC71C71C71C71C, // -1/9
    0x3FC0000000000000  // 1/8
};

const uint64 __RK23_P_Table[12] =
{
    0x3FF0000000000000, 0xBFF5555555555555, 0x3FE1C71C71C71C72,
    0x0000000000000000, 0x3FF0000000000000, 0xBFE5555555555555,
    0x0000000000000000, 0x3FF5555555555555, 0xBFEC71C71C71C71C,
    0x0000000000000000, 0xBFF0000000000000, 0x3FF0000000000000
};

// RK45 Tables

const float64 __RK45_C_Table[6] =
{
    +0x0.0000000000000p+0, // 0
    +0x1.999999999999Ap-3, // 1/5
    +0x1.3333333333333p-2, // 3/10
    +0x1.999999999999Ap-1, // 4/5
    +0x1.C71C71C71C71Cp-1, // 8/9
    +0x1.0000000000000p+0  // 1
};

const float64 __RK45_A_Table[30] =
{
    0, 0, 0, 0, 0,
    +0x1.999999999999Ap-3, 0, 0, 0, 0,
    +0x1.3333333333333p-4, +0x1.CCCCCCCCCCCCDp-3, 0, 0, 0,
    +0x1.F49F49F49F49Fp-1, -0x1.DDDDDDDDDDDDEp+1, +0x1.C71C71C71C71Cp+1, 0, 0,
    +0x1.79EEC0FC37181p+1, -0x1.7310BD29520E4p+3, +0x1.3A552363C5290p+3, -0x1.29C9EBA1E3345p-2, 0,
    +0x1.6C52BF5A814B0p+1, -0x1.583E0F83E0F84p+3, +0x1.1D016A3721E8Bp+3, +0x1.1D1745D1745D1p-2, -0x1.1818970D9CC2Fp-2
};

const float64 __RK45_B_Table[6] =
{
    +0x1.7555555555555p-4, // 35/384
    +0x0.0000000000000p+0, // 0
    +0x1.CC0499A5605FBp-2, // 500/1113
    +0x1.4D55555555555p-1, // 125/192
    -0x1.4A1CFB2B78C13p-2, // -2187/6784
    +0x1.0C30C30C30C31p-3  // 11/84
};

const float64 __RK45_E_Table[7] =
{
    -0x1.4320FEDCBA987p-10, // -71/57600
    +0x0.0000000000000p+00, // 0
    +0x1.16B5A550ABFC4p-08, // 71/16695
    -0x1.2EEEEEEEEEEEFp-05, // -71/1920
    +0x1.A0AD1D2CA1858p-05, // 17253/339200
    -0x1.5748A7BDAF0E2p-05, // -22/525
    +0x1.999999999999Ap-06  // 1/40
};

const float64 __RK45_P_Table[28] =
{
    1, -0x1.6D421C90AD023p+1, +0x1.892EE3CC04AF1p+1, -0x1.208439215A046p+0,
    0, +0x0.0000000000000p+0, +0x0.0000000000000p+0, +0x0.0000000000000p+0,
    0, +0x1.017B046CCF5E3p+2, -0x1.8FF4E27046A46p+2, +0x1.56744F3B9A987p+1,
    0, -0x1.DDBF58F446418p+1, +0x1.423501CF78761p+3, -0x1.6BDFAC7A2320Cp+2,
    0, +0x1.4703CFD92B80Bp+1, -0x1.998B0EA409B10p+2, +0x1.C2CEAE0978C92p+1,
    0, -0x1.5FDA423D67152p+0, +0x1.A2E673007345Fp+1, -0x1.C46C8B61F95E5p+0,
    0, +0x1.61E97BE1B9227p+0, -0x1.E1E97BE1B9227p+1, +0x1.30F4BDF0DC914p+1
};

_CSE_END
