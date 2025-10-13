/*
    U16 to U32 encoder and decoder from GNU IConv
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
    Here we accept FFFE/FEFF marks as endianness indicators everywhere
    in the stream, not just at the beginning. (This is contrary to what
    RFC 2781 section 3.2 specifies, but it allows concatenation of byte
    sequences to work flawlessly, while disagreeing with the RFC behaviour
    only for strings containing U+FEFF characters, which is quite rare.)
    The default is big-endian.
    The state(Reverse) is 0 if big-endian, 1 if little-endian.
    Reference:
    [1] Hoffman P , Yergeau F .UTF-16, an encoding of ISO 10646[J].RFC
        Editor, 2000.
*/

#include "CSE/Base/StelCXXRes/StelCXX-UniString.h"
#include <climits>

_EXTERN_C

#define RET_COUNT_MAX       ((INT_MAX / 2) - 1) // 0x3FFFFFFE

int __Decoder_1200_32(const void* istr, ucs4_t* ostr, size_t size, bool Reverse)
{
    int count = 0;
    while (size >= 2 && count < size)
    {
        const unsigned char* s = (const unsigned char*)istr + count;
        ucs4_t wc = (Reverse ? s[0] + (s[1] << 8) : (s[0] << 8) + s[1]);
        // BOM header
        if (wc == 0xfeff) {Reverse = 0;}
        else if (wc == 0xfffe) {Reverse = 1;}
        // \U00010000 to \U0010FFFF
        else if (wc >= 0xd800 && wc < 0xdc00)
        {
            if (count + 4 < size)
            {
                ucs4_t wc2 = (Reverse ? s[2] + (s[3] << 8) : (s[2] << 8) + s[3]);
                if (!(wc2 >= 0xdc00 && wc2 < 0xe000))
                {
                    return DCRET_SHIFT_ILSEQ(count);
                }
                *ostr = 0x10000 + ((wc - 0xd800) << 10) + (wc2 - 0xdc00);
                count += 4;
                ++ostr;
                continue;
            }
            else {break;}
        }
        else if (wc >= 0xdc00 && wc < 0xe000)
        {
            return DCRET_SHIFT_ILSEQ(count);
        }
        // Other characters
        else
        {
            *ostr = wc;
            count += 2;
            ++ostr;
            continue;
        }
        count += 2;
    }
    if (count != size) {return DCRET_TOOFEW(count);}
    return 0;
}

int __Encoder_1200_32(const ucs4_t* istr, void* ostr, size_t size, bool Reverse)
{
    uint32_t* pwc = (uint32_t*)istr;
    unsigned char* r = (unsigned char*)ostr;
    for (size_t i = 0; i < size; )
    {
        uint32_t wc = pwc[i];
        if (wc != 0xfffe && !(wc >= 0xd800 && wc < 0xe000))
        {
            if (wc < 0x10000)
            {
                r[0] = (unsigned char) (wc >> 8);
                r[1] = (unsigned char) wc;
                ++i;
                r += 2;
                continue;
            }
            else if (wc < 0x110000)
            {
                ucs4_t wc1 = 0xd800 + ((wc - 0x10000) >> 10);
                ucs4_t wc2 = 0xdc00 + ((wc - 0x10000) & 0x3ff);
                r[0] = (unsigned char) (wc1 >> 8);
                r[1] = (unsigned char) wc1;
                r[2] = (unsigned char) (wc2 >> 8);
                r[3] = (unsigned char) wc2;
                ++i;
                r += 4;
                continue;
            }
        }
        return DCRET_ILUNI;
    }
    return 0;
}

_END_EXTERN_C
