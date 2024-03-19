/*
    Copyright (C) StellarDX Astronomy.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <https://www.gnu.org/licenses/>.
*/

/*
    Encoding rule of UTF-8
    +---+-----------------------+-------------------------------------------------------+
    | n |  Unicode Range        |  UTF-8 Encoding                                       |
    +---+-----------------------+-------------------------------------------------------+
    | 1 | 0000 0000 - 0000 007F |                                              0xxxxxxx |
    | 2 | 0000 0080 - 0000 07FF |                                     110xxxxx 10xxxxxx |
    | 3 | 0000 0800 - 0000 FFFF |                            1110xxxx 10xxxxxx 10xxxxxx |
    | 4 | 0001 0000 - 0010 FFFF |                   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx |
    | 5 | 0020 0000 - 03FF FFFF |          111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx |
    | 6 | 0400 0000 - 7FFF FFFF | 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx |
    +---+-----------------------+-------------------------------------------------------+
    Reference: tge7618291 https://blog.csdn.net/tge7618291/article/details/7599902
*/

#include "CSE/Base/StelCXXRes/StelCXX-UniString.h"

_EXTERN_C

int __Decoder_65001_16(const char* istr, ucs2_t* ostr, size_t size)
{
    uint8_t* s = (uint8_t*)istr; // Reload string as byte array
    for (size_t i = 0; i < size; )
    {
        uint8_t c = s[i];
        if (c < 0b10000000) // Single-byte characters
        {
            *ostr = c;
            ++i;
        }
        else if (c < 0b11000010) {return DCRET_ILSEQ;} // Characters begin with 0x80 - 0xC1 is invalid
        else if (c < 0b11100000) // Double-byte characters
        {
            if (i + 1 >= size) {return DCRET_TOOFEW(0);}
            // Second or following bytes strictly begin with 0b10
            if (!((s[i + 1] ^ 0b10000000) < 0b01000000)) {return DCRET_ILSEQ;}
            *ostr = (ucs2_t(c & 0b00011111) << 6)
                    | ucs2_t(s[i + 1] ^ 0b10000000);
            i += 2;
        }
        else if (c < 0b11110000) // Triple-byte characters
        {
            if (i + 2 >= size) {return DCRET_TOOFEW(0);}
            if (!((s[i + 1] ^ 0b10000000) < 0b01000000 && (s[i + 2] ^ 0b10000000) < 0b01000000
                  && (c >= 0b11100001 || s[i + 1] >= 0b10100000)
                  && (c != 0b11101101 || s[i + 1] < 0b10100000)))
            {
                return DCRET_ILSEQ;
            }
            *ostr = (ucs2_t(c & 0b00001111) << 12)
                    | (ucs2_t(s[i + 1] ^ 0b10000000) << 6)
                    | ucs2_t(s[i + 2] ^ 0b10000000);
            i += 3;
        }
        // The following convertion below is out of ucs2_t's size.
        else if (c < 0b11111000) // Quaduple-byte characters (Special code method prooduces double UCS-2 characters)
        {
            if (i + 3 >= size) {return DCRET_TOOFEW(0);}
            if (!((s[i + 1] ^ 0b10000000) < 0b01000000 && (s[i + 2] ^ 0b10000000) < 0b01000000
                  && (s[i + 3] ^ 0b10000000) < 0b01000000
                  && (c >= 0b11110001 || s[i + 1] >= 0b10010000)
                  && (c < 0b11110100 || (c == 0b11110100 && s[i + 1] < 0b10010000))))
            {
                return DCRET_ILSEQ;
            }
            // Convert this character to original unicode value first
            uint32_t LFE = (wchar_t(c & 0b00000111) << 18)
                           | (wchar_t(s[i + 1] ^ 0b10000000) << 12)
                           | (wchar_t(s[i + 2] ^ 0b10000000) << 6)
                           | wchar_t(s[i + 3] ^ 0b10000000);
            LFE -= 0x10000; // Adjust the range into 0x00000 - 0xFFFFF
            // Then cut it into higher 10 bits and lower 10 bits,
            // and add 0xD800 and 0xDC00 marker to the front.
            uint16_t LSg = uint16_t((LFE & 0b1111111111) | 0xDC00);
            uint16_t HSg = uint16_t((LFE >> 10) | 0xD800);
            // Decoding complete, write them into the output string.
            *ostr = HSg;
            *(++ostr) = LSg;
            i += 4;
        }
        else {return DCRET_ILSEQ;}
        ++ostr;
    }
    return 0;
}

int __Encoder_65001_16(const ucs2_t* istr, char* ostr, size_t size)
{
    uint16_t* pwc = (uint16_t*)istr;
    for (size_t i = 0; i < size; ++i)
    {
        uint16_t wc = pwc[i];

        // Load count
        int count;
        if (wc < 0x80) {count = 1;}
        else if (wc < 0x800) {count = 2;}
        else if (wc < 0x10000)
        {
            if (wc < 0xD800 || wc >= 0xE000) {count = 3;}
            else {count = 4;}
        }
        else{return DCRET_ILUNI;}

        switch (count)
        {
        case 4:
        {
            if (i + 1 >= size) {return DCRET_ILSEQ;}
            uint16_t hi = wc & 0b1111111111;
            uint16_t lo = pwc[i + 1] & 0b1111111111;
            uint32_t ucs4 = uint32_t(lo) | (uint32_t(hi) << 10);
            ucs4 += 0x10000;

            ostr[3] = 0b10000000 | (ucs4 & 0b00111111);
            ucs4 = (ucs4 >> 6) | 0x10000;
            ostr[2] = 0b10000000 | (ucs4 & 0b00111111);
            ucs4 = (ucs4 >> 6) | 0x800;
            ostr[1] = 0b10000000 | (ucs4 & 0b00111111);
            ucs4 = (ucs4 >> 6) | 0xc0;
            ostr[0] = ucs4;

            ++i;
        }
        break;
        case 3:
            ostr[2] = 0b10000000 | (wc & 0b00111111);
            wc = (wc >> 6) | 0x800;
        case 2:
            ostr[1] = 0b10000000 | (wc & 0b00111111);
            wc = (wc >> 6) | 0xc0;
        case 1:
            ostr[0] = wc;
        }
        ostr += count;
    }
    return 0;
}

_END_EXTERN_C
