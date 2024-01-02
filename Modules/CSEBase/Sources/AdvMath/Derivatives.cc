#include "CSE/CSEBase/MathFuncs.h"
#include "CSE/CSEBase/AdvMath.h"

using namespace std;

_CSE_BEGIN
_SCICXX_BEGIN

/**
 * Finite difference method for find derivatives
 *
 * @link https://en.wikipedia.org/wiki/Finite_difference
 * @link https://en.wikipedia.org/wiki/Finite_difference_coefficient
 */
float64 __Basic_FDM_Derivative_Engine::operator()(float64 _Xx) const
{
    float64 _Scale = pow(10, -_M_StepScaleNLog / float64(_M_DeriOrder));
    float64 _Sum = 0;
    for (size_t i = 0; i < _M_CoeffOffsets.size(); ++i)
    {
        _Sum += _M_Coefficients[i] * _M_Invoker(_Xx + _M_CoeffOffsets[i] * _Scale);
    }
    return _Sum / pow(_Scale, float64(_M_DeriOrder));
}

_SCICXX_END
_CSE_END
