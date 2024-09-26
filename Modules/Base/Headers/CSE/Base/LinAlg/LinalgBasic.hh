// Basic Linear algebra functions.

#pragma once

#ifndef _LINALG_BASIC_
#define _LINALG_BASIC_

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/GLTypes.h>
#include <CSE/Base/Algorithms.h>

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

_CSE_BEGIN namespace linalg {

/****************************************************************************************\
*                                        PRODUCTS                                        *
\****************************************************************************************/

/**
 * @brief Dot product of two arrays.
 */
template<std::size_t N>
float64 dot(fvec<N> _Left, fvec<N> _Right)
{
    float64 _Res = 0;
    for (int i = 0; i < N; ++i)
    {
        _Res += _Left[i] * _Right[i];
    }
    return _Res;
}

/**
 * @brief Inner product of two arrays.
 */
template<std::size_t N>
float64 InnerProduct(fvec<N> _Left, fvec<N> _Right) {return dot(_Left, _Right);}

/**
 * @brief Inner product of two matrices.
 */
template<std::size_t N, std::size_t M>
float64 InnerProduct(matrix<N, M> _Left, matrix<N, M> _Right)
{
    float64 _Sum = 0;
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t col = 0; col < N; ++col)
        {
            _Sum += _Left[col][row] * _Right[col][row];
        }
    }
    return _Sum;
}

/**
 * @brief Compute the outer product of two vectors.
 */
template<std::size_t N, std::size_t M>
matrix<N, M> OuterProduct(fvec<M> u, fvec<N> v)
{
    return matrix<1, M>(u) * matrix<1, N>(v).Transpose();
}

/**
 * @brief Compute the outer product of two matrices.
 */
template<std::size_t N1, std::size_t M1, std::size_t N2, std::size_t M2>
matrix<N1 * N2, M1 * M2> OuterProduct(matrix<N1, M1> A, matrix<N2, M2> B)
{
    matrix<N1 * N2, M1 * M2> C;
    // i loops till rowa
    for (int i = 0; i < M1; i++)
    {
        // k loops till rowb
        for (int k = 0; k < M2; k++)
        {
            // j loops till cola
            for (int j = 0; j < N1; j++)
            {
                // l loops till colb
                for (int l = 0; l < N2; l++)
                {
                    // Each element of matrix A is
                    // multiplied by whole Matrix B
                    // resp and stored as Matrix C
                    C[j + k + 1][i + l + 1] = A[j][i] * B[l][k];
                }
            }
        }
    }
}

/**
 * @brief Returns the cross product of 3-element vectors.
 */
inline vec3 cross(vec3 a, vec3 b)
{
    return vec3
    (
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x)
    );
}

/**
 * @brief cross product expand to 4-dimension
 */
inline vec4 cross(vec4 u, vec4 v, vec4 t)
{
    //mat4 U
    //({
    //    {0, -u.w * v.z + u.z * v.w, +u.w * v.y - u.y * v.w, +u.y * v.z - u.z * v.y},
    //    {+u.w * v.z - u.z * v.w, 0, -u.w * v.x + u.x * v.w, -u.x * v.z + u.z * v.x},
    //    {-u.w * v.y + u.y * v.w, +u.w * v.x - u.x * v.w, 0, +u.x * v.y - u.y * v.x},
    //    {-u.y * v.z + u.z * v.y, +u.x * v.z - u.z * v.x, -u.x * v.y + u.y * v.x, 0}
    //});
    //matrix<float64, 1, 4> T({t.x, t.y, t.z, t.w});
    //return vec4((U * T)[0]);

    // Expand
    return vec4
    (
        - (t.w * u.y * v.z) + (t.w * u.z * v.y) + (t.y * u.w * v.z) - (t.y * u.z * v.w) - (t.z * u.w * v.y) + (t.z * u.y * v.w),
        + (t.w * u.x * v.z) - (t.w * u.z * v.x) - (t.x * u.w * v.z) + (t.x * u.z * v.w) + (t.z * u.w * v.x) - (t.z * u.x * v.w),
        - (t.w * u.x * v.y) + (t.w * u.y * v.x) + (t.x * u.w * v.y) - (t.x * u.y * v.w) - (t.y * u.w * v.x) + (t.y * u.x * v.w),
        + (t.x * u.y * v.z) - (t.x * u.z * v.y) - (t.y * u.x * v.z) + (t.y * u.z * v.x) + (t.z * u.x * v.y) - (t.z * u.y * v.x)
    );
}

/****************************************************************************************\
*                                         NORMS                                          *
\****************************************************************************************/

/**
 * @brief Matrix or vector L1 norm.
 */
template<std::size_t N, std::size_t M>
float64 AbsoluteNorm(matrix<N, M> a)
{
    float64 _Sum = 0;
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t col = 0; col < N; ++col)
        {
            _Sum += abs(a[col][row]);
        }
    }
    return _Sum;
}

template<std::size_t N>
float64 AbsoluteNorm(fvec<N> a)
{
    return AbsoluteNorm(matrix<1, N>{a});
}

/**
 * @brief Matrix or vector L2 norm.
 */
