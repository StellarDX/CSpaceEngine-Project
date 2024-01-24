/************************************************************
  CSpaceEngine Advanced math Library, also called "SciC++"

  Description:
    This file contains classes and functions used
    to solve advanced math problems.

  Features:
    1. Convertions between XYZ and polar coordinates
    2. Derivatives and integral engines
    3. ODE Solvers
    4. Inverting a function

  A ghost story that's been circulating in many colleges:
    There is a tree in college with a lot of students
    hanging on it. It is called "Advanced Maths".
    (大学里有棵树，上面挂着很多人，这棵树叫“高数”。)
***********************************************************/

/**
    CSpaceEngine Advanced math Library.
    Copyright (C) StellarDX Astronomy.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _ADV_MATH_
#define _ADV_MATH_

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/Algorithms.h"
#include "CSE/CSEBase/MathFuncs.h"
#include "CSE/CSEBase/gltypes.h"
#include <map>
#include <set>
#include <functional>

#if defined _MSC_VER
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
#if defined _STL_DISABLE_CLANG_WARNINGS
_STL_DISABLE_CLANG_WARNINGS
#endif
#pragma push_macro("new")
#undef new
#endif

_CSE_BEGIN

#define _SCICXX_BEGIN namespace SciCxx {
#define _SCICXX SciCxx::
#define _SCICXX_END }

using Function1D = std::function<float64(float64)>; // 1D Function type

/****************************************************************************************\
*                                Coordinate convertions                                  *
\****************************************************************************************/

/**
 * @brief Convert XY coordinate to polar coordinate, in format (r, Theta)
 */
vec2 _cdecl XYToPolar(vec2 XY);

/**
 * @brief Convert XYZ coordinate (Based on X-Z plane) to polar coordinate, in format (Lon, Lat, Dist)
 */
vec3 _cdecl XYZToPolar(vec3 XYZ);

/**
 * @brief Convert polar coordinate in format (r, Theta) to XY coordinate
 */
vec2 _cdecl PolarToXY(vec2 Polar);

/**
 * @brief Convert polar coordinate in format (Lon, Lat, Dist) to XYZ coordinate (Based on X-Z plane)
 */
vec3 _cdecl PolarToXYZ(vec3 Polar);

/****************************************************************************************\
*                                   Function Invoker                                     *
\****************************************************************************************/

_SCICXX_BEGIN

template<typename _Signature> struct __CSE_AdvMath_Function_Invoker;
template<typename _Res, typename... _ArgTypes>
struct __CSE_AdvMath_Function_Invoker<_Res(_ArgTypes...)>
{
    std::function<_Res(_ArgTypes...)> _M_Invoker;

    __CSE_AdvMath_Function_Invoker() {}

    __CSE_AdvMath_Function_Invoker(std::function<_Res(_ArgTypes...)> _Right)
        : _M_Invoker(_Right) {}

    __CSE_AdvMath_Function_Invoker(_Res (*_PFunc)(_ArgTypes...))
        : _M_Invoker(_PFunc) {}

    template<typename _Functor>
    __CSE_AdvMath_Function_Invoker(_Functor _Func)
        : _M_Invoker(_Func) {}

    template<typename _Functor>
    __CSE_AdvMath_Function_Invoker& operator=(_Functor _Func)
    {
        _M_Invoker = _Func;
        return *this;
    }

    virtual _Res operator()(_ArgTypes...)const = 0;
};

_SCICXX_END

/****************************************************************************************\
*                                  Limit of a function                                   *
\****************************************************************************************/

_SCICXX_BEGIN

class __Calculateur_Limite_LHopital
{
    // TODO...
};

_SCICXX_END

template<typename _Functor1, typename _Functor2>
float64 LHopitalDiv(_Functor1 Numerateur, _Functor2 Denominateur, float64 x)
{
    // TODO...
    return 0;
}

template<typename _Functor1, typename _Functor2>
float64 LHopitalMul(_Functor1 Facteur1, _Functor2 Facteur2, float64 x)
{
    // TODO...
    return 0;
}

template<typename _Functor1, typename _Functor2>
float64 LHopitalSub(_Functor1 Diminuende, _Functor2 Diminuteur, float64 x)
{
    // TODO...
    return 0;
}

template<typename _Functor1, typename _Functor2>
float64 LHopitalPow(_Functor1 Base, _Functor2 Exposant, float64 x)
{
    // TODO...
    return 0;
}

/****************************************************************************************\
*                               Derivatives and Integrals                                *
\****************************************************************************************/

/////////////////////////////////////// DERIVATIVE ///////////////////////////////////////

_SCICXX_BEGIN

class __Basic_FDM_Derivative_Engine : public __CSE_AdvMath_Function_Invoker<float64(float64)>
{
public:
    using _Mybase = __CSE_AdvMath_Function_Invoker<float64(float64)>;

    std::vector<float64> _M_Coefficients;
    std::vector<float64> _M_CoeffOffsets;
    uint64               _M_DeriOrder;

    enum DiffType
    {
        Central,
        Forward,
        Backward
    }_M_Differences;

    // StellarDX: Step scale is initially decided to use binary logarithm,
    // but common logarithm is used finally because it need to compat with
    // common usage in scientific calculating.
    constexpr static const float64 _DefStepLength = 7.8267798872635110755572112628368; // -0.5 * log(0x1p-52)
    float64              _M_StepScaleNLog;

    __Basic_FDM_Derivative_Engine() : _Mybase(), _M_StepScaleNLog(_DefStepLength) {}

    __Basic_FDM_Derivative_Engine(std::function<float64(float64)> _Right,
        float64 _Scale = _DefStepLength)
        : _Mybase(_Right), _M_StepScaleNLog(_Scale) {SetCoefficients<1, 3>();}

    __Basic_FDM_Derivative_Engine(float64 (*_PFunc)(float64),
        float64 _Scale = _DefStepLength)
        : _Mybase(_PFunc), _M_StepScaleNLog(_Scale) {SetCoefficients<1, 3>();}

