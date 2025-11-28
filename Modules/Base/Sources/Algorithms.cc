/************************************************************
  CSpaceEngine Internal Algorithms.
***********************************************************/

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/MathFuncs.h"

_CSE_BEGIN

/****************************************************************************************\
*                                      fast math                                         *
\****************************************************************************************/

////////////////////////////////////// ABS /////////////////////////////////////

_NODISCARD float64 abs(float64 _Xx)noexcept
{
    return (_Xx >= 0) ? _Xx : -_Xx;
}

_NODISCARD int64 abs(int64 _Xx)noexcept
{
    return (_Xx >= 0) ? _Xx : -_Xx;
}

_NODISCARD float64 abs(complex64 _Xx)noexcept
{
    return _CSE sqrt(_Xx.real() * _Xx.real() + _Xx.imag() * _Xx.imag());
}

////////////////////////////////////// SGN /////////////////////////////////////

_NODISCARD float64 sgn(float64 _Xx)noexcept
{
    if (_Xx == 0) { return 0; }
    return _Xx / _CSE abs(_Xx);
}

_NODISCARD int64 sgn(int64 _Xx)noexcept
{
    if (_Xx == 0) { return 0; }
    return _Xx / _CSE abs(_Xx);
}

////////////////////////////////////// FLOOR /////////////////////////////////////

_NODISCARD float64 floor(float64 _Xx)noexcept
{
    /* Use GNU implementation.  */
    int64_t i0 = __Float64(_Xx).Bytes;
    int32_t j0 = ((i0 >> 52) & 0x7ff) - 0x3ff;
    if (j0 < 52)
    {
        if (j0 < 0)
        {
            /* return 0 * sign (x) if |x| < 1  */
            if (i0 >= 0) {i0 = 0;}
            else if ((i0 & 0x7fffffffffffffffll) != 0) {i0 = 0xbff0000000000000ll;}
        }
        else
        {
            uint64_t i = 0x000fffffffffffffll >> j0;
            if ((i0 & i) == 0) {return _Xx;} /* x is integral */
            if (i0 < 0)
                i0 += 0x0010000000000000ll >> j0;
            i0 &= ~i;
        }
        _Xx = __Float64::FromBytes(i0);
    }
    else if (j0 == 0x400) {return _Xx;}	/* inf or NaN */
    return _Xx;
}

////////////////////////////////////// CEIL /////////////////////////////////////

_NODISCARD float64 ceil(float64 _Xx)noexcept
{
    /* Use GNU implementation.  */
    int64_t i0 = __Float64(_Xx).Bytes, i;
    int32_t j0;
    j0 = ((i0 >> 52) & 0x7ff) - 0x3ff;
    if (j0 <= 51)
    {
        if (j0 < 0)
        {
            /* return 0 * sign(x) if |x| < 1  */
            if (i0 < 0) {i0 = 0x8000000000000000LL;}
            else if (i0 != 0) {i0 = 0x3ff0000000000000ll;}
        }
        else
        {
            i = INT64_C (0x000fffffffffffff) >> j0;
            if ((i0 & i) == 0) {return _Xx;} /* x is integral  */
            if (i0 > 0) {i0 += 0x0010000000000000ull >> j0;}
            i0 &= ~i;
        }
    }
    else
    {
        if (j0 == 0x400) {return _Xx;} /* inf or NaN  */
        else {return _Xx;} /* x is integral  */
    }
    _Xx = __Float64::FromBytes(i0);
    return _Xx;
}

////////////////////////////////////// FractionalPart /////////////////////////////////////

_NODISCARD float64 FractionalPart(float64 _Xx)noexcept
{
    // Using GNU implementation
    int64_t i0 = __Float64(_Xx).Bytes;
    int32_t j0;
    j0 = ((i0 >> 52) & 0x7ff) - 0x3ff; /* exponent of x */
    if(j0 < 52) /* integer part in x */
    {
        if(j0 < 0) /* |x|<1 */
        {
            /* *iptr = +-0 */
            return _Xx;
        }
        else
        {
            uint64_t i = 0x000fffffffffffffull >> j0;
            if((i0&i)==0)/* x is integral */
            {
                /* return +-0 */
                _Xx = __Float64::FromBytes(i0 & 0x8000000000000000ull);
                return _Xx;
            }
            else
            {
                __Float64 iptr = __Float64::FromBytes(i0 & (~i));
                return _Xx - iptr;
            }
        }
    }
    else /* no fraction part */
    {
        /* We must handle NaNs separately.  */
        if (j0 == 0x400 && (i0 & 0xfffffffffffffull)) {return _Xx * 1.0;}
        _Xx = __Float64::FromBytes(i0 & 0x8000000000000000ull); /* return +-0 */
        return _Xx;
    }
}

////////////////////////////////////// Fract /////////////////////////////////////

