// Natral logarithm function ln(x) for double and quaduple-precision floating point
// Algorithm is taken from GLibC, Made by Stephen L. Moshier (LGPL Licence)
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

// Ordinary Code. (普普通通的代码)

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/Algorithms.h"
#include <cfloat>

_CSE_BEGIN

float64 __cdecl ln(float64 _X) { return __IEEE854_LN128F_C64F(_X); };
complex64 __cdecl lnc(complex64 _X, int64 _K) {return __GLIBCT_LN64C(_X, _K);}

_EXTERN_C

/*
 * Natural logarithm for double precision.
 *
 * Algorithm:

    x = 2^k z
    log(x) = k ln2 + log(c) + log(z/c)
    log(z/c) = poly(z/c - 1)

 * where z is in [1.6p-1; 1.6p0] which is split into N subintervals and z falls
 * into the ith one, then table entries are computed as

    tab[i].invc = 1/c
    tab[i].logc = (double)log(c)
    tab2[i].chi = (double)c
    tab2[i].clo = (double)(c - (double)c)

 * where c is near the center of the subinterval and is chosen by trying +-2^29
 * floating point invc candidates around 1/center and selecting one for which

    1) the rounding error in 0x1.8p9 + logc is 0,
    2) the rounding error in z - chi - clo is < 0x1p-66 and
    3) the rounding error in (double)log(c) is minimized (< 0x1p-66).

 * Note: 1) ensures that k*ln2hi + logc can be computed without rounding error,
         2) ensures that z/c - 1 can be computed as (z - chi - clo)*invc with close to
            a single rounding error when there is no fast fma for z*invc - 1, 3) ensures
            that logc + poly(z/c - 1) has small error, however near x == 1 when
            |log(x)| < 0x1p-4, this is not enough so that is special cased.
 *
 */

