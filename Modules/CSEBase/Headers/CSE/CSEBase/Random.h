// CSpaceEngine random number generator

#pragma once

#ifndef __CSE_RAND_ENGINE__
#define __CSE_RAND_ENGINE__

#include "CSE/CSEBase/CSEBase.h"
#include "CSE/CSEBase/AdvMath.h"

#include <array>
#include <random> // Base on STD random.
#include <vector>

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

namespace Probability {

// Additional Random models

template<class _Ty = float64>
class triangular_distribution : public std::piecewise_linear_distribution<_Ty>
{
public:
    using _Mybase     = std::piecewise_linear_distribution<_Ty>;
    using _Mypbase    = typename _Mybase::param_type;
    using result_type = _Ty;

    triangular_distribution() {}

    triangular_distribution(_Ty _Min0, _Ty _Max0, _Ty _Peak)
    {
        std::array<_Ty, 3> _Values{ _Min0, _Peak, _Max0 };
        std::array<_Ty, 3> _Weights{ 0, 1, 0 };
        _Mybase::_Par = _Mybase::param_type(_Values.begin(), _Values.end(), _Weights.begin());
    }
};

template<class _Ty = float64>
class beta_distribution
{
public:
    using result_type = _Ty;

    _Ty _Ax;
    _Ty _Bx;

    explicit beta_distribution(const _Ty& _Ax0 = _Ty{1}, const _Ty& _Bx0 = _Ty{1}) noexcept
    {
        _Init(_Ax0, _Bx0);
    }

    void _Init(_Ty _Ax0, _Ty _Bx0) noexcept
    {
        // initialize
        if(0.0 >= _Ax0){ throw std::logic_error("invalid a argument for _Beta_distribution");}
        if(0.0 >= _Bx0){ throw std::logic_error("invalid b argument for _Beta_distribution");}
        _Ax = _Ax0;
        _Bx = _Bx0;
    }

    template <class _Engine> // Using generic implemention by Microsoft STL (Apache 2.0 License)
    result_type operator()(_Engine& _Eng) const
    {
        if (_Ax < _Ty{1} && _Bx < _Ty{1})
        {
            // look for a good value
            _Ty _Wx;
            _Ty _Px1;
            _Ty _Px2;
            for (;;)
            {
                // reject large values
                _Px1 = std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng);
                _Px2 = std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng);
                _Px1 = pow(_Px1, _Ty{1} / _Ax);
                _Px2 = pow(_Px2, _Ty{1} / _Bx);
                _Wx  = _Px1 + _Px2;
                if (_Wx <= _Ty{1} && _Wx != _Ty{0}){break;}
            }
            return _Px1 / _Wx;
        }
        else
        {
            // use gamma distributions instead
            _Ty _Px1;
            _Ty _Px2;
            _Ty _PSum;
            std::gamma_distribution<_Ty> _Dist1(_Ax, 1);
            std::gamma_distribution<_Ty> _Dist2(_Bx, 1);

            for (;;)
            {
                // reject pairs whose sum is zero
                _Px1  = _Dist1(_Eng);
                _Px2  = _Dist2(_Eng);
                _PSum = _Px1 + _Px2;

                if (_PSum != _Ty{0}) {break;}
            }

            return _Px1 / _PSum;
        }
    }
};

template <class _Ty = float64, class _CDFty = IntegralFunction>
class _Custom_Distribution_Bi : public BisectionSearcher<_CDFty>
{
public:
    using _Mybase = BisectionSearcher<_CDFty>;
    using result_type = _Ty;
    float64 _MinVal;
    float64 _MaxVal;

    _Custom_Distribution_Bi() {}

    _Custom_Distribution_Bi(_CDFty _CDFunc, float64 _Min, float64 _Max,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _MaxItLog, _TolLog), _MinVal(_Min), _MaxVal(_Max) {}

    _Custom_Distribution_Bi(float64(*_CDFunc)(float64), float64 _Min, float64 _Max,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _MaxItLog, _TolLog), _MinVal(_Min), _MaxVal(_Max) {}

    template<typename _PDFunctor, typename _CDFunctor>
    _Custom_Distribution_Bi(_CDFunctor _CDFunc, float64 _Min, float64 _Max,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _MaxItLog, _TolLog), _MinVal(_Min), _MaxVal(_Max) {}

    template <class _Engine>
    result_type operator()(_Engine& _Eng) const
    {
        return _Mybase::operator()(std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng), _MinVal, _MaxVal);
    }
};

template <class _Ty = float64, class _PDFty = Function1D, class _CDFty = IntegralFunction>
class _Custom_Distribution_NR : public NewtonIterator<_CDFty, _PDFty>
{
public:
    using _Mybase = NewtonIterator<_CDFty, _PDFty>;
    using result_type = _Ty;
    float64 _InitValue;

    _Custom_Distribution_NR() {}

    _Custom_Distribution_NR(_PDFty _PDFunc, _CDFty _CDFunc, float64 _IVal,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _PDFunc, _MaxItLog, _TolLog), _InitValue(_IVal) {}

