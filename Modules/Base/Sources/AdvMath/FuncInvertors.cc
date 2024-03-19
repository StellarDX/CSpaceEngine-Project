#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/AdvMath.h"

using namespace std;

_CSE_BEGIN
_SCICXX_BEGIN

/**
 * Bisection method to finding zero-point for any continuous function
 *
 * input: Function _M_Invoker,
 *        endpoint values _Xa(a), _Xb(b),
 *        tolerance _M_TolerNLog,
 *        maximum iterations _M_MaxIterLog
 *
 * conditions: a < b,
 *             either f(a) < 0 and f(b) > 0 or f(a) > 0 and f(b) < 0
 *
 * output: value which differs from a root of f(x) = 0 by less than TOL
 *
 * Algorithm:
 *
 *      N = 1
 *      while N <= NMAX do // limit iterations to prevent infinite loop
 *          c = (a + b) / 2 // new midpoint
 *          if f(c) = 0 or (b - a) / 2 < TOL then // solution found
 *              Output(c)
 *              Stop
 *          end if
 *          N = N + 1 // increment step counter
 *          if sign(f(c)) = sign(f(a)) then a = c else b = c // new interval
 *      end while
 *      Output("Method failed.") // max number of steps exceeded
 */
float64 __Basic_Bisection_Searcher::operator()(float64 _Xx, float64 _Xa, float64 _Xb)const throw()
{
    //if (_Xa > _Xb) {std::swap(_Xa, _Xb);}
    float64 RealToler = pow(10, -_M_TolerNLog);
    size_t MaxIter = llround(pow(10, _M_MaxIterLog));
    for (size_t i = 0; i < MaxIter; ++i)
    {
        float64 c = (_Xa + _Xb) / 2.;
        if (_M_Invoker(c) == _Xx || (_Xb - _Xa) / 2. < RealToler)
        {
            return c;
        }

        if (sgn(_M_Invoker(c) - _Xx) == sgn(_M_Invoker(_Xa) - _Xx))
        {
            _Xa = c;
        }
        else {_Xb = c;}
    }
    return __Float64::FromBytes(BIG_NAN_DOUBLE);
}

/**
 * Applies the Newton's method to find the inverse of a function
 *
 * Args:
 *     _Xx: Point
 *     _Init: The initial guess
 *     _M_Invoker: The function whose inverse we are trying to find
 *     _M_Derivate: The derivative of the function
 *     _M_MaxIterLog: The maximum number of iterations to compute
 *     _M_TolerNLog: Stop when iterations change by less than this
 *
 * Returns:
 *     The inverse function value at the specified point.
 */
float64 __Newton_Raphson_Iterator::operator()(float64 _Xx, float64 _X0)const throw()
{
    float64 RealToler = pow(10, -_M_TolerNLog);
    size_t MaxIter = llround(pow(10, _M_MaxIterLog));
    for (size_t i = 0; i < MaxIter; ++i)
    {
        float64 y0 = _M_Invoker(_X0) - _Xx;
        float64 y1 = _M_Derivate(_X0); // Derivative value will not affected by Adding a constant

        if (abs(y1) < DOUBLE_EPSILON) {break;} // Give up if the denominator is too small

        float64 x1 = _X0 - y0 / y1; // Do Newton's computation

        // Stop when the result is within the desired tolerance and
        // x1 is a solution within tolerance and maximum number of iterations
        if (abs(x1 - _X0) < RealToler) {return x1;}
        _X0 = x1; // Update x0 to start the process again
    }
    return __Float64::FromBytes(BIG_NAN_DOUBLE); // Newton's method did not converge
}

_SCICXX_END
_CSE_END
