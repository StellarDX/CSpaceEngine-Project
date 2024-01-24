// Basic Matrix standard header (GLSL)

// Reference:
// [GLSL Document]https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.20.pdf
// [GLM by G-Truc]https://glm.g-truc.net/0.9.4/api/a00142.html#gade0eb47c01f79384a6f38017ede17446
// [Introduction to Linear Algebra(5th Edition) by Gilbert Strang]
// https://github.com/Rumi381/All-Programming-E-Books-PDF/blob/master/Books/Introduction%20to%20Linear%20Algebra,%20Fifth%20Edition%20by%20Gilbert%20Strang%20(z-lib.org).pdf
// Port by StellarDX, 2022.

#pragma once

#ifndef __CSE_MATRIX__
#define __CSE_MATRIX__

#include <CSE/CSEBase/gltypes/GLBase.h>
// This matrix is based on STD array, and most functions are taken from STD Library and GLM Library.

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

#if !defined(_MSC_VER)
#define _NODISCARD
#define _STL_VERIFY(cond, mesg)\
if (!(cond)) {throw std::logic_error(mesg);}
#endif

_CSE_BEGIN
_GL_BEGIN

template<typename _Ty, size_t _Column, size_t _Line>
class _Matrix_const_iterator //: private _Iterator_base12_compatible
{
    public:
    #ifdef __cpp_lib_concepts
    using iterator_concept  = std::contiguous_iterator_tag;
    #endif // __cpp_lib_concepts
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = _Ty;
    using difference_type   = ptrdiff_t;
    using pointer           = const std::array<_Ty, _Line>*;
    using pointer_Elem      = const _Ty*;
    using reference         = const _Ty&;

    enum _Option_Tag
    {
        Line, Column
    };

private:
    pointer      _Ptr;
    pointer_Elem _Elm;
    size_t       _IdxC;
    size_t       _IdxL;
    _Option_Tag  _Opt;

public:
    constexpr _Matrix_const_iterator() : _Ptr(), _IdxC(0), _IdxL(0), _Opt(Column) {}

    constexpr explicit _Matrix_const_iterator(pointer _Parg, size_t _OffC = 0, size_t _OffL = 0, _Option_Tag _Option = Column) : _Ptr(_Parg), _IdxC(_OffC), _IdxL(_OffL), _Opt(_Option) {}

    _NODISCARD constexpr pointer_Elem operator->() const
    {
        _STL_VERIFY(_Ptr, "cannot dereference value-initialized matrix iterator");
        _STL_VERIFY(_IdxC < _Column && _IdxL < _Line, "cannot dereference out of range matrix iterator");
        return &(*(_Ptr + _IdxC))[_IdxL];
    }

    _NODISCARD constexpr reference operator*() const
    {
        return *operator->();
    }

    constexpr _Matrix_const_iterator& operator++()
    {
        _STL_VERIFY(_Ptr, "cannot increment value-initialized matrix iterator");
        _STL_VERIFY(_IdxC < _Column && _IdxL < _Line, "cannot increment matrix iterator past end");
        switch (_Opt)
        {
        case Line:
            ++_IdxC;
            if (_IdxC == _Column)
            {
                _IdxC -= _Column;
                ++_IdxL;
            }
            break;
        case Column:
            ++_IdxL;
            if (_IdxL == _Line)
            {
                _IdxL -= _Line;
                ++_IdxC;
            }
            break;
        default:
            break;
        }

        return *this;
    }

    constexpr _Matrix_const_iterator operator++(int)
    {
        _Matrix_const_iterator _Tmp = *this;
        ++(*this);
        return _Tmp;
    }

    constexpr _Matrix_const_iterator& operator--()
    {
        _STL_VERIFY(_Ptr, "cannot decrement value-initialized matrix iterator");
        _STL_VERIFY(_IdxC != 0 || _IdxL != 0, "cannot decrement matrix iterator before begin");
        switch (_Opt)
        {
        case Line:
            if (_IdxC == 0)
            {
                _IdxC += _Column;
                --_IdxL;
            }
            --_IdxC;
            break;
        case Column:
            if (_IdxL == 0)
            {
                _IdxL += _Line;
                --_IdxC;
            }
            --_IdxL;
            break;
        default:
            break;
        }
        return *this;
    }