    template<typename _Functor>
    __Basic_FDM_Derivative_Engine(_Functor _Func,
        float64 _Scale = _DefStepLength)
        : _Mybase(_Func), _M_StepScaleNLog(_Scale) {SetCoefficients<1, 3>();}

    template<typename _Functor>
    __Basic_FDM_Derivative_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    template<typename _Iterator, uint64 DeriOrder, uint64 CoeffCount>
    void SetCoefficients(_Iterator Begin, _Iterator End)
    {
        if (End - Begin < CoeffCount) {throw std::logic_error("Coefficients is too few.");}
        if (DeriOrder >= CoeffCount) {throw std::logic_error("Derivative order can't larger than coefficient count.");}
        std::set<float64> COffsets(Begin, Begin + CoeffCount);
        if (COffsets.size() < CoeffCount) {throw std::logic_error("Coefficients is too few.");}
        _M_CoeffOffsets = std::vector<float64>(COffsets.begin(), COffsets.end());
        _M_DeriOrder = DeriOrder;

        matrix<float64, CoeffCount, CoeffCount> CoeffMatrix(0);
        for (size_t i = 0; i < CoeffCount; ++i)
        {
            for (size_t j = 0; j < CoeffCount; ++j)
            {
                CoeffMatrix[i][j] = pow(_M_CoeffOffsets[i], float64(j));
            }
        }

        matrix<float64, 1, CoeffCount> KroneckerMatrix(0);
        KroneckerMatrix[0][DeriOrder] = tgamma(DeriOrder + 1);

        auto CoeffArray = (CoeffMatrix.Inverse() * KroneckerMatrix)[0];
        _M_Coefficients = std::vector<float64>(CoeffArray.begin(), CoeffArray.end());
    }

    template<uint64 DeriOrder, uint64 CoeffCount>
    void SetCoefficients(std::initializer_list<float64> _Ilist)
    {
        if (DeriOrder >= CoeffCount) {throw std::logic_error("Derivative order can't larger than coefficient count.");}
        SetCoefficients<decltype(_Ilist.begin()), DeriOrder, CoeffCount>(_Ilist.begin(), _Ilist.end());
    }

    template<uint64 DeriOrder, uint64 CoeffCount>
    void SetCoefficients(DiffType FDiff = Central)
    {
        if (DeriOrder >= CoeffCount) {throw std::logic_error("Derivative order can't larger than coefficient count.");}
        std::vector<float64> Offsets;
        switch (FDiff)
        {
        case Central:
        {
            int64 RealCoeffCount = CoeffCount;
            if (CoeffCount % 2 == 0) {++RealCoeffCount;}
            int64 OffsetCount = RealCoeffCount / 2;
            for (int64 i = 0; i < RealCoeffCount; ++i)
            {
                Offsets.push_back(float64(i - OffsetCount));
            }
        }
            break;
        case Forward:
            for (int64 i = 0; i < CoeffCount; ++i)
            {
                Offsets.push_back(float64(i));
            }
            break;
        case Backward:
            for (int64 i = 0; i < CoeffCount; ++i)
            {
                Offsets.push_back(float64(i - int64(CoeffCount)));
            }
            break;
        }

        SetCoefficients<decltype(Offsets.begin()), DeriOrder, CoeffCount>(Offsets.begin(), Offsets.end());
    }

    float64 operator()(float64 _Xx)const;
};

_SCICXX_END

// Default engines
using DerivativeFunction = _SCICXX __Basic_FDM_Derivative_Engine;

//////////////////////////////////////// INTEGRAL ////////////////////////////////////////

/**
 *  Presets of samples for sample-based integral functions
 */

#define __LO_PREC 4.0000000000000000000000000000000 // Low precision with 10000 sample points
#define __MD_PREC 4.5563025007672872650175335959592 // Mid precision with 36000 sample points
#define __HI_PREC 4.8920946026904804017152719559219 // High precision with 78000 sample points
#define __UT_PREC 5.1335389083702175141813865785018 // Ultra precision with 136000 sample points
#define __EX_PREC 5.2504200023088939799372822644269 // Extreme precision with 178000 sample points

_SCICXX_BEGIN

class __Trapezoidal_Integral_Engine : public __CSE_AdvMath_Function_Invoker<float64(float64)>
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker<float64(float64)>; // pointer to the function to be integrated
    using _Sample_Type = std::vector<float64>;

    enum Method
    {
        NonUniform,
        Uniform
    }_M_Method;

    constexpr static const float64 _DefLogSteps = __HI_PREC;
    float64 _M_LogSteps; // logarithm of steps of the procedure

    __Trapezoidal_Integral_Engine() : _Mybase(), _M_LogSteps(_DefLogSteps), _M_Method(Uniform) {}

    __Trapezoidal_Integral_Engine(std::function<float64(float64)> _Right,
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_Right), _M_LogSteps(_Steps), _M_Method(_Method) {}

    __Trapezoidal_Integral_Engine(float64 (*_PFunc)(float64),
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_PFunc), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Trapezoidal_Integral_Engine(_Functor _Func,
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_Func), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Trapezoidal_Integral_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    float64 _E_NonUniform(_Sample_Type _Samples, bool IsInterval = 0)const;
    float64 _E_Uniform(float64 _Min, float64 _Max)const;

    float64 operator()(float64 _Xx)const override; // override
    float64 operator()(float64 _Min, float64 _Max)const;

    template<typename _Iterator>
    float64 operator()(_Iterator& _First, _Iterator& _Last, bool IsInterval = 0)const
    {
        if (IsInterval)
        {
            _Sample_Type _Samples(_First, _Last);
            return _E_NonUniform(_Samples, 1);
        }

        switch (_M_Method)
        {
        default:
        case NonUniform:
            return _E_NonUniform(_Sample_Type(_First, _Last));
        case Uniform:
            return _E_Uniform(*_First, *(_Last - 1));
        }
    }
};