const double __Lnf64_table[512]
{
    0x1.734f0c3e0de9fp+0, -0x1.7cc7f79e69000p-2,
    0x1.713786a2ce91fp+0, -0x1.76feec20d0000p-2,
    0x1.6f26008fab5a0p+0, -0x1.713e31351e000p-2,
    0x1.6d1a61f138c7dp+0, -0x1.6b85b38287800p-2,
    0x1.6b1490bc5b4d1p+0, -0x1.65d5590807800p-2,
    0x1.69147332f0cbap+0, -0x1.602d076180000p-2,
    0x1.6719f18224223p+0, -0x1.5a8ca86909000p-2,
    0x1.6524f99a51ed9p+0, -0x1.54f4356035000p-2,
    0x1.63356aa8f24c4p+0, -0x1.4f637c36b4000p-2,
    0x1.614b36b9ddc14p+0, -0x1.49da7fda85000p-2,
    0x1.5f66452c65c4cp+0, -0x1.445923989a800p-2,
    0x1.5d867b5912c4fp+0, -0x1.3edf439b0b800p-2,
    0x1.5babccb5b90dep+0, -0x1.396ce448f7000p-2,
    0x1.59d61f2d91a78p+0, -0x1.3401e17bda000p-2,
    0x1.5805612465687p+0, -0x1.2e9e2ef468000p-2,
    0x1.56397cee76bd3p+0, -0x1.2941b3830e000p-2,
    0x1.54725e2a77f93p+0, -0x1.23ec58cda8800p-2,
    0x1.52aff42064583p+0, -0x1.1e9e129279000p-2,
    0x1.50f22dbb2bddfp+0, -0x1.1956d2b48f800p-2,
    0x1.4f38f4734ded7p+0, -0x1.141679ab9f800p-2,
    0x1.4d843cfde2840p+0, -0x1.0edd094ef9800p-2,
    0x1.4bd3ec078a3c8p+0, -0x1.09aa518db1000p-2,
    0x1.4a27fc3e0258ap+0, -0x1.047e65263b800p-2,
    0x1.4880524d48434p+0, -0x1.feb224586f000p-3,
    0x1.46dce1b192d0bp+0, -0x1.f474a7517b000p-3,
    0x1.453d9d3391854p+0, -0x1.ea4443d103000p-3,
    0x1.43a2744b4845ap+0, -0x1.e020d44e9b000p-3,
    0x1.420b54115f8fbp+0, -0x1.d60a22977f000p-3,
    0x1.40782da3ef4b1p+0, -0x1.cc00104959000p-3,
    0x1.3ee8f5d57fe8fp+0, -0x1.c202956891000p-3,
    0x1.3d5d9a00b4ce9p+0, -0x1.b81178d811000p-3,
    0x1.3bd60c010c12bp+0, -0x1.ae2c9ccd3d000p-3,
    0x1.3a5242b75dab8p+0, -0x1.a45402e129000p-3,
    0x1.38d22cd9fd002p+0, -0x1.9a877681df000p-3,
    0x1.3755bc5847a1cp+0, -0x1.90c6d69483000p-3,
    0x1.35dce49ad36e2p+0, -0x1.87120a645c000p-3,
    0x1.34679984dd440p+0, -0x1.7d68fb4143000p-3,
    0x1.32f5cceffcb24p+0, -0x1.73cb83c627000p-3,
    0x1.3187775a10d49p+0, -0x1.6a39a9b376000p-3,
    0x1.301c8373e3990p+0, -0x1.60b3154b7a000p-3,
    0x1.2eb4ebb95f841p+0, -0x1.5737d76243000p-3,
    0x1.2d50a0219a9d1p+0, -0x1.4dc7b8fc23000p-3,
    0x1.2bef9a8b7fd2ap+0, -0x1.4462c51d20000p-3,
    0x1.2a91c7a0c1babp+0, -0x1.3b08abc830000p-3,
    0x1.293726014b530p+0, -0x1.31b996b490000p-3,
    0x1.27dfa5757a1f5p+0, -0x1.2875490a44000p-3,
    0x1.268b39b1d3bbfp+0, -0x1.1f3b9f879a000p-3,
    0x1.2539d838ff5bdp+0, -0x1.160c8252ca000p-3,
    0x1.23eb7aac9083bp+0, -0x1.0ce7f57f72000p-3,
    0x1.22a012ba940b6p+0, -0x1.03cdc49fea000p-3,
    0x1.2157996cc4132p+0, -0x1.f57bdbc4b8000p-4,
    0x1.201201dd2fc9bp+0, -0x1.e370896404000p-4,
    0x1.1ecf4494d480bp+0, -0x1.d17983ef94000p-4,
    0x1.1d8f5528f6569p+0, -0x1.bf9674ed8a000p-4,
    0x1.1c52311577e7cp+0, -0x1.adc79202f6000p-4,
    0x1.1b17c74cb26e9p+0, -0x1.9c0c3e7288000p-4,
    0x1.19e010c2c1ab6p+0, -0x1.8a646b372c000p-4,
    0x1.18ab07bb670bdp+0, -0x1.78d01b3ac0000p-4,
    0x1.1778a25efbcb6p+0, -0x1.674f145380000p-4,
    0x1.1648d354c31dap+0, -0x1.55e0e6d878000p-4,
    0x1.151b990275fddp+0, -0x1.4485cdea1e000p-4,
    0x1.13f0ea432d24cp+0, -0x1.333d94d6aa000p-4,
    0x1.12c8b7210f9dap+0, -0x1.22079f8c56000p-4,
    0x1.11a3028ecb531p+0, -0x1.10e4698622000p-4,
    0x1.107fbda8434afp+0, -0x1.ffa6c6ad20000p-5,
    0x1.0f5ee0f4e6bb3p+0, -0x1.dda8d4a774000p-5,
    0x1.0e4065d2a9fcep+0, -0x1.bbcece4850000p-5,
    0x1.0d244632ca521p+0, -0x1.9a1894012c000p-5,
    0x1.0c0a77ce2981ap+0, -0x1.788583302c000p-5,
    0x1.0af2f83c636d1p+0, -0x1.5715e67d68000p-5,
    0x1.09ddb98a01339p+0, -0x1.35c8a49658000p-5,
    0x1.08cabaf52e7dfp+0, -0x1.149e364154000p-5,
    0x1.07b9f2f4e28fbp+0, -0x1.e72c082eb8000p-6,
    0x1.06ab58c358f19p+0, -0x1.a55f152528000p-6,
    0x1.059eea5ecf92cp+0, -0x1.63d62cf818000p-6,
    0x1.04949cdd12c90p+0, -0x1.228fb8caa0000p-6,
    0x1.038c6c6f0ada9p+0, -0x1.c317b20f90000p-7,
    0x1.02865137932a9p+0, -0x1.419355daa0000p-7,
    0x1.0182427ea7348p+0, -0x1.81203c2ec0000p-8,
    0x1.008040614b195p+0, -0x1.0040979240000p-9,
    0x1.fe01ff726fa1ap-1, +0x1.feff384900000p-9,
    0x1.fa11cc261ea74p-1, +0x1.7dc41353d0000p-7,
    0x1.f6310b081992ep-1, +0x1.3cea3c4c28000p-6,
    0x1.f25f63ceeadcdp-1, +0x1.b9fc114890000p-6,
    0x1.ee9c8039113e7p-1, +0x1.1b0d8ce110000p-5,
    0x1.eae8078cbb1abp-1, +0x1.58a5bd001c000p-5,
    0x1.e741aa29d0c9bp-1, +0x1.95c8340d88000p-5,
    0x1.e3a91830a99b5p-1, +0x1.d276aef578000p-5,
    0x1.e01e009609a56p-1, +0x1.07598e598c000p-4,
    0x1.dca01e577bb98p-1, +0x1.253f5e30d2000p-4,
    0x1.d92f20b7c9103p-1, +0x1.42edd8b380000p-4,
    0x1.d5cac66fb5ccep-1, +0x1.606598757c000p-4,
    0x1.d272caa5ede9dp-1, +0x1.7da76356a0000p-4,
    0x1.cf26e3e6b2ccdp-1, +0x1.9ab434e1c6000p-4,
    0x1.cbe6da2a77902p-1, +0x1.b78c7bb0d6000p-4,
    0x1.c8b266d37086dp-1, +0x1.d431332e72000p-4,
    0x1.c5894bd5d5804p-1, +0x1.f0a3171de6000p-4,
    0x1.c26b533bb9f8cp-1, +0x1.067152b914000p-3,
    0x1.bf583eeece73fp-1, +0x1.147858292b000p-3,
    0x1.bc4fd75db96c1p-1, +0x1.2266ecdca3000p-3,
    0x1.b951e0c864a28p-1, +0x1.303d7a6c55000p-3,
    0x1.b65e2c5ef3e2cp-1, +0x1.3dfc33c331000p-3,
    0x1.b374867c9888bp-1, +0x1.4ba366b7a8000p-3,
    0x1.b094b211d304ap-1, +0x1.5933928d1f000p-3,
    0x1.adbe885f2ef7ep-1, +0x1.66acd2418f000p-3,
    0x1.aaf1d31603da2p-1, +0x1.740f8ec669000p-3,
    0x1.a82e63fd358a7p-1, +0x1.815c0f51af000p-3,
    0x1.a5740ef09738bp-1, +0x1.8e92954f68000p-3,
    0x1.a2c2a90ab4b27p-1, +0x1.9bb3602f84000p-3,
    0x1.a01a01393f2d1p-1, +0x1.a8bed1c2c0000p-3,
    0x1.9d79f24db3c1bp-1, +0x1.b5b515c01d000p-3,
    0x1.9ae2505c7b190p-1, +0x1.c2967ccbcc000p-3,
    0x1.9852ef297ce2fp-1, +0x1.cf635d5486000p-3,
    0x1.95cbaeea44b75p-1, +0x1.dc1bd3446c000p-3,
    0x1.934c69de74838p-1, +0x1.e8c01b8cfe000p-3,
    0x1.90d4f2f6752e6p-1, +0x1.f5509c0179000p-3,
    0x1.8e6528effd79dp-1, +0x1.00e6c121fb800p-2,
    0x1.8bfce9fcc007cp-1, +0x1.071b80e93d000p-2,
    0x1.899c0dabec30ep-1, +0x1.0d46b9e867000p-2,
    0x1.87427aa2317fbp-1, +0x1.13687334bd000p-2,
    0x1.84f00acb39a08p-1, +0x1.1980d67234800p-2,
    0x1.82a49e8653e55p-1, +0x1.1f8ffe0cc8000p-2,
    0x1.8060195f40260p-1, +0x1.2595fd7636800p-2,
    0x1.7e22563e0a329p-1, +0x1.2b9300914a800p-2,
    0x1.7beb377dcb5adp-1, +0x1.3187210436000p-2,
    0x1.79baa679725c2p-1, +0x1.377266dec1800p-2,
    0x1.77907f2170657p-1, +0x1.3d54ffbaf3000p-2,
    0x1.756cadbd6130cp-1, +0x1.432eee32fe000p-2,

    0x1.61000014fb66bp-1, +0x1.e026c91425b3cp-56,
    0x1.63000034db495p-1, +0x1.dbfea48005d41p-55,
    0x1.650000d94d478p-1, +0x1.e7fa786d6a5b7p-55,
    0x1.67000074e6fadp-1, +0x1.1fcea6b54254cp-57,
    0x1.68ffffedf0faep-1, -0x1.c7e274c590efdp-56,
    0x1.6b0000763c5bcp-1, -0x1.ac16848dcda01p-55,
    0x1.6d0001e5cc1f6p-1, +0x1.33f1c9d499311p-55,
    0x1.6efffeb05f63ep-1, -0x1.e80041ae22d53p-56,
    0x1.710000e869780p-1, +0x1.bff6671097952p-56,
    0x1.72ffffc67e912p-1, +0x1.c00e226bd8724p-55,
    0x1.74fffdf81116ap-1, -0x1.e02916ef101d2p-57,
    0x1.770000f679c90p-1, -0x1.7fc71cd549c74p-57,
    0x1.78ffffa7ec835p-1, +0x1.1bec19ef50483p-55,
    0x1.7affffe20c2e6p-1, -0x1.07e1729cc6465p-56,
    0x1.7cfffed3fc900p-1, -0x1.08072087b8b1cp-55,
    0x1.7efffe9261a76p-1, +0x1.dc0286d9df9aep-55,
    0x1.81000049ca3e8p-1, +0x1.97fd251e54c33p-55,
    0x1.8300017932c8fp-1, -0x1.afee9b630f381p-55,
    0x1.850000633739cp-1, +0x1.9bfbf6b6535bcp-55,
    0x1.87000204289c6p-1, -0x1.bbf65f3117b75p-55,
    0x1.88fffebf57904p-1, -0x1.9006ea23dcb57p-55,
    0x1.8b00022bc04dfp-1, -0x1.d00df38e04b0ap-56,
    0x1.8cfffe50c1b8ap-1, -0x1.8007146ff9f05p-55,
    0x1.8effffc918e43p-1, +0x1.3817bd07a7038p-55,
    0x1.910001efa5fc7p-1, +0x1.93e9176dfb403p-55,
    0x1.9300013467bb9p-1, +0x1.f804e4b980276p-56,
    0x1.94fffe6ee076fp-1, -0x1.f7ef0d9ff622ep-55,
    0x1.96fffde3c12d1p-1, -0x1.082aa962638bap-56,
    0x1.98ffff4458a0dp-1, -0x1.7801b9164a8efp-55,
    0x1.9afffdd982e3ep-1, -0x1.740e08a5a9337p-55,
    0x1.9cfffed49fb66p-1, +0x1.fce08c19be000p-60,
    0x1.9f00020f19c51p-1, -0x1.a3faa27885b0ap-55,
    0x1.a10001145b006p-1, +0x1.4ff489958da56p-56,
    0x1.a300007bbf6fap-1, +0x1.cbeab8a2b6d18p-55,
    0x1.a500010971d79p-1, +0x1.8fecadd787930p-55,
    0x1.a70001df52e48p-1, -0x1.f41763dd8abdbp-55,
    0x1.a90001c593352p-1, -0x1.ebf0284c27612p-55,
    0x1.ab0002a4f3e4bp-1, -0x1.9fd043cff3f5fp-57,
    0x1.acfffd7ae1ed1p-1, -0x1.23ee7129070b4p-55,
    0x1.aefffee510478p-1, +0x1.a063ee00edea3p-57,
    0x1.b0fffdb650d5bp-1, +0x1.a06c8381f0ab9p-58,
    0x1.b2ffffeaaca57p-1, -0x1.9011e74233c1dp-56,
    0x1.b4fffd995badcp-1, -0x1.9ff1068862a9fp-56,
    0x1.b7000249e659cp-1, +0x1.aff45d0864f3ep-55,
    0x1.b8ffff9871640p-1, +0x1.cfe7796c2c3f9p-56,
    0x1.bafffd204cb4fp-1, -0x1.3ff27eef22bc4p-57,
    0x1.bcfffd2415c45p-1, -0x1.cffb7ee3bea21p-57,
    0x1.beffff86309dfp-1, -0x1.14103972e0b5cp-55,
    0x1.c0fffe1b57653p-1, +0x1.bc16494b76a19p-55,
    0x1.c2ffff1fa57e3p-1, -0x1.4feef8d30c6edp-57,
    0x1.c4fffdcbfe424p-1, -0x1.43f68bcec4775p-55,
    0x1.c6fffed54b9f7p-1, +0x1.47ea3f053e0ecp-55,
    0x1.c8fffeb998fd5p-1, +0x1.383068df992f1p-56,
    0x1.cb0002125219ap-1, -0x1.8fd8e64180e04p-57,
    0x1.ccfffdd94469cp-1, +0x1.e7ebe1cc7ea72p-55,
    0x1.cefffeafdc476p-1, +0x1.ebe39ad9f88fep-55,
    0x1.d1000169af82bp-1, +0x1.57d91a8b95a71p-56,
    0x1.d30000d0ff71dp-1, +0x1.9c1906970c7dap-55,
    0x1.d4fffea790fc4p-1, -0x1.80e37c558fe0cp-58,
    0x1.d70002edc87e5p-1, -0x1.f80d64dc10f44p-56,
    0x1.d900021dc82aap-1, -0x1.47c8f94fd5c5cp-56,
    0x1.dafffd86b0283p-1, +0x1.c7f1dc521617ep-55,
    0x1.dd000296c4739p-1, +0x1.8019eb2ffb153p-55,
    0x1.defffe54490f5p-1, +0x1.e00d2c652cc89p-57,
    0x1.e0fffcdabf694p-1, -0x1.f8340202d69d2p-56,
    0x1.e2fffdb52c8ddp-1, +0x1.b00c1ca1b0864p-56,
    0x1.e4ffff24216efp-1, +0x1.2ffa8b094ab51p-56,
    0x1.e6fffe88a5e11p-1, -0x1.7f673b1efbe59p-58,
    0x1.e9000119eff0dp-1, -0x1.4808d5e0bc801p-55,
    0x1.eafffdfa51744p-1, +0x1.80006d54320b5p-56,
    0x1.ed0001a127fa1p-1, -0x1.002f860565c92p-58,
    0x1.ef00007babcc4p-1, -0x1.540445d35e611p-55,
    0x1.f0ffff57a8d02p-1, -0x1.ffb3139ef9105p-59,
    0x1.f30001ee58ac7p-1, +0x1.a81acf2731155p-55,
    0x1.f4ffff5823494p-1, +0x1.a3f41d4d7c743p-55,
    0x1.f6ffffca94c6bp-1, -0x1.202f41c987875p-57,
    0x1.f8fffe1f9c441p-1, +0x1.77dd1f477e74bp-56,
    0x1.fafffd2e0e37ep-1, -0x1.f01199a7ca331p-57,
    0x1.fd0001c77e49ep-1, +0x1.181ee4bceacb1p-56,
    0x1.feffff7e0c331p-1, -0x1.e05370170875ap-57,
    0x1.00ffff465606ep+0, -0x1.a7ead491c0adap-55,
    0x1.02ffff3867a58p+0, -0x1.77f69c3fcb2e0p-54,
    0x1.04ffffdfc0d17p+0, +0x1.7bffe34cb945bp-54,
    0x1.0700003cd4d82p+0, +0x1.20083c0e456cbp-55,
    0x1.08ffff9f2cbe8p+0, -0x1.dffdfbe37751ap-57,
    0x1.0b000010cda65p+0, -0x1.13f7faee626ebp-54,
    0x1.0d00001a4d338p+0, +0x1.07dfa79489ff7p-55,
    0x1.0effffadafdfdp+0, -0x1.7040570d66bc0p-56,
    0x1.110000bbafd96p+0, +0x1.e80d4846d0b62p-55,
    0x1.12ffffae5f45dp+0, +0x1.dbffa64fd36efp-54,
    0x1.150000dd59ad9p+0, +0x1.a0077701250aep-54,
    0x1.170000f21559ap+0, +0x1.dfdf9e2e3deeep-55,
    0x1.18ffffc275426p+0, +0x1.10030dc3b7273p-54,
    0x1.1b000123d3c59p+0, +0x1.97f7980030188p-54,
    0x1.1cffff8299eb7p+0, -0x1.5f932ab9f8c67p-57,
    0x1.1effff48ad400p+0, +0x1.37fbf9da75bebp-54,
    0x1.210000c8b86a4p+0, +0x1.f806b91fd5b22p-54,
    0x1.2300003854303p+0, +0x1.3ffc2eb9fbf33p-54,
    0x1.24fffffbcf684p+0, +0x1.601e77e2e2e72p-56,
    0x1.26ffff52921d9p+0, +0x1.ffcbb767f0c61p-56,
    0x1.2900014933a3cp+0, -0x1.202ca3c02412bp-56,
    0x1.2b00014556313p+0, -0x1.2808233f21f02p-54,
    0x1.2cfffebfe523bp+0, -0x1.8ff7e384fdcf2p-55,
    0x1.2f0000bb8ad96p+0, -0x1.5ff51503041c5p-55,
    0x1.30ffffb7ae2afp+0, -0x1.10071885e289dp-55,
    0x1.32ffffeac5f7fp+0, -0x1.1ff5d3fb7b715p-54,
    0x1.350000ca66756p+0, +0x1.57f82228b82bdp-54,
    0x1.3700011fbf721p+0, +0x1.000bac40dd5ccp-55,
    0x1.38ffff9592fb9p+0, -0x1.43f9d2db2a751p-54,
    0x1.3b00004ddd242p+0, +0x1.57f6b707638e1p-55,
    0x1.3cffff5b2c957p+0, +0x1.a023a10bf1231p-56,
    0x1.3efffeab0b418p+0, +0x1.87f6d66b152b0p-54,
    0x1.410001532aff4p+0, +0x1.7f8375f198524p-57,
    0x1.4300017478b29p+0, +0x1.301e672dc5143p-55,
    0x1.44fffe795b463p+0, +0x1.9ff69b8b2895ap-55,
    0x1.46fffe80475e0p+0, -0x1.5c0b19bc2f254p-54,
    0x1.48fffef6fc1e7p+0, +0x1.b4009f23a2a72p-54,
    0x1.4afffe5bea704p+0, -0x1.4ffb7bf0d7d45p-54,
    0x1.4d000171027dep+0, -0x1.9c06471dc6a3dp-54,
    0x1.4f0000ff03ee2p+0, +0x1.77f890b85531cp-54,
    0x1.5100012dc4bd1p+0, +0x1.004657166a436p-57,
    0x1.530001605277ap+0, -0x1.6bfcece233209p-54,
    0x1.54fffecdb704cp+0, -0x1.902720505a1d7p-55,
    0x1.56fffef5f54a9p+0, +0x1.bbfe60ec96412p-54,
    0x1.5900017e61012p+0, +0x1.87ec581afef90p-55,
    0x1.5b00003c93e92p+0, -0x1.f41080abf0cc0p-54,
    0x1.5d0001d4919bcp+0, -0x1.8812afb254729p-54,
    0x1.5efffe7b87a89p+0, -0x1.47eb780ed6904p-54,
};

