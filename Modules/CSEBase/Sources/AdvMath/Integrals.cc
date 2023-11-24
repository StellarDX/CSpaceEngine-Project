#include "CSE/CSEBase/MathFuncs.h"
#include "CSE/CSEBase/AdvMath.h"

#include <iostream>

using namespace std;

_CSE_BEGIN

/**
 * Numerical implementation of Trapezoidal rule
 *
 * @param _Min: lower limit
 * @param _Max: upper limit
 * @param _First - _Last: Begin and end iterator of
 * sampling points for the function values, end iterator is not contained
 *
 * @return: approximation for the integral
 *
 * @link See https://en.wikipedia.org/wiki/Trapezoidal_rule
 * for a more performant implementation.
 */

float64 __Trapezoidal_Engine::operator()(float64 _Xx)
{
    return this->operator()(0, _Xx);
}

float64 __Trapezoidal_Engine::operator()(float64 _Min, float64 _Max)
{
    switch (_M_Method)
    {
    default:
    case NonUniform:
    {
        _Sample_Type _Samples;
        size_t _M_Steps = llround(pow(10, _M_LogSteps));
        float64 _Step = (_Max - _Min) / (_M_Steps - 1);
        float64 i = _Min;
        while (i <= _Max)
        {
            _Samples.push_back(i);
            i += _Step;
        }
        return _E_NonUniform(_Samples);
    }
    case Uniform:
        return _E_Uniform(_Min, _Max);
    }
}

float64 __Trapezoidal_Engine::_E_NonUniform(_Sample_Type _Samples, bool IsInterval)
{
    _Sample_Type _SamplePoints, _SubIntervals;

    if (IsInterval) // Construct sample points
    {
        _SubIntervals = _Sample_Type(_Samples.begin() + 1, _Samples.end());
        float64 _Sum = _Samples.front();
        _SamplePoints.push_back(_Sum);
        for (size_t i = 1; i < _Samples.size(); ++i)
        {
            _Sum += _Samples[i];
            _SamplePoints.push_back(_Sum);
        }
    }
    else // Construct sub-intevals
    {
        _SamplePoints = _Samples;
        sort(_SamplePoints.begin(), _SamplePoints.end());
        for (size_t i = 1; i < _Samples.size(); ++i)
        {
            _SubIntervals.push_back(_Samples[i] - _Samples[i - 1]);
        }
    }

    size_t _N = _SubIntervals.size();
    float64 _Sum = 0;
    for (size_t k = 1; k <= _N; ++k)
    {
        _Sum += (_M_Invoker(_SamplePoints[k - 1]) + _M_Invoker(_SamplePoints[k])) * _SubIntervals[k - 1] / 2.;
    }

    return _Sum;
}

float64 __Trapezoidal_Engine::_E_Uniform(float64 _Min, float64 _Max)
{
    size_t _N_Steps = llround(pow(10, _M_LogSteps)) - 1;
    float64 _DelX = (_Max - _Min) / _N_Steps;
    float64 _Sum = 0;
    float64 _Tail = (_M_Invoker(_Max) + _M_Invoker(_Min)) / 2.;

    for (size_t k = 1; k <= _N_Steps - 1; ++k)
    {
        _Sum += _M_Invoker(_Min + k * _DelX);
    }

    return _DelX * (_Sum + _Tail);
}

/**
 * Simpson rule for irregularly spaced data.
 *
 * @param _Min: lower limit
 * @param _Max: upper limit
 * @param _First - _Last: Begin and end iterator of
 * sampling points for the function values, end iterator is not contained
 *
 * @return: approximation for the integral
 *
 * @link See https://en.wikipedia.org/wiki/Simpson%27s_rule
 * for a more performant implementation.
 */

float64 __Basic_Simpson_Engine::operator()(float64 _Min, float64 _Max)
{
    _Sample_Type _Samples;
    size_t _M_Steps = llround(pow(10, _M_LogSteps));
    float64 _Step = (_Max - _Min) / (_M_Steps - 1);
    float64 i = _Min;
    while (i <= _Max)
    {
        _Samples.push_back(i);
        i += _Step;
    }

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
        _Samples.insert(_Samples.begin(), _Min - _Step);
        _Samples.insert(_Samples.end(), _Max + _Step);
        return _E_NarrowPeaks1(_Samples);
    case NarrowPeaks2:
        return _E_NarrowPeaks2(_Samples);
    case Irregularly:
        return _E_Irregularly(_Samples);
    }
}