class __Simpson_Integral_Engine : public __CSE_AdvMath_Function_Invoker<float64(float64)>
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker<float64(float64)>; // pointer to the function to be integrated
    using _Sample_Type = std::vector<float64>;

    enum Method
    {
        /**
         * @brief Simpson method based upon a quadratic interpolation.
         * also called Simpson's 1/3 rule
         */
        CompositeQuadratic,

        /**
         * @brief Simpson method based upon a cubic interpolation.
         * also called Simpson's 3/8 rule
         */
        CompositeCubic,

        /**
         * @brief Method obtained by combining the composite Simpson's 1/3 rule
         * with the one consisting of using Simpson's 3/8 rule in the extreme
         * subintervals and Simpson's 1/3 rule in the remaining subintervals.
         */
        Extended,

        /**
         * @brief Method used for estimating full area of narrow peak-like functions
         * with two points outside of the integrated region are exploited.
         * Sample format: x.front() and x.back() are the points outside the range
         * and real range is x[1 - (size-2)].
         */
        NarrowPeaks1,

        /**
         * @brief Another method used for narrow peak-like functions
         * but only points within integration region are used.
         */
        NarrowPeaks2,

        /**
         * @brief Composite Simpson's rule for irregularly spaced data.
         * Both Sample points and sub-intervals data are supported.
         * When using sub-intervals, Input array must begin with start point
         * and rest values are the length of sub-intervals[1..n], the end
         * point will be auto-calculated.
         * This method has the highest precision from testing.
         */
        Irregularly
    }_M_Method;

    constexpr static const float64 _DefLogSteps = __HI_PREC;
    float64 _M_LogSteps; // logarithm of steps of the procedure

    __Simpson_Integral_Engine() : _Mybase(), _M_LogSteps(_DefLogSteps), _M_Method(CompositeQuadratic) {}

    __Simpson_Integral_Engine(std::function<float64(float64)> _Right,
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_Right), _M_LogSteps(_Steps), _M_Method(_Method) {}

    __Simpson_Integral_Engine(float64 (*_PFunc)(float64),
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_PFunc), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Simpson_Integral_Engine(_Functor _Func,
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_Func), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Simpson_Integral_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    float64 _E_CompositeQuadratic(_Sample_Type _Samples)const;
    float64 _E_CompositeCubic(_Sample_Type _Samples)const;
    float64 _E_Extended(_Sample_Type _Samples)const;
    float64 _E_NarrowPeaks1(_Sample_Type _Samples)const;
    float64 _E_NarrowPeaks2(_Sample_Type _Samples)const;
    float64 _E_Irregularly(_Sample_Type _Samples, bool IsInterval = 0)const;

    float64 operator()(float64 _Xx)const override; // override
    float64 operator()(float64 _Min, float64 _Max)const;

    template<typename _Iterator>
    float64 operator()(_Iterator& _First, _Iterator& _Last, bool IsInterval = 0)const
    {
        _Sample_Type _Samples(_First, _Last);

        if (IsInterval) {return _E_Irregularly(_Samples, 1);}

        switch (_M_Method)
        {
        default:
        case CompositeQuadratic:
            return _E_CompositeQuadratic(_Samples);
        case CompositeCubic:
            return _E_CompositeCubic(_Samples);
        case Extended:
            return _E_Extended(_Samples);
        case NarrowPeaks1:
            return _E_NarrowPeaks1(_Samples);
        case NarrowPeaks2:
            return _E_NarrowPeaks2(_Samples);
        case Irregularly:
            return _E_Irregularly(_Samples);
        }
    }
};

class __Romberg_Integral_Engine : public __CSE_AdvMath_Function_Invoker<float64(float64)>
{
public:
    using _Mybase = __CSE_AdvMath_Function_Invoker<float64(float64)>; // pointer to the function to be integrated

    constexpr static const size_t _DefMaxSteps = 300;
    constexpr static const float64 _DefPAcc = 8.;
    size_t  _M_MaxSteps; // maximum steps of the procedure
    float64 _M_p_Acc;    // negative logarithm of desired accuracy

    __Romberg_Integral_Engine() : _Mybase(), _M_MaxSteps(_DefMaxSteps), _M_p_Acc(_DefPAcc) {}

    __Romberg_Integral_Engine(std::function<float64(float64)> _Right,
        size_t _MaxSteps = _DefMaxSteps, float64 _p_Acc = _DefPAcc)
        : _Mybase(_Right), _M_MaxSteps(_MaxSteps), _M_p_Acc(_p_Acc) {}

    __Romberg_Integral_Engine(float64 (*_PFunc)(float64),
        size_t _MaxSteps = _DefMaxSteps, float64 _p_Acc = _DefPAcc)
        : _Mybase(_PFunc), _M_MaxSteps(_MaxSteps), _M_p_Acc(_p_Acc) {}

    template<typename _Functor>
    __Romberg_Integral_Engine(_Functor _Func,
        size_t _MaxSteps = _DefMaxSteps, float64 _p_Acc = _DefPAcc)
        : _Mybase(_Func), _M_MaxSteps(_MaxSteps), _M_p_Acc(_p_Acc) {}

    template<typename _Functor>
    __Romberg_Integral_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    float64 operator()(float64 _Xx)const override; // override
    float64 operator()(float64 _Min, float64 _Max)const;

    // A debugger for Romberg integration
    matrix<float64, 5, 5> RombergAnalysis(float64 _Min, float64 _Max);
    static matrix<float64, 5, 5> RombergAnalysis(std::function<float64(float64)> _Func, float64 _Min, float64 _Max);
};