    constexpr _Matrix_const_iterator operator--(int)
    {
        _Matrix_const_iterator _Tmp = *this;
        --(*this);
        return _Tmp;
    }

    constexpr size_t _Reduce_Dimension() const
    {
        size_t _Idx;
        switch (_Opt)
        {
        case Line:
            _Idx = _Column * _IdxL + _IdxC;
            break;
        case Column:
            _Idx = _Line * _IdxC + _IdxL;
            break;
        default:
            break;
        }
        return _Idx;
    }

    constexpr void _Incrase_Dimension(size_t _Idx)
    {
        switch (_Opt)
        {
        case Line:
            _IdxL = _Idx / _Column;
            _IdxC = _Idx % _Column;
            break;
        case Column:
            _IdxC = _Idx / _Line;
            _IdxL = _Idx % _Line;
            break;
        default:
            break;
        }
    }

    constexpr void _Verify_offset(const ptrdiff_t _Off) const
    {
        if (_Off != 0)
        {
            _STL_VERIFY(_Ptr, "cannot seek value-initialized matrix iterator");
        }

        size_t _Idx = _Reduce_Dimension();

        if (_Off < 0)
        {
            _STL_VERIFY(_Idx >= size_t{ 0 } - static_cast<size_t>(_Off), "cannot seek matrix iterator before begin");
        }

        if (_Off > 0)
        {
            _STL_VERIFY(_Column * _Line - _Idx >= static_cast<size_t>(_Off), "cannot seek array iterator after end");
        }
    }

    constexpr _Matrix_const_iterator& operator+=(const ptrdiff_t _Off)
    {
        _Verify_offset(_Off);
        size_t _Idx = _Reduce_Dimension();
        _Idx += static_cast<size_t>(_Off);
        _Incrase_Dimension(_Idx);
        return *this;
    }

