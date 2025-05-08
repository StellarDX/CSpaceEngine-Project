/********************************************************************************
  CSpaceEngine高等数学库，也称"SciC++"
  CSpaceEngine Advanced math Library, also called "SciC++"

  介绍 / Description:
    这些功能主要用于解决一些复杂度极高的数学问题
    This file contains classes and functions used to solve math problems
    with very high complexity.

  功能 / Features:
    1. 坐标转换(不在独立命名空间)
    2. 导数
    3. 积分

  「从前有棵树，叫高数，上面挂了很多人；」
  「旁边有座坟，叫微积分，里面葬了很多人。」
********************************************************************************/

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
#include "CSE/Base/CSEBase.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/GLTypes.h"
#include "CSE/Base/LinAlg.h"
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

#define _SCICXX_BEGIN namespace SciCxx {
#define _SCICXX SciCxx::
#define _SCICXX_END }

/****************************************************************************************\
*                                         坐标转换                                        *
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


_SCICXX_BEGIN

using Function1D = std::function<float64(float64)>;

/****************************************************************************************\
*                                         变长矩阵                                        *
\****************************************************************************************/

/**
 * @brief 可变大小的矩阵，功能与定长矩阵一样
 */
template<typename _Ty>
class DynamicMatrix
{
public:
    typedef _Ty               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef uvec2             size_type;

    typedef std::vector<_Ty>  col_type;
    typedef std::vector<_Ty>  row_type;
    typedef DynamicMatrix     transpose_type;

private:
    std::vector<_Ty> Data; // 列向量降维储存
    size_type Size = size_type(0, 0);

public:
    DynamicMatrix() = default;

    /**
     * @brief 创建一个任意大小的矩阵
     */
    explicit DynamicMatrix(size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
    }

    /**
     * @brief 使用指针创建矩阵，并指定大小。指针对应的数组存储方式为行向量。
     */
    explicit DynamicMatrix(const pointer _Ptr, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        for (std::size_t col = 0; col < _Sz.x; ++col)
        {
            for (std::size_t row = 0; row < _Sz.y; ++row)
            {
                at(col, row) = _Ptr[col + row * _Sz.x];
            }
        }
    }

    constexpr DynamicMatrix(DynamicMatrix const& m)
        : Data(m.Data), Size(m.Size) {};

    /**
     * @brief 使用元素创建矩阵，并指定大小。此元素将被填充入矩阵的主对角线。
     */
    explicit constexpr DynamicMatrix(_Ty scalar, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        for (int i = 0; i < min(_Sz.x, _Sz.y); ++i)
        {
            at(i, i) = scalar;
        }
    }

    /**
     * @brief 使用列表创建矩阵，并指定大小。指针对应的数组存储方式为行向量。
     */
    constexpr DynamicMatrix(std::initializer_list<_Ty> _Ilist, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (std::size_t row = 0; row < _Sz.y; ++row)
        {
            for (std::size_t col = 0; col < _Sz.x; ++col)
            {
                at(col, row) = *it;
                ++it;
                if (it == end) {break;}
            }
            if (it == end) {break;}
        }
    }

    /**
     * @brief 使用列向量创建矩阵，大小以列向量的个数和元素最多的列向量为准。
     */
    constexpr DynamicMatrix(std::initializer_list<col_type> _Ilist)
    {
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (; it < end; ++it)
        {
            Size.y = it->size() > Size.y ? it->size() : Size.y;
        }

        it = _Ilist.begin();
        end = _Ilist.end();
        for (; it < end; ++it)
        {
            Data.insert(Data.end(), it->begin(), it->end());
            ++Size.x;
            Data.resize(Size.x * Size.y);
        }
    }

    template<typename _Ty2, std::size_t _Nm> requires std::convertible_to<_Ty2, _Ty>
    constexpr DynamicMatrix(__StelCXX_GLM_GLVector_T<_Ty2, _Nm> Right) : Size(1, _Nm)
    {
        Data.resize(this->col() * this->row());
        for (int i = 0; i < _Nm; ++i)
        {
            Data[i] = Right[i];
        }
    }