class __Infinite_Integral_Nomalizer : public __CSE_AdvMath_Function_Invoker<float64(float64)>
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker<float64(float64)>;
    using _Sample_Type = std::map<float64, float64>;

    enum FuncType
    {
        WholeLine,   // [-inf, +inf]
        HasMaxValue, // [-inf, x]
        HasMinValue  // [x, +inf]
    }_M_FuncType;

    _Sample_Type _M_Special_Cases; // Special case of function points, normally values at breakopint.
    float64 _M_Breakpoint;

    __Infinite_Integral_Nomalizer() : _Mybase() {}

    __Infinite_Integral_Nomalizer(std::function<float64(float64)> _Right)
        : _Mybase(_Right) {}

    __Infinite_Integral_Nomalizer(float64 (*_PFunc)(float64))
        : _Mybase(_PFunc) {}

    template<typename _Functor>
    __Infinite_Integral_Nomalizer(_Functor _Func)
        : _Mybase(_Func) {}

    template<typename _Functor>
    __Infinite_Integral_Nomalizer& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    float64 LowLimit()const;
    float64 UpLimit()const;

    void SetSpecialCase(float64 _Xx, float64 _Fx, bool _Cover = 0);
    void DelSpecialCase(float64 _Xx);

    constexpr auto _K_WholeLine()const;
    constexpr auto _K_HasMaxValue()const;
    constexpr auto _K_HasMinValue()const;

    void CreateDefSpecialCases();
    std::function<float64(float64)> gen()const;

    float64 operator()(float64)const override;
};

_SCICXX_END

/**
 * @brief Normalize a function for solving Integrals over infinite intervals
 * @link https://en.wikipedia.org/wiki/Numerical_integration
 * @param PFunc - Input function
 * @param FuncType - Type of Infinite integral, values can be the whole line or has begin or end point
 * @param Breakpoint - Begin or end point of the integral, used for semi-infinite intervals
 * @param AddDefaultSpecialCases - Create default special cases.
 * @param SpecialCases - Special cases. Some of the points, espeially the upper or lower limit
 * in normalized function, aka. -1 and 1 for whole-line and 0 or 1 for semi-infinite integrals,
 * maybe has undefined values but their limits are exists. These values need to be set manually.
 * @return the normalized function. Use "gen()" to generate lambda expression.
 */
_SCICXX __Infinite_Integral_Nomalizer Normalize(std::function<float64(float64)> PFunc, _SCICXX __Infinite_Integral_Nomalizer::FuncType FuncType = _SCICXX __Infinite_Integral_Nomalizer::WholeLine, float64 Breakpoint = 0, bool AddDefaultSpecialCases = true, std::map<float64, float64> SpecialCases = std::map<float64, float64>());

// Default engines
using IntegralFunction = _SCICXX __Romberg_Integral_Engine;
using NormalizedInfiniteIntegral = _SCICXX __Infinite_Integral_Nomalizer;

/****************************************************************************************\
*                            Ordinary Differential Equations                             *
\****************************************************************************************/

_SCICXX_BEGIN

/**
 * Standard identifier of ODEs
 * Converted from the same feature in SciPy, almost a literal translation.
 *
 * Below is the original description from SciPy:
 *
 *  This function numerically integrates a system of ordinary differential
 *  equations given an initial value:
 *
 *      y' = f(x, y)
 *      y(x0) = y0
 *
 *  Here x is a 1-D independent variable (time), y(x) is an
 *  N-D vector-valued function (state), and an N-D
 *  vector-valued function f(x, y) determines the differential equations.
 *  The goal is to find y(x) approximately satisfying the differential
 *  equations, given an initial value y(x0)=y0.
 *
 *  SciPy is available under the BSD-3-clause license,
 *  see "ODEs.cc" for more information.
 *
 *  This function using iterator-form to make compatibilities for
 *  pointers and iterators in C++, like this:
 *
 *      y' = f(x, begin, end, solutions)
 *
 *  where "begin" and "end" is the begin and end iterator of cofficient
 *  (y0) array, and [begin, end) is used in processing. Results will be
 *  stored in "solution" array and this parameter is the begin iterator
 *  of this array. "x" is the scalar (x0).
 *
 * @param _CoeffFirst - Begin iterator of coefficient array.
 * @param _CoeffLast - End iterator of coefficient array,
 * and this iterator won't included when processing.
 * @param _RootDestination - Begin iterator of solution array,
 * aka. roots will be exported to this array.
 * @param _Scalar - Scalar
 */
template<uint64 EquationCount>
using __ODE_Fty_Untemplated = void(float64 _Scalar, std::array<float64, EquationCount> _Coeffs, std::array<float64, EquationCount>& _Roots);

template<typename _Tp, std::size_t _Nm>
_Tp __RMS_Norm(std::array<_Tp, _Nm> Input)
{
    float64 SumTemp = 0;
    for (auto i : Input){SumTemp += pow(i, 2);}
    return sqrt(SumTemp / float64(_Nm));
}

template<uint64 EquationCount> requires (EquationCount > 0)
struct __ODE_Engine_Base// : public __CSE_AdvMath_Function_Invoker<__ODE_Fty<IteratorType>>
{
public:
    using _Fty       = __ODE_Fty_Untemplated<EquationCount>;
    using _Mybase    = __CSE_AdvMath_Function_Invoker<_Fty>;

    using ValueArray = std::array<float64, EquationCount>;
    using StateType  = std::map<float64, ValueArray>;

    struct DenseOutput;

    virtual void SaveDenseOutput() = 0;

    enum State
    {
        Processing = -1,
        Succeeded  = 0,
        Failed     = 1
    };

protected:
    std::function<_Fty>  _M_Invoker;      // Function invoker
    enum State           _M_State;        // Current status of the engine.
    float64              _M_EndPoint;     // End point
    bool                 _M_Direction;    // 0 if positive and 1 if negative
    StateType            _M_StateBuffer;  // Current state

public:
    __ODE_Engine_Base() {}

    __ODE_Engine_Base(std::function<_Fty> _Right)
        : _M_Invoker(_Right) {}

    __ODE_Engine_Base(_Fty* _PFunc)
        : _M_Invoker(_PFunc) {}