    _NODISCARD constexpr _Matrix_const_iterator operator+(const ptrdiff_t _Off) const
    {
        _Matrix_const_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    constexpr _Matrix_const_iterator& operator-=(const ptrdiff_t _Off)
    {
        return *this += -_Off;
    }

    _NODISCARD constexpr _Matrix_const_iterator operator-(const ptrdiff_t _Off) const
    {
        _Matrix_const_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD constexpr reference operator[](const ptrdiff_t _Off) const
    {
        return *(*this + _Off);
    }

    constexpr void _Compat(const _Matrix_const_iterator& _Right) const
    {
        // test for compatible iterator pair
        _STL_VERIFY(_Ptr == _Right._Ptr, "matrix iterators incompatible");
    }

    _NODISCARD constexpr bool operator==(const _Matrix_const_iterator& _Right) const
    {
        _Compat(_Right);
        return _IdxC == _Right._IdxC && _IdxL == _Right._IdxL;
    }
};

template<typename _Ty, size_t _Column, size_t _Line>
class _Matrix_iterator : public _Matrix_const_iterator<_Ty, _Column, _Line>
{
public:
    using _Mybase           = _Matrix_const_iterator<_Ty, _Column, _Line>;
    #ifdef __cpp_lib_concepts
    using iterator_concept  = std::contiguous_iterator_tag;
    #endif // __cpp_lib_concepts
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = _Ty;
    using difference_type   = ptrdiff_t;
    using pointer           = std::array<_Ty, _Line>*;
    using pointer_Elem      = _Ty*;
    using reference         = _Ty&;

    using typename _Mybase::_Option_Tag;

    constexpr _Matrix_iterator() {}

    constexpr explicit _Matrix_iterator(pointer _Parg, size_t _OffC = 0, size_t _OffL = 0, typename _Mybase::_Option_Tag _Option = _Mybase::Column) : _Mybase(_Parg, _OffC, _OffL, _Option) {}

    _NODISCARD constexpr reference operator*() const
    {
        return const_cast<reference>(_Mybase::operator*());
    }

    _NODISCARD constexpr pointer_Elem operator->() const
    {
        return const_cast<pointer_Elem>(_Mybase::operator->());
    }

    constexpr _Matrix_iterator& operator++()
    {
        _Mybase::operator++();
        return *this;
    }

    constexpr _Matrix_iterator operator++(int)
    {
        _Matrix_iterator _Tmp = *this;
        _Mybase::operator++();
        return _Tmp;
    }

    constexpr _Matrix_iterator& operator--()
    {
        _Mybase::operator--();
        return *this;
    }

    constexpr _Matrix_iterator operator--(int)
    {
        _Matrix_iterator _Tmp = *this;
        _Mybase::operator--();
        return _Tmp;
    }

    constexpr _Matrix_iterator& operator+=(const ptrdiff_t _Off)
    {
        _Mybase::operator+=(_Off);
        return *this;
    }

    _NODISCARD constexpr _Matrix_iterator operator+(const ptrdiff_t _Off) const
    {
        _Matrix_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    constexpr _Matrix_iterator& operator-=(const ptrdiff_t _Off)
    {
        _Mybase::operator-=(_Off);
        return *this;
    }

    using _Mybase::operator-;

    _NODISCARD constexpr _Matrix_iterator operator-(const ptrdiff_t _Off) const
    {
        _Matrix_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD constexpr reference operator[](const ptrdiff_t _Off) const
    {
        return const_cast<reference>(_Mybase::operator[](_Off));
    }
};

template<typename _Ty, size_t _Column, size_t _Line>
class GBasicMatrix // fixed size matrix of values
{
public:
    using value_type      = _Ty;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = _Ty*;
    using const_pointer   = const _Ty*;
    using reference       = _Ty&;
    using const_reference = const _Ty&;

    using line_type      = std::array<_Ty, _Column>;
    using line_ref       = std::array<_Ty, _Column>&;
    using const_line_ref = const std::array<_Ty, _Column>&;
    using column_type    = std::array<_Ty, _Line>;
    using column_ref     = std::array<_Ty, _Line>&;
    using const_col_ref  = const std::array<_Ty, _Line>&;

    using iterator       = _Matrix_iterator<_Ty, _Column, _Line>;
    using const_iterator = _Matrix_const_iterator<_Ty, _Column, _Line>;

    // -- Value container --
    std::array<_Ty, _Line> _Elems[_Column]; // using GLM's method

    _CONSTEXPR20 void fill(const _Ty& _Value)
    {
        for (size_t i = 0; i < _Column; i++)
        {
            _Elems[i].fill(_Value);
        }
    }

    // -- Accesses --

    _NODISCARD constexpr size_type size() const noexcept
    {
        return _Column * _Line;
    }

    _NODISCARD constexpr size_type max_size() const noexcept
    {
        return _Column * _Line;
    }

    _NODISCARD constexpr size_type length() const noexcept
    {
        return _Column;
    }

    _NODISCARD constexpr column_ref operator[](size_type _CPos) noexcept /* strengthened */
    {
        #if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_CPos < _Column, "matrix subscript out of range");
        #endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Elems[_CPos];
    }

    _NODISCARD constexpr const_col_ref operator[](size_type _CPos) const noexcept /* strengthened */
    {
        #if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_CPos < _Column, "matrix subscript out of range");
        #endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Elems[_CPos];
    }

    _NODISCARD constexpr reference front() noexcept /* strengthened */
    {
        return _Elems[0][0];
    }

    _NODISCARD constexpr const_reference front() const noexcept /* strengthened */
    {
        return _Elems[0][0];
    }

    _NODISCARD constexpr column_ref frontcolumn() noexcept /* strengthened */
    {
        return _Elems[0];
    }

    _NODISCARD constexpr const_col_ref frontcolumn() const noexcept /* strengthened */
    {
        return _Elems[0];
    }

    _NODISCARD constexpr reference back() noexcept /* strengthened */
    {
        return _Elems[_Column - 1][_Line - 1];
    }

    _NODISCARD constexpr const_reference back() const noexcept /* strengthened */
    {
        return _Elems[_Column - 1][_Line - 1];
    }

    _NODISCARD constexpr column_ref backcolumn() noexcept /* strengthened */
    {
        return _Elems[_Column - 1];
    }

    _NODISCARD constexpr const_col_ref backcolumn() const noexcept /* strengthened */
    {
        return _Elems[_Column - 1];
    }

    _NODISCARD constexpr bool empty() const noexcept
    {
        return false;
    }

    _NODISCARD constexpr column_ref at(size_type _CPos)
    {
        if (_Column <= _CPos) { _Xran(); }
        return _Elems[_CPos];
    }

    _NODISCARD constexpr const_col_ref at(size_type _CPos) const
    {
        if (_Column <= _CPos) { _Xran(); }
        return _Elems[_CPos];
    }

    void _Xran() const
    {
        throw std::logic_error("invalid matrix subscript");
    }

    template<size_type _NewCol, size_type _NewLine>
    _NODISCARD constexpr GBasicMatrix<_Ty, _NewCol, _NewLine> submat(const size_type _CPos, const size_type _LPos) const
    {
        GBasicMatrix<_Ty, _NewCol, _NewLine> _SubMat;
        for (size_t i = 0; i < _NewLine; i++)
        {
            for (size_t j = 0; j < _NewCol; j++)
            {
                _SubMat[j][i] = this->_Elems[_CPos + j - 1][_LPos + i - 1];
            }
        }
        return _SubMat;
    }

    // -- Constructors --

    constexpr GBasicMatrix() = default;
    inline constexpr GBasicMatrix(const GBasicMatrix<_Ty, _Column, _Line>& m)
    {
        this->fill(0);
        for (size_t i = 0; i < _Column; i++)
        {
            this->_Elems[i] = m._Elems[i];
        }
    }

    inline constexpr explicit GBasicMatrix(_Ty _Scalar)
    {
        this->fill(0);
        for (size_t i = 0; i < (_Column < _Line ? _Column : _Line); i++)
        {
            this->_Elems[i][i] = _Scalar;
        }
    }

    constexpr GBasicMatrix(std::initializer_list<_Ty> _Ilist)
    {
        this->fill(0);
        auto it = _Ilist.begin();
        auto end = _Ilist.end();
        if (it == end) { return; }
        uint64 _CurCol = 0;
        uint64 _CurLin = 0;
        for (; it != end; ++it)
        {
            this->_Elems[_CurCol][_CurLin] = *it;
            if (_CurCol == _Column - 1 && _CurLin == _Line - 1) { break; }
            ++_CurLin;
            if (_CurLin == _Line)
            {
                _CurLin -= _Line;
                ++_CurCol;
            }
        }
    }

    constexpr GBasicMatrix(std::initializer_list<std::initializer_list<_Ty>> _Ilist)
    {
        this->fill(0);
        auto it = _Ilist.begin();
        auto end = _Ilist.end();
        if (it == end) { return; }
        uint64 _CurCol = 0;
        for (; it != end; ++it)
        {
            auto it2 = it->begin();
            auto end2 = it->end();
            uint64 _CurLin = 0;
            for ( ; it2 != end2; ++it2)
            {
                this->_Elems[_CurCol][_CurLin] = *it2;
                if (_CurLin == _Line - 1) { break; }
                ++_CurLin;
            }

            if (_CurCol == _Column - 1){break;}
            ++_CurCol;
        }
    }

    // -- Matrix conversions --

    template<size_t _Column2, size_t _Line2>
    constexpr GBasicMatrix(const GBasicMatrix<_Ty, _Column2, _Line2>& x)
    {
        this->fill(0);
        uint64 _CurCol = 0;
        uint64 _CurLin = 0;
        while(_CurCol != _Column && _CurCol != _Column2)
        {
            this->_Elems[_CurCol][_CurLin] = x[_CurCol][_CurLin];
            ++_CurLin;
            if (_CurLin == _Line2)
            {
                _CurLin -= _Line2;
                ++_CurCol;
            }
            else if (_CurLin == _Line)
            {
                _CurLin -= _Line;
                ++_CurCol;
            }
        }
    }

    // -- STL Iterators --

    _NODISCARD constexpr iterator cbegin() noexcept
    {
        return iterator(_Elems, 0, 0, iterator::Column);
    }

    _NODISCARD constexpr iterator lbegin() noexcept
    {
        return iterator(_Elems, 0, 0, iterator::Line);
    }

    _NODISCARD constexpr iterator cend() noexcept
    {
        return iterator(_Elems, _Column, 0, iterator::Column);
    }

    _NODISCARD constexpr iterator lend() noexcept
    {
        return iterator(_Elems, 0, _Line, iterator::Line);
    }

    _NODISCARD constexpr const_iterator cbegin()const noexcept
    {
        return const_iterator(_Elems, 0, 0, const_iterator::Column);
    }

    _NODISCARD constexpr const_iterator lbegin()const noexcept
    {
        return const_iterator(_Elems, 0, 0, const_iterator::Line);
    }

    _NODISCARD constexpr const_iterator cend()const noexcept
    {
        return const_iterator(_Elems, _Column, 0, const_iterator::Column);
    }

    _NODISCARD constexpr const_iterator lend()const noexcept
    {
        return const_iterator(_Elems, 0, _Line, const_iterator::Line);
    }

    _NODISCARD constexpr iterator begin() noexcept
    {
        return cbegin();
    }

    _NODISCARD constexpr iterator end() noexcept
    {
        return cend();
    }

    _NODISCARD constexpr const_iterator begin()const noexcept
    {
        return cbegin();
    }

    _NODISCARD constexpr const_iterator end()const noexcept
    {
        return cend();
    }

    // -- Elementary row operations --

    _NODISCARD GBasicMatrix<_Ty, _Column, _Line> swap(size_type _Pos1, size_type _Pos2, std::string _Option = "Line")
    {
        _STL_VERIFY(_Option == "Line" || _Option == "Column", "invalid option");
        GBasicMatrix<_Ty, _Column, _Line> _Cpy = *this;
        if (_Option == "Line")
        {
            _STL_VERIFY(_Pos1 <= _Line && _Pos2 <= _Line, "matrix subscript out of range");
            for (size_t i = 0; i < _Column; i++)
            {
                _Cpy[i][_Pos1 - 1] = this->_Elems[i][_Pos2 - 1];
                _Cpy[i][_Pos2 - 1] = this->_Elems[i][_Pos1 - 1];
            }
        }
        else if (_Option == "Column")
        {
            _STL_VERIFY(_Pos1 <= _Column && _Pos2 <= _Column, "matrix subscript out of range");
            _Cpy[_Pos1 - 1].swap(_Cpy[_Pos2 - 1]);
        }
        return _Cpy;
    }

    _NODISCARD GBasicMatrix<_Ty, _Column, _Line> scale(size_type _Pos, _Ty _Multiply, std::string _Option = "Line")
    {
        _STL_VERIFY(_Option == "Line" || _Option == "Column", "invalid option");
        GBasicMatrix<_Ty, _Column, _Line> _Cpy = *this;
        if (_Option == "Line")
        {
            _STL_VERIFY(_Pos <= _Line, "matrix subscript out of range");
            for (size_t i = 0; i < _Column; i++)
            {
                _Cpy[i][_Pos - 1] *= _Multiply;
            }
        }
        else if (_Option == "Column")
        {
            _STL_VERIFY(_Pos <= _Column, "matrix subscript out of range");
            for (size_t i = 0; i < _Line; i++)
            {
                _Cpy[_Pos - 1][i] *= _Multiply;
            }
        }
        return _Cpy;
    }

    _NODISCARD GBasicMatrix<_Ty, _Column, _Line> add(size_type _Pos1, size_type _Pos2, _Ty _Multiply, std::string _Option = "Line")
    {
        _STL_VERIFY(_Option == "Line" || _Option == "Column", "invalid option");
        GBasicMatrix<_Ty, _Column, _Line> _Cpy = *this;
        if (_Option == "Line")
        {
            _STL_VERIFY(_Pos1 <= _Line && _Pos2 <= _Line, "matrix subscript out of range");
            for (size_t i = 0; i < _Column; i++)
            {
                _Cpy[i][_Pos2 - 1] += _Cpy[i][_Pos1 - 1] * _Multiply;
            }
        }
        else if (_Option == "Column")
        {
            _STL_VERIFY(_Pos1 <= _Column && _Pos2 <= _Column, "matrix subscript out of range");
            for (size_t i = 0; i < _Line; i++)
            {
                _Cpy[_Pos2 - 1][i] += _Cpy[_Pos1 - 1][i] * _Multiply;
            }
        }
        return _Cpy;
    }

    // -- Functions in linear algebra --

    /**
     * @brief Returns minor of the entry in the i th row and j th column
     * @param _CPos - column
     * @param _LPos - row
     */
    GBasicMatrix<_Ty, _Column - 1, _Line - 1> Minor(size_type _CPos, size_type _LPos)const
    {
        //if (_Column != _Line) { return GBasicMatrix<_Ty, _Column - 1, _Line - 1>(IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE)); }
        _STL_VERIFY(_Line >= 2 && _Column >= 2, "can't generate minor because size of matrix is less than 2x2");
        GBasicMatrix<_Ty, _Column - 1, _Line - 1> _Minor;
        uint64 _M_Line = 0, _M_Col = 0;
        for (size_t i = 0; i < _Line; i++)
        {
            for (size_t j = 0; j < _Column; j++)
            {
                if (i != _LPos - 1 && j != _CPos - 1)
                {
                    _Minor[_M_Col][_M_Line] = this->_Elems[j][i];
                    ++_M_Col;

                    if (_M_Col == _Column - 1)
                    {
                        _M_Col -= _Column - 1;
                        ++_M_Line;
                    }
                }
            }
        }
        return _Minor;
    }

    /**
     * @brief Returns the determinant value of the matrix.
     * WARNING: This function is EXTREMELY INEFFICIENT for high-dimension matrices,
     * with a time complexity in RIDICULOUS big O notation of O(n!).
     * (But a new algorithm has found, the O notation has been reduced to O(n^3).)
     * @link https://www.tutorialspoint.com/cplusplus-program-to-compute-determinant-of-a-matrix
     * @link https://codereview.stackexchange.com/questions/204135/determinant-using-gauss-elimination
     */
    float64 Determinant()const
    {
        if (_Column != _Line) { return IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE); }
        if (this->length() == 1) { return _Elems[0][0]; }

        GBasicMatrix<_Ty, _Column, _Line> matrix = *this;
        uint64 N = static_cast<uint64>(_Line);
        float64 det = 1;

        for (uint64 i = 0; i < N; ++i)
        {
            float64 pivotElement = matrix[i][i];
            uint64 pivotRow = i;
            for (uint64 row = i + 1; row < N; ++row)
            {
                if (std::abs(matrix[i][row]) > std::abs(pivotElement))
                {
                    pivotElement = matrix[i][row];
                    pivotRow = row;
                }
            }
            if (pivotElement == 0.0)
            {
                return 0.0;
            }
            if (pivotRow != i)
            {
                matrix = matrix.swap(i + 1, pivotRow + 1, "Line");
                det *= -1.0;
            }
            det *= pivotElement;

            for (uint64 row = i + 1; row < N; ++row)
            {
                for (uint64 col = i + 1; col < N; ++col)
                {
                    matrix[col][row] -= matrix[i][row] * matrix[col][i] / pivotElement;
                }
            }
        }

        return det;
    }

    /**
     * @brief Returns the transpose of a matrix
     */
    GBasicMatrix<_Ty, _Line, _Column> Transpose()const
    {
        GBasicMatrix<_Ty, _Line, _Column> _Transpose;
        for (size_t i = 0; i < _Column; i++)
        {
            for (size_t j = 0; j < _Line; j++)
            {
                _Transpose[j][i] = this->_Elems[i][j];
            }
        }
        return _Transpose;
    }

    /**
     * @brief Returns the Adjugate matrix
     */
    GBasicMatrix<_Ty, _Column, _Line> Adjugate()const
    {
        GBasicMatrix<_Ty, _Column, _Line> m0;
        if (_Column != _Line) { return GBasicMatrix<_Ty, _Line, _Column>(IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE)); }
        if (this->length() == 1) { return GBasicMatrix<_Ty, _Line, _Column>(1. / _Elems[0][0]); }
        else
        {
            for (size_t i = 1; i <= this->length(); i++)
            {
                for (size_t j = 1; j <= this->length(); j++)
                {
                    m0[j - 1][i - 1] = std::pow(-1, i + j) * this->Minor(i, j).Determinant();
                }
            }
        }
        return m0;
    }

    /**
     * @brief Returns the Inverse matrix
     */
    GBasicMatrix<_Ty, _Line, _Column> Inverse()const
    {
        if (_Column != _Line) { return GBasicMatrix<_Ty, _Line, _Column>(IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE)); }

        // Matrix Inverse Using Gauss Jordan Method C++ Program
        // Reference: https://www.codesansar.com/numerical-methods/matrix-inverse-using-gauss-jordan-cpp-output.htm
        // This method is much faster than above because this doesn't need to calculate determinant value.

        /* 1. Reading order of matrix */
        /* 2. Reading Matrix */
        GBasicMatrix<_Ty, _Column, _Line> _Cpy = *this;

        /* Augmenting Identity Matrix of Order n */
        GBasicMatrix<_Ty, _Column * 2, _Line> _Arg;
        GBasicMatrix<_Ty, _Column, _Line> _Ident(1);
        for (size_t i = 0;i < _Line;i++)
        {
            for (size_t j = 0;j < _Column;j++)
            {
                _Arg[j][i] = _Cpy[j][i];
                _Arg[j + _Column][i] = _Ident[j][i];
            }
        }

        /* Applying Gauss Jordan Elimination */
        for (size_t i = 0;i < _Line;i++)
        {
            if (_Arg[i][i] == 0.0)
            {
                if (i == _Line - 1)
                {
                    _STL_VERIFY(_Arg[i][i] != 0.0, "Matrix can't be inversed.");
                    return GBasicMatrix<_Ty, _Line, _Column>(IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE));
                }
                _Arg = _Arg.swap(i + 1, i + 2, "Line");
            }

            for (size_t j = 0;j < _Column;j++)
            {
                if (i != j)
                {
                    float64 ratio = _Arg[i][j] / _Arg[i][i];
                    for (size_t k = 1;k < 2 * _Column;k++)
                    {
                        _Arg[k][j] = _Arg[k][j] - ratio * _Arg[k][i];
                    }
                }
            }
        }

        /* Row Operation to Make Principal Diagonal to 1 */
        for (size_t i = 0;i < _Line;i++)
        {
            for (size_t j = _Column;j < 2 * _Column;j++)
            {
                _Arg[j][i] = _Arg[j][i] / _Arg[i][i];
            }
        }

        /* Exporting Inverse Matrix */
        GBasicMatrix<_Ty, _Column, _Line> _Inv;
        for (size_t i = 0;i < _Line;i++)
        {
            for (size_t j = 0;j < _Column;j++)
            {
                _Inv[j][i] = _Arg[j + _Column][i];
            }
        }

        return _Inv;
    }

