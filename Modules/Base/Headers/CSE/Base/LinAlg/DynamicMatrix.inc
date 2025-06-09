#ifndef _SCICXX_BEGIN
#error This file is a part of AdvMath.h, include it instead.
#endif

/**
 * @brief 可变大小的矩阵，功能与定长矩阵一样
 * 本文件为高等数学库的一部分
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

    template<typename _Ty2> requires std::convertible_to<_Ty2, _Ty>
    constexpr DynamicMatrix(std::vector<_Ty2> Right) : Size(1)
    {
        Data.resize(this->col() * this->row());
        for (uint64 i = 0; i < Right.size(); ++i)
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
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v)
{
    return v;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res[i] = -v[i];
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] + scalar);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator+(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar + v[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] + v2[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] - scalar);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator-(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar - v[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] - v2[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator*(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] * scalar);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator*(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar * v[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator*(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] * v2[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator/(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] / scalar);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator/(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar / v[i]);
    }
    return Res;
}

template<typename _Tp>
constexpr std::vector<_Tp> operator/(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] / v2[i]);
    }
    return Res;
}

template<typename _Tp>
std::ostream& operator<<(std::ostream& os, std::vector<_Tp> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << ", " << vec[i];
    }
    os << ')';
    return os;
}

template<typename _Tp>
std::wostream& operator<<(std::wostream& os, std::vector<_Tp> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << L", " << vec[i];
    }
    os << ')';
    return os;
}

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
        throw std::logic_error("Size of matrices are not equal.");
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
        throw std::logic_error("Size of matrices are not equal.");
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