_NODISCARD float64 fract(float64 _Xx)noexcept
{
    return _Xx - floor(_Xx); // GL implementation
}

////////////////////////////////////// MOD /////////////////////////////////////

_NODISCARD float64 mod(float64 _Left, float64 _Right)noexcept
{
    __Float64 x = _Left, y = _Right;
    // Using GNU implementation
    uint64_t hx = x.Bytes;
    uint64_t hy = y.Bytes;

    uint64_t sx = hx & 0x8000000000000000ull;
    /* Get |x| and |y|.  */
    hx ^= sx;
    hy &= ~0x8000000000000000ull;

    /* If x < y, return x (unless y is a NaN).  */
    if (hx < hy)
    {
        /* If y is a NaN, return a NaN.  */
        if (hy > 0x7ff0000000000000ull) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
        return x;
    }

    int ex = hx >> 52;
    int ey = hy >> 52;
    int exp_diff = ex - ey;

    // Clz function re-implemented by StellarDX
    auto clz_uint64 = [](uint64_t x)
    {
        uint64 bitsft = 0;
        while (x)
        {
            x >>= 1;
            ++bitsft;
        }
        return 64 - bitsft;
    };

    /* Common case where exponents are close: |x/y| < 2^12, x not inf/NaN
     and |x%y| not denormal.  */
    if (ey < (0x7ff0000000000000ull >> 52) - 11 && ey > 52 && exp_diff <= 11)
    {
        uint64_t mx = (hx << 11) | 0x8000000000000000ull;
        uint64_t my = (hy << 11) | 0x8000000000000000ull;

        mx %= (my >> exp_diff);

        if (mx == 0) {return __Float64::FromBytes(sx);}
        int shift = clz_uint64(mx);
        ex -= shift + 1;
        mx <<= shift;
        mx = sx | (mx >> 11);
        return __Float64::FromBytes(mx + ((uint64_t)ex << 52));
    }

    if (hy == 0 || hx >= 0x7ff0000000000000ull)
    {
        /* If x is a NaN, return a NaN.  */
        if (hx > 0x7ff0000000000000ull) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}

        /* If x is an infinity or y is zero, return a NaN and set EDOM.  */
        return (x * y) / (x * y);
    }

    /* Special case, both x and y are denormal.  */
    if (ex == 0) {return __Float64::FromBytes(sx | hx % hy);}

    /* Extract normalized mantissas - hx is not denormal and hy != 0.  */
    auto get_mantissa = [](uint64_t x) {return x & 0x000fffffffffffffull;};
    uint64_t mx = get_mantissa (hx) | (0x000fffffffffffffull + 1);
    uint64_t my = get_mantissa (hy) | (0x000fffffffffffffull + 1);
    int lead_zeros_my = 11;

    ey--;
    /* Special case for denormal y.  */
    if (ey < 0)
    {
        my = hy;
        ey = 0;
        exp_diff--;
        lead_zeros_my = clz_uint64 (my);
    }

    // Clz function re-implemented by StellarDX
    auto ctz_uint64 = [](uint64_t x)
    {
        uint64 bitsft = 0;
        while (x)
        {
            x <<= 1;
            ++bitsft;
        }
        return 64 - bitsft;
    };

    int tail_zeros_my = ctz_uint64(my);
    int sides_zeroes = lead_zeros_my + tail_zeros_my;

    int right_shift = exp_diff < tail_zeros_my ? exp_diff : tail_zeros_my;
    my >>= right_shift;
    exp_diff -= right_shift;
    ey += right_shift;

    int left_shift = exp_diff < 11 ? exp_diff : 11;
    mx <<= left_shift;
    exp_diff -= left_shift;

    mx %= my;

    if (mx == 0) {return __Float64::FromBytes(sx);}

    auto make_double = [clz_uint64](uint64_t x, int64_t ep, uint64_t s)
    {
        int lz = clz_uint64(x) - 11;
        x <<= lz;
        ep -= lz;

        if (ep < 0 || x == 0)
        {
            x >>= -ep;
            ep = 0;
        }

        return __Float64::FromBytes(s + x + (ep << 52));
    };

    if (exp_diff == 0) {return make_double(mx, ey, sx);}

    /* Multiplication with the inverse is faster than repeated modulo.  */
    uint64_t inv_hy = UINT64_MAX / my;
    while (exp_diff > sides_zeroes)
    {
        exp_diff -= sides_zeroes;
        uint64_t hd = (mx * inv_hy) >> (64 - sides_zeroes);
        mx <<= sides_zeroes;
        mx -= hd * my;
        while (mx > my) {mx -= my;}
    }
    uint64_t hd = (mx * inv_hy) >> (64 - exp_diff);
    mx <<= exp_diff;
    mx -= hd * my;
    while (mx > my) {mx -= my;}

    return make_double(mx, ey, sx);
}