__Float64 __cdecl __IEEE754_LN64F(__Float64 _X)
{
    __Float64 w, z, r, invc, logc, kd, hi, lo;
    uint64 ix, iz, tmp;
    uint32_t top;
    int k, i;

    __Float64 x = _X;
    ix = x.Bytes;
    top = ix >> 48; // extract top 16 bytes

    uint64 LO = 0x3FEE000000000000; // 1.0 - 0x1p-4 = 0.9375
    uint64 HI = 0x3FF1090000000000; // 1.0 + 0x1.09p-4 = 1.064697265625

    __Float64 NMC[] =
        {
            // Coefficients of taylor series of ln(x)
            // https://en.wikipedia.org/wiki/Mercator_series
            -0.50000000000000000000000000000000,
            +0.33333333333333333333333333333333,
            -0.25000000000000000000000000000000,
            +0.20000000000000000000000000000000,
            -0.16666666666666666666666666666667,
            +0.14285714285714285714285714285714,
            -0.12500000000000000000000000000000,
            +0.11111111111111111111111111111111,
            -0.10000000000000000000000000000000,
            +0.09090909090909090909090909090909,
            -0.08333333333333333333333333333333,
        };

    if (ix - LO < HI - LO)
    {
        if (x == 1.) { return 0; } // ln(1) = 0
        // relative error: 0x1.c04d76cp-63
        // in -0x1p-4 0x1.09p-4 (|log(1+x)| > 0x1p-4 outside the interval)
        r = x - 1.;
        // linear and square term need to be specially processed.
        w = r * 0x1p27;
        __Float64 rhi = r + w - w;
        __Float64 rlo = r - rhi;
        w = rhi * rhi * NMC[0];
        hi = r + w;
        lo = r - hi + w;
        lo += NMC[0] * rlo * (rhi + r);
        return hi + lo + r * r * r * (NMC[1] + r * (NMC[2] + r * (NMC[3] + r * (NMC[4] + r * (NMC[5] + r * (NMC[6] + r * (NMC[7] + r * (NMC[8] + r * (NMC[9] + r * (NMC[10]))))))))));
    }

    if (top - 0x0010 >= 0x7FF0 - 0x0010)
    {
        // x < 0x1p-1022 or inf or nan.
        if (ix * 2 == 0) { return __Float64::FromBytes(NEG_INF_DOUBLE); }
        // ln(inf) == inf
        if (ix == POS_INF_DOUBLE) { return __Float64::FromBytes(ix); }
        if ((top & 0x8000) || (top & 0x7FF0) == 0x7FF0) { return __Float64::FromBytes(BIG_NAN_DOUBLE); }
        // x is subnormal, normalize it.
        ix = __Float64(x * 0x1p52).Bytes;
        ix -= 52ULL << 52;
    }

    // x = 2^k z; where z is in range [OFF,2*OFF) and exact.
    // The range is split into N subintervals.
    // The ith subinterval contains z and c is near its center.

    uint64 OFFSET = 0x3FE6000000000000;
    tmp = ix - OFFSET;
    i = (tmp >> 45) % (1ull << 7);
    k = (int64)tmp >> 52; // arithmetic shift
    iz = ix - (tmp & 0xFFFULL << 52);
    invc = __Lnf64_table[2 * i];
    logc = __Lnf64_table[2 * i + 1];
    z = __Float64::FromBytes(iz);

    // log(x) = log1p(z/c-1) + log(c) + k*Ln2.
    // r ~= z/c - 1, |r| < 1/(2*N).

    // rounding error: 0x1p-55/N + 0x1p-66.
    r = (z - __Lnf64_table[256 + 2 * i] - __Lnf64_table[256 + 2 * i + 1]) * invc;
    kd = __Float64(k);

    __Float64 ln2hi = 0x1.62E42FEFA3800p-1;  // 0x3FE62E42FEFA3800, ln(2) without lower byte.
    __Float64 ln2lo = 0x1.EF35793C76730p-45; // ln(2) - ln2hi
    w = kd * ln2hi + logc;
    hi = w + r;
    lo = w - hi + r + kd * ln2lo;

    // ln(x) = lo + (log1p(r) - r) + hi.
    // Worst case error if | y | > 0x1p-4: 0.519 ULP(0.520 ULP without fma).
    // 0.5 + 2.06 / N + abs - poly - error * 2 ^ 56 ULP(+0.001 ULP without fma).
    return hi + lo + r * r * (NMC[0] + r * (NMC[1] + r * (NMC[2] + r * (NMC[3] + r * (NMC[4])))));
}

