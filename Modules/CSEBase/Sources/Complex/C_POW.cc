#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/MathFuncs.h"
#include "CSE/CSEBase/ConstLists.h"
#include "CSE/CSEBase/Algorithms.h"

_CSE_BEGIN

// Complex form of power functions

int CheckInt(uint64 iy);

complex64 __cdecl powc(complex64 _X, complex64 _Power, int64 K_OFFSET)
{
	return _CSE expc(_Power * lnc(_X, K_OFFSET));
}

std::array<complex64, 2> __cdecl sqrtc(complex64 _X)
{
	complex64 _Res;
    float64 Sign = _CSE sgn(_X.imag());
	if (Sign == 0)
	{
        if (_X.real() > 0) { return { sqrt(_X.real()), -sqrt(_X.real()) }; }
        if (_X.real() < 0) { return { 1i * sqrt(-_X.real()), 1i * -sqrt(-_X.real()) }; }
	}
    _Res = _CSE sqrt((_CSE abs(_X) + _X.real()) / 2.) + 1i * Sign * _CSE sqrt((_CSE abs(_X) - _X.real()) / 2.);
	return { _Res, -_Res };
}

complex64 __cdecl inversesqrtc(complex64 _X)
{
	return 1. / _CSE sqrtc(_X)[0];
}

std::array<complex64, 3> __cdecl cbrtc(complex64 _X)
{
	float64 r = _CSE abs(_X);
    float64 tet = atan2(_X.imag(), _X.real());
	float64 ri = _CSE cbrt(r);
	return
	{
		ri * _CSE expc((1i * float64(tet + 0. * 2. * CSE_PI)) / 3.),
		ri * _CSE expc((1i * float64(tet + 1. * 2. * CSE_PI)) / 3.),
		ri * _CSE expc((1i * float64(tet + 2. * 2. * CSE_PI)) / 3.)
	};
}

std::vector<complex64> __cdecl yrootc(complex64 _Expo, complex64 _X, int64 K_OFFSET)
{
	std::vector<complex64> Buffer;

	float64 Real = _Expo.real();
    int _K = CheckInt(__Float64(_Expo.real()).Bytes);
	if (_Expo.imag() == 0 && _K != 0 && Real > 0)
	{
		float64 r = _CSE abs(_X);
        float64 tet = atan2(_X.imag(), _X.real());
		for (size_t i = 0; i < Real; ++i)
		{
            Buffer.push_back(yroot(Real, r) * _CSE expc(1i * complex64(tet + (i + K_OFFSET) * 2. * CSE_PI) / Real));
		}
		return Buffer;
	}

	Buffer.push_back(_CSE powc(_X, 1. / _Expo));
	return Buffer;
}

_CSE_END
