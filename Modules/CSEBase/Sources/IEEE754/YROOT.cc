// nth-root function by StellarDX, only for real roots

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

float64 __cdecl yroot(float64 _X, float64 _Expo)
{
	// Special returns
	if (_Expo == 1) { return _X; }
	if (_Expo == 2) { return _CSE sqrt(_X); }
	if (_Expo == 3) { return _CSE cbrt(_X); }

	using uint32 = unsigned;
	union half
	{
		float64    _Ref;    // Reference number
        uint32_t   _Hfi[2]; // Half of number, 0 = High, 1 = Low
	}Src, Exp;
	enum Idx { LOW_HALF, HIGH_HALF };

	// Extract bytes
	Src._Ref = _X;
	Exp._Ref = _Expo;

    if ((Src._Hfi[HIGH_HALF] > 0 && Src._Hfi[HIGH_HALF] < 0x7FF00000) || // x>0 and not x->0
		(Src._Hfi[HIGH_HALF] == 0 && Src._Hfi[LOW_HALF] != 0))
	{
		return _CSE pow(_X, 1. / _Expo);
	}

	if (_X == 0)
	{
		if (Exp._Hfi[HIGH_HALF] > 0) { return 0; }
        else { return __Float64::FromBytes(Q_NAN_DOUBLE); } // I don't know if negative(th) root of 0 is exist...
	}

	if (Src._Hfi[HIGH_HALF] < 0)
	{
		return -yroot(_Expo, -_X);
	}

    return __Float64::FromBytes(Q_NAN_DOUBLE); // StellarDX: (maybe) unreachable, to make the compiler happy (*￣3￣)╭
}

_CSE_END