template<std::size_t N, std::size_t M>
float64 EuclideanNorm(matrix<N, M> a)
{
    float64 _Sum = 0;
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t col = 0; col < N; ++col)
        {
            _Sum += abs(a[col][row] * a[col][row]);
        }
    }
    return sqrt(_Sum);
}

template<std::size_t N>
float64 EuclideanNorm(fvec<N> a)
{
    return EuclideanNorm(matrix<1, N>{a});
}

/**
 * @brief Matrix or vector Ln norm.
 */
template<std::size_t N, std::size_t M>
float64 PNorm(matrix<N, M> a, uint64 p)
{
    float64 _Sum = 0;
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t col = 0; col < N; ++col)
        {
            _Sum += pow(abs(a[col][row]), p);
        }
    }
    return yroot(_Sum, p);
}

template<std::size_t N>
float64 PNorm(fvec<N> a, uint64 p)
{
    return PNorm(matrix<1, N>{a}, p);
}

/**
 * @brief Matrix or vector Ln norm.
 */
template<std::size_t N, std::size_t M>
float64 UniformNorm(matrix<N, M> a)
{
    float64 _Sum = abs(a[0][0]);
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t col = 0; col < N; ++col)
        {
            _Sum = max(abs(a[col][row]), _Sum);
        }
    }
    return _Sum;
}

template<std::size_t N>
float64 UniformNorm(fvec<N> a)
{
    return UniformNorm(matrix<1, N>{a});
}

/****************************************************************************************\
*                                        DISTANCES                                       *
\****************************************************************************************/

template<std::size_t N>
float64 ManhattanDistance(fvec<N> _Left, fvec<N> _Right)
{
    float64 _Res = 0;
    for (int i = 0; i < N; ++i)
    {
        _Res += abs(_Left[i] - _Right[i]);
    }
    return _Res;
}

template<std::size_t N>
float64 EuclideanDistance(fvec<N> _Left, fvec<N> _Right)
{
    float64 _Res = 0;
    for (int i = 0; i < N; ++i)
    {
        _Res += (_Left[i] - _Right[i]) * (_Left[i] - _Right[i]);
    }
    return sqrt(_Res);
}

template<std::size_t N>
float64 MinkowskiDistance(fvec<N> _Left, fvec<N> _Right, uint64 p)
{
    float64 _Res = 0;
    for (int i = 0; i < N; ++i)
    {
        _Res += pow(abs(_Left[i] - _Right[i]), p);
    }
    return yroot(_Res, p);
}

template<std::size_t N>
float64 ChebyshevDistance(fvec<N> _Left, fvec<N> _Right)
{
    float64 _Res = abs(_Left[0] - _Right[0]);
    for (int i = 1; i < N; ++i)
    {
        _Res = max(abs(_Left[i] - _Right[i]), _Res);
    }
    return _Res;
}

template<std::size_t N>
float64 distance(fvec<N> _Left, fvec<N> _Right) {return EuclideanDistance(_Left, _Right);}

/****************************************************************************************\
*                                    MATRIX OPERATIONS                                   *
\****************************************************************************************/

namespace __linalg_literals {

/**
 * @brief Doing row operations to a matrix
 */
template<std::size_t N, std::size_t M>
struct __linalg_matrix_row_operator
{
private:
    matrix<N, M> Data;

public:
    __linalg_matrix_row_operator(const matrix<N, M>& in) : Data(in) {}
    __linalg_matrix_row_operator(const __linalg_matrix_row_operator&) = delete;

    matrix<N, M> mat()const {return Data;}

    typename matrix<N, M>::row_type row(std::size_t idx)const
    {
        typename matrix<N, M>::row_type _row;
        for (std::size_t i = 0; i < N; ++i)
        {
            _row[i] = Data[i][idx];
        }
        return _row;
    }

    void swap(std::size_t rdst, std::size_t rsrc)
    {
        assert(rsrc < M && rdst < M);
        for (std::size_t i = 0; i < N; ++i)
        {
            std::swap(Data[i][rdst], Data[i][rsrc]);
        }
    }

    void scale(std::size_t rdst, float64 scale)
    {
        assert(rdst < M);
        for (std::size_t i = 0; i < N; ++i)
        {
            Data[i][rdst] *= scale;
        }
    }