    template<typename _Functor>
    __ODE_Engine_Base(_Functor _Func)
        : _M_Invoker(_Func) {}

    template<typename _Functor>
    __ODE_Engine_Base& operator=(_Functor _Func)
    {
        _M_Invoker = _Func;
        return *this;
    }

    float64 CurrentPoint()const // Current point
    {
        return _M_Direction ? (_M_StateBuffer.begin())->first : (--_M_StateBuffer.end())->first;
    }

    float64 PrevPoint()const // Previous point
    {
        if (_M_StateBuffer.size() <= 1) {return __Float64::FromBytes(CSE_NAN);}
        if (_M_Direction) {return ++_M_StateBuffer.begin()->first;}
        auto end = --_M_StateBuffer.end();
        return (--end)->first;
    }

    StateType Solutions()const {return _M_StateBuffer;}

    enum State CurrentState()const {return _M_State;}

    constexpr float64 size()const {return abs(CurrentPoint() - PrevPoint());}

    void __cdecl InvokeRun() throw()
    {
        if (_M_State != Processing) {throw std::logic_error("Engine is finished.");}

        if (CurrentPoint() == _M_EndPoint)
        {
            _M_State = Succeeded;
            return;
        }

        int ExitCode = Run();

        if (ExitCode)
        {
            _M_State = State(ExitCode);
            return;
        }

        if (((_M_Direction ? -1. : 1.) * (CurrentPoint() - _M_EndPoint)) > 0)
        {
            _M_State = Succeeded;
            return;
        }
    }

    virtual int Run() = 0;

    virtual ValueArray operator()(float64 _Xx)const = 0;
};

template<uint64 EquationCount>
struct __ODE_Dense_Output_Base
{
public:
    using ValueArray = std::array<float64, EquationCount>;

protected:
    float64 _M_First, _M_Last;

public:
    __ODE_Dense_Output_Base() {}
    __ODE_Dense_Output_Base(float64 _Fi, float64 _La)
        : _M_First(_Fi), _M_Last(_La) {}

    float64 size()const {return _M_Last - _M_First;}

    virtual ValueArray operator()(float64 _Xx)const = 0;
};

///////////////////////////////////RUNGE-KUTTA METHODS///////////////////////////////////

/**
 * @brief Base class for explicit Runge-Kutta methods.
 */
template<uint32_t ErrorEsitmatorOrder, uint32_t StepTakenOrder, uint32_t NStages, uint64 DenseOutputOrder, uint64 EquationCount> requires (EquationCount > 0)
struct __Runge_Kutta_ODE_Engine_Base : public __ODE_Engine_Base<EquationCount>
{
public:
    using _Mybase    = __ODE_Engine_Base<EquationCount>;
    using ValueArray = _Mybase::ValueArray;
    using StateType  = matrix<float64, NStages + 1, EquationCount>;

    struct DenseOutput : public __ODE_Dense_Output_Base<EquationCount>
    {
        using Mybase   = __ODE_Dense_Output_Base<EquationCount>;
        using QTblType = matrix<float64, DenseOutputOrder, EquationCount>;

        QTblType   QTable = QTblType(0);
        ValueArray Base;

        DenseOutput() {}
        DenseOutput(float64 First, float64 Last, ValueArray BaseValue, QTblType QTbl)
            : Mybase(First, Last), Base(BaseValue), QTable(QTbl) {}

        ValueArray operator()(float64 _Xx)const override
        {
            float64 x = (_Xx - Mybase::_M_First) / Mybase::size();
            std::array<float64, DenseOutputOrder> p;
            for (int i = 0; i < DenseOutputOrder; ++i)
            {
                p[i] = pow(x, i + 1);
            }
            matrix<float64, 1, DenseOutputOrder> PMat;
            PMat[0] = p;
            auto y = Mybase::size() * (QTable * PMat)[0];
            return Base + y;
        }

        friend struct __Runge_Kutta_ODE_Engine_Base;
    };

    std::map<float64, DenseOutput> Interpolants;

    void SaveDenseOutput()override
    {
        matrix<float64, DenseOutputOrder, NStages + 1> PTable;
        for (int i = 0; i < DenseOutputOrder; ++i)
        {
            for (int j = 0; j < NStages + 1; ++j)
            {
                PTable[i][j] = *(_T_PTable + (DenseOutputOrder) * j + i);
            }
        }

        decltype(_Mybase::_M_StateBuffer.begin()) PrevState;
        if (_Mybase::_M_Direction)
        {
            PrevState = ++_Mybase::_M_StateBuffer.begin();
        }
        else
        {
            auto end = --_Mybase::_M_StateBuffer.end();
            PrevState = --end;
        }

        Interpolants.insert({_Mybase::CurrentPoint(),
            DenseOutput(PrevState->first, _Mybase::CurrentPoint(), PrevState->second, KTable * PTable)});
    }

    constexpr static const float64 MinFactor  = 0.2;
    constexpr static const float64 MaxFactor  = 10.;
    constexpr static const float64 FactorSafe = 0.9;

protected:
    const float64* _T_CTable;
    const float64* _T_ATable;
    const float64* _T_BTable;
    const float64* _T_ETable;
    const float64* _T_PTable;

    ValueArray     CurrentFx;
    StateType      KTable       = StateType(0);

    float64        RelTolerNLog = 3;
    float64        AbsTolerNLog = 6;
    float64        MaxStep      = __Float64::FromBytes(CSE_POSITIVE_INF);
    float64        AbsStep;
    const float64  ErrExponent  = ErrorEsitmatorOrder + 1;

public:
    __Runge_Kutta_ODE_Engine_Base() : _Mybase() {}

    __Runge_Kutta_ODE_Engine_Base(std::function<typename _Mybase::_Fty> _Right)
        : _Mybase(_Right) {}