/*							logll.c
 *
 * Natural logarithm for 128-bit long double precision.
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, logl();
 *
 * y = logl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of x.
 *
 * The argument is separated into its exponent and fractional
 * parts.  Use of a lookup table increases the speed of the routine.
 * The program uses logarithms tabulated at intervals of 1/128 to
 * cover the domain from approximately 0.7 to 1.4.
 *
 * On the interval [-1/128, +1/128] the logarithm of 1+x is approximated by
 *     log(1+x) = x - 0.5 x^2 + x^3 P(x) .
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE   0.875, 1.125   100000      1.2e-34    4.1e-35
 *    IEEE   0.125, 8       100000      1.2e-34    4.1e-35
 *
 *
 * WARNING:
 *
 * This program uses integer operations on bit fields of floating-point
 * numbers.  It does not work with data structures other than the
 * structure assumed.
 *
 */

/* Copyright 2001 by Stephen L. Moshier <moshier@na-net.ornl.gov>

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

// Lookup table of ln(t) - (t-1)
// t = 0.5 + (k+26)/128)
// k = 0, ..., 91
const double __Ln128f_table[92]
{
    -5.5345593589352099112142921677820359632418E-2L,
    -5.2108257402767124761784665198737642086148E-2L,
    -4.8991686870576856279407775480686721935120E-2L,
    -4.5993270766361228596215288742353061431071E-2L,
    -4.3110481649613269682442058976885699556950E-2L,
    -4.0340872319076331310838085093194799765520E-2L,
    -3.7682072451780927439219005993827431503510E-2L,
    -3.5131785416234343803903228503274262719586E-2L,
    -3.2687785249045246292687241862699949178831E-2L,
    -3.0347913785027239068190798397055267411813E-2L,
    -2.8110077931525797884641940838507561326298E-2L,
    -2.5972247078357715036426583294246819637618E-2L,
    -2.3932450635346084858612873953407168217307E-2L,
    -2.1988775689981395152022535153795155900240E-2L,
    -2.0139364778244501615441044267387667496733E-2L,
    -1.8382413762093794819267536615342902718324E-2L,
    -1.6716169807550022358923589720001638093023E-2L,
    -1.5138929457710992616226033183958974965355E-2L,
    -1.3649036795397472900424896523305726435029E-2L,
    -1.2244881690473465543308397998034325468152E-2L,
    -1.0924898127200937840689817557742469105693E-2L,
    -9.6875626072830301572839422532631079809328E-3L,
    -8.5313926245226231463436209313499745894157E-3L,
    -7.4549452072765973384933565912143044991706E-3L,
    -6.4568155251217050991200599386801665681310E-3L,
    -5.5356355563671005131126851708522185605193E-3L,
    -4.6900728132525199028885749289712348829878E-3L,
    -3.9188291218610470766469347968659624282519E-3L,
    -3.2206394539524058873423550293617843896540E-3L,
    -2.5942708080877805657374888909297113032132E-3L,
    -2.0385211375711716729239156839929281289086E-3L,
    -1.5522183228760777967376942769773768850872E-3L,
    -1.1342191863606077520036253234446621373191E-3L,
    -7.8340854719967065861624024730268350459991E-4L,
    -4.9869831458030115699628274852562992756174E-4L,
    -2.7902661731604211834685052867305795169688E-4L,
    -1.2335696813916860754951146082826952093496E-4L,
    -3.0677461025892873184042490943581654591817E-5L,
    +0.0000000000000000000000000000000000000000E+0L,
    -3.0359557945051052537099938863236321874198E-5L,
    -1.2081346403474584914595395755316412213151E-4L,
    -2.7044071846562177120083903771008342059094E-4L,
    -4.7834133324631162897179240322783590830326E-4L,
    -7.4363569786340080624467487620270965403695E-4L,
    -1.0654639687057968333207323853366578860679E-3L,
    -1.4429854811877171341298062134712230604279E-3L,
    -1.8753781835651574193938679595797367137975E-3L,
    -2.3618380914922506054347222273705859653658E-3L,
    -2.9015787624124743013946600163375853631299E-3L,
    -3.4938307889254087318399313316921940859043E-3L,
    -4.1378413103128673800485306215154712148146E-3L,
    -4.8328735414488877044289435125365629849599E-3L,
    -5.5782063183564351739381962360253116934243E-3L,
    -6.3731336597098858051938306767880719015261E-3L,
    -7.2169643436165454612058905294782949315193E-3L,
    -8.1090214990427641365934846191367315083867E-3L,
    -9.0486422112807274112838713105168375482480E-3L,
    -1.0035177140880864314674126398350812606841E-2L,
    -1.1067990155502102718064936259435676477423E-2L,
    -1.2146457974158024928196575103115488672416E-2L,
    -1.3269969823361415906628825374158424754308E-2L,
    -1.4437927104692837124388550722759686270765E-2L,
    -1.5649743073340777659901053944852735064621E-2L,
    -1.6904842527181702880599758489058031645317E-2L,
    -1.8202661505988007336096407340750378994209E-2L,
    -1.9542647000370545390701192438691126552961E-2L,
    -2.0924256670080119637427928803038530924742E-2L,
    -2.2346958571309108496179613803760727786257E-2L,
    -2.3810230892650362330447187267648486279460E-2L,
    -2.5313561699385640380910474255652501521033E-2L,
    -2.6856448685790244233704909690165496625399E-2L,
    -2.8438398935154170008519274953860128449036E-2L,
    -3.0058928687233090922411781058956589863039E-2L,
    -3.1717563112854831855692484086486099896614E-2L,
    -3.3413836095418743219397234253475252001090E-2L,
    -3.5147290019036555862676702093393332533702E-2L,
    -3.6917475563073933027920505457688955423688E-2L,
    -3.8723951502862058660874073462456610731178E-2L,
    -4.0566284516358241168330505467000838017425E-2L,
    -4.2444048996543693813649967076598766917965E-2L,
    -4.4356826869355401653098777649745233339196E-2L,
    -4.6304207416957323121106944474331029996141E-2L,
    -4.8285787106164123613318093945035804818364E-2L,
    -5.0301169421838218987124461766244507342648E-2L,
    -5.2349964705088137924875459464622098310997E-2L,
    -5.4431789996103111613753440311680967840214E-2L,
    -5.6546268881465384189752786409400404404794E-2L,
    -5.8693031345788023909329239565012647817664E-2L,
    -6.0871713627532018185577188079210189048340E-2L,
    -6.3081958078862169742820420185833800925568E-2L,
    -6.5323413029406789694910800219643791556918E-2L,
    -6.7595732653791419081537811574227049288168E-2L
};

__Float64 __cdecl __IEEE854_LN128F_C64F(__Float64 _X)
{
    __Float64 x = _X;
    // ln(2) = ln2a + ln2b with extended precision.
    static const __Float64
        ln2a = 6.93145751953125e-1L,
        ln2b = 1.4286068203094172321214581765680755001344E-6L;

    static const __Float64
        ldbl_epsilon = 0x1p-106L;

    __Float64 z, y, w, t;
    unsigned int m;
    int k, e;
    __Float64 xhi;
    uint32_t hx, lx;

    // split into 2 pieces
    xhi = x;
    hx = x.parts.msw;
    lx = x.parts.lsw;
    m = hx;

    // Check for IEEE special cases.
    k = m & 0x7fffffff;
    // ln(0) = -infinity.
    if ((k | lx) == 0) {return __Float64::FromBytes(NEG_INF_DOUBLE);}
    // ln ( x < 0 ) = NaN
    if (m & 0x80000000) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
    // ln (infinity or NaN)
    if (k >= 0x7ff00000) {return x + x;}

    // On this interval the table is not used due to cancellation error.
    if ((x <= 1.0078125L) && (x >= 0.9921875L))
    {
        if (x == 1.0L) {return 0.0L;}
        z = x - 1.0L;
        k = 64;
        t = 1.0L;
        e = 0;
    }
    else
    {
        // Extract exponent and reduce domain to 0.703125 <= u < 1.40625
        unsigned int w0;
        e = (int)(m >> 20) - (int)0x3fe;
        if (e == -1022)
        {
            x *= 0x1p106L;
            xhi = x;
            hx = x.parts.msw;
            lx = x.parts.lsw;
            m = hx;
            e = (int)(m >> 20) - (int)0x3fe - 106;
        }
        m &= 0xfffff;
        w0 = m | 0x3fe00000;
        m |= 0x100000;
        // Find lookup table index k from high order bits of the significand.
        if (m < 0x168000)
        {
            k = (m - 0xff000) >> 13;
            // t is the argument 0.5 + (k+26)/128 of the nearest item to u in the lookup table.
            xhi.parts.msw = 0x3ff00000 + (k << 13);
            xhi.parts.lsw = 0;
            t = xhi;
            w0 += 0x100000;
            e -= 1;
            k += 64;
        }
        else
        {
            k = (m - 0xfe000) >> 14;
            xhi.parts.msw =  0x3fe00000 + (k << 14);
            xhi.parts.lsw = 0;
            t = xhi;
        }
        x = ::scalbn(x, ((int)((w0 - hx) * 2)) >> 21);
        // ln(u) = ln(t * u/t) = ln(t) + ln(u/t)
        // ln(t) is tabulated in the lookup table.
        // Express ln(u/t) = ln(1+z),  where z = u/t - 1 = (u-t)/t.
        // cf. Cody & Waite.
        z = (x - t) / t;
    }

    /* Series expansion of log(1+z).  */
    w = z * z;
    /* Avoid spurious underflows.  */
    if (abs(z) <= ldbl_epsilon.x) {y = 0.0L;}
    else
    {
        // ln(1+x) = x - .5 x^2 + x^3 l(x) -.0078125 <= x <= +.0078125 peak relative error 1.2e-37
        static const __Float64
            l3 =   3.333333333333333333333333333333336096926E-1L,
            l4 =  -2.499999999999999999999999999486853077002E-1L,
            l5 =   1.999999999999999999999999998515277861905E-1L,
            l6 =  -1.666666666666666666666798448356171665678E-1L,
            l7 =   1.428571428571428571428808945895490721564E-1L,
            l8 =  -1.249999999999999987884655626377588149000E-1L,
            l9 =   1.111111111111111093947834982832456459186E-1L,
            l10 = -1.000000000000532974938900317952530453248E-1L,
            l11 =  9.090909090915566247008015301349979892689E-2L,
            l12 = -8.333333211818065121250921925397567745734E-2L,
            l13 =  7.692307559897661630807048686258659316091E-2L,
            l14 = -7.144242754190814657241902218399056829264E-2L,
            l15 =  6.668057591071739754844678883223432347481E-2L;

        y = ((((((((((((l15.x * z
                        + l14.x) * z
                       + l13.x) * z
                      + l12.x) * z
                     + l11.x) * z
                    + l10.x) * z
                   + l9.x) * z
                  + l8.x) * z
                 + l7.x) * z
                + l6.x) * z
               + l5.x) * z
              + l4.x) * z
             + l3.x) * z * w;
        y -= 0.5 * w;
    }

    y += e * ln2b.x;  /* Base 2 exponent offset times ln(2).  */
    y += z;
    y += __Ln128f_table[k - 26]; /* log(t) - (t-1) */
    y += (t - 1.0L);
    y += e * ln2a.x;
    return y;
}

/*							log1pl.c
 *
 *      Relative error logarithm
 *	Natural logarithm of 1+x, 128-bit long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, log1pl();
 *
 * y = log1pl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of 1+x.
 *
 * The argument 1+x is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x^2 + x^3 P(x)/Q(x).
 *
 * Otherwise, setting  z = 2(w-1)/(w+1),
 *
 *     log(w) = z + z^3 P(z)/Q(z).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -1, 8       100000      1.9e-34     4.3e-35
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

__Float64 __cdecl __IEEE854_LN1PX128F_C64F(__Float64 _X)
{
    // C1 + C2 = ln 2
    static const __Float64 C1 = 6.93145751953125E-1L;
    static const __Float64 C2 = 1.428606820309417232121458176568075500134E-6L;

    static const __Float64 sqrth = 0.7071067811865475244008443621048490392848L;
    // ln (2^16384 * (1 - 2^-113))
    static const __Float64 zero = 0.0L;

    __Float64 xm1 = _X;
    __Float64 x, y, z, r, s;
    __Float64 xhi;
    int32_t hx, lx;
    int e;

    // Test for NaN or infinity input.
    xhi = xm1;
    hx = xhi.parts.msw;
    lx = xhi.parts.lsw;
    if ((hx & 0x7fffffff) >= 0x7ff00000) {return xm1 + xm1 * xm1;}

    // ln(1 + (+-0)) = +-0.
    if (((hx & 0x7fffffff) | lx) == 0) {return xm1;}

    if (xm1 >= 0x1p107L) {x = xm1;}
    else {x = xm1 + 1.0L;}

    /* ln(1 + (-1)) = ln(0) = -inf */
    if (x <= 0.0L)
    {
        if (x == 0.0L) {return __Float64::FromBytes(NEG_INF_DOUBLE);}
        else {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
    }

    // Separate mantissa from exponent.
    // Use frexp used so that denormal numbers will be handled properly.
    x = ::frexp(x, &e);

    // Logarithm using ln(x) = z + z^3 * P(z^2) / Q(z^2),
    // where z = 2 * (x - 1) / (x + 1).
    if ((e > 2) || (e < -2))
    {
        if (x.x < sqrth.x)
        {			/* 2( 2x-1 )/( 2x+1 ) */
            e -= 1;
            z = x - 0.5L;
            y = 0.5L * z + 0.5L;
        }
        else
        {			/*  2 (x-1)/(x+1)   */
            z = x - 0.5L;
            z -= 0.5L;
            y = 0.5L * x + 0.5L;
        }

        /* Coefficients for log(x) = z + z^3 P(z^2)/Q(z^2),
         * where z = 2(x-1)/(x+1)
         * 1/sqrt(2) <= x < sqrt(2)
         * Theoretical peak relative error = 1.1e-35,
         * relative peak error spread 1.1e-9
         */
        static const __Float64
            R5 = -8.828896441624934385266096344596648080902E-1L,
            R4 = 8.057002716646055371965756206836056074715E1L,
            R3 = -2.024301798136027039250415126250455056397E3L,
            R2 = 2.048819892795278657810231591630928516206E4L,
            R1 = -8.977257995689735303686582344659576526998E4L,
            R0 = 1.418134209872192732479751274970992665513E5L,
            /* S6 = 1.000000000000000000000000000000000000000E0L, */
            S5 = -1.186359407982897997337150403816839480438E2L,
            S4 = 3.998526750980007367835804959888064681098E3L,
            S3 = -5.748542087379434595104154610899551484314E4L,
            S2 = 4.001557694070773974936904547424676279307E5L,
            S1 = -1.332535117259762928288745111081235577029E6L,
            S0 = 1.701761051846631278975701529965589676574E6L;

        x = z / y;
        z = x * x;
        r = ((((R5.x * z
                + R4.x) * z
               + R3.x) * z
              + R2.x) * z
             + R1.x) * z
            + R0.x;
        s = (((((z
                 + S5.x) * z
                + S4.x) * z
               + S3.x) * z
              + S2.x) * z
             + S1.x) * z
            + S0.x;
        z = x * (z * r / s);
        z = z + e * C2.x;
        z = z + x;
        z = z + e * C1.x;
        return z;
    }

    // Logarithm using log(1+x) = x - .5x^2 + x^3 P(x)/Q(x).
    if (x.x < sqrth.x)
    {
        e -= 1;
        if (e != 0) {x = 2.0L * x - 1.0L;}	/*  2x - 1  */
        else {x = xm1;}
    }
    else
    {
        if (e != 0) {x = x - 1.0L;}
        else {x = xm1;}
    }

    /* Coefficients for log(1+x) = x - x^2 / 2 + x^3 P(x)/Q(x)
     * 1/sqrt(2) <= 1+x < sqrt(2)
     * Theoretical peak relative error = 5.3e-37,
     * relative peak error spread = 2.3e-14
     */
    static const __Float64
        P12 = 1.538612243596254322971797716843006400388E-6L,
        P11 = 4.998469661968096229986658302195402690910E-1L,
        P10 = 2.321125933898420063925789532045674660756E1L,
        P9 = 4.114517881637811823002128927449878962058E2L,
        P8 = 3.824952356185897735160588078446136783779E3L,
        P7 = 2.128857716871515081352991964243375186031E4L,
        P6 = 7.594356839258970405033155585486712125861E4L,
        P5 = 1.797628303815655343403735250238293741397E5L,
        P4 = 2.854829159639697837788887080758954924001E5L,
        P3 = 3.007007295140399532324943111654767187848E5L,
        P2 = 2.014652742082537582487669938141683759923E5L,
        P1 = 7.771154681358524243729929227226708890930E4L,
        P0 = 1.313572404063446165910279910527789794488E4L,
        /* Q12 = 1.000000000000000000000000000000000000000E0L, */
        Q11 = 4.839208193348159620282142911143429644326E1L,
        Q10 = 9.104928120962988414618126155557301584078E2L,
        Q9 = 9.147150349299596453976674231612674085381E3L,
        Q8 = 5.605842085972455027590989944010492125825E4L,
        Q7 = 2.248234257620569139969141618556349415120E5L,
        Q6 = 6.132189329546557743179177159925690841200E5L,
        Q5 = 1.158019977462989115839826904108208787040E6L,
        Q4 = 1.514882452993549494932585972882995548426E6L,
        Q3 = 1.347518538384329112529391120390701166528E6L,
        Q2 = 7.777690340007566932935753241556479363645E5L,
        Q1 = 2.626900195321832660448791748036714883242E5L,
        Q0 = 3.940717212190338497730839731583397586124E4L;

    z = x * x;
    r = (((((((((((P12.x * x
                   + P11.x) * x
                  + P10.x) * x
                 + P9.x) * x
                + P8.x) * x
               + P7.x) * x
              + P6.x) * x
             + P5.x) * x
            + P4.x) * x
           + P3.x) * x
          + P2.x) * x
         + P1.x) * x
        + P0.x;
    s = (((((((((((x
                   + Q11.x) * x
                  + Q10.x) * x
                 + Q9.x) * x
                + Q8.x) * x
               + Q7.x) * x
              + Q6.x) * x
             + Q5.x) * x
            + Q4.x) * x
           + Q3.x) * x
          + Q2.x) * x
         + Q1.x) * x
        + Q0.x;
    y = x * (z * r / s);
    y = y + e * C2.x;
    z = y - 0.5L * z;
    z = z + x;
    z = z + e * C1.x;
    return z;
}

