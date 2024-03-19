// GL Vector type from GLM library and expanded to higher dimension.
// Dimenssion 2 to 4 use special defines and other dimensions are
// based on std::array.

#pragma once

#ifndef _TP_GLVECTORT_
#define _TP_GLVECTORT_

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <initializer_list>

#include <ostream>
/*#if __has_include(<fmt/core.h>)
#include <fmt/format.h>
#else
#include <format>
#endif*/

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

/**
 *  @brief A standard container for storing a fixed size sequence of elements.
 *  @ingroup sequences
 *  @tparam Tp Type of element. Required to be a complete type.
 *  @tparam Nm Number of elements.
 */
template<typename _Tp, std::size_t _Nm> //requires (_Nm >= 2)
struct __StelCXX_GLM_GLVector_T;

#define __stelcxx_array_operation(var, expression)\
for (std::size_t var = 0; var < this->size(); ++var) {expression}

#define __stelcxx_array_operation_ext(var, sz, expression)\
for (std::size_t var = 0; var < sz; ++var) {expression}

#define __stelcxx_array_operation_return(var, expression)\
__StelCXX_GLM_GLVector_T<_Tp, _Nm> _Res;\
__stelcxx_array_operation_ext(i, _Nm, expression;)\
return _Res;\

#define __stelcxx_requires_subscript(_N)\
assert(_N < this->size())

#define __stelcxx_requires_nonempty()\
assert(!this->empty())

#define __stelcxx_array_default_member_functions(_Nm)\
constexpr void fill(const value_type& __u)\
{\
    __stelcxx_array_operation(i, this->operator[](i) = __u;)\
}\
constexpr void swap(__StelCXX_GLM_GLVector_T& __other) noexcept\
{\
    __stelcxx_array_operation(i, std::swap(i, __other[i]);)\
}\
constexpr size_type size() const noexcept { return _Nm; }\
constexpr size_type max_size() const noexcept { return _Nm; }\
_NODISCARD constexpr bool empty() const noexcept { return this->size() == 0; }\
constexpr reference at(size_type __n) {return operator[](__n);}\
constexpr const_reference at(size_type __n) const {return operator[](__n);}\
constexpr reference front() noexcept\
{\
    __stelcxx_requires_nonempty();\
    return this->operator[](0);\
}\
constexpr const_reference front() const noexcept\
{\
    __stelcxx_requires_nonempty();\
    return this->operator[](0);\
}\
constexpr reference back() noexcept\
{\
    __stelcxx_requires_nonempty();\
    return this->operator[](size() - 1);\
}\
constexpr const_reference back() const noexcept\
{\
    __stelcxx_requires_nonempty();\
    return this->operator[](size() - 1);\
}

#define __stelcxx_array_unary_arithmetic_operator(_Nm, operation)\
template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>\
constexpr __StelCXX_GLM_GLVector_T& operator operation(_Tp1 scalar)\
{\
    __stelcxx_array_operation(i, this->operator[](i) operation static_cast<_Tp>(scalar);)\
    return *this;\
}\
template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>\
constexpr __StelCXX_GLM_GLVector_T& operator operation(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)\
{\
    __stelcxx_array_operation(i, this->operator[](i) operation static_cast<_Tp>(v[i]);)\
    return *this;\
}

#define __stelcxx_array_unary_operators(_Nm)\
constexpr __StelCXX_GLM_GLVector_T& operator=(__StelCXX_GLM_GLVector_T const& v) = default;\
template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>\
constexpr __StelCXX_GLM_GLVector_T& operator=(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)\
{\
    __stelcxx_array_operation(i, this->operator[](i) = static_cast<_Tp>(v[i]);)\
    return *this;\
}\
__stelcxx_array_unary_arithmetic_operator(_Nm, +=)\
__stelcxx_array_unary_arithmetic_operator(_Nm, -=)\
__stelcxx_array_unary_arithmetic_operator(_Nm, *=)\
__stelcxx_array_unary_arithmetic_operator(_Nm, /=)

#define __stelcxx_vec_array_convertibility(_Nm)\
template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>\
constexpr __StelCXX_GLM_GLVector_T(std::array<_Tp1, _Nm> const& __arr)\
{\
    __stelcxx_array_operation(i, this->operator[](i) = static_cast<_Tp>(__arr[i]);)\
}\
constexpr operator std::array<_Tp, _Nm>()\
{\
    std::array<_Tp, _Nm> __arr;\
    __stelcxx_array_operation(i, __arr[i] = this->operator[](i);)\
    return __arr;\
}

#define __stelcxx_array_binary_operators(operation)\
template<typename _Tp, std::size_t _Nm>\
constexpr __StelCXX_GLM_GLVector_T<_Tp, _Nm> operator operation(__StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v, _Tp scalar)\
{\
    __stelcxx_array_operation_return(i, _Res[i] = v[i] operation scalar)\
}\
template<typename _Tp, std::size_t _Nm>\
constexpr __StelCXX_GLM_GLVector_T<_Tp, _Nm> operator operation(_Tp scalar, __StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v)\
{\
    __stelcxx_array_operation_return(i, _Res[i] = scalar operation v[i])\
}\
template<typename _Tp, std::size_t _Nm>\
constexpr __StelCXX_GLM_GLVector_T<_Tp, _Nm> operator operation(__StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v1, __StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v2)\
{\
    __stelcxx_array_operation_return(i, _Res[i] = v1[i] operation v2[i])\
}

template<typename _Tp>
struct __StelCXX_GLM_GLVector_T<_Tp, 2>
{
    typedef _Tp               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef std::size_t       size_type;

    union {_Tp x, r, s;};
    union {_Tp y, g, t;};

    __stelcxx_array_default_member_functions(2)

    // Element access.
    constexpr reference operator[](size_type __n) noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    constexpr const_reference operator[](size_type __n) const noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    constexpr __StelCXX_GLM_GLVector_T() = default;
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T const& v) = default;

    constexpr explicit __StelCXX_GLM_GLVector_T(_Tp __scalar) {fill(__scalar);}
    constexpr __StelCXX_GLM_GLVector_T(_Tp __x, _Tp __y) : x(__x), y(__y) {}

    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, _Tp2 __y) : x(__x), y(__y) {}

    template<typename _Tp1, std::size_t _Nm>
    requires std::convertible_to<_Tp1, _Tp> && (_Nm > 2)
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)
    {
        __stelcxx_array_operation(i, this->operator[](i) = v[i];)
    }

    template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 2> const& v)
        : x(v[0]), y(v[1]) {}

    __stelcxx_array_unary_operators(2)
    __stelcxx_vec_array_convertibility(2)
};

template<typename _Tp>
struct __StelCXX_GLM_GLVector_T<_Tp, 3>
{
    typedef _Tp               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef std::size_t       size_type;

    union { _Tp x, r, s; };
    union { _Tp y, g, t; };
    union { _Tp z, b, p; };

    __stelcxx_array_default_member_functions(3)

    // Element access.
    constexpr reference operator[](size_type __n) noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    constexpr const_reference operator[](size_type __n) const noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    constexpr __StelCXX_GLM_GLVector_T() = default;
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T const& v) = default;

    constexpr explicit __StelCXX_GLM_GLVector_T(_Tp __scalar) {fill(__scalar);}
    constexpr __StelCXX_GLM_GLVector_T(_Tp __x, _Tp __y, _Tp __z) : x(__x), y(__y), z(__z) {}

    template<typename _Tp1, typename _Tp2, typename _Tp3>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
        && std::convertible_to<_Tp3, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, _Tp2 __y, _Tp3 __z) : x(__x), y(__y), z(__z) {}

    template<typename _Tp1, std::size_t _Nm>
    requires std::convertible_to<_Tp1, _Tp> && (_Nm > 3)
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)
    {
        __stelcxx_array_operation(i, this->operator[](i) = v[i];)
    }
    template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 3> const& v)
        : x(v[0]), y(v[1]), z(v[2]) {}

    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 2> const& __xy, _Tp2 __z)
        : x(__xy[0]), y(__xy[1]), z(__z) {}
    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, __StelCXX_GLM_GLVector_T<_Tp2, 2> const& __yz)
        : x(__x), y(__yz[0]), z(__yz[1]) {}

    __stelcxx_array_unary_operators(3)
    __stelcxx_vec_array_convertibility(3)
};

template<typename _Tp>
struct __StelCXX_GLM_GLVector_T<_Tp, 4>
{
    typedef _Tp               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef std::size_t       size_type;

    union { _Tp x, r, s; };
    union { _Tp y, g, t; };
    union { _Tp z, b, p; };
    union { _Tp w, a, q; };

    __stelcxx_array_default_member_functions(4)

    // Element access.
    constexpr reference operator[](size_type __n) noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    constexpr const_reference
    operator[](size_type __n) const noexcept
    {
        __stelcxx_requires_subscript(__n);
        switch(__n)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    constexpr __StelCXX_GLM_GLVector_T() = default;
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T const& v) = default;

    constexpr explicit __StelCXX_GLM_GLVector_T(_Tp __scalar) {fill(__scalar);}
    constexpr __StelCXX_GLM_GLVector_T(_Tp __x, _Tp __y, _Tp __z, _Tp __w)
        : x(__x), y(__y), z(__z), w(__w) {}

    template<typename _Tp1, typename _Tp2, typename _Tp3, typename _Tp4>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
        && std::convertible_to<_Tp3, _Tp> && std::convertible_to<_Tp4, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, _Tp2 __y, _Tp3 __z, _Tp4 __w)
        : x(__x), y(__y), z(__z), w(__w) {}

    template<typename _Tp1, std::size_t _Nm>
    requires std::convertible_to<_Tp1, _Tp> && (_Nm > 4)
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)
    {
        __stelcxx_array_operation(i, this->operator[](i) = v[i];)
    }
    template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 4> const& v)
        : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

    template<typename _Tp1, typename _Tp2, typename _Tp3>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
        && std::convertible_to<_Tp3, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 2> const& __xy, _Tp2 __z, _Tp3 __w)
        : x(__xy[0]), y(__xy[1]), z(__z), w(__w) {}
    template<typename _Tp1, typename _Tp2, typename _Tp3>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
        && std::convertible_to<_Tp3, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, __StelCXX_GLM_GLVector_T<_Tp2, 2> const& __yz, _Tp3 __w)
        : x(__x), y(__yz[0]), z(__yz[1]), w(__w) {}
    template<typename _Tp1, typename _Tp2, typename _Tp3>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
        && std::convertible_to<_Tp3, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, _Tp2 __y, __StelCXX_GLM_GLVector_T<_Tp3, 2> const& __zw)
        : x(__x), y(__y), z(__zw[0]), w(__zw[1]) {}
    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 3> const& __xyz, _Tp2 __w)
        : x(__xyz[0]), y(__xyz[1]), z(__xyz[2]), w(__w) {}
    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(_Tp1 __x, __StelCXX_GLM_GLVector_T<_Tp2, 3> const& __yzw)
        : x(__x), y(__yzw[0]), z(__yzw[1]), w(__yzw[2]) {}
    template<typename _Tp1, typename _Tp2>
    requires std::convertible_to<_Tp1, _Tp> && std::convertible_to<_Tp2, _Tp>
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, 2> const& __xy,
        __StelCXX_GLM_GLVector_T<_Tp2, 2> const& __zw)
        : x(__xy[0]), y(__xy[1]), z(__zw[0]), w(__zw[1]) {}

    __stelcxx_array_unary_operators(4)
    __stelcxx_vec_array_convertibility(4)
};

template<typename _Tp, std::size_t _Nm> //requires (_Nm >= 2)
struct __StelCXX_GLM_GLVector_T : public std::array<_Tp, _Nm>
{
    using _Mybase = std::array<_Tp, _Nm>;

    constexpr __StelCXX_GLM_GLVector_T() = default;
    constexpr __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T const& v) = default;
    constexpr explicit __StelCXX_GLM_GLVector_T(_Tp __scalar) {this->fill(__scalar);}

    template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>
    constexpr explicit __StelCXX_GLM_GLVector_T(__StelCXX_GLM_GLVector_T<_Tp1, _Nm> const& v)
    {
        __stelcxx_array_operation(i, this->operator[](i) = v[i];)
    }

    template<typename _Tp1> requires std::convertible_to<_Tp1, _Tp>
    constexpr explicit __StelCXX_GLM_GLVector_T(std::initializer_list<_Tp1> const& _Ilist)
    {
        size_t i = 0;
        for (auto v : _Ilist)
        {
            this->operator[](i) = v;
            ++i;
            if (i == this->size()) {break;}
        }
    }

    __stelcxx_array_unary_operators(_Nm)
    __stelcxx_vec_array_convertibility(_Nm)
};

template<typename _Tp, typename... _Up>
__StelCXX_GLM_GLVector_T(_Tp, _Up...)
    -> __StelCXX_GLM_GLVector_T<std::enable_if_t<(std::is_same_v<_Tp, _Up> && ...), _Tp>,
    1 + sizeof...(_Up)>;

template<typename _Tp, std::size_t _Nm>
constexpr __StelCXX_GLM_GLVector_T<_Tp, _Nm> operator+(__StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v)
{
    return v;
}

template<typename _Tp, std::size_t _Nm>
constexpr __StelCXX_GLM_GLVector_T<_Tp, _Nm> operator-(__StelCXX_GLM_GLVector_T<_Tp, _Nm> const& v)
{
    __StelCXX_GLM_GLVector_T<_Tp, _Nm> _Res;
    __stelcxx_array_operation_ext(i, _Nm, _Res[i] = -v[i];)
    return _Res;
}

__stelcxx_array_binary_operators(+)
__stelcxx_array_binary_operators(-)
__stelcxx_array_binary_operators(*)
__stelcxx_array_binary_operators(/)

template<typename _Tp, std::size_t _Nm>
std::ostream& operator<<(std::ostream& os, __StelCXX_GLM_GLVector_T<_Tp, _Nm> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << ", " << vec[i];
    }
    os << ')';
    return os;
}

template<typename _Tp, std::size_t _Nm>
std::wostream& operator<<(std::wostream& os, __StelCXX_GLM_GLVector_T<_Tp, _Nm> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << L", " << vec[i];
    }
    os << ')';
    return os;
}

/*#if __has_include(<format>) || __has_include(<fmt/core.h>)
#if __has_include(<fmt/core.h>)
#define _FMT_NS fmt::
#else
#define _FMT_NS std::
#endif
template<typename _Tp, std::size_t _Nm>
struct _FMT_NS formatter<__StelCXX_GLM_GLVector_T<_Tp, _Nm>>
{
    using _Mytype = __StelCXX_GLM_GLVector_T<_Tp, _Nm>;

    constexpr auto parse(_FMT_NS format_parse_context& ctx)
    {
        // TODO...
    }

    auto format(const _Mytype& _Vec, _FMT_NS format_context& ctx) const
    {
        // TODO...
    }
};
#endif*/

#endif