    __Runge_Kutta_ODE_Engine_Base(typename _Mybase::_Fty* _PFunc)
        : _Mybase(_PFunc) {}

    template<typename _Functor>
    __Runge_Kutta_ODE_Engine_Base(_Functor _Func)
        : _Mybase(_Func) {}

    template<typename _Functor>
    __Runge_Kutta_ODE_Engine_Base& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    void Init(ValueArray InitState, float64 First, float64 Last, float64 InitStep = __Float64::FromBytes(CSE_NAN))
    {
        _Mybase::_M_StateBuffer.insert({First, InitState});
        _Mybase::_M_EndPoint = Last;
        _Mybase::_M_Invoker(First, InitState, CurrentFx);
        _Mybase::_M_Direction = __Float64(_Mybase::_M_EndPoint - _Mybase::CurrentPoint()).Negative;
        _Mybase::_M_State = _Mybase::Processing;
        if (isnan(InitStep)){SetInitStep();}
        else {AbsStep = InitStep;}
    }

    void SetInitStep()
    {
        if (_Mybase::_M_StateBuffer.empty()) {AbsStep = __Float64::FromBytes(CSE_POSITIVE_INF);}

        float64 RelToler = pow(10, -RelTolerNLog);
        float64 AbsToler = pow(10, -AbsTolerNLog);

        ValueArray y0 = _Mybase::_M_StateBuffer.begin()->second;
        ValueArray Scale = AbsToler + abs(y0) * RelToler;

        float64 d0 = __RMS_Norm(y0 / Scale), d1 = __RMS_Norm(CurrentFx / Scale), h0;

        if (d0 < 1E-5 || d1 < 1E-5) {h0 = 1E-6;}
        else {h0 = 0.01 * d0 / d1;}

        ValueArray y1 = y0 + h0 * (_Mybase::_M_Direction ? -1 : 1) * CurrentFx, f1;
        _Mybase::_M_Invoker(_Mybase::CurrentPoint() + h0 * (_Mybase::_M_Direction ? -1 : 1), y1, f1);

        float64 d2 = __RMS_Norm((f1 - CurrentFx) / Scale) / h0;

        float64 h1;
        if (d1 <= 1E-15 && d2 <= 1E-15){h1 = max(1E-6, h0 * 1E-3);}
        else {h1 = yroot((0.01 / max(d1, d2)), ErrExponent);}

        AbsStep = min(100 * h0, h1);
    };

    int Run()override
    {
        auto t = _Mybase::CurrentPoint();
        ValueArray y = _Mybase::_M_Direction ?
            _Mybase::_M_StateBuffer.begin()->second :
            (--_Mybase::_M_StateBuffer.end())->second;

        float64 RelToler = pow(10, -RelTolerNLog);
        float64 AbsToler = pow(10, -AbsTolerNLog);
        float64 MinStep = 10. * abs((nextafter(t, (_Mybase::_M_Direction ? -1 : 1) * std::numeric_limits<float64>::infinity())) - t);

        float64 AbsH;
        if (AbsStep > MaxStep) {AbsH = MaxStep;}
        else if (AbsStep < MinStep) {AbsH = MinStep;}
        else {AbsH = AbsStep;}

        bool Accept = 0, Reject = 0;
        float64 h, NextT;
        ValueArray NewY, NewF;

        while (!Accept)
        {
            if (AbsH < MinStep) {return 1;}

            h = (_Mybase::_M_Direction ? -1 : 1) * AbsH;
            NextT = t + h;

            if ((_Mybase::_M_Direction ? -1 : 1) * (NextT - _Mybase::_M_EndPoint) > 0)
            {
                NextT = _Mybase::_M_EndPoint;
            }

            h = NextT - t;
            float64 NewAbsStep = abs(h);

            // ---------- Runge-Kutta Solver Begin ---------- //

            KTable[0] = CurrentFx;
            for (int i = 1; i < NStages; ++i)
            {
                ValueArray dy;
                for (int j = 0; j < EquationCount; ++j)
                {
                    dy[j] = 0;
                    for (int k = 0; k < i; ++k)
                    {
                        dy[j] += KTable[k][j] * _T_ATable[i * StepTakenOrder + k];
                    }
                    dy[j] *= h;
                }
                _Mybase::_M_Invoker(t + _T_CTable[i] * h, y + dy, KTable[i]);
            }

            matrix<float64, NStages, EquationCount> KTableWithoutBack(KTable);
            matrix<float64, 1, NStages> BTable;
            for (size_t i = 0; i < NStages; ++i) {BTable[0][i] = *(_T_BTable + i);}
            auto YScale = KTableWithoutBack * BTable;
            NewY = y + h * YScale[0];
            _Mybase::_M_Invoker(t + h, NewY, NewF);
            KTable[NStages] = NewF;

            // ---------- Runge-Kutta solver End ---------- //

            ValueArray Scale = AbsToler + max(y, NewY) * RelToler;

            // ---------- Error Esitmator Begin ---------- //

            matrix<float64, 1, NStages + 1> ETable;
            for (size_t i = 0; i < NStages + 1; ++i) {ETable[0][i] = *(_T_ETable + i);}
            auto EstmErrorMat = KTable * ETable;
            ValueArray EstmError = (EstmErrorMat[0] * h) / Scale;

            float64 EstmErrNorm, EENTempSum = 0;
            for (auto i : EstmError) {EENTempSum += pow(i, 2);}
            EstmErrNorm = sqrt(EENTempSum / float64(EstmError.size()));

            // ---------- Error Esitmator End ---------- //

            float64 Factor;
            if (EstmErrNorm < 1)
            {
                if (EstmErrNorm == 0) {Factor = MaxFactor;}
                else {Factor = min(MaxFactor, FactorSafe * yroot(EstmErrNorm, -ErrExponent));}
                if (Reject) {Factor = min(1, Factor);}
                AbsH *= Factor;
                Accept = 1;
            }
            else
            {
                AbsH *= max(MinFactor, FactorSafe * yroot(EstmErrNorm, -ErrExponent));
                Reject = 1;
            }
        }

        _Mybase::_M_StateBuffer.insert({NextT, NewY});
        AbsStep = AbsH;
        CurrentFx = NewF;

        return 0;
    }

