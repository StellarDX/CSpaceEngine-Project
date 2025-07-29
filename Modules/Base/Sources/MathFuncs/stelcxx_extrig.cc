// CSE Expanded trigonometric functions

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN

// 正弦，余弦，正切和余切
#ifndef TRIGONOMETRY_USE_RADIANS
float64 __cdecl sin(Angle _X) {return __CV_SIN128F_C64F(_X.ToDegrees());}
float64 __cdecl cos(Angle _X) {return __CV_COS128F_C64F(_X.ToDegrees());}
float64 __cdecl tan(Angle _X) {return __IEEE854_TAN128F_C64F(_X.ToDegrees());}
float64 __cdecl ctg(Angle _X) {return __IEEE854_CTG128F_C64F(_X.ToDegrees());}
#else
float64 __cdecl sin(Angle _X) {return __IBM_SIN64F(_X.ToRadians());}
float64 __cdecl cos(Angle _X) {return __IBM_COS64F(_X.ToRadians());}
float64 __cdecl tan(Angle _X) {return __IBM_TAN64F(_X.ToRadians());}
float64 __cdecl ctg(Angle _X) {return 1. / tan(x);}
#endif

// 正割和余割函数目前没去设计更高精度的算法。
float64 __cdecl sec(Angle _X) {return 1. / cos(_X);}
float64 __cdecl csc(Angle _X) {return 1. / sin(_X);}

int64 __cdecl __Quadrant(float64 _X)
{
    if (isinf(_X) || isnan(_X)) {return -2;}
    if (_X >= 360 || _X < 0)
    {
        float64 IntPart;
        float64 FractPart = modf(_X, &IntPart);
        _X = FractPart + int(IntPart) % 360;
        if (_X < 0) {_X += 360;}
    }
    if (_X == 0) { return 0; }
    if (_X > 0 && _X < 90) { return 1; }
    if (_X == 90) { return 2; }
    if (_X > 90 && _X < 180) { return 3; }
    if (_X == 180) { return 4; }
    if (_X > 180 && _X < 270) { return 5; }
    if (_X == 270) { return 6; }
    if (_X > 270 && _X < 360) { return 7; }
    return -1;
}

int64 __cdecl Quadrant(Angle _X) {return __Quadrant(_X.ToDegrees());}

/* arctan2(y, x)
 * Method :
 *	1. Reduce y to positive by atan2l(y,x)=-atan2l(-y,x).
 *	2. Reduce x to positive by (if x and y are unexceptional):
 *		ARG (x+iy) = arctan(y/x)	   ... if x > 0,
 *		ARG (x+iy) = pi - arctan[y/(-x)]   ... if x < 0,
 *
 * Special cases:
 *
 *	ATAN2((anything), NaN ) is NaN;
 *	ATAN2(NAN , (anything) ) is NaN;
 *	ATAN2(+-0, +(anything but NaN)) is +-0  ;
 *	ATAN2(+-0, -(anything but NaN)) is +-pi ;
 *	ATAN2(+-(anything but 0 and NaN), 0) is +-pi/2;
 *	ATAN2(+-(anything but INF and NaN), +INF) is +-0 ;
 *	ATAN2(+-(anything but INF and NaN), -INF) is +-pi;
 *	ATAN2(+-INF,+INF ) is +-pi/4 ;
 *	ATAN2(+-INF,-INF ) is +-3pi/4;
 *	ATAN2(+-INF, (anything but,0,NaN, and INF)) is +-pi/2;
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */
Angle __cdecl Arctan2(float64 _Y, float64 _X)
{
    // Use Sun Microsystem's implementation, and convert to Degrees

    if (isnan(_Y) || isnan(_X)) {return __Float64::FromBytes(BIG_NAN_DOUBLE).x;}
    if (_X == 1) {return arctan(_Y);}

    int sign_x = std::signbit(_X) ? -1 : 1;
    int sign_y = std::signbit(_Y) ? -1 : 1;
    int m = (sign_x < 0 ? 2 : 0) + (sign_y < 0 ? 1 : 0);

    if (_X == 1) {return arctan(_Y);}

    if (_Y == 0)
    {
        switch(m)
        {
            case 0:
            case 1: return Angle::FromDegrees(_Y);	/* atan(+-0,+anything)=+-0 */
            case 2: return Angle::FromDegrees(180); /* atan(+0,-anything) = pi */
            case 3: return Angle::FromDegrees(-180);/* atan(-0,-anything) =-pi */
        }
    }

    if (_X == 0)
    {
        return (sign_y < 0) ? -90 : 90;
    }

    if (isinf(_X))
    {
        if (isinf(_Y))
        {
            switch(m)
            {
                case 0: return Angle::FromDegrees(45);   /* atan(+INF,+INF) */
                case 1: return Angle::FromDegrees(-45);  /* atan(-INF,+INF) */
                case 2: return Angle::FromDegrees(135);  /*atan(+INF,-INF)*/
                case 3: return Angle::FromDegrees(-135); /*atan(-INF,-INF)*/
            }
        }
        else
        {
            switch(m)
            {
                case 0: return Angle::FromDegrees(0);          /* atan(+...,+INF) */
                case 1: return Angle::FromDegrees
                    (__Float64::FromBytes(NEG_ZERO_DOUBLE).x); /* atan(-...,+INF) */
                case 2: return Angle::FromDegrees(180);        /* atan(+...,-INF) */
                case 3: return Angle::FromDegrees(-180);       /* atan(-...,-INF) */
            }
        }
    }

    if (isinf(_Y))
    {
        return Angle::FromDegrees((sign_y < 0) ? -90 : 90);
    }

    float64 z, ratio = abs(_Y / _X);
    if (ratio > 1.0e120)
    {
        /* |y/x| > 2^120 */
        z = 90;
    }
    else if (sign_x < 0 && ratio < 1.0e-120)
    {
        /* |y/x| < 2^-120 */
        z = 0;
    }
    else
    {
        z = arctan(ratio).ToDegrees(); /* safe to do y/x */
    }

    switch (m)
    {
        case 0: return Angle::FromDegrees(z);       /* atan(+,+) */
        case 1: return Angle::FromDegrees(-z);      /* atan(-,+) */
        case 2: return Angle::FromDegrees(180 - z); /* atan(+,-) */
        case 3: return Angle::FromDegrees(z - 180); /* atan(-,-) */
    }
    return Angle::FromDegrees(z);
}

///////////////////////////////////// EXPAND ////////////////////////////////////

// Chord and Arcs
float64 __cdecl crd(float64 _X, float64 _Radius)
{
    return 2. * _Radius * _CSE sin(float64(_X / 2.));
}

float64 __cdecl arc(float64 _X, float64 _Radius)
{
    return (_X * CSE_PI * _Radius) / 180.;
}

// Versines
float64 __cdecl siv(float64 _X)
{
    return 1. - _CSE cos(_X);
}

float64 __cdecl cvs(float64 _X)
{
    return 1. - _CSE sin(_X);
}

float64 __cdecl vcs(float64 _X)
{
    return 1. + _CSE cos(_X);
}

float64 __cdecl cvc(float64 _X)
{
    return 1. + _CSE sin(_X);
}

float64 __cdecl hvs(float64 _X)
{
    return siv(_X) / 2.;
}

float64 __cdecl hcv(float64 _X)
{
    return cvs(_X) / 2.;
}

float64 __cdecl hvc(float64 _X)
{
    return vcs(_X) / 2.;
}

float64 __cdecl hcc(float64 _X)
{
    return cvc(_X) / 2.;
}

// Exsecants
float64 __cdecl exs(float64 _X)
{
    return sec(_X) - 1.;
}

float64 __cdecl exc(float64 _X)
{
    return csc(_X) - 1.;
}

// Inverse functions

#ifndef TRIGONOMETRY_USE_RADIANS
Angle __cdecl arcsin(float64 _X) { return arctan(_X / sqrt(1. - _X * _X)); }
Angle __cdecl arccos(float64 _X) { return arctan(sqrt(1. - _X * _X) / _X) + (_X < 0 ? 180 : 0); }
#else
Angle __cdecl arcsin(float64 _X) { return __IBM_ASIN64F(_X); }
Angle __cdecl arccos(float64 _X) { return __IBM_ACOS64F(_X); }
#endif
Angle __cdecl arctan(float64 _X) { return __IEEE754_ATAN128F_C64F(_X); }

