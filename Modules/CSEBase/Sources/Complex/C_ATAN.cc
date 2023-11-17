#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

// Complex Form of inverse trigonometric functions

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
