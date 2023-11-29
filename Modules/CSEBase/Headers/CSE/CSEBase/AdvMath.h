/************************************************************
  CSpaceEngine Advanced math Library.

  Description:
    This file contains classes and functions used
    to solve advanced math problems.

  Features:
    1. Convertions between XYZ and polar coordinates
    2. Derivatives and integral engines

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

struct __CSE_AdvMath_Function_Invoker
{
    std::function<float64(float64)> _M_Invoker;

    __CSE_AdvMath_Function_Invoker() {}

    __CSE_AdvMath_Function_Invoker(std::function<float64(float64)> _Right)
        : _M_Invoker(_Right) {}

    __CSE_AdvMath_Function_Invoker(float64 (*_PFunc)(float64))
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

    virtual float64 operator()(float64) = 0;
};

/****************************************************************************************\
*                               Derivatives and Integrals                                *
\****************************************************************************************/

/////////////////////////////////////// DERIVATIVE ///////////////////////////////////////

class __Basic_FDM_Derivative_Engine : public __CSE_AdvMath_Function_Invoker
{
public:
    using _Mybase = __CSE_AdvMath_Function_Invoker;

    std::vector<float64> _M_Coefficients;
    std::vector<float64> _M_CoeffOffsets;
    uint64               _M_DeriOrder;

    enum DiffType
    {
        Central,
        Forward,
        Backward
    }_M_Differences;

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
                CoeffMatrix[i][j] = pow(_M_CoeffOffsets[i], j);
            }
        }

        matrix<float64, 1, CoeffCount> KroneckerMatrix(0);
        for (size_t i = 0; i < CoeffCount; ++i)
        {
            KroneckerMatrix[0][i] = tgamma(DeriOrder + 1) * (i == DeriOrder ? 1. : 0.);
        }

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
                Offsets.push_back(i - OffsetCount);
            }
        }
            break;
        case Forward:
            for (int64 i = 0; i < CoeffCount; ++i)
            {
                Offsets.push_back(i);
            }
            break;
        case Backward:
            for (int64 i = 0; i < CoeffCount; ++i)
            {
                Offsets.push_back(i - int64(CoeffCount));
            }
            break;
        }

        SetCoefficients<decltype(Offsets.begin()), DeriOrder, CoeffCount>(Offsets.begin(), Offsets.end());
    }

    float64 operator()(float64 _Xx);
};

// Default engines
using DerivativeFunction = __Basic_FDM_Derivative_Engine;

//////////////////////////////////////// INTEGRAL ////////////////////////////////////////

/**
 *  Presets of samples for sample-based integral functions
 */

#define __LO_PREC 4.0000000000000000000000000000000 // Low precision with 10000 sample points
#define __MD_PREC 4.5563025007672872650175335959592 // Mid precision with 36000 sample points
#define __HI_PREC 4.8920946026904804017152719559219 // High precision with 78000 sample points
#define __UT_PREC 5.1335389083702175141813865785018 // Ultra precision with 136000 sample points
#define __EX_PREC 5.2504200023088939799372822644269 // Extreme precision with 178000 sample points

class __Trapezoidal_Integral_Engine : public __CSE_AdvMath_Function_Invoker
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker; // pointer to the function to be integrated
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

    float64 _E_NonUniform(_Sample_Type _Samples, bool IsInterval = 0);
    float64 _E_Uniform(float64 _Min, float64 _Max);

    float64 operator()(float64 _Xx); // override
    float64 operator()(float64 _Min, float64 _Max);

    template<typename _Iterator>
    float64 operator()(_Iterator& _First, _Iterator& _Last, bool IsInterval = 0)
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

class __Simpson_Integral_Engine : public __CSE_AdvMath_Function_Invoker
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker; // pointer to the function to be integrated
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

    float64 _E_CompositeQuadratic(_Sample_Type _Samples);
    float64 _E_CompositeCubic(_Sample_Type _Samples);
    float64 _E_Extended(_Sample_Type _Samples);
    float64 _E_NarrowPeaks1(_Sample_Type _Samples);
    float64 _E_NarrowPeaks2(_Sample_Type _Samples);
    float64 _E_Irregularly(_Sample_Type _Samples, bool IsInterval = 0);

    float64 operator()(float64 _Xx); // override
    float64 operator()(float64 _Min, float64 _Max);

    template<typename _Iterator>
    float64 operator()(_Iterator& _First, _Iterator& _Last, bool IsInterval = 0)
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

class __Romberg_Integral_Engine : public __CSE_AdvMath_Function_Invoker
{
public:
    using _Mybase = __CSE_AdvMath_Function_Invoker; // pointer to the function to be integrated

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

    float64 operator()(float64 _Xx); // override
    float64 operator()(float64 _Min, float64 _Max);

    // A debugger for Romberg integration
    matrix<float64, 5, 5> RombergAnalysis(float64 _Min, float64 _Max);
    static matrix<float64, 5, 5> RombergAnalysis(std::function<float64(float64)> _Func, float64 _Min, float64 _Max);
};

class __Infinite_Integral_Nomalizer : public __CSE_AdvMath_Function_Invoker
{
public:
    using _Mybase      = __CSE_AdvMath_Function_Invoker;
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

    float64 operator()(float64);
};

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
__Infinite_Integral_Nomalizer Nomalize(std::function<float64(float64)> PFunc, __Infinite_Integral_Nomalizer::FuncType FuncType = __Infinite_Integral_Nomalizer::WholeLine, float64 Breakpoint = 0, bool AddDefaultSpecialCases = true, std::map<float64, float64> SpecialCases = std::map<float64, float64>());

// Default engines
using IntegralFunction = __Romberg_Integral_Engine;
using NormalizedInfiniteIntegral = __Infinite_Integral_Nomalizer;

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