Angle __cdecl arcctg(float64 _X)
{
    return arctan(float64(1. / _X));
}

Angle __cdecl arcsec(float64 _X)
{
    return arccos(float64(1. / _X));
}

Angle __cdecl arccsc(float64 _X)
{
    return arcsin(float64(1. / _X));
}

Angle __cdecl arccrd(float64 _X, float64 _Radius)
{
    return 2. * arcsin(float64(_X / (2. * _Radius)));
}

Angle __cdecl arcsiv(float64 _X)
{
    return arccos(float64(1. - _X));
}

Angle __cdecl arcvcs(float64 _X)
{
    return arccos(float64(_X - 1.));
}

Angle __cdecl arccvs(float64 _X)
{
    return arcsin(float64(1. - _X));
}

Angle __cdecl arccvc(float64 _X)
{
    return arcsin(float64(_X - 1.));
}

Angle __cdecl archvs(float64 _X)
{
    return arccos(float64(1. - 2. * _X));
}

Angle __cdecl archvc(float64 _X)
{
    return arccos(float64(2. * _X - 1.));
}

Angle __cdecl archcv(float64 _X)
{
    return arcsin(float64(1. - 2. * _X));
}

Angle __cdecl archcc(float64 _X)
{
    return arcsin(float64(2. * _X - 1.));
}

Angle __cdecl arcexs(float64 _X)
{
    return arcsec(float64(_X + 1.));
}

Angle __cdecl arcexc(float64 _X)
{
    return arccsc(float64(_X + 1.));
}

// Complex form

complex64 __cdecl sinc(complex64 _X)
{
    return (_CSE expc(1i * _X) - _CSE expc(-1i * _X)) / 2.i;
}

complex64 __cdecl cosc(complex64 _X)
{
    return (_CSE expc(1i * _X) + _CSE expc(-1i * _X)) / 2.;
}

complex64 __cdecl tanc(complex64 _X)
{
    return _CSE sinc(_X) / _CSE cosc(_X);
}

complex64 __cdecl ctgc(complex64 _X)
{
    return complex64(1.) / _CSE tanc(_X);
}

complex64 __cdecl secc(complex64 _X)
{
    return complex64(1.) / _CSE cosc(_X);
}

complex64 __cdecl cscc(complex64 _X)
{
    return complex64(1.) / _CSE sinc(_X);
}

complex64 __cdecl arcsinc(complex64 _X, int _SqrtIdx, int64 K_OFFSET)
{
    return -1i * lnc(_CSE sqrtc(1. - _X * _X)[_SqrtIdx] + 1i * _X, K_OFFSET);
}

complex64 __cdecl arccosc(complex64 _X, int _SqrtIdx, int64 K_OFFSET)
{
    return -1i * lnc(1i * _CSE sqrtc(1. - _X * _X)[_SqrtIdx] + _X, K_OFFSET);
}

complex64 __cdecl arctanc(complex64 _X, int64 K_OFFSET)
{
    return -0.5i * lnc((1i - _X) / (1i + _X), K_OFFSET);
}

complex64 __cdecl arcctgc(complex64 _X, int64 K_OFFSET)
{
    return -0.5i * lnc((_X + 1i) / (_X - 1i), K_OFFSET);
}

complex64 __cdecl arcsecc(complex64 _X, int _SqrtIdx, int64 K_OFFSET)
{
    return -1i * lnc(1i * _CSE sqrtc(1. - 1. / (_X * _X))[_SqrtIdx] + 1. / _X, K_OFFSET);
}

complex64 __cdecl arccscc(complex64 _X, int _SqrtIdx, int64 K_OFFSET)
{
    return -1i * lnc(_CSE sqrtc(1. - 1. / (_X * _X))[_SqrtIdx] + 1.i / _X, K_OFFSET);
}

_CSE_END