    ValueArray operator()(float64 _Xx)const override
    {
        DenseOutput Segment;
        for (auto i : Interpolants)
        {
            float64 Min = min(i.second._M_First, i.second._M_Last);
            float64 Max = max(i.second._M_First, i.second._M_Last);
            if (Min <= _Xx && _Xx <= Max)
            {
                Segment = i.second;
                break;
            }
        }
        return Segment(_Xx);
    }
};

// Below are implementions of this base class above.

extern const uint64 __RK23_C_Table[3];
extern const uint64 __RK23_A_Table[9];
extern const uint64 __RK23_B_Table[3];
extern const uint64 __RK23_E_Table[4];
extern const uint64 __RK23_P_Table[12];

/**
 * Explicit Runge-Kutta method of order 3(2). (From SciPy)
 *
 * Explicit Runge-Kutta method of order 3(2) [3]. The error is controlled
 * assuming accuracy of the second-order method, but steps are taken using
 * the third-order accurate formula (local extrapolation is done). A cubic
 * Hermite polynomial is used for the dense output. Can be applied in the
 * complex domain.
 *
 * Can be applied in the complex domain.(Not implemented)
 *
 * @link P. Bogacki, L.F. Shampine, “A 3(2) Pair of Runge-Kutta Formulas”,
 * Appl. Math. Lett. Vol. 2, No. 4. pp. 321-325, 1989.
 * https://www.sciencedirect.com/science/article/pii/0893965989900797?via%3Dihub
 */
template<uint64 EquationCount> requires (EquationCount > 0)
class __Single_Precision_Runge_Kutta_Engine : public __Runge_Kutta_ODE_Engine_Base<2, 3, 3, 3, EquationCount>
{
public:
    using _Mybase = __Runge_Kutta_ODE_Engine_Base<2, 3, 3, 3, EquationCount>;

    __Single_Precision_Runge_Kutta_Engine() : _Mybase() {}

    __Single_Precision_Runge_Kutta_Engine(std::function<typename _Mybase::_Fty> _Right)
        : _Mybase(_Right) {}

    __Single_Precision_Runge_Kutta_Engine(typename _Mybase::_Fty* _PFunc)
        : _Mybase(_PFunc) {}

    template<typename _Functor>
    __Single_Precision_Runge_Kutta_Engine(_Functor _Func)
        : _Mybase(_Func) {}

    template<typename _Functor>
    __Single_Precision_Runge_Kutta_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    void Init(_Mybase::ValueArray InitState, float64 First, float64 Last, float64 InitStep = __Float64::FromBytes(CSE_NAN))
    {
        _Mybase::_T_CTable = (float64*)__RK23_C_Table;
        _Mybase::_T_ATable = (float64*)__RK23_A_Table;
        _Mybase::_T_BTable = (float64*)__RK23_B_Table;
        _Mybase::_T_ETable = (float64*)__RK23_E_Table;
        _Mybase::_T_PTable = (float64*)__RK23_P_Table;
        _Mybase::Init(InitState, First, Last, InitStep);
    }
};

extern const float64 __RK45_C_Table[6];
extern const float64 __RK45_A_Table[30];
extern const float64 __RK45_B_Table[6];
extern const float64 __RK45_E_Table[7];
extern const float64 __RK45_P_Table[28];

/**
 * Explicit Runge-Kutta method of order 4 and 5. (From SciPy)
 *
 * This uses the Dormand-Prince pair of formulas [1]. The error is controlled
 * assuming accuracy of the fourth-order method accuracy, but steps are taken
 * using the fifth-order accurate formula (local extrapolation is done).
 * A quartic interpolation polynomial is used for the dense output [2].
 *
 * Can be applied in the complex domain.(Not implemented)
 *
 * @link [1] J. R. Dormand, P. J. Prince, “A family of embedded Runge-Kutta formulae”,
 * Journal of Computational and Applied Mathematics, Vol. 6, No. 1, pp. 19-26, 1980.
 * https://core.ac.uk/download/pdf/81989096.pdf
 * @link [2] L. W. Shampine, “Some Practical Runge-Kutta Formulas”,
 * Mathematics of Computation,, Vol. 46, No. 173, pp. 135-150, 1986.
 * https://www.semanticscholar.org/paper/Some-practical-Runge-Kutta-formulas-Shampine/61b1c882c3c728c2772dd19f75ba41e7b3e5e9af
 */
template<uint64 EquationCount> requires (EquationCount > 0)
class __Double_Precision_Runge_Kutta_Engine : public __Runge_Kutta_ODE_Engine_Base<4, 5, 6, 4, EquationCount>
{
public:
    using _Mybase = __Runge_Kutta_ODE_Engine_Base<4, 5, 6, 4, EquationCount>;

    __Double_Precision_Runge_Kutta_Engine() : _Mybase() {}

    __Double_Precision_Runge_Kutta_Engine(std::function<typename _Mybase::_Fty> _Right)
        : _Mybase(_Right) {}

    __Double_Precision_Runge_Kutta_Engine(typename _Mybase::_Fty* _PFunc)
        : _Mybase(_PFunc) {}

    template<typename _Functor>
    __Double_Precision_Runge_Kutta_Engine(_Functor _Func)
        : _Mybase(_Func) {}

    template<typename _Functor>
    __Double_Precision_Runge_Kutta_Engine& operator=(_Functor _Func)
    {
        return _Mybase::operator=(_Func);
    }

