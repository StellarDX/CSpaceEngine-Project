#include "CSE/SCStream/StelCXXRes/CodeCvt.h"
#include <cstring>

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

std::wstring __StelCXX_Text_Codecvt_65001::ConvertToUnicode(std::string in, int* state) const
{
    auto istr = in.c_str();
    wchar_t* wc = (wchar_t*)malloc(in.size() * 6);
    memset(wc, 0, in.size() * 6);
    uint8_t* s = (uint8_t*)istr;
    wchar_t* pwc = wc;

    // Encoder based on GNU IConv
    for (int i = 0; i < in.size();)
    {
        uint8_t c = s[i];

        if (c < 0x80)
        {
            *pwc = c;
            i += 1;
        }
        else if (c < 0xc2)
        {
            *state = RET_ILSEQ;
            break;
        }
        else if (c < 0xe0)
        {
            if (i + 1 >= in.size())
            {
                *state = RET_TOOFEW(0);
                break;
            }
            if (!((s[i + 1] ^ 0x80) < 0x40))
            {
                *state = RET_ILSEQ;
                break;
            }
            *pwc = (wchar_t(c & 0x1f) << 6)
                   | wchar_t(s[i + 1] ^ 0x80);
            i += 2;
        }
        else if (c < 0xf0)
        {
            if (i + 2 >= in.size())
            {
                *state = RET_TOOFEW(0);
                break;
            }
            if (!((s[i + 1] ^ 0x80) < 0x40 && (s[i + 2] ^ 0x80) < 0x40
                  && (c >= 0xe1 || s[i + 1] >= 0xa0)
                  && (c != 0xed || s[i + 1] < 0xa0)))
            {
                *state = RET_ILSEQ;
                break;
            }
            *pwc = (wchar_t(c & 0x0f) << 12)
                   | (wchar_t(s[i + 1] ^ 0x80) << 6)
                   | wchar_t(s[i + 2] ^ 0x80);
            i += 3;
        }
        // The following convertion below is out of wchar_t's size.
        else if (c < 0xf8)
        {
            if (i + 3 >= in.size())
            {
                *state = RET_TOOFEW(0);
                break;
            }
            if (!((s[i + 1] ^ 0x80) < 0x40 && (s[i + 2] ^ 0x80) < 0x40
                  && (s[i + 3] ^ 0x80) < 0x40
                  && (c >= 0xf1 || s[i + 1] >= 0x90)
                  && (c < 0xf4 || (c == 0xf4 && s[i + 1] < 0x90))))
            {
                *state = RET_ILSEQ;
                break;
            }

            uint32_t ucs4 = (wchar_t(c & 0x07) << 18)
                   | (wchar_t(s[i + 1] ^ 0x80) << 12)
                   | (wchar_t(s[i + 2] ^ 0x80) << 6)
                   | wchar_t(s[i + 3] ^ 0x80);
            ucs4 -= 0x10000;
            uint16_t hi = uint16_t((ucs4 >> 10) | (0b110110 << 10));
            uint16_t lo = uint16_t((ucs4 & 0b1111111111) | (0b110111 << 10));
            *pwc = hi;
            *(++pwc) = lo;
            i += 4;
        }
        // Addition convertion from FLTK
        /*else if (c < 0xfc)
        {
            if (i + 4 >= in.size())
            {
                *state = RET_TOOFEW(0);
                break;
            }
            if (!((s[i + 1] ^ 0x80) < 0x40 && (s[i + 2] ^ 0x80) < 0x40
                  && (s[i + 3] ^ 0x80) < 0x40 && (s[i + 4] ^ 0x80) < 0x40
                  && (c >= 0xf9 || s[i + 1] >= 0x88)))
            {
                *state = RET_ILSEQ;
                break;
            }
            *pwc = (wchar_t(c & 0x03) << 24)
                   | (wchar_t(s[1] ^ 0x80) << 18)
                   | (wchar_t(s[2] ^ 0x80) << 12)
                   | (wchar_t(s[3] ^ 0x80) << 6)
                   | wchar_t(s[4] ^ 0x80);
            i += 5;
        }
        else if (c < 0xfe)
        {
            if (i + 5 >= in.size())
            {
                *state = RET_TOOFEW(0);
                break;
            }
            if (!((s[i + 1] ^ 0x80) < 0x40 && (s[i + 2] ^ 0x80) < 0x40
                  && (s[i + 3] ^ 0x80) < 0x40 && (s[i + 4] ^ 0x80) < 0x40
                  && (s[i + 5] ^ 0x80) < 0x40
                  && (c >= 0xfd || s[i + 1] >= 0x84)))
            {
                *state = RET_ILSEQ;
                break;
            }
            *pwc = (wchar_t(c & 0x01) << 30)
                   | (wchar_t(s[i + 1] ^ 0x80) << 24)
                   | (wchar_t(s[i + 2] ^ 0x80) << 18)
                   | (wchar_t(s[i + 3] ^ 0x80) << 12)
                   | (wchar_t(s[i + 4] ^ 0x80) << 6)
                   | wchar_t(s[i + 5] ^ 0x80);
            i += 6;
        }*/
        else
        {
            *state = RET_ILSEQ;
            break;
        }
        ++pwc;
    }

    std::wstring wstr = wc;
    free(wc);
    return wstr;
}

std::string __StelCXX_Text_Codecvt_65001::ConvertFromUnicode(std::wstring in, int* state) const
{
    auto istr = in.c_str();
    char* cstr = (char*)malloc(in.size() * 6);
    memset(cstr, 0, in.size() * 6);
    uint16_t* pwc = (uint16_t*)istr;
    char* r = cstr;

    for (int i = 0; i < in.size(); ++i)
    {
        uint16_t wc = pwc[i];
        int count;
        if (wc < 0x80) {count = 1;}
        else if (wc < 0x800) {count = 2;}
        else if (wc < 0x10000)
        {
            if (wc < 0xd800 || wc >= 0xe000) {count = 3;}
            else {count = 4;}
        }
        /*else if (wc < 0x200000) {count = 4;}
        else if (wc < 0x4000000) {count = 5;}
        else if (wc <= 0x7fffffff) {count = 6;}*/
        else
        {
            *state = RET_ILUNI;
            break;
        }

        switch (count)
        {
        case 4:
        {
            if (i + 1 >= in.size())
            {
                *state = RET_ILSEQ;
                break;
            }
            uint16_t hi = wc & 0b1111111111;
            uint16_t lo = pwc[i + 1] & 0b1111111111;
            uint32_t ucs4 = uint32_t(lo) | (uint32_t(hi) << 10);
            ucs4 += 0x10000;

            r[3] = 0x80 | (ucs4 & 0x3f);
            ucs4 = (ucs4 >> 6) | 0x10000;
            r[2] = 0x80 | (ucs4 & 0x3f);
            ucs4 = (ucs4 >> 6) | 0x800;
            r[1] = 0x80 | (ucs4 & 0x3f);
            ucs4 = (ucs4 >> 6) | 0xc0;
            r[0] = ucs4;

            ++i;
        }
        break;
        case 3:
            r[2] = 0x80 | (wc & 0x3f);
            wc = (wc >> 6) | 0x800;
        case 2:
            r[1] = 0x80 | (wc & 0x3f);
            wc = (wc >> 6) | 0xc0;
        case 1:
            r[0] = wc;
        }
        r += count;
    }

    std::string str = cstr;
    free(cstr);
    return str;
}
