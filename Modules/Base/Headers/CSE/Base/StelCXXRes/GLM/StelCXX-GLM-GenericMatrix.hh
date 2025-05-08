// Generic Matrix standard header (GLSL)

// Reference:
// [GLSL Document]https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.20.pdf
// [GLM by G-Truc]https://glm.g-truc.net/0.9.4/api/a00142.html#gade0eb47c01f79384a6f38017ede17446
// [Qt Document]https://doc.qt.io/qt-6/qgenericmatrix.html
// [Introduction to Linear Algebra(5th Edition) by Gilbert Strang]
// https://github.com/Rumi381/All-Programming-E-Books-PDF/blob/master/Books/Introduction%20to%20Linear%20Algebra,%20Fifth%20Edition%20by%20Gilbert%20Strang%20(z-lib.org).pdf

// This matrix is an integration of 3 existing matrix types.
// They are 2-dimensional std::array, matrices in glm and QGenericMatrix.
// Linear algebra is also supported.

// note: std::linalg is only supported in C++26 or later standards.

#pragma once

#ifndef _GENERIC_MATRIX_
#define _GENERIC_MATRIX_

#include <array>
#include <initializer_list>
#include <ostream>

/*#if __has_include(<fmt/core.h>)
#include <fmt/format.h>
#else
#include <format>
#endif*/

template<typename _Tp, std::size_t _Nm> //requires (_Nm >= 2)
struct __StelCXX_GLM_GLVector_T;

#define __stelcxx_generic_matrix_operation(code)\
for (std::size_t col = 0; col < _Col; ++col)\
{\
    for (std::size_t row = 0; row < _Row; ++row)\
    {\
        code\
    }\
}

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

/**
 * @brief The __StelCXX_GLM_Generic_Matrix class represents a NxM
 * transformation matrix with N columns and M rows
 * @tparam _Tp  Element type that is visible to users of the class.
 * @tparam _Col Number of columns.
 * @tparam _Row Number of rows.
 */
template<typename _Tp, std::size_t _Col, std::size_t _Row>
requires (_Col > 0) && (_Row > 0)
struct __StelCXX_GLM_Generic_Matrix :
public std::array<__StelCXX_GLM_GLVector_T<_Tp, _Row>, _Col>  // Column-major order to match OpenGL.
{
    using _Mybase = std::array<__StelCXX_GLM_GLVector_T<_Tp, _Row>, _Col>;

    typedef _Tp               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef std::size_t       size_type;

    typedef __StelCXX_GLM_GLVector_T<_Tp, _Row>           col_type;
    typedef __StelCXX_GLM_GLVector_T<_Tp, _Col>           row_type;
    typedef __StelCXX_GLM_Generic_Matrix<_Tp, _Row, _Col> transpose_type;

    _NODISCARD transpose_type Transpose() const
    {
        transpose_type result;
        __stelcxx_generic_matrix_operation
        (
            result[row][col] = this->operator[](col)[row];
        )
        return result;
    }

    __StelCXX_GLM_Generic_Matrix() = default;

    /**
     * @brief Constructs a matrix from the given N * M values.
     * The contents of the array values is assumed to be in row-major order.
     */
    explicit __StelCXX_GLM_Generic_Matrix(const pointer _Ptr)
    {
        __stelcxx_generic_matrix_operation
        (
            this->operator[](col)[row] = _Ptr[row * _Col + col];
        )
    }

    constexpr __StelCXX_GLM_Generic_Matrix(__StelCXX_GLM_Generic_Matrix const& m)
        : _Mybase(m) {};

    explicit constexpr __StelCXX_GLM_Generic_Matrix(_Tp scalar)
    {
        __stelcxx_generic_matrix_operation
        (
            if (row == col) {this->operator[](col)[row] = scalar;}
            else {this->operator[](col)[row] = 0;}
        )
    }

    /**
     * @brief Constructs a matrix from the given N * M values (Initializer list version).
     * The contents of the array values is assumed to be in row-major order.
     */
    constexpr __StelCXX_GLM_Generic_Matrix(std::initializer_list<_Tp> _Ilist)
    {
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (std::size_t row = 0; row < _Row; ++row)
        {
            for (std::size_t col = 0; col < _Col; ++col)
            {
                this->operator[](col)[row] = *it;
                ++it;
                if (it == end) {break;}
            }
            if (it == end) {break;}
        }
    }

    /**
     * @brief Constructs a matrix from the given column vectors.
     * The contents of the array values is assumed to be in column-major order.
     */
    constexpr __StelCXX_GLM_Generic_Matrix(std::initializer_list<col_type> _Ilist)
    {
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (std::size_t col = 0; col < _Col; ++col)
        {
            this->operator[](col) = *it;
            ++it;
            if (it == end) {break;}
        }
    }

    // GLM adds conversion constructors for its matrix types,
    // but in this case it can only be implemented with variadic templates

    /*template<typename... _Args> requires (std::convertible_to<_Args, _Tp> && ...)
    constexpr __StelCXX_GLM_Generic_Matrix(_Args... Args)
    {
        // TODO...
    }

    template<typename... _Args> requires (std::convertible_to<_Args, _Tp> && ...)
    constexpr __StelCXX_GLM_Generic_Matrix(__StelCXX_GLM_GLVector_T<_Args, _Row>... Args)
    {
        // TODO...
    }*/

    template<std::size_t _Col1, std::size_t _Row1>
    explicit constexpr __StelCXX_GLM_Generic_Matrix(__StelCXX_GLM_Generic_Matrix<_Tp, _Col1, _Row1> right)
    {
        for (std::size_t col = 0; col < (_Col > _Col1 ? _Col1 : _Col); ++col)
        {
            for (std::size_t row = 0; row < (_Row > _Row1 ? _Row1 : _Row) ; ++row)
            {
                this->operator[](col)[row] = right[col][row];
            }
        }
    }

    void fill(_Tp value)
    {
        __stelcxx_generic_matrix_operation(this->operator[](col)[row] = value;)
    }

    __StelCXX_GLM_Generic_Matrix& operator+=(const __StelCXX_GLM_Generic_Matrix& other)
    {
        __stelcxx_generic_matrix_operation(this->operator[](col)[row] += other[col][row];)
        return *this;
    }

    __StelCXX_GLM_Generic_Matrix& operator-=(const __StelCXX_GLM_Generic_Matrix& other)
    {
        __stelcxx_generic_matrix_operation(this->operator[](col)[row] -= other[col][row];)
        return *this;
    }

    __StelCXX_GLM_Generic_Matrix& operator*=(_Tp factor)
    {
        __stelcxx_generic_matrix_operation(this->operator[](col)[row] *= factor;)
        return *this;
    }

    __StelCXX_GLM_Generic_Matrix& operator/=(_Tp divisor)
    {
        __stelcxx_generic_matrix_operation(this->operator[](col)[row] /= divisor;)
        return *this;
    }

    bool operator==(const __StelCXX_GLM_Generic_Matrix& other) const
    {
        __stelcxx_generic_matrix_operation
        (
            if (this->operator[](col)[row] != other[col][row])
            {
                return false;
            }
        )
        return true;
    }

    bool operator!=(const __StelCXX_GLM_Generic_Matrix& other) const
    {
        return !(*this == other);
    }
};

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator+(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& m1,
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& m2)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = m1[col][row] + m2[col][row];)
    return result;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator-(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& m1,
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& m2)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = m1[col][row] - m2[col][row];)
    return result;
}

/**
 * @brief operator* Matrix product in linear algebra
 */
template<typename _Tp, std::size_t _Col, std::size_t _Sz, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator*(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Sz, _Row>& m1,
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Sz>& m2)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    for (int row = 0; row < _Row; ++row)
    {
        for (int col = 0; col < _Col; ++col)
        {
            _Tp sum(0.0);
            for (int j = 0; j < _Sz; ++j) {sum += m1[j][row] * m2[col][j];}
            result[col][row] = sum;
        }
    }
    return result;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator+(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& matrix)
{
    return matrix;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator-(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& matrix)
{
     __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = -matrix[col][row];)
    return result;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator*(_Tp factor,
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& matrix)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = matrix[col][row] * factor;)
    return result;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator*(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& matrix, _Tp factor)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = matrix[col][row] * factor;)
    return result;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
inline __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> operator/(
    const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>& matrix, _Tp divisor)
{
    __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> result;
    __stelcxx_generic_matrix_operation(result[col][row] = matrix[col][row] / divisor;)
    return result;
}

/**
 * @brief operator<< OpenCV style output
 */
template<typename _Tp, std::size_t _Col, std::size_t _Row>
std::ostream& operator<<(std::ostream& dbg, const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> &m)
{
    dbg << '[';
    for (int row = 0; row < _Row; ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < _Col; ++col)
        {
            dbg << m[col][row];
            if (row < _Row - 1 || col < _Col - 1) {dbg << ", ";}
        }
        if (row < _Row - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}

template<typename _Tp, std::size_t _Col, std::size_t _Row>
std::wostream& operator<<(std::wostream& dbg, const __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row> &m)
{
    dbg << '[';
    for (int row = 0; row < _Row; ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < _Col; ++col)
        {
            dbg << m[col][row];
            if (row < _Row - 1 || col < _Col - 1) {dbg << L", ";}
        }
        if (row < _Row - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}

/*#if __has_include(<format>) || __has_include(<fmt/core.h>)
#if __has_include(<fmt/core.h>)
#define _FMT_NS fmt::
#else
#define _FMT_NS std::
#endif
template<typename _Tp, std::size_t _Col, std::size_t _Row>
struct _FMT_NS formatter<__StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>>
{
    using _Mytype = __StelCXX_GLM_Generic_Matrix<_Tp, _Col, _Row>;

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