complex64 __cdecl __GLIBCT_LN64C(complex64 _X, int64 _K)
{
    float64 XReal = _X.real();
    float64 XImag = _X.imag();
    int RClass = std::fpclassify(XReal);
    int IClass = std::fpclassify(XImag);
    float64 YReal, YImag;

    if (RClass == FP_ZERO && IClass == FP_ZERO)
    {
        // Real and imaginary part are 0.0.
        YImag = std::signbit(XReal) ? CSE_PI : 0;
        YImag = ::copysign(YImag, XImag);
        // Yes, the following line raises an exception.
        YReal = __Float64::FromBytes(NEG_INF_DOUBLE);
    }
    else if (RClass != FP_NAN && IClass != FP_NAN)
    {
        // Neither real nor imaginary part is NaN.
        float64 AbsReal = abs(XReal);
        float64 AbsImag = abs(XImag);
        int Scale = 0;

        if (AbsReal < AbsImag)
        {
            std::swap(AbsReal, AbsImag);
        }

        if (AbsReal > DBL_MAX / 2.)
        {
            Scale = -1;
            AbsReal = ::scalbn(AbsReal, Scale);
            AbsImag = (AbsImag >= DBL_MIN * 2 ? ::scalbn(AbsImag, Scale) : 0);
        }
        else if (AbsReal < DBL_MIN && AbsImag < DBL_MIN)
        {
            Scale = DBL_MANT_DIG;
            AbsReal = ::scalbn(AbsReal, Scale);
            AbsImag = ::scalbn(AbsImag, Scale);
        }

        if (AbsReal == 1 && Scale == 0)
        {
            YReal = __IEEE854_LN1PX128F_C64F(AbsImag * AbsImag) / 2.;
        }
        else if (AbsReal > 1 && AbsReal < 2 && AbsImag < 1 && Scale == 0)
        {
            float64 d2m1 = (AbsReal - 1) * (AbsReal + 1);
            if (AbsImag >= DOUBLE_EPSILON)
            {
                d2m1 += AbsImag * AbsImag;
            }
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) / 2;
        }
        else if (AbsReal < 1 && AbsReal >= 0.5
            && AbsImag < DOUBLE_EPSILON / 2 && Scale == 0)
        {
            float64 d2m1 = (AbsReal - 1) * (AbsReal + 1);
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) / 2;
        }
        else if (AbsReal < 1 && AbsReal >= 0.5 && Scale == 0
            && AbsReal * AbsReal + AbsImag * AbsImag >= 0.5)
        {
            static auto __DEKKER_ADD64F = []
                (double *hi, double *lo, double x, double y)
            {
                *hi = x + y;
                *lo = (x - *hi) + y;
            };

            static auto __DEKKER_MULTIPLY64F = []
                (float64* hi, float64* lo, float64 x, float64 y)
            {
                *hi = x * y;
                float64 x1 = x * ((1 << (DBL_MANT_DIG + 1) / 2) + 1);
                float64 y1 = y * ((1 << (DBL_MANT_DIG + 1) / 2) + 1);
                x1 = (x - x1) + x1;
                y1 = (y - y1) + y1;
                float64 x2 = x - x1;
                float64 y2 = y - y1;
                *lo = (((x1 * y1 - *hi) + x1 * y2) + x2 * y1) + x2 * y2;
            };

            static auto __GLIBC_X2Y2M164F = [](float64 x, float64 y)
            {
                double vals[5];
                __DEKKER_MULTIPLY64F(&vals[1], &vals[0], x, x);
                __DEKKER_MULTIPLY64F(&vals[3], &vals[2], y, y);
                vals[4] = -1.0;

                static auto compare = [](float64 p, float64 q)
                {
                    return abs(p) < abs(q);
                };
                std::sort(std::begin(vals), std::end(vals), compare);

                // Add up the values so that each element of VALS has absolute value
                //at most equal to the last set bit of the next nonzero
                // element.
                for (size_t i = 0; i <= 3; i++)
                {
                    __DEKKER_ADD64F(&vals[i + 1], &vals[i], vals[i + 1], vals[i]);
                    std::sort(std::begin(vals) + i + 1, std::end(vals), compare);
                }
                // Now any error from this addition will be small.
                return vals[4] + vals[3] + vals[2] + vals[1] + vals[0];
            };

            float64 d2m1 = __GLIBC_X2Y2M164F(AbsReal, AbsImag);
            YReal = __IEEE854_LN1PX128F_C64F(d2m1) / 2;
        }
        else
        {
            float64 d = ::hypot(AbsReal, AbsImag);
            YReal = __IEEE854_LN128F_C64F(d) - Scale * CSE_LN2;
        }

        YImag = Arctan2(XImag, XReal).ToRadians();
    }
    else
    {
        YImag = __Float64::FromBytes(BIG_NAN_DOUBLE);
        if (RClass == FP_INFINITE || IClass == FP_INFINITE)
        {
            /* Real or imaginary part is infinite.  */
            YReal = __Float64::FromBytes(POS_INF_DOUBLE);
        }
        else {YReal = __Float64::FromBytes(BIG_NAN_DOUBLE);}
    }

    return complex64(YReal, YImag + _K * CSE_2PI);
}

_END_EXTERN_C
_CSE_END

#if 0 // ln test program generated by Deepseek
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <random>

#include <CSE/Base.h>

using namespace cse;

// 测试用例结构体
struct TestCase
{
    double x;
    double expected;  // std::log的结果
    double actual;    // __IEEE854_LN128F_C64F的结果
    double abs_error;
    double rel_error;
};

// 生成测试用例
std::vector<TestCase> generate_test_cases()
{
    std::vector<TestCase> cases;

    // 特殊值
    cases.push_back({0.0, NAN, NAN, NAN, NAN});  // 会被特殊处理
    cases.push_back({-1.0, NAN, NAN, NAN, NAN});
    cases.push_back({1.0, 0.0, 0.0, 0.0, 0.0});
    cases.push_back({INFINITY, INFINITY, INFINITY, 0.0, 0.0});
    cases.push_back({NAN, NAN, NAN, NAN, NAN});

    // 接近1的值
    for (double x = 0.99; x <= 1.01; x += 0.001)
    {
        cases.push_back({x, std::log(x), NAN, NAN, NAN});
    }

    // 一般值
    const double test_values[] =
    {
        1e-100, 1e-20, 0.5, 2.0, 10.0, 100.0, 1e20, 1e100,
        0.9921874, 0.9921875, 1.0078125, 1.0078126,
        2.2250738585072014e-308, 1.7976931348623157e308
    };

    for (double x : test_values)
    {
        cases.push_back({x, std::log(x), NAN, NAN, NAN});
    }

    // 随机值
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(1e-308, 1e308);
    for (int i = 0; i < 500; ++i)
    {
        double x = dist(rng);
        cases.push_back({x, std::log(x), NAN, NAN, NAN});
    }
    std::exponential_distribution<double> dist2(1E-12);
    for (int i = 0; i < 500; ++i)
    {
        double x = dist2(rng);
        cases.push_back({x, std::log(x), NAN, NAN, NAN});
    }

    return cases;
}

// 运行测试并计算误差
void run_tests(std::vector<TestCase>& cases)
{
    for (auto& test : cases)
    {
        if (std::isnan(test.x) || std::isinf(test.x) || test.x < 0)
        {
            continue;  // 跳过无效输入
        }

        test.actual = cse::__IEEE854_LN128F_C64F(test.x);

        if (std::isnan(test.expected) || std::isnan(test.actual) ||
            std::isinf(test.expected) || std::isinf(test.actual))
        {
            continue;  // 跳过无法计算误差的情况
        }

        test.abs_error = std::abs(test.actual - test.expected);
        test.rel_error = test.abs_error / std::abs(test.expected);
    }
}

// 输出结果到CSV文件
void write_to_csv(const std::vector<TestCase>& cases, const std::string& filename)
{
    std::ofstream out(filename);

    // 写入表头
    out << "x,std::log(x),__IEEE854_LN128F_C64F(x),Absolute Error,Relative Error\n";

    // 设置科学计数法和高精度
    out << std::scientific << std::setprecision(15);

    for (const auto& test : cases)
    {
        out << test.x << ",";

        if (std::isnan(test.expected))
        {
            out << "NaN,";
        }
        else if (std::isinf(test.expected))
        {
            out << (test.expected > 0 ? "Inf" : "-Inf") << ",";
        }
        else
        {
            out << test.expected << ",";
        }

        if (std::isnan(test.actual))
        {
            out << "NaN,";
        }
        else if (std::isinf(test.actual))
        {
            out << (test.actual > 0 ? "Inf" : "-Inf") << ",";
        }
        else
        {
            out << test.actual << ",";
        }

        if (std::isnan(test.abs_error))
        {
            out << "NaN,";
        }
        else
        {
            out << test.abs_error << ",";
        }

        if (std::isnan(test.rel_error))
        {
            out << "NaN";
        }
        else
        {
            out << test.rel_error;
        }

        out << "\n";
    }
}

int main()
{
    // 生成测试用例
    auto test_cases = generate_test_cases();
    // 运行测试
    run_tests(test_cases);
    // 输出结果到CSV文件
    write_to_csv(test_cases, "ln_test_report.csv");
    std::cout << "测试完成，结果已保存到 ln_test_report.csv" << std::endl;
}
#endif

#if 0 // ln1px test program generated by Deepseek
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdint>

#include <CSE/Base.h>

using namespace cse;

