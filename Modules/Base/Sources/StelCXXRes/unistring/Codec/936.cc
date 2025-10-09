/*
    GBK encoder and decoder from GNU IConv
    Copyright (C) 1999-2001, 2005, 2008, 2016 Free Software Foundation, Inc.
    Copyright (C) StellarDX Astronomy. 2025 (Adapted material)

    The GNU LIBICONV Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either version 2.1
    of the License, or (at your option) any later version.

    The GNU LIBICONV Library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with the GNU LIBICONV Library; see the file COPYING.LIB.
    If not, see <https://www.gnu.org/licenses/>.
*/

/*
 * GBK, as described in Ken Lunde's book, is an extension of GB 2312-1980
 * (shifted by adding 0x8080 to the range 0xA1A1..0xFEFE, as used in EUC-CN).
 * It adds the following ranges:
 *
 * (part of GBK/1)  0xA2A1-0xA2AA  Small Roman numerals
 * GBK/3   0x{81-A0}{40-7E,80-FE}  6080 new characters, all in Unicode
 * GBK/4   0x{AA-FE}{40-7E,80-A0}  8160 new characters, 8080 in Unicode
 * GBK/5   0x{A8-A9}{40-7E,80-A0}  166 new characters, 153 in Unicode
 *
 * Furthermore, all four tables I have looked at
 *   - the CP936 table by Microsoft, found on ftp.unicode.org in 1999,
 *   - the GBK table by Sun, investigated on a Solaris 2.7 machine,
 *   - the GBK tables by CWEX, found in the Big5+ package,
 *   - the GB18030 standard (second printing),
 * agree in the following extensions. (Ken Lunde must have overlooked these
 * differences between GB2312 and GBK. Also, the CWEX tables have additional
 * differences.)
 *
 * 1. Some characters in the GB2312 range are defined differently:
 *
 *     code    GB2312                         GBK
 *    0xA1A4   0x30FB # KATAKANA MIDDLE DOT   0x00B7 # MIDDLE DOT
 *    0xA1AA   0x2015 # HORIZONTAL BAR        0x2014 # EM DASH
 *
 * 2. 19 characters added in the range 0xA6E0-0xA6F5.
 *
 * 3. 4 characters added in the range 0xA8BB-0xA8C0.
 *
 * CP936 as of 1999 was identical to GBK. However, since 1999, Microsoft has
 * added new mappings to CP936...
 */

#include "CSE/Base/StelCXXRes/StelCXX-UniString.h"

_EXTERN_C

#include "GB2312.1980-0.tbl"
#include "CP936.tbl"
#include "GBK3.tbl"
#include "GBK4-5.tbl"