    /**
     * @brief Finding the rank of a matrix with Gaussian elimination.
     * @link https://cp-algorithms.com/linear_algebra/rank-matrix.html#implementation
     */
    uint64 Rank()const
    {
        float64 _ZERO_ = 1E-9;
        uint64 _Rank = 0;
        std::array<bool, _Column> _SelLine;
        _SelLine.fill(false);
        GBasicMatrix<_Ty, _Column, _Line> _Cpy = *this;
        for (size_t i = 0; i < _Line; i++)
        {
            size_t j;
            for (j = 0; j < _Column; j++)
            {
                if (!_SelLine[j] && std::abs(_Cpy[j][i]) > _ZERO_) { break; }
            }

            if (j != _Column)
            {
                ++_Rank;
                _SelLine[j] = true;
                for (size_t p = i + 1; p < _Line; p++)
                {
                    _Cpy[j][p] /= _Cpy[j][i];
                }
                for (size_t k = 0; k < _Column; k++)
                {
                    if (k != j && std::abs(_Cpy[k][i]) > _ZERO_)
                    {
                        for (size_t p = i + 1; p < _Line; p++)
                        {
                            _Cpy[k][p] -= _Cpy[j][p] * _Cpy[k][i];
                        }
                    }
                }
            }
        }
        return _Rank;
    }

    /**
     * @brief Returns the trace of a square matrix
     */
    _Ty Trace()const
    {
        if (_Column != _Line) { return GBasicMatrix<_Ty, _Line, _Column>(IEEE754_Dbl64::FromBytes(Q_NAN_DOUBLE)); }
        _Ty _Trace = 0;

        for (size_t i = 0; i < _Column; i++)
        {
            _Trace += _Elems[i][i];
        }

        return _Trace;
    }
};

// -- Unary operators --

template<typename _Ty, size_t _Column, size_t _Line> // OpenCV style output
constexpr std::ostream& operator<<(std::ostream& os, const GBasicMatrix<_Ty, _Column, _Line>& c)
{
    os << '[';
    for (size_t i = 0; i < _Line; i++)
    {
        if (i != 0) { os << ' '; }
        for (size_t j = 0; j < _Column; j++)
        {
            os << c[j][i];
            if (j < _Column - 1) { os << ", "; }
            else { os << ';'; }
        }
        if (i < _Line - 1) { os << '\n'; }
        else { os << ']'; }
    }
    return os;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator+(const GBasicMatrix<_Ty, _Column, _Line>& m)
{
    return m;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator-(const GBasicMatrix<_Ty, _Column, _Line>& m)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = -m[i][j];
        }
    }
    return m0;
}

