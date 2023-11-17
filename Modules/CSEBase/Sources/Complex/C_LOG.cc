#include "CSE/CSEBase/MathFuncs.h"
#include "CSE/CSEBase/ConstLists.h"

_CSE_BEGIN

// Complex Form of logarithm functions

complex64 __cdecl lnc(complex64 _X, int64 K_OFFSET)
{
	// Convert to polar form (r, tet)
    float64 r = abs(_X);
    float64 tet = atan2(_X.imag(), _X.real());
    return ln(r) + 1i * float64(tet + 2. * CSE_2PI * K_OFFSET);
}

complex64 __cdecl logc(complex64 _X, int64 K_OFFSET)
{
	return lnc(_X, K_OFFSET) / complex64(CSE_LN10);
}

complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 K_OFFSET)
{
	return lnc(_X, K_OFFSET) / lnc(_Base, K_OFFSET);
}

_CSE_END