    template<typename _Ty2, std::size_t _Col, std::size_t _Row> requires std::convertible_to<_Ty2, _Ty>
    constexpr DynamicMatrix(__StelCXX_GLM_Generic_Matrix<_Ty2, _Col, _Row> Right) : Size(_Col, _Row)
    {
        Data.resize(this->col() * this->row());
        for (std::size_t col = 0; col < _Col; ++col)
        {
            for (std::size_t row = 0; row < _Row; ++row)
            {
                at(col, row) = Right[col][row];
            }
        }
    }

    _NODISCARD _CONSTEXPR20 size_type size() const noexcept {return Size;}

    _CONSTEXPR20 void resize(size_type NewSize)
    {
        if (NewSize.x < this->col())
        {
            Data.erase(Data.begin() + NewSize.x * this->row(), Data.end());
        }
        else if (NewSize.x > this->col())
        {
            for (int i = 0; i < (NewSize.x - this->col()) * this->row(); ++i)
            {
                Data.push_back(_Ty());
            }
        }
        Size.x = NewSize.x;

        if (NewSize.y < this->row())
        {
            std::size_t EraseSize = this->row() - NewSize.y;
            std::size_t OffsetSize = 0;
            for (std::size_t i = 0; i < this->col(); ++i)
            {
                Data.erase(Data.begin() + ((i + 1) * NewSize.y),
                    Data.begin() + ((i + 1) * this->row() - OffsetSize));
                OffsetSize += EraseSize;
            }
        }
        else if (NewSize.y > this->row())
        {
            std::size_t InsertSize = NewSize.y - this->row();
            std::size_t OffsetSize = 0;
            for (std::size_t i = 0; i < this->col(); ++i)
            {
                std::vector<_Ty> Insert(InsertSize);
                Data.insert(Data.begin() + (i + 1) * this->row() + OffsetSize,
                    Insert.begin(), Insert.end());
                OffsetSize += InsertSize;
            }
        }
        Size.y = NewSize.y;
    }

    _NODISCARD _CONSTEXPR20 std::size_t col()const noexcept {return Size.x;}
    _NODISCARD _CONSTEXPR20 std::size_t row()const noexcept {return Size.y;}

    _NODISCARD _CONSTEXPR20 bool empty() const noexcept {return Data.empty();}

    _NODISCARD _CONSTEXPR20 col_type operator[](size_t row)const noexcept
    {
        return GetColumn(row);
    }

protected:
    void _M_throw_out_of_range()const {throw std::logic_error("Dynamic Matrix index out of range.");}

    _CONSTEXPR20 void _M_range_check(size_type __n)const
    {
        if (__n.x >= this->col() || __n.y >= this->row())
        {
            _M_throw_out_of_range();
        }
    }

public:
    _CONSTEXPR20 reference at(size_type pos)
    {
        return at(pos.x, pos.y);
    }

    _CONSTEXPR20 reference at(size_t col, size_t row)
    {
        _M_range_check({col, row});
        return Data[col * this->row() + row];
    }

    _CONSTEXPR20 const_reference at(size_type pos)const
    {
        return at(pos.x, pos.y);
    }

    _CONSTEXPR20 const_reference at(size_t col, size_t row)const
    {
        _M_range_check({col, row});
        return Data[col * this->row() + row];
    }

    void fill(_Ty value)
    {
        std::fill(Data.begin(), Data.end(), value);
    }

    // ------------------------------ CURD ------------------------------ //

    /**
     * @brief 添加整列
     */
    _CONSTEXPR20 void AddColumn(size_t pos, const col_type& col)noexcept
    {
        if (pos > this->col()) {_M_throw_out_of_range();}
        if (col.size() >= this->row())
        {
            Data.insert(Data.begin() + (pos * this->row()), col.begin(), col.begin() + this->row());
        }
        else
        {
            col_type NewCol(col);
            NewCol.resize(this->row());
            Data.insert(Data.begin() + (pos * this->row()), NewCol.begin(), NewCol.end());
        }
        ++Size.x;
    }

    /**
     * @brief 添加整行
     */
    _CONSTEXPR20 void AddRow(size_t pos, const row_type& row)noexcept
    {
        if (pos > this->row()) {_M_throw_out_of_range();}
        std::size_t OffsetSize = 0;
        for (std::size_t i = 0; i < this->col(); ++i)
        {
            Data.insert(Data.begin() + i * this->row() + pos + OffsetSize,
                row.size() > i ? row[i] : _Ty());
            ++OffsetSize;
        }
        ++Size.y;
    }

