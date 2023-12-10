#include "CSE/Extensions/Hyperbolic.h"

_CSE_BEGIN
namespace Hyperbolic {

// BASE FUNCTIONS

float64 __cdecl sinh(float64 _X)
{
    return (exp(_X) - exp(-_X)) / 2.;
}

float64 __cdecl cosh(float64 _X)
{
    return (exp(_X) + exp(-_X)) / 2.;
}

float64 __cdecl tanh(float64 _X)
{
    return sinh(_X) / cosh(_X);
}

float64 __cdecl coth(float64 _X)
{
    return cosh(_X) / sinh(_X);
}

float64 __cdecl sech(float64 _X)
{
    return 1. / cosh(_X);
}

float64 __cdecl csch(float64 _X)
{
    return 1. / sinh(_X);
}

// COMPLEX FUNCTIONS

complex64 __cdecl sinh(complex64 _X)
{
    return (exp(_X) - exp(-_X)) / 2.;
}

complex64 __cdecl cosh(complex64 _X)
{
    return (exp(_X) + exp(-_X)) / 2.;
}

complex64 __cdecl tanh(complex64 _X)
{
    return sinh(_X) / cosh(_X);
}

complex64 __cdecl coth(complex64 _X)
{
    return cosh(_X) / sinh(_X);
}

complex64 __cdecl sech(complex64 _X)
{
    return 1. / cosh(_X);
}

complex64 __cdecl csch(complex64 _X)
{
    return 1. / sinh(_X);
}

// INVERSE FUNCTIONS

float64 __cdecl arsinh(float64 _X)
{
    return ln(_X + sqrt(pow(_X, 2) + 1.));
}

float64 __cdecl arcosh(float64 _X)
{
    return ln(_X + sqrt(pow(_X, 2) - 1.));
}

float64 __cdecl artanh(float64 _X)
{
    return (1. / 2.) * ln((1. + _X) / (1. - _X));
}

float64 __cdecl arcsch(float64 _X)
{
    return arsinh(1. / _X);
}

float64 __cdecl arsech(float64 _X)
{
    return arcosh(1. / _X);
}

float64 __cdecl arcoth(float64 _X)
{
    return artanh(1. / _X);
}

// INVERSE FUNCTIONS FOR COMPLEX

complex64 __cdecl arsinhc(complex64 _X, uint64 K1, int64 K2, int64 K3)
{
    return lnc(_X + sqrtc(powc(_X, 2, K2) + 1.)[K1], K3);
}

complex64 __cdecl arcoshc(complex64 _X, uint64 K1, int64 K2, int64 K3)
{
    return lnc(_X + sqrtc(powc(_X, 2, K2) - 1.)[K1], K3);
}

complex64 __cdecl artanhc(complex64 _X, int64 K)
{
    return (1. / 2.) * lnc((1. + _X) / (1. - _X), K);
}

complex64 __cdecl arcschc(complex64 _X, uint64 K1, int64 K2, int64 K3)
{
    return arsinhc(1. / _X, K1, K2, K3);
}

complex64 __cdecl arsechc(complex64 _X, uint64 K1, int64 K2, int64 K3)
{
    return arcoshc(1. / _X, K1, K2, K3);
}

complex64 __cdecl arcothc(complex64 _X, int64 K)
{
    return artanhc(1. / _X, K);
}

}
_CSE_END
