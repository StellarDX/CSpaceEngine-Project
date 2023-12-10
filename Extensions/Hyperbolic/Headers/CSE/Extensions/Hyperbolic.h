/****************************************************************
  Hyperbolic functions extension for CSE Base
  Reference: https://en.wikipedia.org/wiki/Hyperbolic_functions
****************************************************************/

#pragma once

#ifndef _HYPERBOLIC_
#define _HYPERBOLIC_

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/MathFuncs.h>

_CSE_BEGIN
namespace Hyperbolic {

float64 __cdecl sinh(float64 _X);
float64 __cdecl cosh(float64 _X);
float64 __cdecl tanh(float64 _X);
float64 __cdecl coth(float64 _X);
float64 __cdecl sech(float64 _X);
float64 __cdecl csch(float64 _X);

complex64 __cdecl sinh(complex64 _X);
complex64 __cdecl cosh(complex64 _X);
complex64 __cdecl tanh(complex64 _X);
complex64 __cdecl coth(complex64 _X);
complex64 __cdecl sech(complex64 _X);
complex64 __cdecl csch(complex64 _X);

float64 __cdecl arsinh(float64 _X);
float64 __cdecl arcosh(float64 _X);
float64 __cdecl artanh(float64 _X);
float64 __cdecl arcoth(float64 _X);
float64 __cdecl arsech(float64 _X);
float64 __cdecl arcsch(float64 _X);

complex64 __cdecl arsinhc(complex64 _X, uint64 K1 = 0, int64 K2 = 0, int64 K3 = 0);
complex64 __cdecl arcoshc(complex64 _X, uint64 K1 = 0, int64 K2 = 0, int64 K3 = 0);
complex64 __cdecl artanhc(complex64 _X, int64 K = 0);
complex64 __cdecl arcschc(complex64 _X, uint64 K1 = 0, int64 K2 = 0, int64 K3 = 0);
complex64 __cdecl arsechc(complex64 _X, uint64 K1 = 0, int64 K2 = 0, int64 K3 = 0);
complex64 __cdecl arcothc(complex64 _X, int64 K = 0);

#define _HYP_VECN_FUNCTION_TEMPLATE(Func)\
template<typename genType> requires vecType<genType>\
genType __cdecl Func(genType _X)\
{\
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = Func(_X[i]); }\
    return _X;\
}\
\
template<uint64 size>\
std::array<float64, size> __cdecl Func(std::array<float64, size> _X)\
{\
    for (size_t i = 0; i < _X.size(); i++) { _X[i] = Func(_X[i]); }\
    return _X;\
}

_HYP_VECN_FUNCTION_TEMPLATE(sinh)
_HYP_VECN_FUNCTION_TEMPLATE(cosh)
_HYP_VECN_FUNCTION_TEMPLATE(tanh)
_HYP_VECN_FUNCTION_TEMPLATE(sech)
_HYP_VECN_FUNCTION_TEMPLATE(csch)
_HYP_VECN_FUNCTION_TEMPLATE(coth)

_HYP_VECN_FUNCTION_TEMPLATE(arsinh)
_HYP_VECN_FUNCTION_TEMPLATE(arcosh)
_HYP_VECN_FUNCTION_TEMPLATE(artanh)
_HYP_VECN_FUNCTION_TEMPLATE(arsech)
_HYP_VECN_FUNCTION_TEMPLATE(arcsch)
_HYP_VECN_FUNCTION_TEMPLATE(arcoth)

}
_CSE_END

#endif