std::vector<double> generate_test_cases()
{
    std::vector<double> test_cases;

    // 特殊值
    test_cases.push_back(0.0);
    test_cases.push_back(-0.0);
    test_cases.push_back(std::numeric_limits<double>::infinity());
    test_cases.push_back(-std::numeric_limits<double>::infinity());
    test_cases.push_back(std::numeric_limits<double>::quiet_NaN());

    // 接近零的值
    for (int i = 1; i <= 10; ++i) {
        double x = std::ldexp(1.0, -i);
        test_cases.push_back(x);
        test_cases.push_back(-x);
    }

    // 正常范围内的值
    for (int i = -10; i <= 10; ++i) {
        double x = i * 0.1;
        test_cases.push_back(x);
    }

    // 边界值
    test_cases.push_back(-1.0);
    test_cases.push_back(-1.0 + std::numeric_limits<double>::epsilon());
    test_cases.push_back(-1.0 - std::numeric_limits<double>::epsilon());
    test_cases.push_back(1.0);
    test_cases.push_back(1.0 + std::numeric_limits<double>::epsilon());
    test_cases.push_back(1.0 - std::numeric_limits<double>::epsilon());

    // 极大值和极小值
    test_cases.push_back(std::ldexp(1.0, 100));
    test_cases.push_back(std::ldexp(1.0, -100));
    test_cases.push_back(std::numeric_limits<double>::max());
    test_cases.push_back(std::numeric_limits<double>::min());
    test_cases.push_back(std::numeric_limits<double>::denorm_min());

    // 随机值
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10.0, 10.0);
    for (int i = 0; i < 1000; ++i)
    {
        test_cases.push_back(dis(gen));
    }

    return test_cases;
}

int main()
{
    auto test_cases = generate_test_cases();

    std::ofstream out("lnxp1_test_report.csv");

    // 写入表头
    out << "Input Value,Custom log1p Result,Std log1p Result,"
        << "Absolute Error,Relative Error,ULP Difference\n";

    out << std::setprecision(17);

    for (double x : test_cases)
    {
        __Float64 custom_x(x);
        __Float64 custom_result = __IEEE854_LN1PX128F_C64F(custom_x);
        double std_result = std::log1p(x);

        // 计算误差
        double abs_error = std::numeric_limits<double>::quiet_NaN();
        double rel_error = std::numeric_limits<double>::quiet_NaN();
        double ulp_diff = std::numeric_limits<double>::quiet_NaN();

        if (!std::isnan(static_cast<double>(custom_result)))
        {
            if (!std::isnan(std_result))
            {
                abs_error = std::abs(static_cast<double>(custom_result) - std::abs(std_result));
                if (std_result != 0.0)
                {
                    rel_error = abs_error / std::abs(std_result);
                }

                // 计算ULP差异（简化版）
                int64_t custom_bits = *reinterpret_cast<int64_t*>(&custom_result.x);
                int64_t std_bits = *reinterpret_cast<int64_t*>(&std_result);
                ulp_diff = static_cast<double>(custom_bits - std_bits);
            }
        }

        out << x << ","
            << static_cast<double>(custom_result) << ","
            << std_result << ","
            << abs_error << ","
            << rel_error << ","
            << ulp_diff << "\n";
    }

    out.close();
    std::cout << "Results exported to lnxp1_test_report.csv\n";
}
#endif

#if 0 // complex ln test program generated by Deepseek
#include <iostream>
#include <complex>
#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cfloat>

#include <CSE/Base.h>

using namespace cse;

// 辅助函数：比较两个复数并返回最大相对误差
void compare_complex_log(const complex64& z, int k = 0)
{
    complex64 custom_log = __GLIBCT_LN64C(z, k);
    complex64 std_log = std::log(z);

    if (k != 0)
    {
        // 调整标准库结果以匹配分支选择
        std_log.imag(std_log.imag() + k * CSE_2PI);
    }

    double real_error = std::abs(custom_log.real() - std_log.real());
    double imag_error = std::abs(custom_log.imag() - std_log.imag());

    // 计算相对误差（避免除以零）
    double real_rel_error = 0;
    double imag_rel_error = 0;

    if (std_log.real() != 0)
    {
        real_rel_error = real_error / std::abs(std_log.real());
    }
    else if (custom_log.real() != 0)
    {
        real_rel_error = real_error / std::abs(custom_log.real());
    }

    if (std_log.imag() != 0)
    {
        imag_rel_error = imag_error / std::abs(std_log.imag());
    }
    else if (custom_log.imag() != 0)
    {
        imag_rel_error = imag_error / std::abs(custom_log.imag());
    }

    std::cout << "Input: " << z << "\n";
    std::cout << "Custom log: " << custom_log << "\n";
    std::cout << "Std log:    " << std_log << "\n";
    std::cout << "Real part relative error: " << real_rel_error << "\n";
    std::cout << "Imag part relative error: " << imag_rel_error << "\n";
    std::cout << "----------------------------------------\n";
}

int main()
{
    std::cout << std::setprecision(17);

    // 测试用例设计

    // 1. 零值测试
    std::cout << "=== Zero values ===\n";
    compare_complex_log(complex64(0.0, 0.0));
    compare_complex_log(complex64(-0.0, 0.0));
    compare_complex_log(complex64(0.0, -0.0));
    compare_complex_log(complex64(-0.0, -0.0));

    // 2. 实数测试（虚部为0）
    std::cout << "\n=== Real values ===\n";
    compare_complex_log(complex64(1.0, 0.0));
    compare_complex_log(complex64(2.0, 0.0));
    compare_complex_log(complex64(0.5, 0.0));
    compare_complex_log(complex64(-1.0, 0.0));
    compare_complex_log(complex64(1e-20, 0.0));
    compare_complex_log(complex64(1e20, 0.0));

    // 3. 纯虚数测试
    std::cout << "\n=== Pure imaginary values ===\n";
    compare_complex_log(complex64(0.0, 1.0));
    compare_complex_log(complex64(0.0, -1.0));
    compare_complex_log(complex64(0.0, 1e-20));
    compare_complex_log(complex64(0.0, 1e20));

    // 4. 一般复数测试
    std::cout << "\n=== General complex values ===\n";
    compare_complex_log(complex64(1.0, 1.0));
    compare_complex_log(complex64(1.0, -1.0));
    compare_complex_log(complex64(-1.0, 1.0));
    compare_complex_log(complex64(-1.0, -1.0));
    compare_complex_log(complex64(0.5, 0.5));
    compare_complex_log(complex64(0.999, 0.001));  // 接近(1,0)
    compare_complex_log(complex64(1.001, 0.001));  // 刚超过1

    // 5. 极值测试
    std::cout << "\n=== Extreme values ===\n";
    compare_complex_log(complex64(DBL_MAX, 0.0));
    compare_complex_log(complex64(0.0, DBL_MAX));
    compare_complex_log(complex64(DBL_MIN, 0.0));
    compare_complex_log(complex64(0.0, DBL_MIN));
    compare_complex_log(complex64(DBL_MAX, DBL_MAX));
    compare_complex_log(complex64(DBL_MIN, DBL_MIN));

    // 6. NaN和Inf测试
    std::cout << "\n=== NaN and Inf values ===\n";
    compare_complex_log(complex64(NAN, 0.0));
    compare_complex_log(complex64(0.0, NAN));
    compare_complex_log(complex64(NAN, NAN));
    compare_complex_log(complex64(INFINITY, 0.0));
    compare_complex_log(complex64(0.0, INFINITY));
    compare_complex_log(complex64(INFINITY, INFINITY));
    compare_complex_log(complex64(-INFINITY, 0.0));
    compare_complex_log(complex64(0.0, -INFINITY));

    // 7. 多值函数分支测试
    std::cout << "\n=== Multi-valued function branches ===\n";
    compare_complex_log(complex64(1.0, 0.0), 1);
    compare_complex_log(complex64(1.0, 0.0), -1);
    compare_complex_log(complex64(0.0, 1.0), 1);
    compare_complex_log(complex64(0.0, 1.0), -1);

    // 8. 特殊路径测试
    std::cout << "\n=== Special path values ===\n";
    // 测试AbsReal == 1 && Scale == 0路径
    compare_complex_log(complex64(1.0, 1e-8));
    // 测试AbsReal > 1 && AbsReal < 2 && AbsImag < 1 && Scale == 0路径
    compare_complex_log(complex64(1.5, 0.5));
    // 测试AbsReal < 1 && AbsReal >= 0.5 && AbsImag < EPSILON/2路径
    compare_complex_log(complex64(0.75, 1e-17));
    // 测试Dekker算法路径
    compare_complex_log(complex64(0.6, 0.8));
    return 0;
}
#endif