    void axpy(std::size_t rdst, std::size_t rsrc, float64 scale)
    {
        assert(rsrc < M && rdst < M);
        for (std::size_t i = 0; i < N; ++i)
        {
            Data[i][rdst] += scale * Data[i][rsrc];
        }
    }
};

template<typename _Tp, std::size_t _Nm>
_NODISCARD constexpr _Tp __linalg_vector_max_abs(gvec<_Tp, _Nm> _Vec, std::size_t* _Pos, std::size_t _StartPos = 0, std::size_t _EndPos = _Nm)noexcept
{
    if (_Nm == 1) {return _Vec[0];}
    _Tp _Res = _Vec[_StartPos];
    for (std::size_t i = _StartPos + 1; i < _EndPos; ++i)
    {
        _Tp _Abs = abs(_Vec[i]);
        if (_Abs > _Res)
        {
            _Res = _Abs;
            if (_Pos) {*_Pos = i;}
        }
    }
    return _Res;
}

template<typename _Tp, std::size_t _Nm>
_NODISCARD constexpr _Tp __linalg_vector_min(gvec<_Tp, _Nm> _Vec, std::size_t* _Pos, std::size_t _StartPos = 0, std::size_t _EndPos = _Nm)noexcept
{
    if (_Nm == 1) {return _Vec[0];}
    _Tp _Res = _Vec[_StartPos];
    for (std::size_t i = _StartPos + 1; i < _EndPos; ++i)
    {
        _Tp _Abs = abs(_Vec[i]);
        if (_Abs < _Res)
        {
            _Res = _Abs;
            if (_Pos) {*_Pos = i;}
        }
    }
    return _Res;
}

/**
 * @brief Returns the row echelon form of a matrix using Gauss Elimination
 * @param a Input matrix pointer
 */
template<std::size_t N, std::size_t M>
void __linalg_gauss_elimination_rref(__linalg_matrix_row_operator<N, M>* op)
{
    std::size_t _Rsize = min(uint64(N), M);
    std::size_t PivotOffset = 0;
    for (std::size_t col = 0; col < _Rsize; ++col)
    {
        std::size_t RealPos = col + PivotOffset;

        // Check if the pivot element is zero
        // if true, search the non-zero element in the next elems of the same row.
        if (!op->mat()[RealPos][col])
        {
            bool AllZero = 1;
            std::size_t NonZeroPos = col;
            while (AllZero && RealPos < N)
            {
                for (size_t row = col; row < M; ++row)
                {
                    if (op->mat()[RealPos][row])
                    {
                        NonZeroPos = row;
                        AllZero = 0;
                        break;
                    }
                }

                // If all elements in the row are zero, move to the next row
                if (AllZero)
                {
                    ++PivotOffset;
                    RealPos = col + PivotOffset;
                }
            }
            if (RealPos == N) {break;}
            // Swap rows to make the pivot element non-zero
            if (NonZeroPos != col) {op->swap(NonZeroPos, col);}
        }

        // Perform row operations to eliminate elements below the main diagonal
        for (int row = col + 1; row < M; ++row)
        {
            op->axpy(row, col, -op->mat()[RealPos][row] / op->mat()[RealPos][col]);
        }
    }
}

}

template<std::size_t N>
float64 Trace(matrix<N, N> a);
/**
 * @brief Compute the determinant of an array.
 */
template<std::size_t N>
float64 Determinant(matrix<N, N> a)
{
    __linalg_literals::__linalg_matrix_row_operator op(a);
    __linalg_literals::__linalg_gauss_elimination_rref(&op);
    return Trace(op.mat());
}

/**
 * @brief Returns the product along the main diagonals of a matrix x.
 */
template<std::size_t N>
float64 Trace(matrix<N, N> a)
{
    float64 _Res = 1;
    for (std::size_t i = 0; i < N; ++i)
    {
        _Res *= a[i][i];
    }
    return _Res;
}

namespace __linalg_literals {

template<std::size_t N, std::size_t M>
struct __linalg_matrix_equation_solution
{
    // TODO
};

}

/**
 * @todo Solve a linear matrix equation, or system of linear scalar equations.
 */
template<std::size_t N, std::size_t M>
__linalg_literals::__linalg_matrix_equation_solution<N, M>
SolveMatrixEquation(matrix<N, M> Coeffs, fvec<M> v)
{
    // TODO
}

/**
 * @brief Compute the inverse of a matrix.
 */
template<std::size_t N>
matrix<N, N> Inverse(matrix<N, N> a)
{
    // Push an identity matrix to the right of this matrix
    matrix<2 * N, N> im(a);
    for (size_t col = N; col < 2 * N; ++col)
    {
        for (size_t row = 0; row < N; ++row)
        {
            if (row == col - N) {im[col][row] = 1;}
            else {im[col][row] = 0;}
        }
    }

    __linalg_literals::__linalg_matrix_row_operator op(im);
    __linalg_literals::__linalg_gauss_elimination_rref(&op);

    // All Invertible matrices are square matrices, this means that only the
    // final element of the final row need to be checked, if this element is
    // zero, matrix is uninvertible.
    if (abs(op.mat()[N - 1][N - 1]) < 1e-12) {throw std::logic_error("Matrix is uninvertible.");}

    // Turn the left part into an identity
    matrix<N, N> result;
    for (size_t col = N - 1; col; --col)
    {
        op.scale(col, 1. / op.mat()[col][col]);
        for (size_t row = col - 1; ; --row)
        {
            op.axpy(row, col, -op.mat()[col][row]);
            if (!row) {break;}
        }
    }

    // Extract inverse matrix
    for (int col = 0; col < N; ++col)
    {
        result[col] = op.mat()[N + col];
    }
    return result;
}

/**
 * @brief Transposes a matrix
 */
template<std::size_t N, std::size_t M>
auto Transpose(matrix<N, M> a) {return a.Transpose();}

template<std::size_t N>
auto Transpose(fvec<N> a) {return matrix<1, N>{a}.Transpose();}

} _CSE_END

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
_STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif
