// Fast tangent function for double precision by IBM
// Functions based on radians

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/ConstLists.h"
#include <cfloat>

_CSE_BEGIN

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

// Unnoteworthy code. (不值一提的代码)

const __Float64 __IBM_TANF64_XFGTBL[186][4]
{
    /* xi,Fi,Gi,FFi, i=16..201 */
    {__Float64::FromBytes(0x3fb000001e519d60),
     __Float64::FromBytes(0x3fb0055796c4e240),
     __Float64::FromBytes(0x402ff554628127b7),
     __Float64::FromBytes(0xbb9a1dee9e355b06),},
    {__Float64::FromBytes(0x3fb100001b1a7010),
     __Float64::FromBytes(0x3fb10668aab892b7),
     __Float64::FromBytes(0x402e12c7be3fdf74),
     __Float64::FromBytes(0x3ba89234037da741),},
    {__Float64::FromBytes(0x3fb200002505e350),
     __Float64::FromBytes(0x3fb2079bff547824),
     __Float64::FromBytes(0x402c65c5de853633),
     __Float64::FromBytes(0x3bb7486ee9614250),},
    {__Float64::FromBytes(0x3fb2fffffcdc4252),
     __Float64::FromBytes(0x3fb308f35eb16c68),
     __Float64::FromBytes(0x402ae5dae56be74f),
     __Float64::FromBytes(0xbb82c72691a23034),},
    {__Float64::FromBytes(0x3fb3ffffe3ff849f),
     __Float64::FromBytes(0x3fb40a71154999cc),
     __Float64::FromBytes(0x40298c43046b7352),
     __Float64::FromBytes(0x3b9aceaf3843738f),},
    {__Float64::FromBytes(0x3fb4ffffedc9590f),
     __Float64::FromBytes(0x3fb50c17429bdd80),
     __Float64::FromBytes(0x4028538491b5d674),
     __Float64::FromBytes(0xbbc1d02db4403d22),},
    {__Float64::FromBytes(0x3fb6000000ee83f7),
     __Float64::FromBytes(0x3fb60de7da80cc21),
     __Float64::FromBytes(0x40273724ef21a2a7),
     __Float64::FromBytes(0xbb95e53c72523ffd),},
    {__Float64::FromBytes(0x3fb6ffffeb05ea41),
     __Float64::FromBytes(0x3fb70fe4b8c51bea),
     __Float64::FromBytes(0x40263370fae562ff),
     __Float64::FromBytes(0xbb99ad0e8ffe0626),},
    {__Float64::FromBytes(0x3fb7ffffdc0515f7),
     __Float64::FromBytes(0x3fb812101db54498),
     __Float64::FromBytes(0x402545530e7eab5c),
     __Float64::FromBytes(0xbb914c87d62ed686),},
    {__Float64::FromBytes(0x3fb8ffffe384d7ab),
     __Float64::FromBytes(0x3fb9146c2a8d3727),
     __Float64::FromBytes(0x40246a33fd57f3fd),
     __Float64::FromBytes(0xbbbbda8d5381e06d),},
    {__Float64::FromBytes(0x3fb9ffffe4832347),
     __Float64::FromBytes(0x3fba16fad50e1050),
     __Float64::FromBytes(0x40239fe2c5537a96),
     __Float64::FromBytes(0x3bc7f695c111eabb),},
    {__Float64::FromBytes(0x3fbb0000274540e3),
     __Float64::FromBytes(0x3fbb19be7ae68517),
     __Float64::FromBytes(0x4022e4813637e946),
     __Float64::FromBytes(0x3bc307f88dbd9d93),},
    {__Float64::FromBytes(0x3fbbfffffebf2e9b),
     __Float64::FromBytes(0x3fbc1cb88369cd19),
     __Float64::FromBytes(0x4022367617aef223),
     __Float64::FromBytes(0x3bc50038424a9cf3),},
    {__Float64::FromBytes(0x3fbd000023529045),
     __Float64::FromBytes(0x3fbd1febc11d7ef7),
     __Float64::FromBytes(0x4021945fb8e43d4e),
     __Float64::FromBytes(0x3b81200752a6f224),},
    {__Float64::FromBytes(0x3fbdffffd872a829),
     __Float64::FromBytes(0x3fbe23598ee4d6b7),
     __Float64::FromBytes(0x4020fd0c76195d5f),
     __Float64::FromBytes(0xbbb4d9ab85fdca85),},
    {__Float64::FromBytes(0x3fbeffffff323b84),
     __Float64::FromBytes(0x3fbf2704ec9073e5),
     __Float64::FromBytes(0x40206f713020200f),
     __Float64::FromBytes(0x3bb77aa212836992),},
    {__Float64::FromBytes(0x3fc000000ce79195),
     __Float64::FromBytes(0x3fc01577bc30cc61),
     __Float64::FromBytes(0x401fd549d6564a88),
     __Float64::FromBytes(0xbbc8926f965c0ad0),},
    {__Float64::FromBytes(0x3fc07fffee40e918),
     __Float64::FromBytes(0x3fc0978d8279ac01),
     __Float64::FromBytes(0x401edbb59294bc03),
     __Float64::FromBytes(0xbb80a5334aae45d6),},
    {__Float64::FromBytes(0x3fc100000cc091fd),
     __Float64::FromBytes(0x3fc119c544dfb2f7),
     __Float64::FromBytes(0x401df0bb067d8e18),
     __Float64::FromBytes(0xbbcc2c184ff642a4),},
    {__Float64::FromBytes(0x3fc180000d9936a1),
     __Float64::FromBytes(0x3fc19c1fb9085a4b),
     __Float64::FromBytes(0x401d131a71ce3629),
     __Float64::FromBytes(0xbbc365530669355b),},
    {__Float64::FromBytes(0x3fc1ffffed5f3188),
     __Float64::FromBytes(0x3fc21e9dee74bf2d),
     __Float64::FromBytes(0x401c41b6ff0cd655),
     __Float64::FromBytes(0x3b8867f5478ecfc5),},
    {__Float64::FromBytes(0x3fc2800005f06a51),
     __Float64::FromBytes(0x3fc2a141550b313f),
     __Float64::FromBytes(0x401b7b921702e6d2),
     __Float64::FromBytes(0xbbadab51380131fe),},
    {__Float64::FromBytes(0x3fc2fffffe3d339e),
     __Float64::FromBytes(0x3fc3240aa75f76df),
     __Float64::FromBytes(0x401abfc8fcb6409d),
     __Float64::FromBytes(0x3bc60bcf0d291d83),},
    {__Float64::FromBytes(0x3fc37fffed888d6f),
     __Float64::FromBytes(0x3fc3a6fb13cc5db7),
     __Float64::FromBytes(0x401a0d8f8ed5320d),
     __Float64::FromBytes(0x3bb8a48e4eef03ab),},
    {__Float64::FromBytes(0x3fc4000002ca050d),
     __Float64::FromBytes(0x3fc42a13e25776bb),
     __Float64::FromBytes(0x4019642dfa84c2bc),
     __Float64::FromBytes(0xbbd0bd5dcc56516f),},
    {__Float64::FromBytes(0x3fc47ffff2531f5c),
     __Float64::FromBytes(0x3fc4ad55deb73404),
     __Float64::FromBytes(0x4018c2fef86e9035),
     __Float64::FromBytes(0x3b9cffe75aa287c8),},
    {__Float64::FromBytes(0x3fc5000013774992),
     __Float64::FromBytes(0x3fc530c27d0ee307),
     __Float64::FromBytes(0x4018296c370caf35),
     __Float64::FromBytes(0xbbcf75d1f91d6532),},
    {__Float64::FromBytes(0x3fc57fffedddcb2d),
     __Float64::FromBytes(0x3fc5b45a5db4347d),
     __Float64::FromBytes(0x401796ee52190c0e),
     __Float64::FromBytes(0x3b88a25f17d5d076),},
    {__Float64::FromBytes(0x3fc5fffff41949a0),
     __Float64::FromBytes(0x3fc6381f13bf986a),
     __Float64::FromBytes(0x40170b092d2255fd),
     __Float64::FromBytes(0xbb9bfb23b1bcd5e7),},
    {__Float64::FromBytes(0x3fc67ffff834d3a1),
     __Float64::FromBytes(0x3fc6bc118ec85952),
     __Float64::FromBytes(0x4016854c62cf2268),
     __Float64::FromBytes(0x3b9ee53b82e39e04),},
    {__Float64::FromBytes(0x3fc6fffffd9106ea),
     __Float64::FromBytes(0x3fc74032f298f6f7),
     __Float64::FromBytes(0x401605511f4f84a9),
     __Float64::FromBytes(0xbbb59c4a112634b8),},
    {__Float64::FromBytes(0x3fc780000f649a4f),
     __Float64::FromBytes(0x3fc7c4846ca53abc),
     __Float64::FromBytes(0x40158ab94809d175),
     __Float64::FromBytes(0x3bc91c7573d3cd2e),},
    {__Float64::FromBytes(0x3fc7ffffef06bbd8),
     __Float64::FromBytes(0x3fc84906df7d76ad),
     __Float64::FromBytes(0x4015152edd2b30a6),
     __Float64::FromBytes(0xbbbfa2da084c3eef),},
    {__Float64::FromBytes(0x3fc88000021c6334),
     __Float64::FromBytes(0x3fc8cdbbd965f986),
     __Float64::FromBytes(0x4014a46251b74296),
     __Float64::FromBytes(0xbb9ec02e74dcfe0b),},
    {__Float64::FromBytes(0x3fc8fffff38d0756),
     __Float64::FromBytes(0x3fc952a428e173c7),
     __Float64::FromBytes(0x4014380b17b59ebd),
     __Float64::FromBytes(0xbbcd0f1cb77589f0),},
    {__Float64::FromBytes(0x3fc98000104efca1),
     __Float64::FromBytes(0x3fc9d7c14644d23c),
     __Float64::FromBytes(0x4013cfe5cb1eabd5),
     __Float64::FromBytes(0xbbd5d6f7ea188d9e),},
    {__Float64::FromBytes(0x3fca000009417b30),
     __Float64::FromBytes(0x3fca5d14096d76aa),
     __Float64::FromBytes(0x40136bb4b3723db0),
     __Float64::FromBytes(0x3bbe3e0dfbf3979c),},
    {__Float64::FromBytes(0x3fca7fffeb1c23ec),
     __Float64::FromBytes(0x3fcae29dab60288d),
     __Float64::FromBytes(0x40130b3e783071d7),
     __Float64::FromBytes(0xbbc7dd823d5384bf),},
    {__Float64::FromBytes(0x3fcafffffb171c13),
     __Float64::FromBytes(0x3fcb685fa221a96b),
     __Float64::FromBytes(0x4012ae4dd8c0747d),
     __Float64::FromBytes(0x3bd4644bd5554972),},
    {__Float64::FromBytes(0x3fcb80000aba44be),
     __Float64::FromBytes(0x3fcbee5aecdf241f),
     __Float64::FromBytes(0x401254b1c6fad63b),
     __Float64::FromBytes(0x3ba41916d092b85a),},
    {__Float64::FromBytes(0x3fcc0000113d2a3e),
     __Float64::FromBytes(0x3fcc7490b3e92543),
     __Float64::FromBytes(0x4011fe3c9a62c035),
     __Float64::FromBytes(0xbba3cc3941a03739),},
    {__Float64::FromBytes(0x3fcc7ffff49e00ce),
     __Float64::FromBytes(0x3fccfb020f59eab0),
     __Float64::FromBytes(0x4011aac3e956a631),
     __Float64::FromBytes(0xbbb7a383bfa8cb5b),},
    {__Float64::FromBytes(0x3fcd000005f611ab),
     __Float64::FromBytes(0x3fcd81b089e6844e),
     __Float64::FromBytes(0x40115a1ff391268d),
     __Float64::FromBytes(0x3bd39b5cb2dc91f3),},
    {__Float64::FromBytes(0x3fcd800014764ceb),
     __Float64::FromBytes(0x3fce089d27debf0d),
     __Float64::FromBytes(0x40110c2bfbc84740),
     __Float64::FromBytes(0x3bc14d4d84712510),},
    {__Float64::FromBytes(0x3fce000014bcea76),
     __Float64::FromBytes(0x3fce8fc916dbc820),
     __Float64::FromBytes(0x4010c0c5a00ca48e),
     __Float64::FromBytes(0xbbd33788640f1b9e),},
    {__Float64::FromBytes(0x3fce7ffffd7995bd),
     __Float64::FromBytes(0x3fcf173588b50424),
     __Float64::FromBytes(0x401077ccbe02169a),
     __Float64::FromBytes(0xbbb61fee221fdf77),},
    {__Float64::FromBytes(0x3fcf00000cc35436),
     __Float64::FromBytes(0x3fcf9ee3fd21a40b),
     __Float64::FromBytes(0x401031231ee7ffe8),
     __Float64::FromBytes(0x3bd427e3c79ff5c1),},
    {__Float64::FromBytes(0x3fcf800001d1da33),
     __Float64::FromBytes(0x3fd0136ab7dbe15c),
     __Float64::FromBytes(0x400fd95977d559e5),
     __Float64::FromBytes(0x3bb0c6a1d67948d7),},
    {__Float64::FromBytes(0x3fd00000060c13b2),
     __Float64::FromBytes(0x3fd05785aaad4f18),
     __Float64::FromBytes(0x400f549e2675d182),
     __Float64::FromBytes(0xbbc1520818f0dd10),},
    {__Float64::FromBytes(0x3fd0400003885492),
     __Float64::FromBytes(0x3fd09bc3660542d7),
     __Float64::FromBytes(0x400ed3e2df3f5fec),
     __Float64::FromBytes(0xbbd95657b883ae62),},
    {__Float64::FromBytes(0x3fd08000052f5a13),
     __Float64::FromBytes(0x3fd0e0249a195045),
     __Float64::FromBytes(0x400e56f8fa68f2c8),
     __Float64::FromBytes(0x3bded7ba5a543e8e),},
    {__Float64::FromBytes(0x3fd0c00002ba1af5),
     __Float64::FromBytes(0x3fd124a9e2e7f24b),
     __Float64::FromBytes(0x400dddb4bffe633f),
     __Float64::FromBytes(0xbbdcba860c60278f),},
    {__Float64::FromBytes(0x3fd0fffff76642c1),
     __Float64::FromBytes(0x3fd16953e162ffe6),
     __Float64::FromBytes(0x400d67ed0311d5d5),
     __Float64::FromBytes(0x3b7b1f4ae40c5f9e),},
    {__Float64::FromBytes(0x3fd14000033602f0),
     __Float64::FromBytes(0x3fd1ae235f49508e),
     __Float64::FromBytes(0x400cf57ab8708266),
     __Float64::FromBytes(0xbbd6a6c28620f301),},
    {__Float64::FromBytes(0x3fd17ffffefd1a13),
     __Float64::FromBytes(0x3fd1f318db2a9ba1),
     __Float64::FromBytes(0x400c86398d11009e),
     __Float64::FromBytes(0x3bd3a9c669b21d3b),},
    {__Float64::FromBytes(0x3fd1bffff718365d),
     __Float64::FromBytes(0x3fd238350c41e3ac),
     __Float64::FromBytes(0x400c1a06e02be47c),
     __Float64::FromBytes(0x3bdb961a129e8cd1),},
    {__Float64::FromBytes(0x3fd1ffffff001e00),
     __Float64::FromBytes(0x3fd27d78b2f6395e),
     __Float64::FromBytes(0x400bb0c1f2fe9a85),
     __Float64::FromBytes(0x3be074a9e68fd7d8),},
    {__Float64::FromBytes(0x3fd23ffffe425a6a),
     __Float64::FromBytes(0x3fd2c2e4618faabe),
     __Float64::FromBytes(0x400b4a4c190b18df),
     __Float64::FromBytes(0xbbdf0d1ff615aad1),},
    {__Float64::FromBytes(0x3fd28000059ec1db),
     __Float64::FromBytes(0x3fd30878d8583884),
     __Float64::FromBytes(0x400ae6880cd82bc2),
     __Float64::FromBytes(0xbbd563c3141c1f8d),},
    {__Float64::FromBytes(0x3fd2c000000dd081),
     __Float64::FromBytes(0x3fd34e36affdb6d8),
     __Float64::FromBytes(0x400a855a5270fc15),
     __Float64::FromBytes(0xbbc6d88d9f2cdafd),},
    {__Float64::FromBytes(0x3fd2fffffc1dcd2b),
     __Float64::FromBytes(0x3fd3941ea95875bc),
     __Float64::FromBytes(0x400a26a8aa9502b6),
     __Float64::FromBytes(0xbbe13cad8389b15c),},
    {__Float64::FromBytes(0x3fd33ffff6c0d4a0),
     __Float64::FromBytes(0x3fd3da31739845f5),
     __Float64::FromBytes(0x4009ca5a4d2573a0),
     __Float64::FromBytes(0xbbc71636acaee379),},
    {__Float64::FromBytes(0x3fd3800006b16793),
     __Float64::FromBytes(0x3fd4206fdbc088f0),
     __Float64::FromBytes(0x400970579344e33a),
     __Float64::FromBytes(0xbbc2c0521d7a4f81),},
    {__Float64::FromBytes(0x3fd3c00007358fa3),
     __Float64::FromBytes(0x3fd466da6f23311d),
     __Float64::FromBytes(0x4009188a5aa612ea),
     __Float64::FromBytes(0x3b8653a5685e8edc),},
    {__Float64::FromBytes(0x3fd3fffffc3b18cf),
     __Float64::FromBytes(0x3fd4ad71e9282e6b),
     __Float64::FromBytes(0x4008c2dd641e643d),
     __Float64::FromBytes(0x3b95f0ef3f567c64),},
    {__Float64::FromBytes(0x3fd44000000dd2a8),
     __Float64::FromBytes(0x3fd4f4371fa3f2d1),
     __Float64::FromBytes(0x40086f3c6072f821),
     __Float64::FromBytes(0x3bb68efa95ff68b5),},
    {__Float64::FromBytes(0x3fd47ffffbb43713),
     __Float64::FromBytes(0x3fd53b2ab3ac333c),
     __Float64::FromBytes(0x40081d943da56692),
     __Float64::FromBytes(0xbbbf4d7f2985fd3f),},
    {__Float64::FromBytes(0x3fd4bffffb113bf4),
     __Float64::FromBytes(0x3fd5824d6e8ed9c2),
     __Float64::FromBytes(0x4007cdd2a8add00f),
     __Float64::FromBytes(0x3bcf478a1c9b3657),},
    {__Float64::FromBytes(0x3fd4fffff7f087c9),
     __Float64::FromBytes(0x3fd5c9a007446496),
     __Float64::FromBytes(0x40077fe6444588eb),
     __Float64::FromBytes(0xbbc177dca4eabb0c),},
    {__Float64::FromBytes(0x3fd54000088b3814),
     __Float64::FromBytes(0x3fd61123564125f9),
     __Float64::FromBytes(0x400733be6281a765),
     __Float64::FromBytes(0xbbc2c52cf57051c4),},
    {__Float64::FromBytes(0x3fd57ffff7d55966),
     __Float64::FromBytes(0x3fd658d7e194a5d5),
     __Float64::FromBytes(0x4006e94b73b47d1f),
     __Float64::FromBytes(0x3bda2fcff9996dc6),},
    {__Float64::FromBytes(0x3fd5c00008bf2490),
     __Float64::FromBytes(0x3fd6a0beb775b28d),
     __Float64::FromBytes(0x4006a07e15b6ec28),
     __Float64::FromBytes(0xbbe0ca90aa5285b8),},
    {__Float64::FromBytes(0x3fd6000009fa853f),
     __Float64::FromBytes(0x3fd6e8d865a66cfd),
     __Float64::FromBytes(0x400659481c701269),
     __Float64::FromBytes(0x3bd9ea958591e13a),},
    {__Float64::FromBytes(0x3fd6400007595fca),
     __Float64::FromBytes(0x3fd73125c0556a7c),
     __Float64::FromBytes(0x4006139bbaae9d02),
     __Float64::FromBytes(0x3bd88aff40152b83),},
    {__Float64::FromBytes(0x3fd68000031687da),
     __Float64::FromBytes(0x3fd779a792e2cfd0),
     __Float64::FromBytes(0x4005cf6bcae0882b),
     __Float64::FromBytes(0xbbd8a4a29f439451),},
    {__Float64::FromBytes(0x3fd6bffff5c8cfe2),
     __Float64::FromBytes(0x3fd7c25e9fb452ed),
     __Float64::FromBytes(0x40058cabc561f1cd),
     __Float64::FromBytes(0xbbe371a6f6a37d74),},
    {__Float64::FromBytes(0x3fd6fffff81df231),
     __Float64::FromBytes(0x3fd80b4bcfb4dab5),
     __Float64::FromBytes(0x40054b4f8d3ca5d3),
     __Float64::FromBytes(0x3bcb4686679dc99f),},
    {__Float64::FromBytes(0x3fd73ffffa71385e),
     __Float64::FromBytes(0x3fd8546fe007a9b6),
     __Float64::FromBytes(0x40050b4bb3b22176),
     __Float64::FromBytes(0xbbcd1540a5c73477),},
    {__Float64::FromBytes(0x3fd78000024a9c2b),
     __Float64::FromBytes(0x3fd89dcba7fcf5cf),
     __Float64::FromBytes(0x4004cc953159cbe1),
     __Float64::FromBytes(0xbbdc25ead58a6ad0),},
    {__Float64::FromBytes(0x3fd7c00002eb62b8),
     __Float64::FromBytes(0x3fd8e75fec0ba5cf),
     __Float64::FromBytes(0x40048f218731eeea),
     __Float64::FromBytes(0xbbc1cb73cc1adafb),},
    {__Float64::FromBytes(0x3fd80000054a52d1),
     __Float64::FromBytes(0x3fd9312d8bb822e9),
     __Float64::FromBytes(0x400452e69170a729),
     __Float64::FromBytes(0xbbd8bb17eac002ee),},
    {__Float64::FromBytes(0x3fd83ffff93a00a3),
     __Float64::FromBytes(0x3fd97b354bb9ad2a),
     __Float64::FromBytes(0x400417daae924e7f),
     __Float64::FromBytes(0x3bd4b8009a378cc7),},
    {__Float64::FromBytes(0x3fd87ffffbdc91c1),
     __Float64::FromBytes(0x3fd9c5782771b601),
     __Float64::FromBytes(0x4003ddf478855799),
     __Float64::FromBytes(0x3bd9077da00445d9),},
    {__Float64::FromBytes(0x3fd8bffff6d215e6),
     __Float64::FromBytes(0x3fda0ff6e0ea4a0b),
     __Float64::FromBytes(0x4003a52b189a0989),
     __Float64::FromBytes(0xbbda683189c0613d),},
    {__Float64::FromBytes(0x3fd9000002f734ef),
     __Float64::FromBytes(0x3fda5ab2736bf579),
     __Float64::FromBytes(0x40036d75e9244ca6),
     __Float64::FromBytes(0x3be3a6d84b722377),},
    {__Float64::FromBytes(0x3fd9400004eef8b4),
     __Float64::FromBytes(0x3fdaa5ab9fb6e3d0),
     __Float64::FromBytes(0x400336ccc9089cb7),
     __Float64::FromBytes(0x3b9f696322cc00bb),},
    {__Float64::FromBytes(0x3fd98000041ec76a),
     __Float64::FromBytes(0x3fdaf0e35176c7e4),
     __Float64::FromBytes(0x40030127cb0b9506),
     __Float64::FromBytes(0x3bb1ffdb5385a849),},
    {__Float64::FromBytes(0x3fd9c00008044e47),
     __Float64::FromBytes(0x3fdb3c5a77071224),
     __Float64::FromBytes(0x4002cc7f50d75ec7),
     __Float64::FromBytes(0xbbb0fade78effc8a),},
    {__Float64::FromBytes(0x3fda000001f8235b),
     __Float64::FromBytes(0x3fdb8811e725782e),
     __Float64::FromBytes(0x400298cc18fbfb37),
     __Float64::FromBytes(0xbbe55ed33b50e71b),},
    {__Float64::FromBytes(0x3fda3ffffb8c6f08),
     __Float64::FromBytes(0x3fdbd40a97b086f3),
     __Float64::FromBytes(0x40026607154de04b),
     __Float64::FromBytes(0xbbdec65e455faae3),},
    {__Float64::FromBytes(0x3fda7ffffb3d63e1),
     __Float64::FromBytes(0x3fdc20457d9a3b8a),
     __Float64::FromBytes(0x400234297e60bfbb),
     __Float64::FromBytes(0x3be3001c154ebd33),},
    {__Float64::FromBytes(0x3fdabffff5f45c48),
     __Float64::FromBytes(0x3fdc6cc37b8d45e6),
     __Float64::FromBytes(0x4002032cdb1ace69),
     __Float64::FromBytes(0xbbe5ebf83ed33616),},
    {__Float64::FromBytes(0x3fdb00000508b34c),
     __Float64::FromBytes(0x3fdcb985a27e8d37),
     __Float64::FromBytes(0x4001d30ad4459a2b),
     __Float64::FromBytes(0xbbd01432ae61e2d1),},
    {__Float64::FromBytes(0x3fdb40000a84710c),
     __Float64::FromBytes(0x3fdd068cc3e50155),
     __Float64::FromBytes(0x4001a3bd775034dd),
     __Float64::FromBytes(0xbbe80b1e58e0e228),},
    {__Float64::FromBytes(0x3fdb7ffff692e9d8),
     __Float64::FromBytes(0x3fdd53d9c49d6627),
     __Float64::FromBytes(0x4001753efe18066a),
     __Float64::FromBytes(0xbbb004c8f760d33e),},
    {__Float64::FromBytes(0x3fdbc0000280f14d),
     __Float64::FromBytes(0x3fdda16de4e81013),
     __Float64::FromBytes(0x40014789a38ea052),
     __Float64::FromBytes(0x3be848bc27c9c4ea),},
    {__Float64::FromBytes(0x3fdc0000001121d1),
     __Float64::FromBytes(0x3fddef49eac018f0),
     __Float64::FromBytes(0x40011a9820b8be0c),
     __Float64::FromBytes(0xbbe1527ed0d6010e),},
    {__Float64::FromBytes(0x3fdc3ffffef662aa),
     __Float64::FromBytes(0x3fde3d6eea0c7070),
     __Float64::FromBytes(0x4000ee6532f46ccd),
     __Float64::FromBytes(0x3be8d241189a000d),},
    {__Float64::FromBytes(0x3fdc800009845818),
     __Float64::FromBytes(0x3fde8bddf36a8b1b),
     __Float64::FromBytes(0x4000c2ebcac73476),
     __Float64::FromBytes(0x3bd221f712bed284),},
    {__Float64::FromBytes(0x3fdcbffffb0493bf),
     __Float64::FromBytes(0x3fdeda97e0c60d10),
     __Float64::FromBytes(0x40009827251c7836),
     __Float64::FromBytes(0xbbe0bd546eec41b7),},
    {__Float64::FromBytes(0x3fdcfffffd52961f),
     __Float64::FromBytes(0x3fdf299defb3e44b),
     __Float64::FromBytes(0x40006e1274e459f5),
     __Float64::FromBytes(0xbbd93f77e969c82f),},
    {__Float64::FromBytes(0x3fdd3ffffe2319a4),
     __Float64::FromBytes(0x3fdf78f117139490),
     __Float64::FromBytes(0x400044a93e737e94),
     __Float64::FromBytes(0xbb91e7cc49594b7a),},
    {__Float64::FromBytes(0x3fdd7ffffa4de596),
     __Float64::FromBytes(0x3fdfc892638f49e8),
     __Float64::FromBytes(0x40001be7231057a5),
     __Float64::FromBytes(0x3bd482b0f5af9f5f),},
    {__Float64::FromBytes(0x3fddbffffe729a69),
     __Float64::FromBytes(0x3fe00c417c6ab019),
     __Float64::FromBytes(0x3fffe78fbf612660),
     __Float64::FromBytes(0x3bea5cda00da681e),},
    {__Float64::FromBytes(0x3fde000009d66802),
     __Float64::FromBytes(0x3fe03461f6b883cf),
     __Float64::FromBytes(0x3fff988ebc05a87c),
     __Float64::FromBytes(0xbbe06c33f2372669),},
    {__Float64::FromBytes(0x3fde3ffffb211657),
     __Float64::FromBytes(0x3fe05cab191db8e8),
     __Float64::FromBytes(0x3fff4ac37bcfe6be),
     __Float64::FromBytes(0xbbd5d51f5ed8d35b),},
    {__Float64::FromBytes(0x3fde80000a3f068a),
     __Float64::FromBytes(0x3fe0851d95fb54f0),
     __Float64::FromBytes(0x3ffefe26144ca408),
     __Float64::FromBytes(0xbbc7c894a2c169c5),},
    {__Float64::FromBytes(0x3fdec00001adb060),
     __Float64::FromBytes(0x3fe0adb9dc7b54f9),
     __Float64::FromBytes(0x3ffeb2af5ebe52a7),
     __Float64::FromBytes(0x3bd4e740312c5ffd),},
    {__Float64::FromBytes(0x3fdeffffff5c0d01),
     __Float64::FromBytes(0x3fe0d68092550a8d),
     __Float64::FromBytes(0x3ffe68580d71fdf0),
     __Float64::FromBytes(0x3bddd8a696b35499),},
    {__Float64::FromBytes(0x3fdf3ffff93d5fcc),
     __Float64::FromBytes(0x3fe0ff7245cb4374),
     __Float64::FromBytes(0x3ffe1f193cce5040),
     __Float64::FromBytes(0xbbc9f0ec7c1efab4),},
    {__Float64::FromBytes(0x3fdf7ffffa0dd18f),
     __Float64::FromBytes(0x3fe1288f944dd508),
     __Float64::FromBytes(0x3ffdd6ec298b874d),
     __Float64::FromBytes(0x3bea6ebd9642a0a6),},
    {__Float64::FromBytes(0x3fdfbffffd3a9f1a),
     __Float64::FromBytes(0x3fe151d913750f3e),
     __Float64::FromBytes(0x3ffd8fca5806a27e),
     __Float64::FromBytes(0x3bda2a03fc65ac7a),},
    {__Float64::FromBytes(0x3fdffffffc481400),
     __Float64::FromBytes(0x3fe17b4f598944ca),
     __Float64::FromBytes(0x3ffd49ad82532170),
     __Float64::FromBytes(0x3bc4412e3d236dc3),},
    {__Float64::FromBytes(0x3fe01fffff53786c),
     __Float64::FromBytes(0x3fe1a4f307d83d47),
     __Float64::FromBytes(0x3ffd048f851bffeb),
     __Float64::FromBytes(0x3bd1589d29f81b14),},
    {__Float64::FromBytes(0x3fe03ffffee301b7),
     __Float64::FromBytes(0x3fe1cec4b8a6a382),
     __Float64::FromBytes(0x3ffcc06a7c519db6),
     __Float64::FromBytes(0x3bd370e65b24d6b2),},
    {__Float64::FromBytes(0x3fe06000006e36bf),
     __Float64::FromBytes(0x3fe1f8c5114eb8be),
     __Float64::FromBytes(0x3ffc7d38a34d6786),
     __Float64::FromBytes(0xbbea92de4b98c1d4),},
    {__Float64::FromBytes(0x3fe07ffffd60aa43),
     __Float64::FromBytes(0x3fe222f4abeccecb),
     __Float64::FromBytes(0x3ffc3af477342ac4),
     __Float64::FromBytes(0xbbdd47f603a5c2c2),},
    {__Float64::FromBytes(0x3fe0a000037762e8),
     __Float64::FromBytes(0x3fe24d543f99efe8),
     __Float64::FromBytes(0x3ffbf99875f54fab),
     __Float64::FromBytes(0x3bedf7f415771a46),},
    {__Float64::FromBytes(0x3fe0bfffff1c6921),
     __Float64::FromBytes(0x3fe277e4598e35d0),
     __Float64::FromBytes(0x3ffbb91f8addd186),
     __Float64::FromBytes(0x3be0f16c5e0e5a73),},
    {__Float64::FromBytes(0x3fe0dfffff07154b),
     __Float64::FromBytes(0x3fe2a2a5b6bc3986),
     __Float64::FromBytes(0x3ffb79848301646d),
     __Float64::FromBytes(0xbbf02dd0bbaa5310),},
    {__Float64::FromBytes(0x3fe1000002fcdda4),
     __Float64::FromBytes(0x3fe2cd9902a59f1e),
     __Float64::FromBytes(0x3ffb3ac2705219bf),
     __Float64::FromBytes(0xbbe59357112fa616),},
    {__Float64::FromBytes(0x3fe1200001ce1140),
     __Float64::FromBytes(0x3fe2f8bedf0a67c2),
     __Float64::FromBytes(0x3ffafcd49ab8ae2a),
     __Float64::FromBytes(0x3be2c5429303f346),},
    {__Float64::FromBytes(0x3fe1400004d0f355),
     __Float64::FromBytes(0x3fe3241808fcc7bf),
     __Float64::FromBytes(0x3ffabfb6497b9a36),
     __Float64::FromBytes(0x3bebc044b5a59234),},
    {__Float64::FromBytes(0x3fe1600000fb0c8a),
     __Float64::FromBytes(0x3fe34fa52471618b),
     __Float64::FromBytes(0x3ffa83630d26d117),
     __Float64::FromBytes(0xbbdbfbb23f7bb7c9),},
    {__Float64::FromBytes(0x3fe18000026f10b3),
     __Float64::FromBytes(0x3fe37b66f7579056),
     __Float64::FromBytes(0x3ffa47d66b4cf4b1),
     __Float64::FromBytes(0x3bf0f6b4af0b5de9),},
    {__Float64::FromBytes(0x3fe19ffffd0978f8),
     __Float64::FromBytes(0x3fe3a75e290cc78c),
     __Float64::FromBytes(0x3ffa0d0c36c21315),
     __Float64::FromBytes(0x3beb2129a296b262),},
    {__Float64::FromBytes(0x3fe1bffffd94840b),
     __Float64::FromBytes(0x3fe3d38b85b4e4a4),
     __Float64::FromBytes(0x3ff9d30032f2ecef),
     __Float64::FromBytes(0xbbdbab1ab9bb7d74),},
    {__Float64::FromBytes(0x3fe1dffffbda1ea1),
     __Float64::FromBytes(0x3fe3ffefbf3cee2f),
     __Float64::FromBytes(0x3ff999ae6770fed8),
     __Float64::FromBytes(0x3bda0bdcb4ace9a4),},
    {__Float64::FromBytes(0x3fe1fffffc989533),
     __Float64::FromBytes(0x3fe42c8b9c27900c),
     __Float64::FromBytes(0x3ff96112e0d9f1ac),
     __Float64::FromBytes(0xbbee19eb2fa2d81a),},
    {__Float64::FromBytes(0x3fe22000012b8d26),
     __Float64::FromBytes(0x3fe4595fe11975ca),
     __Float64::FromBytes(0x3ff92929cdaa4e80),
     __Float64::FromBytes(0x3bf23382acc82d4b),},
    {__Float64::FromBytes(0x3fe2400004f4d6af),
     __Float64::FromBytes(0x3fe4866d4d224131),
     __Float64::FromBytes(0x3ff8f1ef815c34e8),
     __Float64::FromBytes(0xbbd0c6ff3b740a99),},
    {__Float64::FromBytes(0x3fe25ffffcc07bda),
     __Float64::FromBytes(0x3fe4b3b498b7d010),
     __Float64::FromBytes(0x3ff8bb6073e7ffa1),
     __Float64::FromBytes(0x3bebc31b1ad7a9c2),},
    {__Float64::FromBytes(0x3fe28000042d9639),
     __Float64::FromBytes(0x3fe4e136b64540d1),
     __Float64::FromBytes(0x3ff88578f4374938),
     __Float64::FromBytes(0x3be36de91b85e901),},
    {__Float64::FromBytes(0x3fe2a00003be29a0),
     __Float64::FromBytes(0x3fe50ef452bffd96),
     __Float64::FromBytes(0x3ff85035c0042c06),
     __Float64::FromBytes(0x3be15d0176f5efbd),},
    {__Float64::FromBytes(0x3fe2bffffaa91f12),
     __Float64::FromBytes(0x3fe53cee3e2f4e0d),
     __Float64::FromBytes(0x3ff81b938542df07),
     __Float64::FromBytes(0x3be555cd17662a2b),},
    {__Float64::FromBytes(0x3fe2dffffe884891),
     __Float64::FromBytes(0x3fe56b256c1a2470),
     __Float64::FromBytes(0x3ff7e78ee422ea70),
     __Float64::FromBytes(0x3bf03504bd030c11),},
    {__Float64::FromBytes(0x3fe2fffffe87152b),
     __Float64::FromBytes(0x3fe5999a9beaaaa1),
     __Float64::FromBytes(0x3ff7b424d18fe9b3),
     __Float64::FromBytes(0xbb649a5f773e0e64),},
    {__Float64::FromBytes(0x3fe31fffffc1a721),
     __Float64::FromBytes(0x3fe5c84eafe0e564),
     __Float64::FromBytes(0x3ff78152338db8d4),
     __Float64::FromBytes(0x3beaf4285da8e935),},
    {__Float64::FromBytes(0x3fe33fffff70a372),
     __Float64::FromBytes(0x3fe5f74282191d64),
     __Float64::FromBytes(0x3ff74f141122bcae),
     __Float64::FromBytes(0x3bdb1c4bdee4bfaf),},
    {__Float64::FromBytes(0x3fe360000436e836),
     __Float64::FromBytes(0x3fe62676fde6ccff),
     __Float64::FromBytes(0x3ff71d677644252c),
     __Float64::FromBytes(0xbbec3d10e08c3afb),},
    {__Float64::FromBytes(0x3fe37ffffcbe9641),
     __Float64::FromBytes(0x3fe655ecee9ffdaf),
     __Float64::FromBytes(0x3ff6ec49a6fc0515),
     __Float64::FromBytes(0x3bdda4532ed29567),},
    {__Float64::FromBytes(0x3fe39fffffb6d6ca),
     __Float64::FromBytes(0x3fe685a55e67a1e1),
     __Float64::FromBytes(0x3ff6bbb7bc2ae969),
     __Float64::FromBytes(0x3becbf7b2ef43882),},
    {__Float64::FromBytes(0x3fe3c00004934fec),
     __Float64::FromBytes(0x3fe6b5a12cc07d75),
     __Float64::FromBytes(0x3ff68baf10b02ef8),
     __Float64::FromBytes(0xbbe7c8fbfeb7cabd),},
    {__Float64::FromBytes(0x3fe3e00003f5cf7f),
     __Float64::FromBytes(0x3fe6e5e13e59def6),
     __Float64::FromBytes(0x3ff65c2d0e61500f),
     __Float64::FromBytes(0xbbe30ba4035f7845),},
    {__Float64::FromBytes(0x3fe4000005280ad9),
     __Float64::FromBytes(0x3fe7166691ab4c3e),
     __Float64::FromBytes(0x3ff62d2f19f01c90),
     __Float64::FromBytes(0xbbf1e9f5ffe95f6a),},
    {__Float64::FromBytes(0x3fe42000049efb65),
     __Float64::FromBytes(0x3fe7473218af3b9d),
     __Float64::FromBytes(0x3ff5feb2b86465e4),
     __Float64::FromBytes(0x3bc4cad7280d591e),},
    {__Float64::FromBytes(0x3fe440000035ccb6),
     __Float64::FromBytes(0x3fe77844cb4ff1e5),
     __Float64::FromBytes(0x3ff5d0b57c455428),
     __Float64::FromBytes(0x3bed8c187ba5617c),},
    {__Float64::FromBytes(0x3fe4600003346717),
     __Float64::FromBytes(0x3fe7a99fba258778),
     __Float64::FromBytes(0x3ff5a334f4392254),
     __Float64::FromBytes(0xbbefd14afc84a570),},
    {__Float64::FromBytes(0x3fe4800003002575),
     __Float64::FromBytes(0x3fe7db43d836768f),
     __Float64::FromBytes(0x3ff5762edcf97e0c),
     __Float64::FromBytes(0xbbdd7eba5f5df49e),},
    {__Float64::FromBytes(0x3fe4a000055bf381),
     __Float64::FromBytes(0x3fe80d3235edeefa),
     __Float64::FromBytes(0x3ff549a0ea46e31f),
     __Float64::FromBytes(0xbbdba52276823eac),},
    {__Float64::FromBytes(0x3fe4c00004ce10e3),
     __Float64::FromBytes(0x3fe83f6bd67dc1a8),
     __Float64::FromBytes(0x3ff51d88ed82bcc4),
     __Float64::FromBytes(0xbbeae92d077d29ea),},
    {__Float64::FromBytes(0x3fe4e000016c60e1),
     __Float64::FromBytes(0x3fe871f1ca0aaf31),
     __Float64::FromBytes(0x3ff4f1e4bdacbf16),
     __Float64::FromBytes(0x3be8295846ee425e),},
    {__Float64::FromBytes(0x3fe4ffffff966f0a),
     __Float64::FromBytes(0x3fe8a4c52bff2dae),
     __Float64::FromBytes(0x3ff4c6b23917657e),
     __Float64::FromBytes(0xbbf127c25c86c705),},
    {__Float64::FromBytes(0x3fe520000076e6eb),
     __Float64::FromBytes(0x3fe8d7e7175651e8),
     __Float64::FromBytes(0x3ff49bef4f459b05),
     __Float64::FromBytes(0xbbb1e9d14181bbfc),},
    {__Float64::FromBytes(0x3fe5400003d12d3b),
     __Float64::FromBytes(0x3fe90b58a976ed56),
     __Float64::FromBytes(0x3ff47199fdf24af4),
     __Float64::FromBytes(0x3be38c17c30decaf),},
    {__Float64::FromBytes(0x3fe55ffffce7fa8d),
     __Float64::FromBytes(0x3fe93f1af03a3a09),
     __Float64::FromBytes(0x3ff447b05f13234b),
     __Float64::FromBytes(0x3bf1b8b270df7e20),},
    {__Float64::FromBytes(0x3fe580000331b46a),
     __Float64::FromBytes(0x3fe9732f38e83134),
     __Float64::FromBytes(0x3ff41e3068d8b41b),
     __Float64::FromBytes(0xbbee24d8b90bc28b),},
    {__Float64::FromBytes(0x3fe59ffffc14848e),
     __Float64::FromBytes(0x3fe9a7968471b489),
     __Float64::FromBytes(0x3ff3f5185de3aa73),
     __Float64::FromBytes(0xbbecacd9e0761536),},
    {__Float64::FromBytes(0x3fe5bffffb7cd395),
     __Float64::FromBytes(0x3fe9dc5224a8b955),
     __Float64::FromBytes(0x3ff3cc664f8fff15),
     __Float64::FromBytes(0xbbf67c9782045611),},
    {__Float64::FromBytes(0x3fe5e000000dcc40),
     __Float64::FromBytes(0x3fea11634df5b93e),
     __Float64::FromBytes(0x3ff3a41875853228),
     __Float64::FromBytes(0xbbf585dad481f350),},
    {__Float64::FromBytes(0x3fe6000002efd2fc),
     __Float64::FromBytes(0x3fea46cb30d16323),
     __Float64::FromBytes(0x3ff37c2d187962ae),
     __Float64::FromBytes(0x3bf004c3a5f77bb0),},
    {__Float64::FromBytes(0x3fe61ffffeb8088a),
     __Float64::FromBytes(0x3fea7c8b053920c0),
     __Float64::FromBytes(0x3ff354a2891769a9),
     __Float64::FromBytes(0x3bbc6b303fee3029),},
    {__Float64::FromBytes(0x3fe6400000f3ca06),
     __Float64::FromBytes(0x3feab2a428a1911a),
     __Float64::FromBytes(0x3ff32d770a6f0a4a),
     __Float64::FromBytes(0x3bf2a6f8fac5081a),},
    {__Float64::FromBytes(0x3fe65ffffe9ec2f4),
     __Float64::FromBytes(0x3feae917d4ce7239),
     __Float64::FromBytes(0x3ff306a90751a948),
     __Float64::FromBytes(0xbbe950b551ab9dbd),},
    {__Float64::FromBytes(0x3fe6800003d43966),
     __Float64::FromBytes(0x3feb1fe7708b998a),
     __Float64::FromBytes(0x3ff2e036d7a153c7),
     __Float64::FromBytes(0x3bdd36e2a1e4a14e),},
    {__Float64::FromBytes(0x3fe69ffffab67783),
     __Float64::FromBytes(0x3feb57142e575464),
     __Float64::FromBytes(0x3ff2ba1f05006cb6),
     __Float64::FromBytes(0x3bea9a4a473c2e31),},
    {__Float64::FromBytes(0x3fe6bffffcb65f89),
     __Float64::FromBytes(0x3feb8e9f981efd2f),
     __Float64::FromBytes(0x3ff2945fe948d9f7),
     __Float64::FromBytes(0xbbca5294e802df72),},
    {__Float64::FromBytes(0x3fe6dffffc5609a9),
     __Float64::FromBytes(0x3febc68afaed6ff1),
     __Float64::FromBytes(0x3ff26ef81533411e),
     __Float64::FromBytes(0xbbf89153f51bc566),},
    {__Float64::FromBytes(0x3fe6fffffc4eef86),
     __Float64::FromBytes(0x3febfed7c62205fe),
     __Float64::FromBytes(0x3ff249e60e70978c),
     __Float64::FromBytes(0x3bc39021a2b9ff56),},
    {__Float64::FromBytes(0x3fe72000004d98b3),
     __Float64::FromBytes(0x3fec3787716968ad),
     __Float64::FromBytes(0x3ff2252861be7751),
     __Float64::FromBytes(0x3befc9c574ee2211),},
    {__Float64::FromBytes(0x3fe73ffffc155075),
     __Float64::FromBytes(0x3fec709b5ec6fd4e),
     __Float64::FromBytes(0x3ff200bdb5d53311),
     __Float64::FromBytes(0x3be28a4da269ae63),},
    {__Float64::FromBytes(0x3fe760000498c203),
     __Float64::FromBytes(0x3fecaa15323d08c1),
     __Float64::FromBytes(0x3ff1dca493433f65),
     __Float64::FromBytes(0x3bf8cae414a28fb7),},
    {__Float64::FromBytes(0x3fe77fffff1e5636),
     __Float64::FromBytes(0x3fece3f64147c12c),
     __Float64::FromBytes(0x3ff1b8dbbfe294a8),
     __Float64::FromBytes(0xbbe7e19c4b56a744),},
    {__Float64::FromBytes(0x3fe7a0000226d45a),
     __Float64::FromBytes(0x3fed1e404120eb7f),
     __Float64::FromBytes(0x3ff19561d15f8278),
     __Float64::FromBytes(0x3be64b28032c5d4c),},
    {__Float64::FromBytes(0x3fe7c0000250a5aa),
     __Float64::FromBytes(0x3fed58f4b112a1e1),
     __Float64::FromBytes(0x3ff172358a59d565),
     __Float64::FromBytes(0xbbe716deb8dc7867),},
    {__Float64::FromBytes(0x3fe7e0000482f82e),
     __Float64::FromBytes(0x3fed94153576bdf0),
     __Float64::FromBytes(0x3ff14f55a22a1c5b),
     __Float64::FromBytes(0x3bf207e1e1305604),},
    {__Float64::FromBytes(0x3fe800000205003e),
     __Float64::FromBytes(0x3fedcfa364d69ff7),
     __Float64::FromBytes(0x3ff12cc0e37eb26f),
     __Float64::FromBytes(0xbbd52ec6e32395f8),},
    {__Float64::FromBytes(0x3fe81ffffbf99411),
     __Float64::FromBytes(0x3fee0ba0ebf98f51),
     __Float64::FromBytes(0x3ff10a7616ddd5d6),
     __Float64::FromBytes(0xbbece0d659866045),},
    {__Float64::FromBytes(0x3fe840000248e3a3),
     __Float64::FromBytes(0x3fee480f9bb7f565),
     __Float64::FromBytes(0x3ff0e873fb84e05c),
     __Float64::FromBytes(0x3bf4e5e81595df92),},
    {__Float64::FromBytes(0x3fe860000145c157),
     __Float64::FromBytes(0x3fee84f10a10b3ab),
     __Float64::FromBytes(0x3ff0c6b97cbd7b1e),
     __Float64::FromBytes(0xbbe19de6d5f121d0),},
    {__Float64::FromBytes(0x3fe88000022631b9),
     __Float64::FromBytes(0x3feec2470be1f047),
     __Float64::FromBytes(0x3ff0a5456d0b3ee6),
     __Float64::FromBytes(0xbbc272b1a3ba2c6f),},
    {__Float64::FromBytes(0x3fe8a000045f7828),
     __Float64::FromBytes(0x3fef00136c45ba1c),
     __Float64::FromBytes(0x3ff08416af2a0f09),
     __Float64::FromBytes(0x3be82b565b63c799),},
    {__Float64::FromBytes(0x3fe8bfffffc686cf),
     __Float64::FromBytes(0x3fef3e57f03c824b),
     __Float64::FromBytes(0x3ff0632c33502220),
     __Float64::FromBytes(0xbbd039ad2dbeeb25),},
    {__Float64::FromBytes(0x3fe8dffffd8644c6),
     __Float64::FromBytes(0x3fef7d168774261d),
     __Float64::FromBytes(0x3ff04284dd5b3019),
     __Float64::FromBytes(0x3bd79f33e1eba933),},
    {__Float64::FromBytes(0x3fe8fffffe4e7937),
     __Float64::FromBytes(0x3fefbc511a99a641),
     __Float64::FromBytes(0x3ff0221f9f69840b),
     __Float64::FromBytes(0xbbea9e847beee018),},
    {__Float64::FromBytes(0x3fe920000435251f),
     __Float64::FromBytes(0x3feffc099eb22390),
     __Float64::FromBytes(0x3ff001fb6f7c51e8),
     __Float64::FromBytes(0xbb5a12e731032e0a),},
 };

int __IBM_branred(__Float64 x, __Float64* a, __Float64* aa);

__Float64 __cdecl __IBM_TAN64F(__Float64 x)
{
    __Float64
        d3      = __Float64::FromBytes(0x3FD5555555555555), /*  0.333... */
        d5      = __Float64::FromBytes(0x3FC11111111107C6), /*  0.133... */
        d7      = __Float64::FromBytes(0x3FABA1BA1CDB8745), /*    .      */
        d9      = __Float64::FromBytes(0x3F9664ED49CFC666), /*    .      */
        d11     = __Float64::FromBytes(0x3F82385A3CF2E4EA), /*    .      */
        /* polynomial II */
        /* polynomial III */
        e0      = __Float64::FromBytes(0x3FD5555555554DBD), /*    .      */
        e1      = __Float64::FromBytes(0x3FC11112E0A6B45F), /*    .      */

        /* constants    */
        mfftnhf = __Float64::FromBytes(0xc02f000000000000), /*-15.5      */

        g1      = __Float64::FromBytes(0x3e4b096c00000000), /* 1.259e-8  */
        g2      = __Float64::FromBytes(0x3faf212d00000000), /* 0.0608    */
        g3      = __Float64::FromBytes(0x3fe92f1a00000000), /* 0.787     */
        g4      = __Float64::FromBytes(0x4039000000000000), /* 25.0      */
        g5      = __Float64::FromBytes(0x4197d78400000000), /* 1e8       */
        gy2     = __Float64::FromBytes(0x3faf212d00000000), /* 0.0608    */

        mp1     = __Float64::FromBytes(0x3FF921FB58000000),
        mp2     = __Float64::FromBytes(0xBE4DDE973C000000),
        mp3     = __Float64::FromBytes(0xBC8CB3B399D747F2),
        pp3     = __Float64::FromBytes(0xBC8CB3B398000000),
        pp4     = __Float64::FromBytes(0xbacd747f23e32ed7),
        hpinv   = __Float64::FromBytes(0x3FE45F306DC9C883),
        toint   = __Float64::FromBytes(0x4338000000000000);

    int ux, i, n;
    __Float64 a, da, a2, b, db, c, dc, fi, gi, pz,
        s, sy, t, t1, t2, t3, t4, w, x2, xn, y, ya,
        yya, z0, z, z2;
    __Float64 num, v;

    __Float64 retval;

    /* x=+-INF, x=NaN */
    num = x;
    ux = num.parts.msw;
    if ((ux & 0x7ff00000) == 0x7ff00000)
    {
        if ((ux & 0x7fffffff) == 0x7ff00000)
            return x - x;
    }

    w = (x < 0.0) ? -x.x : x.x;

    /* (I) The case abs(x) <= 1.259e-8 */
    if (w <= g1) { return x; }

    /* (II) The case 1.259e-8 < abs(x) <= 0.0608 */
    if (w <= g2)
    {
        x2 = x * x;

        t2 = d9 + x2 * d11;
        t2 = d7 + x2 * t2;
        t2 = d5 + x2 * t2;
        t2 = d3 + x2 * t2;
        t2 *= x * x2;

        return x + t2;
        /* Max ULP is 0.504.  */
    }

    /* (III) The case 0.0608 < abs(x) <= 0.787 */
    if (w <= g3)
    {
        i = ((int)(mfftnhf + 256 * w));
        z = w - __IBM_TANF64_XFGTBL[i][0].x;
        z2 = z * z;
        s = (x < 0.0) ? -1 : 1;
        pz = z + z * z2 * (e0 + z2 * e1);
        fi = __IBM_TANF64_XFGTBL[i][1];
        gi = __IBM_TANF64_XFGTBL[i][2];
        t2 = pz * (gi + fi) / (gi - pz);
        y = fi + t2;
        return (s * y);
        /* Max ULP is 0.60.  */
    }

    /* (---) The case 0.787 < abs(x) <= 25 */
    auto EADD = [](__Float64* x, __Float64* y, __Float64* z, __Float64* zz)
    {
        *z = (*x) + (*y);
        *zz = (fabs(*x) > fabs(*y)) ? (((*x)-(*z)) + (*y)) : (((*y)-(*z)) + (*x));
    };

    auto MUL12 = [](__Float64* x, __Float64* y, __Float64* z, __Float64* zz)
    {
        __Float64 CN = __Float64::FromBytes(0x41A0000002000000);
        __Float64 __p, hx, tx, hy, ty, __q;
        __p = CN * (*x);
        hx = ((*x)-__p) + __p;
        tx = (*x) - hx;
        __p = CN * (*y);
        hy = ((*y) - __p) + __p;
        ty = (*y) - hy;
        __p = hx * hy;
        __q = hx * ty + tx * hy;
        *z = __p + __q;
        *zz = ((__p - *z) + __q) + tx * ty;
    };

    auto DIV2 = [&MUL12](__Float64* x, __Float64* xx, __Float64* y, __Float64* yy, __Float64* z, __Float64* zz, __Float64* c, __Float64* cc, __Float64* u, __Float64* uu)
    {
        *c = (*x) / (*y);
        MUL12(c, y, u, uu);
        *cc = (((((*x) - *u) - *uu) + (*xx)) - *c * (*yy)) / (*y);
        *z = *c + *cc;
        *zz = (*c - *z) + *cc;
    };

    if (w <= g4)
    {
        /* Range reduction by algorithm i */
        t = (x * hpinv + toint);
        xn = t - toint;
        v = t;
        t1 = (x - xn * mp1) - xn * mp2;
        n = v.parts.lsw & 0x00000001;
        da = xn * mp3;
        a = t1 - da;
        da = (t1 - a) - da;
        if (a < 0.0)
        {
            ya = -a;
            yya = -da;
            sy = -1;
        }
        else
        {
            ya = a;
            yya = da;
            sy = 1;
        }

        /* (VI) The case 0.787 < abs(x) <= 25,    0 < abs(y) <= 0.0608 */
        if (ya <= gy2)
        {
            a2 = a * a;
            t2 = d9 + a2 * d11;
            t2 = d7 + a2 * t2;
            t2 = d5 + a2 * t2;
            t2 = d3 + a2 * t2;
            t2 = da + a * a2 * t2;

            if (n)
            {
                /* -cot */
                EADD(&a, &t2, &b, &db);
                __Float64 I = 1.0, O = 0.0;
                DIV2(&I, &O, &b, &db, &c, &dc, &t1, &t2, &t3, &t4);
                y = c + dc;
                return (-y);
                /* Max ULP is 0.506.  */
            }
            else
            {
                /* tan */
                return a + t2;
                /* Max ULP is 0.506.  */
            }
        }

        /* (VII) The case 0.787 < abs(x) <= 25,    0.0608 < abs(y) <= 0.787 */

        i = ((int)(mfftnhf + 256 * ya));
        z = (z0 = (ya - __IBM_TANF64_XFGTBL[i][0].x)) + yya;
        z2 = z * z;
        pz = z + z * z2 * (e0 + z2 * e1);
        fi = __IBM_TANF64_XFGTBL[i][1];
        gi = __IBM_TANF64_XFGTBL[i][2];

        if (n)
        {
            /* -cot */
            t2 = pz * (fi + gi) / (fi + pz);
            y = gi - t2;
            return (-sy * y);
            /* Max ULP is 0.62.  */
        }
        else
        {
            /* tan */
            t2 = pz * (gi + fi) / (gi - pz);
            y = fi + t2;
            return (sy * y);
            /* Max ULP is 0.62.  */
        }
    }

    /* (---) The case 25 < abs(x) <= 1e8 */
    if (w <= g5)
    {
        /* Range reduction by algorithm ii */
        t = (x * hpinv + toint);
        xn = t - toint;
        v = t;
        t1 = (x - xn * mp1) - xn * mp2;
        n = v.parts.lsw & 0x00000001;
        da = xn * pp3;
        t = t1 - da;
        da = (t1 - t) - da;
        t1 = xn * pp4;
        a = t - t1;
        da = ((t - a) - t1) + da;
        EADD(&a, &da, &t1, &t2);
        a = t1;
        da = t2;
        if (a < 0.0)
        {
            ya = -a;
            yya = -da;
            sy = -1;
        }
        else
        {
            ya = a;
            yya = da;
            sy = 1;
        }

        /* (VIII) The case 25 < abs(x) <= 1e8,    0 < abs(y) <= 0.0608 */
        if (ya <= gy2)
        {
            a2 = a * a;
            t2 = d9 + a2 * d11;
            t2 = d7 + a2 * t2;
            t2 = d5 + a2 * t2;
            t2 = d3 + a2 * t2;
            t2 = da + a * a2 * t2;

            if (n)
            {
                /* -cot */
                EADD(&a, &t2, &b, &db);
                __Float64 I = 1.0, O = 0.0;
                DIV2(&I, &O, &b, &db, &c, &dc, &t1, &t2, &t3, &t4);
                y = c + dc;
                return (-y);
            }
            else
            {
                /* tan */
                return a + t2;
                /* Max ULP is 0.506.  */
            }
        }

        /* (IX) The case 25 < abs(x) <= 1e8,    0.0608 < abs(y) <= 0.787 */
        i = ((int)(mfftnhf + 256 * ya));
        z = (z0 = (ya - __IBM_TANF64_XFGTBL[i][0].x)) + yya;
        z2 = z * z;
        pz = z + z * z2 * (e0 + z2 * e1);
        fi = __IBM_TANF64_XFGTBL[i][1];
        gi = __IBM_TANF64_XFGTBL[i][2];

        if (n)
        {
            /* -cot */
            t2 = pz * (fi + gi) / (fi + pz);
            y = gi - t2;
            return (-sy * y);
            /* Max ULP is 0.62.  */
        }
        else
        {
            /* tan */
            t2 = pz * (gi + fi) / (gi - pz);
            y = fi + t2;
            return (sy * y);
            /* Max ULP is 0.62.  */
        }
    }

    /* (---) The case 1e8 < abs(x) < 2**1024 */
    /* Range reduction by algorithm iii */
    n = (__IBM_branred(x, &a, &da)) & 0x00000001;
    EADD(&a, &da, &t1, &t2);
    a = t1;
    da = t2;
    if (a < 0.0)
    {
        ya = -a;
        yya = -da;
        sy = -1;
    }
    else
    {
        ya = a;
        yya = da;
        sy = 1;
    }

    /* (X) The case 1e8 < abs(x) < 2**1024,    0 < abs(y) <= 0.0608 */
    if (ya <= gy2)
    {
        a2 = a * a;
        t2 = d9 + a2 * d11;
        t2 = d7 + a2 * t2;
        t2 = d5 + a2 * t2;
        t2 = d3 + a2 * t2;
        t2 = da + a * a2 * t2;
        if (n)
        {
            /* -cot */
            EADD(&a, &t2, &b, &db);
            __Float64 I = 1.0, O = 0.0;
            DIV2(&I, &O, &b, &db, &c, &dc, &t1, &t2, &t3, &t4);
            y = c + dc;
            return (-y);
            /* Max ULP is 0.506.  */
        }
        else
        {
            /* tan */
            return a + t2;
            /* Max ULP is 0.507.  */
        }
    }

    /* (XI) The case 1e8 < abs(x) < 2**1024,    0.0608 < abs(y) <= 0.787 */
    i = ((int)(mfftnhf + 256 * ya));
    z = (z0 = (ya - __IBM_TANF64_XFGTBL[i][0].x)) + yya;
    z2 = z * z;
    pz = z + z * z2 * (e0 + z2 * e1);
    fi = __IBM_TANF64_XFGTBL[i][1];
    gi = __IBM_TANF64_XFGTBL[i][2];

    if (n)
    {
        /* -cot */
        t2 = pz * (fi + gi) / (fi + pz);
        y = gi - t2;
        return (-sy * y);
        /* Max ULP is 0.62.  */
    }
    else
    {
        /* tan */
        t2 = pz * (gi + fi) / (gi - pz);
        y = fi + t2;
        return (sy * y);
        /* Max ULP is 0.62.  */
    }

    return __Float64::FromBytes(BIG_NAN_DOUBLE);
}