float64 __Basic_Simpson_Engine::operator()(float64 _Xx)
{
    return this->operator()(0, _Xx);
}

float64 __Basic_Simpson_Engine::_E_CompositeQuadratic(_Sample_Type _Samples)
{
    if (_Samples.size() < 3)
    {
        throw logic_error("At least 3 sampler points are required but receives " + to_string(_Samples.size()) + "\n");
    }
    sort(_Samples.begin(), _Samples.end());

    float64 h = (_Samples.back() - _Samples.front()) / _Samples.size();
    float64 _Sum = _M_Invoker(_Samples.front());
    for (size_t i = 1; i < _Samples.size() - 1; ++i)
    {
        _Sum += (i % 2 == 0 ? 2. : 4.) * _M_Invoker(_Samples[i]);
    }
    _Sum += _M_Invoker(_Samples.back());

    return (h * _Sum) / 3.;
}

float64 __Basic_Simpson_Engine::_E_CompositeCubic(_Sample_Type _Samples)
{
    if (_Samples.size() < 4)
    {
        throw logic_error("At least 4 sampler points are required but receives " + to_string(_Samples.size()) + "\n");
    }
    sort(_Samples.begin(), _Samples.end());

    float64 h = (_Samples.back() - _Samples.front()) / _Samples.size();
    float64 _Sum = _M_Invoker(_Samples.front());
    for (size_t i = 1; i < _Samples.size() - 1; ++i)
    {
        _Sum += (i % 3 == 0 ? 2. : 3.) * _M_Invoker(_Samples[i]);
    }
    _Sum += _M_Invoker(_Samples.back());

    return 0.375 * h * _Sum;
}

float64 __Basic_Simpson_Engine::_E_Extended(_Sample_Type _Samples)
{
    if (_Samples.size() < 9)
    {
        throw logic_error("At least 9 sampler points are required but receives " + to_string(_Samples.size()) + "\n");
    }
    sort(_Samples.begin(), _Samples.end());

    float64 h = (_Samples.back() - _Samples.front()) / _Samples.size();

    float64 _Sum =
        17. * _M_Invoker(_Samples[0]) +
        59. * _M_Invoker(_Samples[1]) +
        43. * _M_Invoker(_Samples[2]) +
        49. * _M_Invoker(_Samples[3]);

    for (size_t i = 4; i < _Samples.size() - 4; ++i)
    {
        _Sum += 48. * _M_Invoker(_Samples[i]);
    }

    _Sum +=
        49. * _M_Invoker(_Samples[_Samples.size() - 4]) +
        43. * _M_Invoker(_Samples[_Samples.size() - 3]) +
        59. * _M_Invoker(_Samples[_Samples.size() - 2]) +
        17. * _M_Invoker(_Samples[_Samples.size() - 1]);

    return (h * _Sum) / 48.;
}

float64 __Basic_Simpson_Engine::_E_NarrowPeaks1(_Sample_Type _Samples)
{
    if (_Samples.size() < 7) // 5 inside and 2 outside
    {
        throw logic_error("At least 7 sampler points are required but receives " + to_string(_Samples.size()) + "\n");
    }
    sort(_Samples.begin(), _Samples.end());

    float64 h = (_Samples[_Samples.size() - 2] - _Samples[1]) / _Samples.size();

    float64 _Sum =
        -1. * _M_Invoker(_Samples[0]) +
        12. * _M_Invoker(_Samples[1]) +
        25. * _M_Invoker(_Samples[2]);

    for (size_t i = 3; i < _Samples.size() - 3; ++i)
    {
        _Sum += 24. * _M_Invoker(_Samples[i]);
    }

    _Sum +=
        25. * _M_Invoker(_Samples[_Samples.size() - 3]) +
        12. * _M_Invoker(_Samples[_Samples.size() - 2]) +
        -1. * _M_Invoker(_Samples[_Samples.size() - 1]);

    return (h * _Sum) / 24.;
}

