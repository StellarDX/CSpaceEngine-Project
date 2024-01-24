// CSE Expanded trigonometric functions

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/ConstLists.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

// Sin, cos and tan defines
float64 __cdecl sin(float64 _X) { return __CV_SIN_DEGREES(_X); }
float64 __cdecl cos(float64 _X) { return __CV_COS_DEGREES(_X); }
float64 __cdecl tan(float64 _X) { return __CV_TAN_DEGREES(_X); }

float64 __cdecl sin(Angle _X) {return sin(_X.ToDegrees());}
float64 __cdecl cos(Angle _X) {return cos(_X.ToDegrees());}
float64 __cdecl tan(Angle _X) {return tan(_X.ToDegrees());}

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

int64 __cdecl Quadrant(float64 _X)
{
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

int64 __cdecl Quadrant(Angle _X) {return Quadrant(_X.ToDegrees());}

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

float64 __cdecl arcctg(float64 _X)
{
    return arctan(float64(1. / _X));
}

float64 __cdecl arcsec(float64 _X)
{
    return arccos(float64(1. / _X));
}

float64 __cdecl arccsc(float64 _X)
{
    return arcsin(float64(1. / _X));
}

float64 __cdecl arccrd(float64 _X, float64 _Radius)
{
    return 2. * arcsin(float64(_X / (2. * _Radius)));
}

float64 __cdecl arcsiv(float64 _X)
{
    return arccos(float64(1. - _X));
}

float64 __cdecl arcvcs(float64 _X)
{
    return arccos(float64(_X - 1.));
}

float64 __cdecl arccvs(float64 _X)
{
    return arcsin(float64(1. - _X));
}

float64 __cdecl arccvc(float64 _X)
{
    return arcsin(float64(_X - 1.));
}

float64 __cdecl archvs(float64 _X)
{
    return arccos(float64(1. - 2. * _X));
}

float64 __cdecl archvc(float64 _X)
{
    return arccos(float64(2. * _X - 1.));
}

float64 __cdecl archcv(float64 _X)
{
    return arcsin(float64(1. - 2. * _X));
}

float64 __cdecl archcc(float64 _X)
{
    return arcsin(float64(2. * _X - 1.));
}

float64 __cdecl arcexs(float64 _X)
{
    return arcsec(float64(_X + 1.));
}

float64 __cdecl arcexc(float64 _X)
{
    return arccsc(float64(_X + 1.));
}


_CSE_END