int __Decoder_936_32(const unsigned char* istr, ucs4_t* ostr, size_t size)
{
    uint8_t* s = (uint8_t*)istr;
    for (int i = 0; i < size; )
    {
        #define __decode_936_continue(sz)\
        i += sz;\
        ++ostr;\
        continue

        uint8_t c = s[i];
        if (c < 0b10000000) // Store ASCII Characters directly
        {
            *ostr = c;
            __decode_936_continue(1);
        }

        // 0x81 - 0xFF, double-byte characters
        if (c >= 0x81 && c < 0xff)
        {
            if (i + 1 >= size) {return DCRET_TOOFEW(0);}

            // GBK1 and GBK2
            if (c >= 0xa1 && c <= 0xf7)
            {
                unsigned char c2 = s[i + 1];
                if (c == 0xa1)
                {
                    if (c2 == 0xa4)
                    {
                        *ostr = 0x00b7;
                        __decode_936_continue(2);
                    }
                    else if (c2 == 0xaa)
                    {
                        *ostr = 0x2014;
                        __decode_936_continue(2);
                    }
                }

                if (c2 >= 0xa1 && c2 < 0xff)
                {
                    unsigned char buf[2];
                    int ret;
                    buf[0] = c - 0x80; buf[1] = c2 - 0x80;

                    static auto GB2312 = [](unsigned char c1, unsigned char c2, ucs4_t* out)
                    {
                        if (!((c1 >= 0x21 && c1 <= 0x29) || (c1 >= 0x30 && c1 <= 0x77))
                            || !(c2 >= 0x21 && c2 < 0x7f))
                        {
                            return DCRET_ILSEQ;
                        }
                        unsigned int i = 94 * (c1 - 0x21) + (c2 - 0x21);
                        unsigned short wc = 0xfffd;
                        if (i < 1410)
                        {
                            if (i < 831) {wc = gb2312_2uni_page21[i];}
                        }
                        else
                        {
                            if (i < 8178) {wc = gb2312_2uni_page30[i - 1410];}
                        }
                        if (wc == 0xfffd) {return DCRET_ILSEQ;}
                        *out = (ucs4_t) wc;
                        return 0;
                    };

                    ret = GB2312(buf[0], buf[1], ostr);
                    if (ret != DCRET_ILSEQ) {__decode_936_continue(2);}

                    buf[0] = c; buf[1] = c2;

                    static auto CP936 = [](unsigned char c1, unsigned char c2, ucs4_t* out)
                    {
                        if (!((c1 == 0xa6) || (c1 == 0xa8)) ||
                            !((c2 >= 0x40 && c2 < 0x7f) || (c2 >= 0x80 && c2 < 0xff)))
                        {
                            return DCRET_ILSEQ;
                        }
                        unsigned int i = 190 * (c1 - 0x81) + (c2 - (c2 >= 0x80 ? 0x41 : 0x40));
                        unsigned short wc = 0xfffd;
                        if (i < 7410)
                        {
                            if (i >= 7189 && i < 7211) {wc = cp936ext_2uni_pagea6[i - 7189];}
                        }
                        else
                        {
                            if (i >= 7532 && i < 7538) {wc = cp936ext_2uni_pagea8[i - 7532];}
                        }
                        if (wc == 0xfffd) {return DCRET_ILSEQ;}
                        *out = (ucs4_t) wc;
                        return 0;
                    };

                    ret = CP936(buf[0], buf[1], ostr);
                    if (ret != DCRET_ILSEQ) {__decode_936_continue(2);}
                }
            }

            // GBK3
            if (c >= 0x81 && c <= 0xa0)
            {
                static auto GBK3 = [](unsigned char c1, unsigned char c2, ucs4_t* out)
                {
                    if (!((c1 >= 0x81 && c1 <= 0xa0)) ||
                        !((c2 >= 0x40 && c2 < 0x7f) || (c2 >= 0x80 && c2 < 0xff)))
                    {
                        return DCRET_ILSEQ;
                    }
                    unsigned int i = 190 * (c1 - 0x81) + (c2 - (c2 >= 0x80 ? 0x41 : 0x40));
                    unsigned short wc = 0xfffd;
                    if (i < 6080) {wc = gbkext1_2uni_page81[i];}
                    if (wc == 0xfffd) {return DCRET_ILSEQ;}
                    *out = (ucs4_t) wc;
                    return 0;
                };
                int ret = GBK3(c, s[i + 1], ostr);
                if (ret != DCRET_ILSEQ) {__decode_936_continue(2);}
            }

            // GBK4 and GBK5
            if (c >= 0xa8 && c <= 0xfe)
            {
                static auto GBK4_5 = [](unsigned char c1, unsigned char c2, ucs4_t* out)
                {
                    if (!(c1 >= 0xa8 && c1 <= 0xfe) ||
                        !((c2 >= 0x40 && c2 < 0x7f) || (c2 >= 0x80 && c2 < 0xa1)))
                    {
                        return DCRET_ILSEQ;
                    }
                    unsigned int i = 96 * (c1 - 0x81) + (c2 - (c2 >= 0x80 ? 0x41 : 0x40));
                    unsigned short wc = 0xfffd;
                    if (i < 12016) {wc = gbkext2_2uni_pagea8[i - 3744];}
                    if (wc == 0xfffd) {return DCRET_ILSEQ;}
                    *out = (ucs4_t) wc;
                    return 0;
                };
                int ret = GBK4_5(c, s[i + 1], ostr);
                if (ret != DCRET_ILSEQ) {__decode_936_continue(2);}
            }

            if (c == 0xa2)
            {
                unsigned char c2 = s[1];
                if (c2 >= 0xa1 && c2 <= 0xaa)
                {
                    *ostr = 0x2170 + (c2 - 0xa1);
                    __decode_936_continue(2);
                }
            }
        }

        return DCRET_ILSEQ;
        #undef __decode_936_continue
    }
    return 0;
}

