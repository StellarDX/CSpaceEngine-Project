#include "CSE/CSEBase/MathFuncs.h"

// Exponential function in complex form

_CSE_BEGIN

complex64 __cdecl expc(complex64 _X)
{
    return _CSE exp(_X.real()) * (cos(Degrees(_X.imag())) + 1i * sin(Degrees(_X.imag())));
}

_CSE_END