complex64 __cdecl __GLIBCT_TAN64C(complex64 _X)
{
    float64 XReal = _X.real(), XImag = _X.imag();
    float64 YReal, YImag;
    int RClass = std::fpclassify(XReal);
    int IClass = std::fpclassify(XImag);

    if (RClass < FP_ZERO || IClass < FP_ZERO)
    {
        if (IClass == FP_INFINITE)
        {
            if (RClass >= FP_ZERO && abs(XReal) > 1)
            {
                float64 sinrx, cosrx;
                sinrx = __IBM_SIN64F(XReal);
                cosrx = __IBM_COS64F(XReal);
                YReal = ::copysign(0, sinrx * cosrx);
            }
            else {YReal = ::copysign(0, XReal);}
            YImag = ::copysign(1, XImag);
        }
        else if (RClass == FP_ZERO)
        {
            YReal = XReal;
            YImag = XImag;
        }
        else
        {
            YReal = __Float64::FromBytes(BIG_NAN_DOUBLE);
            if (IClass == FP_ZERO) {YImag = XImag;}
            else {YImag = __Float64::FromBytes(BIG_NAN_DOUBLE);}
        }
    }
    else
    {
        float64 sinrx, cosrx;
        float64 den;
        const int t = (int) ((DBL_MAX_EXP - 1) * CSE_LN2 / 2);

        // tan(x+iy) = (sin(2x) + i*sinh(2y))/(cos(2x) + cosh(2y))
        // = (sin(x)*cos(x) + i*sinh(y)*cosh(y)/(cos(x)^2 + sinh(y)^2).

        if (abs(XReal) > DBL_MIN)
        {
            sinrx = __IBM_SIN64F(XReal);
            cosrx = __IBM_COS64F(XReal);
        }
        else
        {
            sinrx = XReal;
            cosrx = 1;
        }

        if (abs(XImag) > t)
        {
            /* Avoid intermediate overflow when the real part of the
               result may be subnormal.  Ignoring negligible terms, the
               imaginary part is +/- 1, the real part is
               sin(x)*cos(x)/sinh(y)^2 = 4*sin(x)*cos(x)/exp(2y).  */
            float64 exp_2t = __IEEE754_EXP64F(2 * t);

            YImag = ::copysign(1, XImag);
            YReal = 4 * sinrx * cosrx;
            XImag = abs(XImag);
            XImag -= t;
            YReal /= exp_2t;
            if (XImag > t)
            {
                /* Underflow (original imaginary part of x has absolute
                   value > 2t).  */
                YReal /= exp_2t;
            }
            else {YReal /= __IEEE754_EXP64F(2 * XImag);}
        }
        else
        {
            float64 sinhix, coshix;
            if (abs(XImag) > DBL_MIN)
            {
                sinhix = sinh(XImag);
                coshix = cosh(XImag);
            }
            else
            {
                sinhix = XImag;
                coshix = 1;
            }

            if (abs(sinhix) > abs(cosrx) * DOUBLE_EPSILON)
            {
                den = cosrx * cosrx + sinhix * sinhix;
            }
            else {den = cosrx * cosrx;}
            YReal = sinrx * cosrx / den;
            YImag = sinhix * coshix / den;
        }
    }

    return {YReal, YImag};
}

_END_EXTERN_C

_CSE_END

#if 0 // Test program for complex tan generated by Deepseek
#include <iostream>
#include <iostream>
#include <complex>
#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>
#include <algorithm>

#include <CSE/Base.h>

using namespace cse;

// 辅助函数，用于比较和打印结果
void compare_tan_results(complex64 input)
{
    // 使用您的实现
    complex64 custom_result = __GLIBCT_TAN64C(input);

    // 使用标准库实现
    std::complex<double> std_input(input.real(), input.imag());
    std::complex<double> std_result = tan(std_input);

    // 计算差异
    double real_diff = abs(custom_result.real() - std_result.real());
    double imag_diff = abs(custom_result.imag() - std_result.imag());

    // 打印结果
    std::cout << std::scientific << std::setprecision(15);
    std::cout << "Input: (" << input.real() << ", " << input.imag() << ")\n";
    std::cout << "Custom tan: (" << custom_result.real() << ", " << custom_result.imag() << ")\n";
    std::cout << "Std   tan: (" << std_result.real() << ", " << std_result.imag() << ")\n";
    std::cout << "Real diff: " << real_diff << ", Imag diff: " << imag_diff << "\n";
    std::cout << "--------------------------------------------------\n";
}

int main()
{
    // 定义测试用例
    std::vector<complex64> test_cases =
    {
        // 常规情况
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {M_PI/4, M_PI/4},

        // 边界情况
        {std::numeric_limits<float>::max(), 0.0f},
        {0.0f, std::numeric_limits<float>::max()},
        {std::numeric_limits<float>::min(), 0.0f},
        {0.0f, std::numeric_limits<float>::min()},

        // 特殊值
        {std::numeric_limits<float>::infinity(), 0.0f},
        {0.0f, std::numeric_limits<float>::infinity()},
        {-std::numeric_limits<float>::infinity(), 0.0f},
        {0.0f, -std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::quiet_NaN(), 0.0f},
        {0.0f, std::numeric_limits<float>::quiet_NaN()},

        // 大值和小值组合
        {1e20f, 1e-20f},
        {1e-20f, 1e20f},

        // 接近π/2的值(正切函数的奇点)
        {M_PI/2 - 1e-6f, 0.0f},
        {M_PI/2 + 1e-6f, 0.0f},

        // 随机测试用例
        {0.5f, 0.5f},
        {-0.5f, -0.5f},
        {2.0f, 3.0f},
        {-2.0f, 3.0f},
        {2.0f, -3.0f},
        {-2.0f, -3.0f}
    };

    // 运行所有测试用例
    for (const auto& test_case : test_cases)
    {
        compare_tan_results(test_case);
    }

    return 0;
}
#endif