    _Custom_Distribution_NR(float64(*_PDFunc)(float64), float64(*_CDFunc)(float64), float64 _IVal,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _PDFunc, _MaxItLog, _TolLog), _InitValue(_IVal) {}

    template<typename _Functor>
    _Custom_Distribution_NR(_Functor _PDFunc, float64(*_CDFunc)(float64), float64 _IVal,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _PDFunc, _MaxItLog, _TolLog), _InitValue(_IVal) {}

    template<typename _Functor>
    _Custom_Distribution_NR(float64(*_PDFunc)(float64),  _Functor _CDFunc, float64 _IVal,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _PDFunc, _MaxItLog, _TolLog), _InitValue(_IVal) {}

    template<typename _PDFunctor, typename _CDFunctor>
    _Custom_Distribution_NR(_PDFunctor _PDFunc, _CDFunctor _CDFunc, float64 _IVal,
        float64 _MaxItLog = 3, float64 _TolLog = 8)
        : _Mybase(_CDFunc, _PDFunc, _MaxItLog, _TolLog), _InitValue(_IVal) {}

    template <class _Engine>
    result_type operator()(_Engine& _Eng) const
    {
        return _Mybase::operator()(std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng), _InitValue);
    }
};

/**
 * @brief Fast custom distribution based on Newton iterator, init with a PDF and its CDF.
 */
template <class _Ty = float64, class _PDFty = Function1D, class _CDFty = IntegralFunction>
using fast_custom_distribution = _Custom_Distribution_NR<_Ty, _PDFty, _CDFty>;

/**
 * @brief Custom distribution based on Bisection search, only init with a CDF, slower but safer.
 */
template <class _Ty = float64, class _CDFty = IntegralFunction>
using safe_custom_distribution = _Custom_Distribution_Bi<_Ty, _CDFty>;

// Random Engine
// reference: https://docs.python.org/3/library/random.html

template<typename _Rand_Engine>
class _CSE_Random_Engine
{
public:
    using Engine  = _Rand_Engine;
    using Seed    = uint64;

private:
    _Rand_Engine _Rd;
    Seed _Seed;

public:

    // ---------- Constructors ---------- //

    _CSE_Random_Engine() = delete;
    _CSE_Random_Engine(Seed _Xx0 = 0x1571U) : _Seed(_Xx0)
    {
        _Rd = _Rand_Engine(_Xx0);
    };

    void reset(){ _Rd = _Rand_Engine(_Seed); }

    // Bookkeeping functions

    Seed seed()const{return _Seed;}

    /// <summary>
    /// Initialize the random number generator with a new seed.
    /// </summary>
    Seed seed(Seed _New)
    {
        Seed _Old = _Seed;
        _Seed = _New;
        reset();
        return _Old;
    }

    // ---------- Functions for integers ---------- //

    /**
     * @brief Return a randomly selected element from range(start, stop, step).
     * This is equivalent to choice(range(start, stop, step)), but doesn’t actually build a range object.
     * @param _Begin - Start
     * @param _End - Stop
     * @param _Step - Step
     */
    template<typename genType>
    genType randrange(genType _Begin, genType _End, genType _Step)
    {
        std::vector<genType> _Range;
        for (genType i = _Begin; i < _End; i += _Step)
        {
            _Range.push_back(i);
        }
        std::uniform_int_distribution<uint64> _Gen(0, _Range.size() - 1);
        return _Range[_Gen(_Rd)];
    }

    /**
     * @brief Return a random integer N in the range[a, b]
     * @param _Min0 - Min Value
     * @param _Max0 - Max Value
     */
    int64 randint(int64 _Min0, int64 _Max0)
    {
        return std::uniform_int_distribution<int64>(_Min0, _Max0)(_Rd);
    }

    // ---------- Functions for sequences ---------- //

    /**
     * @brief Return a random element from the non-empty sequence.
     * @param _Begin - Begin iterator
     * @param _End - End iterator
     */
    template<typename Iterator, typename genType>
    genType choice(Iterator _Begin, Iterator _End)
    {
        if(_Begin > _End){throw std::logic_error("Sequence is empty.");}
        std::vector<genType> _Range;
        for (; _Begin < _End; ++_Begin)
        {
            _Range.push_back(*_Begin);
        }
        std::uniform_int_distribution<uint64> _Gen(0, _Range.size() - 1);
        return _Range[_Gen(_Rd)];
    }

    /**
     * @brief Shuffle the sequence x in place.
     * @param _Begin - Begin iterator
     * @param _End - End iterator
     */
    template<typename Iterator>
    void shuffle(Iterator _Begin, Iterator _End)
    {
        if(_Begin > _End){throw std::logic_error("Sequence is empty.");}
        if (_Begin == _End) {return;}

        using _Diff = ptrdiff_t;
        auto _UTarget = _Begin;
        _Diff _Target_index = 1;
        for ( ; ++_UTarget != _End; ++_Target_index)
        {
            // randomly place an element from [_First, _Target] at _Target
            _Diff _Off = randint(0, static_cast<_Diff>(_Target_index));
            if(0 > _Off && _Off > _Target_index){throw std::logic_error("Random value out of range");}
            if (_Off != _Target_index)
            {
                // avoid self-move-assignment
                std::iter_swap(_UTarget, _Begin + _Off);
            }
        }
    }

    // ---------- Real-valued distributions ---------- //

    /**
     * @brief Return the next random floating point number in the range [0.0, 1.0].
     */
    float64 random()
    {
        return std::uniform_real_distribution<float64>(0., 1.)(_Rd);
    }

    /**
     * @brief Return a random floating point number N in the range [a, b]
     * @param _Min0 - Min Value
     * @param _Max0 - Max Value
     */
    float64 uniform(float64 _Min0, float64 _Max0)
    {
        return std::uniform_real_distribution<float64>(_Min0, _Max0)(_Rd);
    }

    /**
     * @brief Return a random floating point number N in [low, high] and with the specified mode between those bounds.
     * @param _Min0 - Min Value, default is 0
     * @param _Max0 - Max Value, default is 1
     * @param _Peak - Peak, default is median of min and max
     */
    float64 triangular(float64 _Min0, float64 _Max0, float64 _Peak)
    {
        return triangular_distribution<float64>(_Min0, _Max0, _Peak)(_Rd);
    }

    float64 triangular()
    {
        return triangular(0., 1., 0.5);
    }

    float64 triangular(float64 _Min0, float64 _Max0)
    {
        return triangular(_Min0, _Max0, (_Min0 + _Max0) / 2.);
    }

    /**
     * @brief Beta distribution.
     * @param _Alf - Alpha, default is 1
     * @param _Bet - Beta, default is 1
     * @return Values range between 0 and 1.
     */
    float64 betavariate(float64 _Alf = 1., float64 _Bet = 1.)
    {
        return beta_distribution<float64>(_Alf, _Bet)(_Rd);
    }

    /**
     * @brief Exponential distribution.
     * @param _Lam - Lambda, default is 1
     * @return Values range from 0 to positive infinity if lambda is
     * positive, and from negative infinity to 0 if lambda is negative.
     */
    float64 expovariate(float64 _Lam = 1.)
    {
        return std::exponential_distribution<float64>(_Lam)(_Rd);
    }

    /**
     * @brief Gamma distribution
     * @param _Alf - shape parameter, default is 1
     * @param _Bet - scale parameter, default is 1
     */
    float64 gammavariate(float64 _Alf = 1., float64 _Bet = 1.)
    {
        return std::gamma_distribution<float64>(_Alf, _Bet)(_Rd);
    }

    float64 kgamma(float64 _K = 1., float64 _Tet = 1.)
    {
        return gammavariate(_K, 1. / _Tet);
    }

    /**
     * @brief Normal distribution, also called the Gaussian distribution.
     * @param _Mean - Mean, default is 0
     * @param _StDev - Standard deviation, default is 1
     */
    float64 normalvariate(float64 _Mean = 0., float64 _StDev = 1.)
    {
        return std::normal_distribution<float64>(_Mean, _StDev)(_Rd);
    }

    /**
     * @brief Log normal distribution.
     * @param _Mean - Mean, default is 0
     * @param _StDev - Standard deviation, default is 1
     */
    float64 lognormvariate(float64 _Mean = 0., float64 _StDev = 1.)
    {
        return std::lognormal_distribution<float64>(_Mean, _StDev)(_Rd);
    }

    /**
     * @brief von Mises distribution
     * @param _Mu - mean angle
     * @param _Kap - concentration parameter
     */
    /*float64 vonmisesvariate(float64 _Mu = 0., float64 _Kap = 1.)
    {
        return vonmises_distribution<float64>(_Mu, _Kap)(_Rd);
    }*/

    /**
     * @brief Pareto distribution
     * @param _Alf - shape parameter
     */
    /*float64 paretovariate(float64 _Alf = 0.)
    {
        return pareto_distribution<float64>(_Alf)(_Rd);
    }*/

    /**
     * @brief Weibull distribution.
     * @param _Lam - scale parameter
     * @param _k - shape parameter
     */
    float64 weibullvariate(float64 _Lam = 0., float64 _k = 1.)
    {
        return std::weibull_distribution<float64>(_Lam, _k)(_Rd);
    }

    /**
     * @brief Generate numbers in probilities
     */
    uint64 probability(std::initializer_list<float64> _Ilist)
    {
        return std::discrete_distribution<uint64>(_Ilist)(_Rd);
    }
};

} // End Probability

extern Probability::_CSE_Random_Engine<std::mersenne_twister_engine
    <unsigned long long, 64, 312, 156, 31, 0xB5026F5AA96619E9ULL, 29,
    0x5555555555555555ULL, 17, 0x71D67FFFEDA60000ULL, 37, 0xFFF7EEE000000000ULL, 43, 0x5851F42D4C957F2DULL>>
    random;

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