float64 __Basic_Simpson_Engine::_E_NarrowPeaks2(_Sample_Type _Samples)
{
    if (_Samples.size() < 7)
    {
        throw logic_error("At least 7 sampler points are required but receives " + to_string(_Samples.size()) + "\n");
    }
    sort(_Samples.begin(), _Samples.end());

    float64 h = (_Samples[_Samples.size() - 1] - _Samples[0]) / _Samples.size();

    float64 _Sum =
         9. * _M_Invoker(_Samples[0]) +
        28. * _M_Invoker(_Samples[1]) +
        23. * _M_Invoker(_Samples[2]);

    for (size_t i = 3; i < _Samples.size() - 3; ++i)
    {
        _Sum += 24. * _M_Invoker(_Samples[i]);
    }

    _Sum +=
        23. * _M_Invoker(_Samples[_Samples.size() - 3]) +
        28. * _M_Invoker(_Samples[_Samples.size() - 2]) +
         9. * _M_Invoker(_Samples[_Samples.size() - 1]);

    return (h * _Sum) / 24.;
}

float64 __Basic_Simpson_Engine::_E_Irregularly(_Sample_Type _Samples, bool IsInterval)
{
    _Sample_Type _SamplePoints, _SubIntervals;

    if (IsInterval) // Construct sample points
    {
        _SubIntervals = _Sample_Type(_Samples.begin() + 1, _Samples.end());
        float64 _Sum = _Samples.front();
        _SamplePoints.push_back(_Sum);
        for (size_t i = 1; i < _Samples.size(); ++i)
        {
            _Sum += _Samples[i];
            _SamplePoints.push_back(_Sum);
        }
    }
    else // Construct sub-intevals
    {
        _SamplePoints = _Samples;
        sort(_SamplePoints.begin(), _SamplePoints.end());
        for (size_t i = 1; i < _Samples.size(); ++i)
        {
            _SubIntervals.push_back(_Samples[i] - _Samples[i - 1]);
        }
    }

    float64 _Sum = 0;

    /**
     * @brief Suppose we divide the interval I into an even number N of subintervals of widths h_k.
     * Then the composite Simpson's rule is given below. In case of an odd number N of subintervals,
     * this formula is used up to the second to last interval
     */
    size_t _N = _SubIntervals.size();
    size_t _Size = (_N % 2 == 0) ? _N : _N - 1;
    for (int i = 0; i <= _Size / 2 - 1; ++i)
    {
        _Sum += (_SubIntervals[2 * i] + _SubIntervals[2 * i + 1]) / 6. *
        ((2. - (_SubIntervals[2 * i + 1] / _SubIntervals[2 * i])) * _M_Invoker(_SamplePoints[2 * i]) +
        (pow(_SubIntervals[2 * i] + _SubIntervals[2 * i + 1], 2) / (_SubIntervals[2 * i] * _SubIntervals[2 * i + 1])) * _M_Invoker(_SamplePoints[2 * i + 1]) +
         (2. - (_SubIntervals[2 * i] / _SubIntervals[2 * i + 1])) * _M_Invoker(_SamplePoints[2 * i + 2]));
    }

    /**
     * @brief The last interval is handled separately by adding the following to the result
     */
    float64 _Tail = 0;
    if (_N % 2 != 0)
    {
        float64 _Alf = (2. * pow(_SubIntervals[_N - 1], 2) + 3. * _SubIntervals[_N - 1] * _SubIntervals[_N - 2]) /
                       (6. * (_SubIntervals[_N - 2] + _SubIntervals[_N - 1]));
        float64 _Bet = (pow(_SubIntervals[_N - 1], 2) + 3. * _SubIntervals[_N - 1] * _SubIntervals[_N - 2]) /
                       (6. * _SubIntervals[_N - 2]);
        float64 _Gam = pow(_SubIntervals[_N - 1], 3) /
                       (6. * _SubIntervals[_N - 2] * (_SubIntervals[_N - 2] + _SubIntervals[_N - 1]));
        _Tail = _Alf * _M_Invoker(_SamplePoints[_N]) +
                _Bet * _M_Invoker(_SamplePoints[_N - 1]) -
                _Gam * _M_Invoker(_SamplePoints[_N - 2]);
    }

    return _Sum + _Tail;
}

