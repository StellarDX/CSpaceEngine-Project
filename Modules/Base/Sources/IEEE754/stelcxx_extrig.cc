// CSE Expanded trigonometric functions

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN

// Sin, cos and tan defines
#ifndef TRIGONOMETRY_USE_RADIANS
float64 __cdecl sin(Angle _X) {return __CV_SIN_DEGREES(_X.ToDegrees());}
float64 __cdecl cos(Angle _X) {return __CV_COS_DEGREES(_X.ToDegrees());}
float64 __cdecl tan(Angle _X) {return __CV_TAN_DEGREES(_X.ToDegrees());}
#else
float64 __cdecl sin(Angle _X) {return __IBM_SIN64F(_X.ToRadians());}
float64 __cdecl cos(Angle _X) {return __IBM_COS64F(_X.ToRadians());}
float64 __cdecl tan(Angle _X) {return __IBM_TAN64F(_X.ToRadians());}
#endif

// Cotangent and secants

float64 __cdecl ctg(float64 _X)
{
    return 1. / _CSE tan(_X);
}

float64 __cdecl sec(float64 _X)
{
    return 1. / _CSE cos(_X);
}

float64 __cdecl csc(float64 _X)
{
    return 1. / _CSE sin(_X);
}

float64 __cdecl ctg(Angle _X) {return ctg(_X.ToDegrees());}
float64 __cdecl sec(Angle _X) {return sec(_X.ToDegrees());}
float64 __cdecl csc(Angle _X) {return csc(_X.ToDegrees());}

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
Angle __cdecl arcsin(float64 _X) { return arctan(_X / _CSE sqrt(1. - _X * _X)); }
Angle __cdecl arccos(float64 _X) { return arctan(_CSE sqrt(1. - _X * _X) / _X); }
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
