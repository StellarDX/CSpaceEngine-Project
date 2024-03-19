// nth-root function by StellarDX, only for real roots

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/Algorithms.h"

_CSE_BEGIN

float64 __cdecl yroot(float64 _X, float64 _Expo)
{
    // Special returns
    if (_Expo == 1) { return _X; }
    if (_Expo == 2) { return _CSE sqrt(_X); }
    if (_Expo == 3) { return _CSE cbrt(_X); }

    __Float64 Exp = _Expo;

    // Every positive real number x has a single positive nth root
    if (_X > 0 && _Expo != 0) {return _CSE pow(_X, 1. / _Expo);}

    // For even values of n, positive numbers also have a negative
    // nth root, while negative numbers do not have a real nth root.
    // For odd values of n, every negative number x has a real
    // negative nth root.
    if (_X < 0 && _Expo != 0)
    {
        if (CheckInt(Exp.Bytes) == 1) {return -yroot(-_X, _Expo);}
        else {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
    }

    // The only root of 0 is 0.
    if (_X == 0 && _Expo != 0) {return 0;}

    // positive number's 0-th root is infinity, negative number's
    // 0-th root is undefined.
    if (Exp == 0)
    {
        if (_X > 0) {return __Float64::FromBytes(POS_INF_DOUBLE);}
        if (_X == 0) {return 0;}
        if (_X < 0) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}
    }

    return __Float64::FromBytes(BIG_NAN_DOUBLE); // StellarDX: (maybe) unreachable, to make the compiler happy (*￣3￣)╭
}

std::vector<complex64> __cdecl yrootc(complex64 _Expo, complex64 _X, int64 K_OFFSET)
{
    std::vector<complex64> Buffer;

    float64 Real = _Expo.real();
    int _K = CheckInt(__Float64(_Expo.real()).Bytes);
    if (_Expo.imag() == 0 && _K != 0 && Real > 0)
    {
        float64 r = abs(_X);
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
