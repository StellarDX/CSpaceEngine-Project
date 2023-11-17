#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

// Complex form of trigonometric functions

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

_CSE_END