////////////////////////////////////// MIN /////////////////////////////////////

_NODISCARD constexpr float64 min(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left))
{
    // return smaller of _Left and _Right
    return _Right < _Left ? _Right : _Left;
}

_NODISCARD constexpr int64 min(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left))
{
    return _Right < _Left ? _Right : _Left;
}

_NODISCARD constexpr uint64 min(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left))
{
    return _Right < _Left ? _Right : _Left;
}

////////////////////////////////////// MAX /////////////////////////////////////

_NODISCARD constexpr float64 max(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left))
{
    // return larger of _Left and _Right
    return _Left < _Right ? _Right : _Left;
}

_NODISCARD constexpr int64 max(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left))
{
    return _Left < _Right ? _Right : _Left;
}

_NODISCARD constexpr uint64 max(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left))
{
    return _Left < _Right ? _Right : _Left;
}

////////////////////////////////////// CLAMP /////////////////////////////////////

float64 clamp(float64 x, float64 MinVal, float64 MaxVal)
{
    if (MinVal > MaxVal)
    {
        return __Float64::FromBytes(Q_NAN_DOUBLE);
    }
    return _CSE min(_CSE max(x, MinVal), MaxVal);
}

int64 clamp(int64 x, int64 MinVal, int64 MaxVal)
{
    if (MinVal > MaxVal)
    {
        return 0xFFFFFFFFFFFFFFFF;
    }
    return _CSE min(_CSE max(x, MinVal), MaxVal);
}

uint64 clamp(uint64 x, uint64 MinVal, uint64 MaxVal)
{
    if (MinVal > MaxVal)
    {
        return 0xFFFFFFFFFFFFFFFF;
    }
    return _CSE min(_CSE max(x, MinVal), MaxVal);
}

////////////////////////////////////// MIX /////////////////////////////////////

float64 mix(float64 x, float64 y, float64 a)
{
    return x * (1. - a) + y * a;
}

float64 mix(float64 x, float64 y, bool a)
{
    return x * (1. - a) + y * a;
}

int64 mix(int64 x, int64 y, bool a)
{
    return x * (1ll - a) + y * a;
}

uint64 mix(uint64 x, uint64 y, bool a)
{
    return x * (1ull - a) + y * a;
}

bool mix(bool x, bool y, bool a)
{
    return x * (1 - a) + y * a;
}

////////////////////////////////////// STEP /////////////////////////////////////

float64 step(float64 edge, float64 x)
{
    return x < edge ? 0. : 1.;
}

////////////////////////////////////// SmoothStep /////////////////////////////////////

float64 __Smoothstep_GL(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1){return __Float64::FromBytes(Q_NAN_DOUBLE);}
    float64 t;  /* Or genDType t; */
    t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

float64 __Smoothstep_AMD(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1) { return __Float64::FromBytes(Q_NAN_DOUBLE); }
    if (x < edge0){return 0;}
    if (x >= edge1){return 1;}
    // Scale/bias into [0..1] range
    x = (x - edge0) / (edge1 - edge0);
    return x * x * (3 - 2 * x);
}

float64 __Smoothstep_KP(float64 edge0, float64 edge1, float64 x)
{
    if (edge0 > edge1) { return __Float64::FromBytes(Q_NAN_DOUBLE); }
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float64 smoothstep(float64 edge0, float64 edge1, float64 x)
{
    return __Smoothstep_GL(edge0, edge1, x);
}

float64 inverse_smoothstep(float64 x)
{
    return 0.5 - _CSE sin(_CSE arcsin(1.0 - 2.0 * x) / 3.0);
}

////////////////////////////////////// INF and NAN /////////////////////////////////////

int64 isinf(float64 _X)throw()
{
    // Using GNU implementation
    int64_t ix = __Float64(_X).Bytes;
    int64_t t = ix & 0x7fffffffffffffffull;
    t ^= 0x7ff0000000000000ull;
    t |= -t;
    return ~(t >> 63) & (ix >> 62);
}

int64 isnan(float64 _X)throw()
{
    // Using GNU implementation
    int64_t hx = __Float64(_X).Bytes;
    hx &= 0x7fffffffffffffffull;
    hx = 0x7ff0000000000000ull - hx;
    return (((uint64_t)hx) >> 63);
}

int64 isfinite(float64 _X) throw()
{
    return !isinf(_X) && !isnan(_X);
}

int FPClassify(float64 _X)throw()
{
    // Using GNU implementation
    uint32_t hx = __Float64(_X).parts.msw, lx = __Float64(_X).parts.lsw;
    lx |= hx & 0xfffff;
    hx &= 0x7ff00000;
    if ((hx | lx) == 0) {return Zero;}
    if (hx == 0) {return SubNormal;}
    if (hx == 0x7ff00000) {return lx != 0 ? Nan : Inf;}
    return Normal;
}

_CSE_END