// -- Binary operators --

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator+(const GBasicMatrix<_Ty, _Column, _Line>& m1, const GBasicMatrix<_Ty, _Column, _Line>& m2)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator-(const GBasicMatrix<_Ty, _Column, _Line>& m1, const GBasicMatrix<_Ty, _Column, _Line>& m2)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = m1[i][j] - m2[i][j];
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator*(const GBasicMatrix<_Ty, _Column, _Line>& m1, const _Ty& scalar)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = m1[i][j] * scalar;
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator*(const _Ty& scalar, const GBasicMatrix<_Ty, _Column, _Line>& m1)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = m1[i][j] * scalar;
        }
    }
    return m0;
}
template<typename _Ty, size_t _Column, size_t _Line, size_t _Size>
GBasicMatrix<_Ty, _Column, _Line> operator*(const GBasicMatrix<_Ty, _Size, _Line>& m1, const GBasicMatrix<_Ty, _Column, _Size>& m2)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Line; i++)
    {
        for (size_t j = 0; j < _Column; j++)
        {
            _Ty _Total = 0;
            for (size_t k = 0; k < _Size; k++)
            {
                _Total += m1[k][i] * m2[j][k];
            }
            m0[j][i] = _Total;
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator/(const GBasicMatrix<_Ty, _Column, _Line>& m1, const _Ty& scalar)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = m1[i][j] / scalar;
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line>
GBasicMatrix<_Ty, _Column, _Line> operator/(const _Ty& scalar, const GBasicMatrix<_Ty, _Column, _Line>& m1)
{
    GBasicMatrix<_Ty, _Column, _Line> m0, m1i = m1.Inverse();
    for (size_t i = 0; i < _Column; i++)
    {
        for (size_t j = 0; j < _Line; j++)
        {
            m0[i][j] = scalar * m1i[i][j];
        }
    }
    return m0;
}

template<typename _Ty, size_t _Column, size_t _Line, size_t _Size>
GBasicMatrix<_Ty, _Column, _Line> operator/(const GBasicMatrix<_Ty, _Size, _Line>& m1, const GBasicMatrix<_Ty, _Column, _Size>& m2)
{
    GBasicMatrix<_Ty, _Column, _Line> m0;
    GBasicMatrix<_Ty, _Column, _Size> _m2 = m2.Inverse();
    for (size_t i = 0; i < _Line; i++)
    {
        for (size_t j = 0; j < _Column; j++)
        {
            _Ty _Total = 0;
            for (size_t k = 0; k < _Size; k++)
            {
                _Total += m1[k][i] * _m2[j][k];
            }
            m0[j][i] = _Total;
        }
    }
    return m0;
}

_GL_END

using mat2   = _GL GBasicMatrix<float64, 2, 2>;
using mat2x2 = _GL GBasicMatrix<float64, 2, 2>;
using mat2x3 = _GL GBasicMatrix<float64, 2, 3>;
using mat2x4 = _GL GBasicMatrix<float64, 2, 4>;
using mat3x2 = _GL GBasicMatrix<float64, 3, 2>;
using mat3   = _GL GBasicMatrix<float64, 3, 3>;
using mat3x3 = _GL GBasicMatrix<float64, 3, 3>;
using mat3x4 = _GL GBasicMatrix<float64, 3, 4>;
using mat4x2 = _GL GBasicMatrix<float64, 4, 2>;
using mat4x3 = _GL GBasicMatrix<float64, 4, 3>;
using mat4   = _GL GBasicMatrix<float64, 4, 4>;
using mat4x4 = _GL GBasicMatrix<float64, 4, 4>;

template<typename _Ty, size_t _Column, size_t _Line>
using matrix = _GL GBasicMatrix<_Ty, _Column, _Line>;

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