int __Encoder_936_32(const ucs4_t* istr, unsigned char* ostr, size_t size)
{
    uint32_t* pwc = (uint32_t*)istr;
    for (size_t i = 0; i < size; )
    {
        #define __encode_936_continue(sz)\
        ++i;\
        ostr += sz;\
        continue

        ucs4_t wc = pwc[i];
        unsigned char buf[2];
        int ret;

        if (wc < 0b10000000) // Store ASCII Characters directly
        {
            *ostr = (unsigned char)(wc);
            __encode_936_continue(1);
        }

        // GB2312
        if (wc != 0x30fb && wc != 0x2015)
        {
            static auto GB2312 = [](ucs4_t wc, unsigned char* r)
            {
                const Summary16* summary = nullptr;
                if (wc >= 0x0000 && wc < 0x0460)
                {
                    summary = gb2312_uni2indx_page00 + (wc >> 4);
                }
                else if (wc >= 0x2000 && wc < 0x2650)
                {
                    summary = gb2312_uni2indx_page20 + (wc >> 4) - 0x200;
                }
                else if (wc >= 0x3000 && wc < 0x3230)
                {
                    summary = gb2312_uni2indx_page30 + (wc >> 4) - 0x300;
                }
                else if (wc >= 0x4e00 && wc < 0x9cf0)
                {
                    summary = gb2312_uni2indx_page4e + (wc >> 4) - 0x4e0;
                }
                else if (wc >= 0x9e00 && wc < 0x9fb0)
                {
                    summary = gb2312_uni2indx_page9e + (wc >> 4) - 0x9e0;
                }
                else if (wc >= 0xff00 && wc < 0xfff0)
                {
                    summary = gb2312_uni2indx_pageff + (wc >> 4) - 0xff0;
                }
                if (summary)
                {
                    unsigned short used = summary->used;
                    unsigned int i = wc & 0x0f;
                    if (used & ((unsigned short) 1 << i))
                    {
                        unsigned short c;
                        /* Keep in 'used' only the bits 0..i-1. */
                        used &= ((unsigned short) 1 << i) - 1;
                        /* Add 'summary->indx' and the number of bits set in 'used'. */
                        used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
                        used = (used & 0x3333) + ((used & 0xcccc) >> 2);
                        used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
                        used = (used & 0x00ff) + (used >> 8);
                        c = gb2312_2charset[summary->indx + used];
                        r[0] = (c >> 8);
                        r[1] = (c & 0xff);
                        return 0;
                    }
                }
                return DCRET_ILUNI;
            };

            ret = GB2312(wc, buf);
            if (ret != DCRET_ILUNI)
            {
                ostr[0] = buf[0] + 0x80;
                ostr[1] = buf[1] + 0x80;
                __encode_936_continue(2);
            }
        }

        // GBK3 to GBK5
        static auto GBK = [](ucs4_t wc, unsigned char* r)
        {
            const Summary16 *summary = NULL;
            if (wc >= 0x0200 && wc < 0x02e0)
            {
                summary = gbkext_inv_uni2indx_page02 + (wc >> 4) - 0x020;
            }
            else if (wc >= 0x2000 && wc < 0x22c0)
            {
                summary = gbkext_inv_uni2indx_page20 + (wc >> 4) - 0x200;
            }
            else if (wc >= 0x2500 && wc < 0x2610)
            {
                summary = gbkext_inv_uni2indx_page25 + (wc >> 4) - 0x250;
            }
            else if (wc >= 0x3000 && wc < 0x3100)
            {
                summary = gbkext_inv_uni2indx_page30 + (wc >> 4) - 0x300;
            }
            else if (wc >= 0x3200 && wc < 0x33e0)
            {
                summary = gbkext_inv_uni2indx_page32 + (wc >> 4) - 0x320;
            }
            else if (wc >= 0x4e00 && wc < 0x9fb0)
            {
                summary = gbkext_inv_uni2indx_page4e + (wc >> 4) - 0x4e0;
            }
            else if (wc >= 0xf900 && wc < 0xfa30)
            {
                summary = gbkext_inv_uni2indx_pagef9 + (wc >> 4) - 0xf90;
            }
            else if (wc >= 0xfe00 && wc < 0xfff0)
            {
                summary = gbkext_inv_uni2indx_pagefe + (wc >> 4) - 0xfe0;
            }
            if (summary)
            {
                unsigned short used = summary->used;
                unsigned int i = wc & 0x0f;
                if (used & ((unsigned short) 1 << i))
                {
                    unsigned short c;
                    /* Keep in 'used' only the bits 0..i-1. */
                    used &= ((unsigned short) 1 << i) - 1;
                    /* Add 'summary->indx' and the number of bits set in 'used'. */
                    used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
                    used = (used & 0x3333) + ((used & 0xcccc) >> 2);
                    used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
                    used = (used & 0x00ff) + (used >> 8);
                    c = gbkext_inv_2charset[summary->indx + used];
                    r[0] = (c >> 8); r[1] = (c & 0xff);
                    return 0;
                }
            }
            return DCRET_ILUNI;
        };
        ret = GBK(wc, buf);
        if (ret != DCRET_ILUNI)
        {
            ostr[0] = buf[0];
            ostr[1] = buf[1];
            __encode_936_continue(2);
        }

        if (wc >= 0x2170 && wc <= 0x2179)
        {
            ostr[0] = 0xa2;
            ostr[1] = 0xa1 + (wc - 0x2170);
            __encode_936_continue(2);
        }

        // CP936
        static auto CP936 = [](ucs4_t wc, unsigned char* r)
        {
            unsigned short c = 0;
            if (wc >= 0x0140 && wc < 0x0150)
            {
                c = cp936ext_page01[wc - 0x0140];
            }
            else if (wc >= 0x0250 && wc < 0x0268)
            {
                c = cp936ext_page02[wc - 0x0250];
            }
            else if (wc >= 0xfe30 && wc < 0xfe48)
            {
                c = cp936ext_pagefe[wc - 0xfe30];
            }
            if (c != 0)
            {
                r[0] = (c >> 8);
                r[1] = (c & 0xff);
                return 0;
            }
            return DCRET_ILUNI;
        };
        ret = CP936(wc, buf);
        if (ret != DCRET_ILUNI)
        {
            ostr[0] = buf[0];
            ostr[1] = buf[1];
            __encode_936_continue(2);
        }

        if (wc == 0x00b7)
        {
            ostr[0] = 0xa1;
            ostr[1] = 0xa4;
            __encode_936_continue(2);
        }

        if (wc == 0x2014)
        {
            ostr[0] = 0xa1;
            ostr[1] = 0xaa;
            __encode_936_continue(2);
        }

        return DCRET_ILUNI;
        #undef __encode_936_continue
    }
    return 0;
}

_END_EXTERN_C