    /**
     * @brief 设置整列
     */
    _CONSTEXPR20 void SetColumn(size_t pos, const col_type& col)noexcept
    {
        for (size_t i = 0; i < this->row(); ++i)
        {
            at(pos, i) = col[i];
        }
    }

    /**
     * @brief 设置整行
     */
    _CONSTEXPR20 void SetRow(size_t pos, const row_type& row)noexcept
    {
        for (size_t i = 0; i < this->col(); ++i)
        {
            at(i, pos) = row[i];
        }
    }

    /**
     * @brief 获取整列
     */
    _NODISCARD _CONSTEXPR20 col_type GetColumn(size_t pos)const noexcept
    {
        col_type col(this->row());
        for (size_t i = 0; i < this->row(); ++i)
        {
            col[i] = at(pos, i);
        }
        return col;
    }

    /**
     * @brief 获取整行
     */
    _NODISCARD _CONSTEXPR20 row_type GetRow(size_t pos)const noexcept
    {
        row_type row(this->col());
        for (size_t i = 0; i < this->col(); ++i)
        {
            row[i] = at(i, pos);
        }
        return row;
    }

    /**
     * @brief 删除整列
     */
    _CONSTEXPR20 void DeleteColumn(size_t pos)noexcept
    {
        if (pos > this->col()) {_M_throw_out_of_range();}
        Data.erase(Data.begin() + (pos * this->row()), Data.begin() + (pos * this->row() + 1));
        --Size.x;
    }

    /**
     * @brief 删除整行
     */
    _CONSTEXPR20 void DeleteRow(size_t pos)noexcept
    {
        if (pos > this->row()) {_M_throw_out_of_range();}
        std::size_t OffsetSize = 0;
        for (std::size_t i = 0; i < this->col(); ++i)
        {
            Data.erase(Data.begin() + (i * this->row() + pos - OffsetSize));
            ++OffsetSize;
        }
        --Size.y;
    }

    // ----------------------------- 运算符 ----------------------------- //

    DynamicMatrix& operator+=(const DynamicMatrix& other)
    {
        if (other.col() != col() || other.row() != row())
        {
            throw std::logic_error("Size of matrices is not equal.");
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) += other.at(col, row);
            }
        }
        return *this;
    }

    DynamicMatrix& operator-=(const DynamicMatrix& other)
    {
        if (other.col() != col() || other.row() != row())
        {
            throw std::logic_error("Size of matrices is not equal.");
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) -= other.at(col, row);
            }
        }
        return *this;
    }

    DynamicMatrix& operator*=(_Ty factor)
    {
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) *= factor;
            }
        }
        return *this;
    }

    DynamicMatrix& operator/=(_Ty divisor)
    {
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) /= divisor;
            }
        }
        return *this;
    }

    bool operator==(const DynamicMatrix& other) const
    {
        if (other.col() != col() || other.row() != row())
        {
            return false;
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                if (at(col,row) != other.at(col,row))
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const DynamicMatrix& other) const
    {
        return !(*this == other);
    }
};

template<typename _Tp>
inline DynamicMatrix<_Tp> operator+(const DynamicMatrix<_Tp>& matrix)
{
    return matrix;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator-(const DynamicMatrix<_Tp>& matrix)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) = -result.at(col, row);
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator+(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.col() || m1.row() != m2.row())
    {
        throw std::logic_error("Size of matrices is not equal.");
    }
    DynamicMatrix<_Tp> result(m1);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) += m2.at(col, row);
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator-(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.col() || m1.row() != m2.row())
    {
        throw std::logic_error("Size of matrices is not equal.");
    }
    DynamicMatrix<_Tp> result(m1);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) -= m2.at(col, row);
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(_Tp factor, const DynamicMatrix<_Tp>& matrix)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) *= factor;
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(const DynamicMatrix<_Tp>& matrix, _Tp factor)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) *= factor;
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator/(const DynamicMatrix<_Tp>& matrix, _Tp divisor)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) /= divisor;
        }
    }
    return result;
}

template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.row())
    {
        throw std::logic_error("Matrices can't multiply.");
    }
    DynamicMatrix<_Tp> result;
    result.resize({m2.col(), m1.row()});
    for (int row = 0; row < m1.row(); ++row)
    {
        for (int col = 0; col < m2.col(); ++col)
        {
            _Tp sum(0.0);
            for (int j = 0; j < m1.col(); ++j) {sum += m1.at(j, row) * m2.at(col, j);}
            result.at(col, row) = sum;
        }
    }
    return result;
}

template<typename _Tp>
std::ostream& operator<<(std::ostream& dbg, const DynamicMatrix<_Tp> &m)
{
    dbg << '[';
    for (int row = 0; row < m.row(); ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < m.col(); ++col)
        {
            dbg << m.at(col, row);
            if (row < m.row() - 1 || col < m.col() - 1) {dbg << ", ";}
        }
        if (row < m.row() - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}

template<typename _Tp>
std::wostream& operator<<(std::wostream& dbg, const DynamicMatrix<_Tp> &m)
{
    dbg << '[';
    for (int row = 0; row < m.row(); ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < m.col(); ++col)
        {
            dbg << m.at(col, row);
            if (row < m.row() - 1 || col < m.col() - 1) {dbg << L", ";}
        }
        if (row < m.row() - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}


/****************************************************************************************\
*                                         迭代器                                         *
\****************************************************************************************/

/**
 * @brief 迭代型求解算法使用的通用迭代器(由SciPy翻译而来，并转写为抽象基类)
 */
class ElementwiseIterator
{
protected:
    enum
    {
        InProgress = 1,
        Finished   = 0,
        ValueError = -3
    }State;

    uint64     IterCount     = 0;
    uint64     EvaluateCount = 0;

public:
    //virtual void Callback() = 0;
    virtual DynamicMatrix<float64> PreEvaluator() = 0;
    virtual void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) = 0;
    virtual bool CheckTerminate() = 0;
    virtual void Finalize() = 0;

    void Run(Function1D Function, float64 MaxIterLog);
};


/****************************************************************************************\
*                                         特殊函数                                        *
\****************************************************************************************/

/**
 * @brief 计算初等对称多项式的值
 *   https://en.wikipedia.org/wiki/Elementary_symmetric_polynomial
 * @param Coeffs 参数
 * @return 所有基本对称多项式的值，从e0-en排列(定义e0 = 1)
 * @example
 *      输入Coeffs = (2, 3, 4)
 *
 *      按定义：
 *          e0 = 1;
 *          e1 = 2 + 3 + 4 = 9;
 *          e2 = 2 * 3 + 2 * 4 + 3 * 4 = 26;
 *          e3 = 2 * 3 * 4 = 24;
 *
 *      结果为 (1, 9, 26, 24)
 */
std::vector<float64> ElementarySymmetricPolynomial(std::vector<float64> V);

/**
 * @brief 给定参数生成范德蒙德矩阵
 * @param V 范德蒙德矩阵的参数
 * @return 范德蒙德矩阵
 * @example
 *      输入Coeffs = (2, 3, 4, 5)
 *      输出的矩阵为
 *          [[1,  1,  1,  1  ],
 *           [2,  3,  4,  5  ],
 *           [4,  9,  16, 25 ],
 *           [8,  27, 64, 125]]
 */
DynamicMatrix<float64> Vandermonde(std::vector<float64> V);

/**
 * @brief 给定参数快速生成范德蒙德矩阵的逆矩阵
 *  算法来源：https://zhuanlan.zhihu.com/p/678666109
 * @param V 范德蒙德矩阵的参数
 * @return 范德蒙德矩阵的逆矩阵
 * @example
 *      输入Coeffs = (2, 3, 4, 5)
 *      即范德蒙德矩阵
 *          [[1,  1,  1,  1  ],
 *           [2,  3,  4,  5  ],
 *           [4,  9,  16, 25 ],
 *           [8,  27, 64, 125]]
 *      则其逆矩阵为
 *          [ 10,  -7.83333333333333,  2,   -0.166666666666667,
 *           -20,  19,                -5.5,  0.5,
 *            15, -15.5,               5,   -0.5,
 *           -4,    4.33333333333333, -1.5,  0.166666666666667]
 */
DynamicMatrix<float64> InverseVandermonde(std::vector<float64> V);

/**
 * @brief 多项式
 */
class Polynomial
{
protected:
    std::vector<float64> Coefficients; // 多项式系数，降幂排序

public:
    Polynomial() {}
    Polynomial(const std::vector<float64>& Coeffs) : Coefficients(Coeffs) {}

    uint64 MaxPower()const {return Coefficients.size() - 1;}

    float64 operator()(float64 x)const;
    Polynomial Derivative()const; // 多项式的导函数还是多项式
};

/**
 * @brief 生成n次第一类勒让德多项式的系数
 * https://en.wikipedia.org/wiki/Legendre_polynomials
 * @param n 勒让德多项式的次数
 * @return n次第一类勒让德多项式的系数，降幂排列，没有的次数填0
 */
std::vector<float64> LegendrePolynomialCoefficients(uint64 n);

/**
 * @brief 生成n次斯蒂尔杰斯多项式的系数
 *
 *  斯蒂尔杰斯多项式定义为：
 *       1
 *       ∫ K_n+1(x) * P_n(x) * x^k * dx = 0
 *      -1
 *
 *  其中K_n+1(x)即为n次斯蒂尔杰斯多项式，P_n(x)为n次勒让德多项式。
 *  根据以上定义可以得出：
 *                  r
 *      K_n+1(x) =  Σ (a_i * P_2*i-1-q(x))
 *                 i=1
 *
 *  其中系数a_i的递推公式为：
 *              n-1
 *      a_r-n =  Σ (-a_r-i * (S(r - i, n) / S(r - n, n)))
 *              i=0
 *
 *      其中 S(i, k) / S(r - k, k) =
 *          S(i - 1, k) / S(r - k, k) *
 *          ((n - q + 2 * (i + k - 1)) * (n + q + 2 * (k - i + 1)) * (n - 1 - q + 2 * (i - k)) * (2 * (k + i - 1) - 1 - q - n)) /
 *          ((n - q + 2 * (i - k)) * (2 * (k + i - 1) - q - n) * (n + 1 + q + 2 * (k - i)) * (n - 1 - q + 2 * (i + k)))
 *
 *  根据上式可以得到1-5次斯蒂尔杰斯多项式分别为：
 *      E_1(x) = P_1(x)
 *      E_2(x) = P_2(x) - 2/5 * P_0(x)
 *      E_3(x) = P_3(x) - 9/14 * P_1(x)
 *      E_4(x) = P_4(x) - 20/27 * P_2(x) + 14/891 * P_0(x)
 *      E_5(x) = P_5(x) - 35/44 * P_3(x) + 135/12584 * P_1(x)
 *
 * 参考文献：
 *  Patterson T N L. The optimum addition of points to quadrature formulae[J].
 *  Mathematics of Computation, 1968, 22(104): 847-856.
 *  http://www.ams.org/journals/mcom/1968-22-104/S0025-5718-68-99866-9/S0025-5718-68-99866-9.pdf
 *
 * @param N 斯蒂尔杰斯多项式的次数
 * @return n次斯蒂尔杰斯多项式的系数，降幂排列，没有的次数填0
 */
std::vector<float64> StieltjesPolynomialCoefficients(uint64 N);


/****************************************************************************************\
*                                          导数                                          *
\****************************************************************************************/

// 参见：https://en.wikipedia.org/wiki/Numerical_differentiation

/**
 * @brief 生成一元函数的导函数。
 */
class DerivativeFunction
{
protected:
    Function1D OriginalFunction; // 原函数
    float64    DerivativeOrder;  // 导数阶数(可以为非整数)

public:
    virtual float64 operator()(float64 x) = 0;
};

/**
 * @brief 基于有限差分法的一元函数的数值导数 (此功能翻译自SciPy，并转化为独立的类)
 * 据SciPy的文档所说，此方法的实现受到了jacobi、numdifftools和DERIVEST的启发，但
 * 其实现更直接地遵循了泰勒级数理论（可以说是天真地遵循了泰勒级数理论）。
 * <a href="https://docs.scipy.org/doc/scipy/reference/generated/scipy.differentiate.derivative.html#scipy.differentiate.derivative">参见官方文档</a>
 *
 * @example
 * 求ln(x)的导函数，已知ln'(x) = 1 / x
 *  float64 (*f)(float64) = cse::ln;
 *  SciCxx::FiniteDifferenceDerivativeFunction df(f);
 *  cout << df(1) << ' ';
 *  cout << df(2) << ' ';
 *  cout << df(5) << '\n';
 *
 * 输出: 0.999999999998355 0.499999999999178 0.200000000000003
 */
class FiniteDifferenceDerivativeFunction : public DerivativeFunction
{
public:
    using Mybase = DerivativeFunction;

protected:
    float64 AbsoluteTolerence = 300; // 绝对误差的负对数，默认 -log(0x1p-2022) = 307.65265556858878150844115040843
    float64 RealtiveTolerence = 7.5; // 相对误差的负对数，默认 -log(sqrt(0x1p-52)) ~= 7.8267798872635110755572112628368
    uint64  FDMOrder          = 8;   // 有限差分阶数，必须为偶数(与导函数阶数无关)
    float64 InitialStepSize   = 0.5; // 初始步长
    float64 StepFactor        = 2;   // 每次迭代时步长减小的系数，即每迭代1次，步长减小n。
    float64 MaxIteration      = 1;   // 最大迭代次数的对数，向下取整

    enum DirectionType
    {
        Center = 0, Forward = 1, Backward = -1
    }Direction = Center; // 采样点取值方向

public:
    FiniteDifferenceDerivativeFunction(Function1D Function)
    {
        OriginalFunction = Function;
        DerivativeOrder = 1;
    }

    class Iterator : public ElementwiseIterator
    {
    public:
        using Mybase = ElementwiseIterator;
        friend class FiniteDifferenceDerivativeFunction;

        enum StateType
        {
            ErrorIncrease = 2
        };

    protected:
        float64                Input;
        float64                Output;
        DynamicMatrix<float64> Intermediates;
        float64                Error;
        float64                Step;
        float64                LastOutput;
        float64                LastError;
        float64                StepFactor;
        float64                AbsoluteTolerence;
        float64                RealtiveTolerence;
        uint64                 Terms;
        DirectionType          Direction;
        std::vector<float64>   CentralWeights;
        std::vector<float64>   ForwardWeights;

    public:
        std::vector<float64> DerivativeWeight(uint64 n);

        DynamicMatrix<float64> PreEvaluator() override; // 获取采样点
        void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) override; // 求解主函数
        bool CheckTerminate() override; // 终止检测
        void Finalize() override {}
    };

    float64 operator()(float64 x) override;
};

using DefaultDerivativeFunction = FiniteDifferenceDerivativeFunction;

/****************************************************************************************\
*                                          积分                                          *
\****************************************************************************************/

// 参见：https://en.wikipedia.org/wiki/Numerical_integration

void __Infinite_Integration_Normalize
    (const Function1D& f, const float64& a, const float64& b,
    Function1D* F, float64* A, float64* B);

/**
 * @brief 一元函数的定积分。
 */
class DefiniteIntegratingFunction
{
protected:
    virtual float64 Run(Function1D f, float64 a, float64 b) = 0;
public:
    float64 operator()(Function1D f, float64 a, float64 b);
};

/**
 * @brief 基于采样点的定积分。
 */
class SampleBasedIntegratingFunction
{
protected:
    virtual float64 Run(std::vector<vec2> Samples) = 0;
public:
    virtual std::vector<vec2> GetSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples) = 0;
    float64 operator()(std::vector<vec2> Samples);
    float64 operator()(Function1D f, float64 a, float64 b, uint64 Samples = 0);
};

// ------------------------------------------------------------------------------------- //

#if defined __GNUG__
#define __Tbl_FpType __float128 // GCC已经支持四倍精度(15+112)
#define __Tbl_Fp(X) X ## Q
#else
#define __Tbl_FpType long double // 设置为当前编译器最大可支持精度(一般为15+64长精度)
#define __Tbl_Fp(X) X ## L
#endif

extern const __Tbl_FpType __Gaussian07_Kronrod15_Table[16];
extern const __Tbl_FpType __Gaussian10_Kronrod21_Table[22];
extern const __Tbl_FpType __Gaussian15_Kronrod31_Table[32];
extern const __Tbl_FpType __Gaussian20_Kronrod41_Table[42];
extern const __Tbl_FpType __Gaussian25_Kronrod51_Table[52];
extern const __Tbl_FpType __Gaussian30_Kronrod61_Table[62];

#undef __Tbl_FpType
#undef __Tbl_Fp

/**
 * @brief 高斯-克朗罗德积分
 */
class GaussKronrodQuadrature : public DefiniteIntegratingFunction
{
protected:
    std::vector<float64> Coefficients;
    bool EnableAdaptive = 1;

    float64 Run(Function1D f, float64 a, float64 b) override;

public:
    static std::vector<float64> GetNodesAndWeights(uint64 N);
};

_SCICXX_END

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