    void Init(_Mybase::ValueArray InitState, float64 First, float64 Last, float64 InitStep = __Float64::FromBytes(CSE_NAN))
    {
        _Mybase::_T_CTable = __RK45_C_Table;
        _Mybase::_T_ATable = __RK45_A_Table;
        _Mybase::_T_BTable = __RK45_B_Table;
        _Mybase::_T_ETable = __RK45_E_Table;
        _Mybase::_T_PTable = __RK45_P_Table;
        _Mybase::Init(InitState, First, Last, InitStep);
    }
};

#if 0
extern const __float128 __DOP853_C_Table[16];
extern const __float128 __DOP853_AB_Table[256];
extern const __float128 __DOP853_E3_Table_Offset[6];
extern const __float128 __DOP853_E5_Table[13];
extern const __float128 __DOP853_D_Table[64];

/**
 * Explicit Runge-Kutta method of order 8. (From SciPy, Unused feature)
 *
 *  This is a Python implementation of "DOP853" algorithm originally written
 *  in Fortran [1], [2]. Note that this is not a literate translation, but
 *  the algorithmic core and coefficients are the same.
 *
 *  Can be applied in the complex domain.(Not implemented)
 *
 *  @link [1] E. Hairer, S. P. Norsett G. Wanner, "Solving Ordinary Differential
 *  Equations I: Nonstiff Problems", Sec. II. (ISBN 978-3-540-56670-0)
 *  @link [2] Page with original Fortran code of DOP853
 *  http://www.unige.ch/~hairer/software.html.
 */
template<uint64 EquationCount> requires (EquationCount > 0)
class __DOP853_Quaduple_Precision_Runge_Kutta_Engine : public __Runge_Kutta_ODE_Engine_Base<7, 8, 12, 4, EquationCount>
{
    // Not implemented...
};
#endif

_SCICXX_END

/**
 * @brief Solve an initial value problem for a system of ODEs.
 * @param _Func - Input Function: the time derivative of the state y at time x.
 * The calling signature is _Func(x, y, Roots&), where x is a scalar and y is
 * an std::array with y.size() = y0.size(). Result will be stored in Roots.
 * @param _Coeffs - Initial state.
 * @param _First, _Last - The solver starts with _First and integrates until it reaches _Last.
 * @return Result function, with the domain of [_First, _Last]
 */
template<typename _Engine, typename _Functor>
_Engine CreateODEFunction(_Functor _Func, typename _Engine::ValueArray _Coeffs, float64 _First, float64 _Last)
{
    _Engine Engine = _Func;
    Engine.Init(_Coeffs, _First, _Last);
    while (Engine.CurrentState() == Engine.Processing)
    {
        Engine.InvokeRun();
        Engine.SaveDenseOutput();
    }
    return Engine;
}

// Default engines
template<uint64 EquationCount> requires (EquationCount > 0)
using DefaultODEFunction = _SCICXX __Double_Precision_Runge_Kutta_Engine<EquationCount>;

/****************************************************************************************\
*                                  Inverting a function                                  *
\****************************************************************************************/

_SCICXX_BEGIN

template<typename _Signature = std::function<float64(float64)>>
class __Basic_Bisection_Searcher
{
public:
    _Signature _M_Invoker;    // The function whose inverse we are trying to find
    float64    _M_MaxIterLog; // The maximum number of iterations to compute
    float64    _M_TolerNLog;  // Stop when iterations change by less than this

    __Basic_Bisection_Searcher() {}

    __Basic_Bisection_Searcher(_Signature _PFunc, float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    __Basic_Bisection_Searcher(float64(*_PFunc)(float64), float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    template<typename _Functor>
    __Basic_Bisection_Searcher(_Functor _PFunc, float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    template<typename _Functor>
    __Basic_Bisection_Searcher& operator=(_Functor _Func)
    {
        this->_M_Invoker = _Func;
        return *this;
    }

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
    float64 operator()(float64 _Xx, float64 _Xa, float64 _Xb)const throw()
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
        return __Float64::FromBytes(CSE_NAN);
    }
};

template<typename _Signature = std::function<float64(float64)>,
         typename _Derivate = __Basic_FDM_Derivative_Engine>
class __Newton_Raphson_Iterator
{
public:
    _Signature _M_Invoker;    // The function whose inverse we are trying to find
    _Derivate  _M_Derivate;   // The derivative of the function
    float64    _M_MaxIterLog; // The maximum number of iterations to compute
    float64    _M_TolerNLog;  // Stop when iterations change by less than this

    __Newton_Raphson_Iterator() {}

    __Newton_Raphson_Iterator(_Signature _PFunc, _Derivate _PDFunc,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_Derivate(_PDFunc),
        _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    __Newton_Raphson_Iterator(float64(*_PFunc)(float64), float64(*_PDFunc)(float64),
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_Derivate(_PDFunc),
        _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    template<typename _Functor>
    __Newton_Raphson_Iterator(_Functor _PFunc, float64(*_PDFunc)(float64),
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_Derivate(_PDFunc),
        _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    template<typename _Functor>
    __Newton_Raphson_Iterator(float64(*_PFunc)(float64), _Functor _PDFunc,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_Derivate(_PDFunc),
        _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

    template<typename _Functor, typename _DFunctor>
    __Newton_Raphson_Iterator(_Functor _PFunc, _DFunctor _PDFunc,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _M_Invoker(_PFunc), _M_Derivate(_PDFunc),
        _M_MaxIterLog(_MaxItLog), _M_TolerNLog(_TolLog) {}

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
    float64 operator()(float64 _Xx, float64 _X0)const throw()
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
        return __Float64::FromBytes(CSE_NAN); // Newton's method did not converge
    }
};

_SCICXX_END

template<typename _Signature = std::function<float64(float64)>>
using BisectionSearcher = _SCICXX __Basic_Bisection_Searcher<_Signature>;

template<typename _Signature = std::function<float64(float64)>,
         typename _Derivate = _SCICXX __Basic_FDM_Derivative_Engine>
using NewtonIterator = _SCICXX __Newton_Raphson_Iterator<_Signature, _Derivate>;

_CSE_END

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
_STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif
