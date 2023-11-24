/************************************************************
  CSpaceEngine Advanced math.
***********************************************************/

#pragma once

#ifndef _ADV_MATH_
#define _ADV_MATH_

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/gltypes.h"
#include <map>
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
*                                Derivates and Integrals                                 *
\****************************************************************************************/

/**
 *  Presets of samples for sample-based integral functions
 */

#define __LO_PREC 4.0000000000000000000000000000000 // Low precision with 10000 sample points
#define __MD_PREC 4.5563025007672872650175335959592 // Mid precision with 36000 sample points
#define __HI_PREC 4.8920946026904804017152719559219 // High precision with 78000 sample points
#define __UT_PREC 5.1335389083702175141813865785018 // Ultra precision with 136000 sample points
#define __EX_PREC 5.2504200023088939799372822644269 // Extreme precision with 178000 sample points

class __Trapezoidal_Engine : public __CSE_AdvMath_Function_Invoker
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

    __Trapezoidal_Engine() : _Mybase(), _M_LogSteps(_DefLogSteps), _M_Method(Uniform) {}

    __Trapezoidal_Engine(std::function<float64(float64)> _Right,
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_Right), _M_LogSteps(_Steps), _M_Method(_Method) {}

    __Trapezoidal_Engine(float64 (*_PFunc)(float64),
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_PFunc), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Trapezoidal_Engine(_Functor _Func,
        float64 _Steps = _DefLogSteps, Method _Method = Uniform)
        : _Mybase(_Func), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Trapezoidal_Engine& operator=(_Functor _Func)
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

class __Basic_Simpson_Engine : public __CSE_AdvMath_Function_Invoker
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

    __Basic_Simpson_Engine() : _Mybase(), _M_LogSteps(_DefLogSteps), _M_Method(CompositeQuadratic) {}

    __Basic_Simpson_Engine(std::function<float64(float64)> _Right,
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_Right), _M_LogSteps(_Steps), _M_Method(_Method) {}

    __Basic_Simpson_Engine(float64 (*_PFunc)(float64),
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_PFunc), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Basic_Simpson_Engine(_Functor _Func,
        float64 _Steps = _DefLogSteps, Method _Method = CompositeQuadratic)
        : _Mybase(_Func), _M_LogSteps(_Steps), _M_Method(_Method) {}

    template<typename _Functor>
    __Basic_Simpson_Engine& operator=(_Functor _Func)
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
 * in normalized function, aka. -1 and 1 for whole-line and 0 or 1 for semi-infinate integrals,
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