/**
 * Romberg's method to estimate the definite integral
 *
 * REFERENCE:
 *     https://en.wikipedia.org/wiki/Romberg%27s_method
 *
 * INPUT:
 *     _Min : lower limit
 *     _Max : upper limit
 *     max_steps: maximum steps of the procedure
 *     acc  : desired accuracy
 *
 * OUTPUT:
 *     Rp[max_steps-1]: approximate value of the integral
 *     of the function f for x in [a,b] with accuracy 'acc'
 *     and steps 'max_steps'.
 */
float64 __Romberg_Integral_Engine::operator()(float64 _Min, float64 _Max)
{
    if (_Min > _Max) {swap(_Min, _Max);}
    float64 R1[_M_MaxSteps], R2[_M_MaxSteps];
    float64* Rp = R1 + 0; // Rp is previous row, Rc is current row
    float64* Rc = R2 + 0;
    float64 h = _Max - _Min; // step size
    Rp[0] = (_M_Invoker(_Min) + _M_Invoker(_Max)) * h * 0.5; // first trapezoidal step

    for (size_t i = 1; i < _M_MaxSteps; ++i)
    {
        h /= 2.;
        float64 c = 0;
        size_t ep = 1 << (i-1); // 2^(n-1)

        for (size_t j = 1; j <= ep; ++j)
        {
            c += _M_Invoker(_Min + (2 * j - 1) * h);
        }

        Rc[0] = h * c + .5 * Rp[0]; // R(i,0)

        for (size_t j = 1; j <= i; ++j)
        {
            float64 n_k = pow(4, j);
            Rc[j] = (n_k * Rc[j - 1] - Rp[j - 1]) / (n_k - 1); // compute R(i,j)
        }

        if (i > 1 && fabs(Rp[i - 1] - Rc[i]) < pow(10, -_M_p_Acc))
        {
            return Rc[i];
        }

        swap(Rp, Rc); // swap Rn and Rc as we only need the last row
    }

    return Rp[_M_MaxSteps - 1]; // return our best guess
}

float64 __Romberg_Integral_Engine::operator()(float64 _Xx)
{
    return this->operator()(0, _Xx);
}

matrix<float64, 5, 5> __Romberg_Integral_Engine::RombergAnalysis(function<float64(float64)> _Func, float64 _Min, float64 _Max)
{
    enum {hm, T, S, C, R};
    matrix<float64, 5, 5> Sequenses(0);
    for (size_t i = 0; i < 5; i++)
    {
        Sequenses[hm][i] = (_Max - _Min) / pow(2, (float64)i);
    }

    auto fa = _Func(_Min);
    auto fb = _Func(_Max);

    auto t0 = (1. / 2.) * (_Max - _Min) * (fa + fb);
    Sequenses[T][0] = t0;

    for (size_t i = 1; i < 5; i++)
    {
        float64 sum = 0;
        for (size_t each = 1; each < pow(2, (float64)i); each += 2)
        {
            sum += Sequenses[hm][i] * _Func(_Min + each * Sequenses[hm][i]);
        }
        auto temp1 = 1. / 2. * Sequenses[T][i - 1];
        auto temp2 = sum;
        auto temp = temp1 + temp2;
        Sequenses[T][i] = temp;
    }

    for (size_t i = 0; i < 4; i++)
    {
        Sequenses[S][i] = (4 * Sequenses[T][i + 1] - Sequenses[T][i]) / 3.;
    }

    for (size_t i = 0; i < 3; i++)
    {
        Sequenses[C][i] = (16 * Sequenses[S][i + 1] - Sequenses[S][i]) / 15.;
    }

    for (size_t i = 0; i < 2; i++)
    {
        Sequenses[R][i] = (64 * Sequenses[C][i + 1] - Sequenses[C][i]) / 63.;
    }

    return Sequenses; // Report
}

matrix<float64, 5, 5> __Romberg_Integral_Engine::RombergAnalysis(float64 _Min, float64 _Max)
{
    return RombergAnalysis(_M_Invoker, _Min, _Max);
}

/**
 * Integrals over infinite intervals
 *
 * @link https://en.wikipedia.org/wiki/Numerical_integration
 *
 * @brief Several methods exist for approximate integration
 * over unbounded intervals. The standard technique involves
 * specially derived quadrature rules, such as Gauss-Hermite
 * quadrature for integrals on the whole real line and Gauss-
 * Laguerre quadrature for integrals on the positive reals.
 * Monte Carlo methods can also be used, or a change of
 * variables to a finite interval;
 */

float64 __Infinite_Integral_Nomalizer::operator()(float64 x)
{
    return gen()(x);
}

float64 __Infinite_Integral_Nomalizer::LowLimit()const
{
    return _M_FuncType == WholeLine ? -1 : 0;
}

float64 __Infinite_Integral_Nomalizer::UpLimit()const
{
    return 1;
}

void __Infinite_Integral_Nomalizer::SetSpecialCase(float64 _Xx, float64 _Fx, bool _Cover)
{
    if (_Cover) {_M_Special_Cases[_Xx] = _Fx;}
    else {_M_Special_Cases.insert({_Xx, _Fx});}
}

void __Infinite_Integral_Nomalizer::DelSpecialCase(float64 _Xx)
{
    _M_Special_Cases.erase(_Xx);
}

constexpr auto __Infinite_Integral_Nomalizer::_K_WholeLine()const
{
    return [this](float64 _Xx)
    {
        auto _SpecialCase = _M_Special_Cases.find(_Xx);
        if (_SpecialCase != _M_Special_Cases.end()) {return _SpecialCase->second;}
        return _M_Invoker(_Xx / (1. - pow(_Xx, 2))) * ((1. + pow(_Xx, 2)) / pow(1. - pow(_Xx, 2), 2));
    };
}

constexpr auto __Infinite_Integral_Nomalizer::_K_HasMinValue()const
{
    return [this](float64 _Xx)
    {
        auto _SpecialCase = _M_Special_Cases.find(_Xx);
        if (_SpecialCase != _M_Special_Cases.end()) {return _SpecialCase->second;}
        return _M_Invoker(_M_Breakpoint + _Xx / (1. - _Xx)) / pow(1. - _Xx, 2);
    };
}

constexpr auto __Infinite_Integral_Nomalizer::_K_HasMaxValue()const
{
    return [this](float64 _Xx)
    {
        auto _SpecialCase = _M_Special_Cases.find(_Xx);
        if (_SpecialCase != _M_Special_Cases.end()) {return _SpecialCase->second;}
        return _M_Invoker(_M_Breakpoint - (1. - _Xx) / _Xx) / pow(_Xx, 2);
    };
}

void __Infinite_Integral_Nomalizer::CreateDefSpecialCases()
{
    switch (_M_FuncType)
    {
    default:
    case WholeLine:
        SetSpecialCase(LowLimit(), 0);
        SetSpecialCase(UpLimit(), 0);
        break;
    case HasMaxValue:
        SetSpecialCase(LowLimit(), 0);
        break;
    case HasMinValue:
        SetSpecialCase(UpLimit(), 0);
        break;
    }
}

function<float64(float64)> __Infinite_Integral_Nomalizer::gen()const
{
    switch (_M_FuncType)
    {
    default:
    case WholeLine:
        return function<float64(float64)>(_K_WholeLine());
    case HasMaxValue:
        return function<float64(float64)>(_K_HasMaxValue());
    case HasMinValue:
        return function<float64(float64)>(_K_HasMinValue());
    }
}

__Infinite_Integral_Nomalizer Nomalize(function<float64(float64)> PFunc, __Infinite_Integral_Nomalizer::FuncType FuncType, float64 Breakpoint, bool AddDefaultSpecialCases, map<float64, float64> SpecialCases)
{
    __Infinite_Integral_Nomalizer NomalizedFunc = PFunc;
    NomalizedFunc._M_FuncType = FuncType;
    NomalizedFunc._M_Breakpoint = Breakpoint;
    NomalizedFunc._M_Special_Cases = SpecialCases;
    if (AddDefaultSpecialCases) {NomalizedFunc.CreateDefSpecialCases();}
    return NomalizedFunc;
}

_CSE_END
