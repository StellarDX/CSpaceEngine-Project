/********************************************************************************
  CSpaceEngine高等数学库，也称"SciC++"
  CSpaceEngine Advanced math Library, also called "SciC++"

  介绍 / Description:
    这些功能主要用于解决一些复杂度极高的数学问题
    This file contains classes and functions used to solve math problems
    with very high complexity.

  功能 / Features:
    1. 坐标转换(不在独立命名空间)
    2. std::vector的四则运算符和动态矩阵
    3. 一些特殊函数
    4. 导数
    5. 积分
    6. 常微分方程
    7. 反函数

  「从前有棵树，叫高数，上面挂了很多人；」
  「旁边有座坟，叫微积分，里面葬了很多人。」
********************************************************************************/

/*
    CSpaceEngine Advanced math Library.
    Copyright 漏 StellarDX Astronomy.

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

// 另外，此模块部分功能译自Boost，以及SciPy等使用其他语言编写的科学计算库，这些库遵循以下协议开源。

// Boost：提供高斯积分，二分搜索
/*
    Copyright John Maddock 2017.
    Copyright Nick Thompson 2017.
    Use, modification and distribution are subject to the
    Boost Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

// SciPy：提供一阶导函数，牛顿-科特斯积分，龙格-库塔微分方程，牛顿迭代
/*
    Copyright (c) 2001-2002 Enthought, Inc. 2003-2024, SciPy Developers.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials provided
       with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Pynverse：提供反函数
/*
    Copyright (c) 2016 Alvaro Sanchez-Gonzalez

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
    IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// 「常言道，前人栽树后人乘凉，但是如果前人栽的树太多，后人也就太容易挂在上面了。」

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
#include <memory>

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
 * @brief 将直角坐标(XY)转换为极坐标(r, θ)
 * @param[in] XY 直角坐标输入 (X: 水平分量, Y: 垂直分量)
 * @return 极坐标输出 (r: 距离, θ: 方位角[角度制, 范围-180°~180°])
 * @note
 * - 坐标系定义：
 *   - 本初子午线方向：X轴正方向
 *   - 角度方向：X轴逆时针旋转为正（数学标准）
 */
vec2 __cdecl XYToPolar(vec2 XY);

/**
 * @brief 将三维直角坐标(XYZ)转换为极坐标(Lon, Lat, Dist)
 * @param[in] XYZ 直角坐标输入 (X: 前方向分量, Y: 垂直分量, Z: 右方向分量)
 * @return 极坐标输出 (Lon: 经度[角度制, 范围-180°~180°],
 *                    Lat: 纬度[角度制, 范围-90°~90°],
 *                    Dist: 距离)
 * @note
 * - 坐标系定义：
 *   - 赤道平面：X-Z平面
 *   - 春分点方向：Z轴负方向
 *   - 经度方向：由西向东为正（从北极俯视逆时针）
 */
vec3 __cdecl XYZToPolar(vec3 XYZ);

/**
 * @brief 将极坐标(r, θ)转换为直角坐标(XY)
 * @param[in] Polar 极坐标输入 (r: 距离, θ: 方位角[角度制])
 * @return 直角坐标输出 (X: 水平分量, Y: 垂直分量)
 * @note
 * - 坐标系定义：
 *   - 本初子午线方向：X轴正方向
 *   - 角度方向：X轴逆时针旋转为正（数学标准）
 */
vec2 __cdecl PolarToXY(vec2 Polar);

/**
 * @brief 将极坐标(Lon, Lat, Dist)转换为三维直角坐标(XYZ)
 * @param[in] Polar 极坐标输入 (Lon: 经度[角度制], Lat: 纬度[角度制], Dist: 空间距离)
 * @return 直角坐标输出 (X: 前方向分量, Y: 垂直分量, Z: 右方向分量)
 * @note
 * - 坐标系定义：
 *   - 赤道平面：X-Z平面
 *   - 春分点方向：Z轴负方向
 *   - 经度方向：由西向东为正（从北极俯视逆时针）
 */
vec3 __cdecl PolarToXYZ(vec3 Polar);


_SCICXX_BEGIN

using Function1D = std::function<float64(float64)>;

/****************************************************************************************\
*                                         变长矩阵                                        *
\****************************************************************************************/

#include "CSE/Base/LinAlg/DynamicMatrix.inc"


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
 * @param V 参数
 * @return 所有基本对称多项式的值，从e0-en排列(定义e0 = 1)
 * @example
 *      输入V = (2, 3, 4)
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
 *      输入V = (2, 3, 4, 5)
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
 *      输入V = (2, 3, 4, 5)
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
    std::vector<float64> GetCoefficients()const {return Coefficients;}

    float64 operator()(float64 x)const;
    Polynomial Derivative()const; // 多项式的导函数还是多项式
};

/**
 * @brief 生成n次第一类勒让德多项式的系数
 * https://en.wikipedia.org/wiki/Legendre_polynomials
 * @param n 勒让德多项式的次数
 * @return n次第一类勒让德多项式的系数，降幂排列，没有的次数填0
 * @example
 *      输入n = 2
 *      得到的结果为：(1.5, 0, -0.5)
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

uint64 __Int_Comb(uint64 n, uint64 k);

/**
 * @brief 生成n阶不完全贝尔多项式的值
 *  参考：https://en.wikipedia.org/wiki/Bell_polynomials
 * @param x 贝尔多项式的参数，参数的个数为求解阶数
 * @return 从0到n阶的所有不完全贝尔多项式的值，以上三角矩阵格式输出
 * @example
 *      输入：x = {1, 2, 3, 4, 5, 6}
 *      则得到的结果为：
 *      [  1,   0,   0,   0,   0,   0,   0,
 *       nan,   1,   2,   3,   4,   5,   6,
 *       nan, nan,   1,   6,  24,  80, 240,
 *       nan, nan, nan,   1,  12,  90, 540,
 *       nan, nan, nan, nan,   1,  20, 240,
 *       nan, nan, nan, nan, nan,   1,  30,
 *       nan, nan, nan, nan, nan, nan,   1]
 */
DynamicMatrix<float64> BellPolynomialsTriangularArray(std::vector<float64> x);


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
    virtual float64 operator()(float64 x)const = 0;
};

// ------------------------------------------------------------------------------------- //

/**
 * @brief 基于有限差分法的一元函数的数值导数 (此功能翻译自SciPy，并转化为独立的类)
 * 据SciPy的文档所说，此方法的实现受到了jacobi、numdifftools和DERIVEST的启发，但
 * 其实现更直接地遵循了泰勒级数理论（可以说是天真地遵循了泰勒级数理论）。
 * <a href="https://docs.scipy.org/doc/scipy/reference/generated/scipy.differentiate.derivative.html#scipy.differentiate.derivative">参见官方文档</a>
 *
 * 「凌波能信步，苦海岂无边。函数千千万万，一次最简单。」
 *
 * @example
 * 求ln(x)的导函数，已知ln'(x) = 1 / x
 *  float64 (*f)(float64) = cse::ln;
 *  SciCxx::Adaptive1stOrderFDDerivativeFunction df(f);
 *  cout << df(1) << ' ';
 *  cout << df(2) << ' ';
 *  cout << df(5) << '\n';
 *
 * 输出: 0.999999999998355 0.499999999999178 0.200000000000003
 */
class Adaptive1stOrderFDDerivativeFunction : public DerivativeFunction
{
public:
    using Mybase = DerivativeFunction;

    enum DirectionType
    {
        Center = 0, Forward = 1, Backward = -1
    };

    class Iterator : public ElementwiseIterator
    {
    public:
        using Mybase = ElementwiseIterator;
        friend class Adaptive1stOrderFDDerivativeFunction;

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
        float64                RelativeTolerence;
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

protected:
    // 一些附加选项，懒得加getter和setter了
    float64       AbsoluteTolerence = 300; // 绝对误差的负对数，默认 -log(0x1p-2022) = 307.65265556858878150844115040843
    float64       RelativeTolerence = 7.5; // 相对误差的负对数，默认 -log(sqrt(0x1p-52)) ~= 7.8267798872635110755572112628368
    uint64        FDMOrder          = 8;   // 有限差分阶数，必须为偶数(与导函数阶数无关)
    float64       InitialStepSize   = 0.5; // 初始步长
    float64       StepFactor        = 2;   // 每次迭代时步长减小的系数，即每迭代1次，步长减小n。
    float64       MaxIteration      = 1;   // 最大迭代次数的对数，向下取整
    DirectionType Direction         = Center; // 采样点取值方向

public:
    Adaptive1stOrderFDDerivativeFunction(Function1D Function)
    {
        OriginalFunction = Function;
        DerivativeOrder = 1;
    }

    float64 operator()(float64 x)const override;
};

/* ************************************************************************** *\
   丹灵：莱布尼茨于1695年9月30日在致洛必达的信中提过这样一个问题，大致意思是整数阶的
   导数概念是否能够被推广到非整数阶。洛必达当时收到信也好奇，并在回应中提到了这样一个
   问题，就是一个函数的半阶导函数会是什么样的。莱布尼茨最后认为，这是个悖论，但总有一
   天，我们会得出有意义的结论。目前，我们将幂，阶乘等看似离散的定义成功的推广到了全体
   实数甚至是复数域上，当然导数与积分也不例外。先是柯西推出了迭代积分公式，将这个公式
   继续向实数域推广，即为黎曼-刘维尔积分，再根据这一定义又可以定义黎曼-刘维尔导数和卡
   普托导数。使用黎曼-刘维尔积分生成不定积分的方式很简单，只需要一个定积分算法就能完
   成。但是数值计算黎曼-刘维尔导数就有些复杂了，其中有一个办法就是使用格林沃尔德-莱特
   尼科夫算法将原来的导数化为有限差分。然而，据薛定宇的文章所说，相比整数阶的导函数仅
   需要求导点附近的几个点就能算，非整数阶导需要从t0开始的所有函数值才能计算，也就是说
   尽管能使用有限差分计算但是计算结果仍然为“递推函数”，使得要获取一个确定点的函数值变
   得难上加难。所以本文中的求高阶导的方法是从定义下手，先用二项式差分求整数阶导，然后
   套黎曼-刘维尔导数或卡普托导数得到非整数阶的导函数。

   参考文献：
    [1] 薛定宇.分数阶微积分学与分数阶控制[M].科学出版社,2018.
    [2] Das S. L’Hopital’s question-"What is ½ derivative of f (x) = x i.e.
        d1/2 f(x) / dx1/2?"[A]. 2013.
    [3] Wikipedia Contributors. Riemann–Liouville integral — Wikipedia, The
        Free Encyclopedia[A/OL]. (2025).
        https://en.wikipedia.org/wiki/Riemann%E2%80%93Liouville_integral
    [4] Wikipedia Contributors. Caputo fractional derivative — Wikipedia,
        The Free Encyclopedia[A/OL]. (2025).
        https://en.wikipedia.org/wiki/Caputo_fractional_derivative
\* ************************************************************************** */

/**
 * @brief 黎曼-刘维尔/二项式差分导数，基于黎曼-刘维尔导数和有限差分得到。因导数算法的权重
 * 生成部分使用了二项式定理而得名。严格来说此方法也是有限差分的一个子集，但是目前还无法将此
 * 类与上述类合并。
 *
 *  此导函数一次性封装了三种算法，分别是二项式差分，黎曼-刘维尔导数和卡普托导数。其中二项
 *  式差分用于求整数阶的导数，剩下两种则将导数扩展到非整数阶。
 *
 *  其中二项式差分求导的定义如下，用于求整数阶导数：
 *      (α)                           n
 *      f(x) = lim(h->0) ((1 / h) * ( Σ (pow(-1, j) * C(n, j) * f(x - j * h))))
 *                                   j=1
 *
 *  黎曼-刘维尔导数的定义为：
 *      (α)                          x                                    (n)
 *      f(x) = (1 / gamma(n - α)) * (∫ (f(t) / pow(x - t, α - n + 1) * dt))
 *       i                           i
 *
 *  卡普托导数的定义为：
 *      (α)                         x  (n)
 *      f(x) = (1 / gamma(n - α)) * ∫ (f(t) / pow(x - t, α - n + 1) * dt)
 *       i                          i
 *
 *  其中α为导函数的阶数，n = ceil(α)，i为积分初值，一般置0。
 *
 *  事实上这两者的共同点在于，它们都是一个函数的n阶不定积分的ceil(n)阶导，只是黎曼-刘维尔
 *  算法是先积分后求导，而卡普托算法则是先求导后积分。尽管它们的顺序不同，但是在某些情况下
 *  仍然会得到截然不同的结果。相比之下，卡普托算法的优点是积分初值可以被保持下来，而黎曼-
 *  刘维尔算法则需要额外引入这个分数阶初值条件。
 *
 * @warning 注：此方法目前正在优化中，精度较低且延迟极高，慎用。
 */
class RiemannLiouvilleBinomialFDDerivativeFunction;

using DefaultDerivativeFunction = Adaptive1stOrderFDDerivativeFunction;


/****************************************************************************************\
*                                          积分                                          *
\****************************************************************************************/

// 参见：https://en.wikipedia.org/wiki/Numerical_integration

/**
 * @brief 一元函数的定积分。
 */
typedef class DefiniteIntegratingFunction
{
protected:
    virtual float64 Run(Function1D f, float64 a, float64 b)const = 0;
public:
    float64 operator()(Function1D f, float64 a, float64 b)const;
}IntegralFunction;

/**
 * @brief 基于采样点的定积分。
 */
class SampleBasedIntegratingFunction : public DefiniteIntegratingFunction
{
public:
    using Mybase = DefiniteIntegratingFunction;
    using Mybase::operator();

protected:
    float64 Run(Function1D f, float64 a, float64 b)const final;
    virtual float64 Run(std::vector<vec2> Samples)const = 0;

public:
    //virtual ~SampleBasedIntegratingFunction() {} // “氨醛”
    // 采样函数
    static std::vector<vec2> GetEvenlySpacedSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples); // 一元函数固定步长采样
    static std::vector<vec2> GetSamplesFromParametricCurve(Function1D x, Function1D y, float64 a, float64 b, uint64 Samples);

    virtual std::vector<vec2> GetSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples = 0)const;
    float64 operator()(std::vector<vec2> Samples)const;
};

/**
 * @brief 生成一元函数的不定积分。
 */
typedef class IndefiniteIntegratingFunction
{
protected:
    Function1D OriginalFunction; // 原函数
    float64    IntegralOrder;  // 积分阶数(可以为非整数)

public:
    virtual float64 operator()(float64 x)const = 0;
}AntiderivativeFunction;

// ------------------------------------------------------------------------------------- //

#if defined __GNUG__ && !defined(__clang__)
#define __Tbl_FpType __float128 // GCC已经支持四倍精度(15+112)
#else
#define __Tbl_FpType long double // 设置为当前编译器最大可支持精度(一般为15+64长精度)
#endif

extern const __Tbl_FpType __Gaussian07_Table[8];
extern const __Tbl_FpType __Kronrod15_Table[16];
extern const __Tbl_FpType __Gaussian10_Table[10];
extern const __Tbl_FpType __Kronrod21_Table[22];
extern const __Tbl_FpType __Gaussian15_Table[16];
extern const __Tbl_FpType __Kronrod31_Table[32];
extern const __Tbl_FpType __Gaussian20_Table[20];
extern const __Tbl_FpType __Kronrod41_Table[42];
extern const __Tbl_FpType __Gaussian25_Table[26];
extern const __Tbl_FpType __Kronrod51_Table[52];
extern const __Tbl_FpType __Gaussian30_Table[30];
extern const __Tbl_FpType __Kronrod61_Table[62];

#undef __Tbl_FpType
#undef __Tbl_Fp

/**
 * @brief 高斯-克朗罗德积分 (实为高斯积分和高斯-克朗罗德积分两种方法的合并)
 *
 * 「每日一积，成为积佬」
 *
 * @example
 *  计算exp(-t^2)从0到+inf的积分：
 *      auto f1 = [](double t) { return exp(-t*t); };
 *      GaussKronrodQuadrature IntegralFunc;
 *      cout << IntegralFunc(f1, 0, std::numeric_limits<float64>::infinity()) << '\n';
 *  输出：0.886226925452758 (sqrt(pi)/2)
 */
class GaussKronrodQuadrature : public DefiniteIntegratingFunction
{
public:
    using Mybase = DefiniteIntegratingFunction;

protected:
    std::vector<float64> GaussCoefficients;
    std::vector<float64> KronrodCoefficients;

    uint64  Order     = 21;
    float64 Tolerence = 14;
    uint64  MaxLevels = 15;

    static bool GetNodesAndWeightsSpecialCases(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs = nullptr);

    float64 GIntegrate(Function1D f, float64* pL1)const;
    float64 GKAdaptiveIntegrate(Function1D f, float64 a, float64 b, uint64 Level, float64 Tol, float64* LastErr, float64* L1)const;
    float64 GKNonAdaptiveIntegrate(Function1D f, float64* Error, float64* pL1)const;
    float64 Run(Function1D f, float64 a, float64 b)const override;

public:
    bool    GaussOnly = 0; // 仅使用高斯积分 (不推荐)

    GaussKronrodQuadrature() : GaussKronrodQuadrature(21) {}
    GaussKronrodQuadrature(uint64 N) : Order(N) {GetNodesAndWeights(N, &GaussCoefficients, &KronrodCoefficients);}

    static void GetNodesAndWeights(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs = nullptr);

    float64 GaussIntegrate(Function1D f, float64 a, float64 b, float64* L1Norm = nullptr)const;
    float64 GaussKronrodIntegrate(Function1D f, float64 a, float64 b, float64* LastError = nullptr, float64* L1Norm = nullptr)const;
};

struct __Newton_Cotes_Param_Table_Type
{
    int64 Scale;
    const int64* Weights;
    int64 ErrorCoeff1;
    int64 ErrorCoeff2;
};
extern const __Newton_Cotes_Param_Table_Type __Newton_Cotes_Table[14];

/**
 * @brief 牛顿-科特斯积分，梯形，辛普森等积分算法均为此算法的子集
 * 注：使用此方法计算封闭图形的面积(即曲线积分)时阶数不能太高，最好直接使用梯形或辛普森，要不然结果会很怪
 *
 * 「一帆难遇风顺，一路高低不平。平平淡淡分秒，编织百味人生。」
 */
class NewtonCotesFormulae : public SampleBasedIntegratingFunction
{
public:
    using Mybase = SampleBasedIntegratingFunction;

    struct Block
    {
        std::vector<float64> Samples;
        float64              BStep;
        std::vector<float64> Weights;
        float64              ErrorCoeff;

        float64 Integrate();
        float64 ErrorEstimate();
    };

protected:
    uint64 Level;

    Block CreateBlock(uint64 N)const;
    Block CreateBlock(std::vector<vec2> Samples)const;

    float64 SingleIntegrateImpl(std::vector<vec2> Samples, uint64 L)const;
    static float64 SimpsonImpl(std::vector<vec2> Samples);

    float64 Run(std::vector<vec2> Samples) const override;

    static void GetWeightsErrorsFromSamples(std::vector<float64> SamplePos, bool IsEqual,
        std::vector<float64>* WeightOut, float64* ErrorOut);

public:
    NewtonCotesFormulae() : NewtonCotesFormulae(1) {}
    NewtonCotesFormulae(uint64 N) : Level(N) {}

    static void GetEvenlySizedParameters(uint64 N, std::vector<float64>* Weight, float64* Error);
    static void GetSpecialCaseParameters(uint64 N, std::vector<float64>* Weight, float64* Error);
    static void GetParametersFromSamples(std::vector<float64> SamplePos,
        std::vector<float64>* Weight, float64* Error);

    static bool IsEvenlySized(std::vector<vec2> Samples);

    static float64 Trapezoidal(std::vector<vec2> Samples);
    static float64 Simpson(std::vector<vec2> Samples);
    static float64 Romberg(std::vector<vec2> Samples,
        DynamicMatrix<float64>* RichardsonExtrapolationTable = nullptr); // 严格来说，龙贝格积分也是此积分的一个子集

    float64 SingleIntegrate(std::vector<vec2> Samples)const;
    float64 CompositeIntegrate(std::vector<vec2> Samples)const;
    float64 DiscreteIntegrate(std::vector<vec2> Samples)const;
};

using DefaultIntegratingFunction = GaussKronrodQuadrature;

// ------------------------------------------------------------------------------------- //

/**
 * @brief 黎曼-刘维尔积分，可计算非整数阶积分
 *
 *  此广义不定积分的定义为：
 *     (α)                          x
 *      ∫ f(x)dx = (1 / gamma(α)) * ∫ (f(t) * pow(x - t, α - 1)) * dt
 *                                  C
 *
 *  其中：
 *  α为不定积分的阶数。C为积分基数，决定积分得到的函数的初始条件，即F(C) = 0。
 *  (因为一个函数的不定积分有无限多个，故此处需要一个辅助点以确定最终得到的函数)
 *
 *  例如：被积函数为f(x) = x，初始条件F(1) = 0
 *  此时f(x)的一阶不定积分为 F1(x) = (1 / 2) * x^2 - (1 / 2)
 *  二阶不定积分为 F2(x) = (1 / 6) * x^3 - (1 / 2) * x + (1 / 3)
 *  更高阶的不定积分以此类推。
 *
 * 「量天何必苦登高，借问银河落九霄。直下凡尘几万里，几多里处宴蟠桃。」
 */
class RiemannLiouvilleIntegratingFunction : public IndefiniteIntegratingFunction
{
public:
    using Mybase        = IndefiniteIntegratingFunction;
    using DefaultEngine = DefaultIntegratingFunction;
    using EnginePtr     = std::shared_ptr<DefiniteIntegratingFunction>;

protected:
    EnginePtr Engine; // 定积分引擎
    vec2      InitValue = vec2(0, 0); // 积分基数

public:
    RiemannLiouvilleIntegratingFunction(Function1D f, float64 Order = 1,
        vec2 a = vec2(0, 0), EnginePtr IFunc = std::make_shared<DefaultEngine>(DefaultEngine()))
        : InitValue(a), Engine(IFunc)
    {
        OriginalFunction = f;
        IntegralOrder = Order;
    }

    float64 operator()(float64 x)const override;
};

typedef class RiemannLiouvilleBinomialFDDerivativeFunction : public DerivativeFunction
{
public:
    using Mybase        = DerivativeFunction;
    using DefaultEngine = DefaultIntegratingFunction;
    using EnginePtr     = std::shared_ptr<DefiniteIntegratingFunction>;

    class Iterator : public ElementwiseIterator
    {
    public:
        using Mybase = ElementwiseIterator;
        friend class RiemannLiouvilleBinomialFDDerivativeFunction;

        enum StateType
        {
            ErrorIncrease = 2
        };

    protected:
        float64                DerivativeOrder;
        float64                Input;
        float64                Output;
        float64                Error;
        float64                Step;
        float64                LastOutput;
        float64                LastError;
        float64                StepFactor;
        float64                AbsoluteTolerence;
        float64                RelativeTolerence;
        uint64                 Terms;
        std::vector<float64>   Weights;

    public:
        std::vector<float64> DerivativeWeight(uint64 n);

        DynamicMatrix<float64> PreEvaluator() override; // 获取采样点
        void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) override; // 求解主函数
        bool CheckTerminate() override; // 终止检测
        void Finalize() override {}
    };

protected:
    float64 AbsoluteTolerence = 300; // 绝对误差的负对数，默认 -log(0x1p-2022) = 307.65265556858878150844115040843
    float64 RelativeTolerence = 7.5; // 相对误差的负对数，默认 -log(sqrt(0x1p-52)) ~= 7.8267798872635110755572112628368
    float64 InitialStepSize   = 3; // 初始步长的负对数
    float64 StepFactor        = 10; // 每次迭代时步长减小的系数
    float64 MaxIteration      = 3;   // 最大迭代次数的对数，向下取整

    EnginePtr Engine; // 定积分引擎
    float64 InitValue = 0; // 积分基数

    float64 IntegralInline(Function1D Func, float64 x)const;
    float64 DerivativeInline(Function1D Func, float64 x)const;

public:
    RiemannLiouvilleBinomialFDDerivativeFunction(Function1D Function, float64 DerivOrder = 1,
        float64 a = 0, EnginePtr IFunc = std::make_shared<DefaultEngine>(DefaultEngine()))
        : InitValue(a), Engine(IFunc)
    {
        OriginalFunction = Function;
        DerivativeOrder = DerivOrder;
    }

    float64 Binomial(float64 x)const;
    float64 RiemannLiouville(float64 x)const;
    float64 Caputo(float64 x)const;

    float64 operator()(float64 x)const override;
}RiemannLiouvilleDerivativeFunction;

/**
 * @brief 多重积分(TODO)
 */
class __Multidimensional_Integral
{
public:
    using DefaultEngine = DefaultIntegratingFunction;

protected:
    static const DefaultEngine DefEngine;
    const DefiniteIntegratingFunction* Engine;

public:
    __Multidimensional_Integral() : Engine(&DefEngine) {}
    __Multidimensional_Integral(const DefiniteIntegratingFunction& IFunc) : Engine(&IFunc) {}

    // TODO...
};


/****************************************************************************************\
*                                         微分方程                                        *
\****************************************************************************************/

/**
 * @brief 常微分方程(ODE)求解器，此功能译自Scipy
 * 以下是来自SciPy的原始功能描述：
 *
 *  本函数对给定初始值的常微分方程组进行数值积分：
 *
 *      y' = f(x, y)
 *      y(x0) = y0
 *
 *  其中x为一维自变量，y(x)是N维向量值函数，f(x, y)是待求解微分方程。
 *  求解目标是在已知初始条件y(x0)=y0时，近似计算满足微分方程组的y(x)。
 *
 * @example 以下为一个求解常微分方程的例子：
 *  洛特卡-沃尔泰拉方程的定义如下：
 *      x'(t) = α * x - β * x * y
 *      y'(t) = -γ * y + δ * x * y
 *
 *  以α = 1.5, β = 1, γ = 3, δ = 1为例，可以使用以下方法生成此函数：
 *      float64 a = 1.5, b = 1, c = 3, d = 1;
 *      auto lotkavolterra = [a, b, c, d](float64 t, std::vector<float64> z)
 *      {
 *          float64 x = z[0], y = z[1];
 *          return std::vector<float64>{a*x - b*x*y, -c*y + d*x*y};
 *      };
 *      auto ODE = CreateODEFunction<...>(lotkavolterra, {10, 5}, 0, 15);
 *      cout << '[';
 *      for (int i = 0; i <= 300; ++i)
 *      {
 *          cout << (*ODE)((15. / 300.) * i) << ", ";
 *      }
 *      cout << "]\n";
 *
 *  将输出的数组使用matplotlib打印出来，即可看到函数图像
 */
class OrdinaryDifferentialEquation
{
public:
    using Fty        = std::vector<float64>(float64 Scalar, std::vector<float64> Coeffs);
    using ValueArray = std::vector<float64>;
    using StateType  = std::map<float64, ValueArray>;

    struct DenseOutput
    {
    public:
        using ValueArray = std::vector<float64>;

    protected:
        float64 First, Last;

    public:
        DenseOutput() {}
        DenseOutput(float64 Fi, float64 La) : First(Fi), Last(La) {}

        float64 first()const {return First;}
        float64 last()const {return Last;}
        float64 size()const {return Last - First;}

        virtual ValueArray operator()(float64 _Xx)const = 0;
    };

    enum StateCode
    {
        Processing = -1,
        Succeeded  = 0,
        Failed     = 1
    };

protected:
    std::function<Fty>   Invoker;      // 原函数
    enum StateCode       State;        // 当前状态
    float64              EndPoint;     // 结束点
    bool                 Direction;    // 求解方向，0表示正方向，1表示负方向
    StateType            StateBuffer;  // 采样缓冲区

    virtual int Run() = 0;

public:
    OrdinaryDifferentialEquation() {}
    OrdinaryDifferentialEquation(std::function<Fty> Right) : Invoker(Right) {}

    float64 CurrentPoint()const;
    float64 PrevPoint()const;
    StateType Solutions()const {return StateBuffer;}
    enum StateCode CurrentState()const {return State;}
    constexpr float64 size()const {return abs(CurrentPoint() - PrevPoint());}

    virtual void Init(ValueArray InitState, float64 First, float64 Last) = 0;
    virtual void Clear() = 0;
    void __cdecl InvokeRun() noexcept(0);
    virtual void SaveDenseOutput() = 0;

    virtual ValueArray operator()(float64 x)const = 0;
};

extern const uint64  __RK23_C_Table[3];
extern const uint64  __RK23_A_Table[9];
extern const uint64  __RK23_B_Table[3];
extern const uint64  __RK23_E_Table[4];
extern const uint64  __RK23_P_Table[12];
extern const float64 __RK45_C_Table[6];
extern const float64 __RK45_A_Table[30];
extern const float64 __RK45_B_Table[6];
extern const float64 __RK45_E_Table[7];
extern const float64 __RK45_P_Table[28];
#if 0
extern const __float128 __DOP853_C_Table[16];
extern const __float128 __DOP853_AB_Table[256];
extern const __float128 __DOP853_E3_Table_Offset[6];
extern const __float128 __DOP853_E5_Table[13];
extern const __float128 __DOP853_D_Table[64];
#endif

class RungeKuttaODEEngine : public OrdinaryDifferentialEquation
{
public:
    using Mybase     = OrdinaryDifferentialEquation;
    using ValueArray = Mybase::ValueArray;
    using StateType  = DynamicMatrix<float64>;

    struct DenseOutput : public Mybase::DenseOutput
    {
        using _Mybase  = Mybase::DenseOutput;
        using QTblType = DynamicMatrix<float64>;

        uint64     DenseOutputOrder;
        QTblType   QTable;
        ValueArray Base;

        DenseOutput() {}
        DenseOutput(uint64 DenseOutputOrder, float64 First, float64 Last,
            ValueArray BaseValue, QTblType QTbl)
            : DenseOutputOrder(DenseOutputOrder), _Mybase(First, Last),
            Base(BaseValue), QTable(QTbl) {}

        ValueArray operator()(float64 _Xx)const override;

        friend struct RungeKuttaODEEngine;
    };

    constexpr static const float64 MinFactor  = 0.2;
    constexpr static const float64 MaxFactor  = 10.;
    constexpr static const float64 FactorSafe = 0.9;

protected:
    uint32_t ErrorEsitmatorOrder;
    uint32_t StepTakenOrder;
    uint32_t NStages;
    uint64   DenseOutputOrder;
    uint64   EquationCount;

    const float64* CTable;
    const float64* ATable;
    const float64* BTable;
    const float64* ETable;
    const float64* PTable;

    std::map<float64, DenseOutput> Interpolants;

    ValueArray     CurrentFx;
    StateType      KTable;

    float64        RelTolerNLog = 3;
    float64        AbsTolerNLog = 6;
    float64        MaxStep      = __Float64::FromBytes(POS_INF_DOUBLE);
    float64        AbsStep;
    const float64  ErrExponent  = ErrorEsitmatorOrder + 1;

    float64 RMSNorm(std::vector<float64> Input);

    void SetInitStep();

    RungeKuttaODEEngine(std::function<Fty> Right,
        uint32_t ErrorEsitmatorOrder, uint32_t StepTakenOrder, uint32_t NStages,
        uint64 DenseOutputOrder, uint64 EquationCount) : Mybase(Right),
        ErrorEsitmatorOrder(ErrorEsitmatorOrder), StepTakenOrder(StepTakenOrder), NStages(NStages),
        DenseOutputOrder(DenseOutputOrder), EquationCount(EquationCount),
        CurrentFx(EquationCount), KTable({NStages + 1, EquationCount}) {}

    int Run()override;

public:
    void Init(ValueArray InitState, float64 First, float64 Last)override;
    virtual void Init(ValueArray InitState, float64 First, float64 Last, float64 InitStep);
    void Clear()override;
    void SaveDenseOutput()override;

    ValueArray operator()(float64 _Xx)const override;
};

typedef class RungeKutta2ndOrderODEEngine : public RungeKuttaODEEngine
{
public:
    using Mybase = RungeKuttaODEEngine;

    RungeKutta2ndOrderODEEngine(std::function<Fty> Function, uint64 EquationCount) :
        Mybase(Function, 2, 3, 3, 3, EquationCount) {}

    void Init(ValueArray InitState, float64 First, float64 Last,
        float64 InitStep = __Float64::FromBytes(BIG_NAN_DOUBLE))override
    {
        CTable = (float64*)__RK23_C_Table;
        ATable = (float64*)__RK23_A_Table;
        BTable = (float64*)__RK23_B_Table;
        ETable = (float64*)__RK23_E_Table;
        PTable = (float64*)__RK23_P_Table;
        Mybase::Init(InitState, First, Last, InitStep);
    }
}BogackiShampineODEEngine;

typedef class RungeKutta4thOrderODEEngine : public RungeKuttaODEEngine
{
public:
    using Mybase = RungeKuttaODEEngine;

    RungeKutta4thOrderODEEngine(std::function<Fty> Function, uint64 EquationCount) :
        Mybase(Function, 4, 5, 6, 4, EquationCount) {}

    void Init(ValueArray InitState, float64 First, float64 Last,
        float64 InitStep = __Float64::FromBytes(BIG_NAN_DOUBLE))override
    {
        CTable = __RK45_C_Table;
        ATable = __RK45_A_Table;
        BTable = __RK45_B_Table;
        ETable = __RK45_E_Table;
        PTable = __RK45_P_Table;
        Mybase::Init(InitState, First, Last, InitStep);
    }
}DormandPrinceODEEngine, RungeKuttaDPODEEngine, DOPRIODEEngine;

using DefaultODEEngine = RungeKutta4thOrderODEEngine;

// ------------------------------------------------------------------------------------- //

/**
 * @brief 快速创建常微分方程
 * @param Func 原函数
 * @param Coeffs 初值，值的个数即方程个数
 * @param First 积分起点
 * @param Last 积分终点
 * @return 常微分方程的函数指针
 */
template<typename Engine> requires std::is_base_of_v<OrdinaryDifferentialEquation, Engine>
std::shared_ptr<OrdinaryDifferentialEquation>
CreateODEFunction(std::function<OrdinaryDifferentialEquation::Fty> Func,
    OrdinaryDifferentialEquation::ValueArray Coeffs, float64 First, float64 Last)
{
    OrdinaryDifferentialEquation* Eng = new Engine(Func, Coeffs.size());
    Eng->Init(Coeffs, First, Last);
    while (Eng->CurrentState() == Eng->Processing)
    {
        Eng->InvokeRun();
        Eng->SaveDenseOutput();
    }
    return std::shared_ptr<OrdinaryDifferentialEquation>(Eng);
}

/****************************************************************************************\
*                                         数值优化                                        *
\****************************************************************************************/

/**
 * @brief 此功能由SciPy的minimize_scalar函数转写而成，用于求函数极小值
 */
class MinimizerBase
{
public:
    virtual vec2 operator()(Function1D Func)const = 0;
};

// ------------------------------------------------------------------------------------- //

/**
 * @brief 布伦特无约束极小值算法，此功能译自SciPy，不过Boost库中也有类似功能
 */
class BrentUnboundedMinimizer : public MinimizerBase
{
public:
    using Mybase = MinimizerBase;

    constexpr static const float64 GoldenRatio  = 0.61803398874989484820458683436564;
    constexpr static const float64 GoldenRatio2 = 1. + GoldenRatio;
    constexpr static const float64 GRatioConj   = 1. - GoldenRatio;

    struct BracketType
    {
        vec2 First;
        vec2 Centre;
        vec2 Last;
    };

protected:
    float64     Tolerence    = 7.83; // 误差的负对数 (SciPy给的默认值为1.48e-8)
    float64     MaxIter      = 2.7;  // 最大迭代次数的对数，向下取整
    float64     MinTolerence = 11;   // 最小误差的负对数

public:
    BrentUnboundedMinimizer() {}

    /**
     * 在目标函数 func 附近搜索一个有效的区间 (xa, xb, xc)，满足：
     *  1. xa < xb < xc
     *  2. f(xb) < f(xa) && f(xb) < f(xc)
     *
     * @note 此函数译自SciPy的bracket函数
     *
     * @param Function 目标函数
     * @param InitStart 初始点1 (可选，默认 0.0)
     * @param InitEnd 初始点2 (可选，默认 1.0)
     * @param FCalls 函数调用次数
     * @param GrowLimit 最大增长因子 (默认 110.0)
     * @param MaxIter 最大迭代次数的对数 (默认 3)
     * @return 返回BracketType封装的xa，xb和xc和对应函数值
     */
    static BracketType CreateBracket(Function1D Function, float64 InitStart = 0, float64 InitEnd = 1,
        uint64* FCalls = nullptr, float64 MaxIter = 3,
        float64 AbsoluteTolerence = 21, float64 GrowLimit = 110)noexcept(0);

    static BracketType CreateBracketFromArray(Function1D Function,
        std::vector<float64> Points = {}, uint64* FCalls = nullptr);

    vec2 Run(Function1D Func, std::vector<float64> Points = {})const;
    vec2 operator()(Function1D Func)const override;
};


/****************************************************************************************\
*                                         反函数                                         *
\****************************************************************************************/

class InverseFunction
{
protected:
    Function1D OriginalFunction; // 原函数
public:
    virtual float64 operator()(float64 x)const = 0;
};

class BracketingRootFindingEngine : public InverseFunction
{
public:
    using Mybase = InverseFunction;
protected:
    float64 First; // 起始点
    float64 Last;  // 终点
};

class IterativeRootFindingEngine : public InverseFunction
{
public:
    using Mybase = InverseFunction;
protected:
    float64 ReferencePoint; // 迭代参考点
};

// ------------------------------------------------------------------------------------- //

/* ************************************************************************** *\
   丹霞：Pynverse的作者Alvaro Sanchez Gonzalez(英)在Pynverse的免责声明中表示，它
   的算法尽管性能很好，但是在某些特定条件下仍会掉精度，并且它的精度是不可控的，所以也
   不能说是一种完全通用的方法。既然如此就会产生一个问题，就是对一个函数数值求反函数是
   否有一种精度可控且普适性的方法呢？答案是目前没有，甚至来说在一个开放区间内数值计算
   任意函数的反函数本身仍是数学界的一个尚未解决的难题。本质上来说，Pynverse的算法为
   一种可以“自适应”的布伦特算法——一种混合了二分法、割线法和逆二次插值的函数求解算法。
\* ************************************************************************** */

/*
    Pynverse库的免责声明原文：

    The problem of calculating the numerical inverse of an arbitrary
    function in unlimited or open intervals is still an open question in
    applied mathematics. The main purpose of this package is not to be fast,
    or as accurate as it could be if the inverse was calculated specifically
    for a known function, using more specialised techniques. The current
    implementation essentially uses the existing tools in scipy to solve the
    particular problem of finding the inverse of a function meeting the
    continuity and monotonicity conditions, but while it performs really
    well it may fail under certain conditions. For example when inverting a
    log10 it is known to start giving inccacurate values when being asked to
    invert -10, which should correspond to 0.0000000001 (1e-10), but gives
    instead 0.0000000000978 (0.978e-10).

    The advantage about estimating the inverse function is that the accuracy
    can always be verified by checking if f(finv(x))==x.
*/

// 一个-inf到+inf的范围
static const vec2 __Whole_Line =
    {__Float64::FromBytes(NEG_INF_DOUBLE), __Float64::FromBytes(POS_INF_DOUBLE)};

/**
 * @brief 布伦特反函数，译自pynverse，并转写为独立的类。
 *
 *  此方法的原理就是通过求一个与f(x)-y相关的损失函数的极小值以确定反函数
 *  的值。可选用如下函数作为损失函数：
 *      L_1(x) = pow(f(x) - y, 2)
 *      L_2(x) = abs(f(x) - y)
 *
 *  其中f(x)为原函数，y为反函数的参数。这里最好是使用L_1作为损失函数，因
 *  为它是光滑的，易于求导，而L_2可能会出现部分点不可导的情况。损失函数的
 *  极小值就是反函数值
 *
 * @example 求y = exp(x)在y = 10处的反函数值(为ln(10))
 *  先定义损失函数：
 *      L(x) = pow(exp(x) - 10, 2)
 *
 *  求导得：
 *      ∇L(x) = 2 * (exp(x) - 10) * exp(x)
 *
 *  令∇L(x) = 0，得x = ln(10)，为所求反函数值。
 */
class BrentInverseFunction : public InverseFunction
{
public:
    using Mybase        = InverseFunction;

protected:
    vec2    Domain;     // 定义域
    bvec2   OpenDomain; // 区间类型
    vec2    Range;      // 值域
    //float64 Tolerence;  // 精度的负对数

    void CheckParameters();

    vec2 GetReferencePoints(vec2 Domain)const;
    vec2 GetReferenceValues(vec2 Domain, int Trend)const;

public:
    /**
     * @brief 用一个一元函数创建其反函数
     * @param Func 原函数
     * @param Domain 原函数定义域
     * @param OpenDomain 定义域区间选项，0为闭区间，1为开区间
     * @param Range 原函数值域
     * @note 对一个函数取反后，原函数定义域映射到反函数值域，原函数值域映射到反函数定义域
     */
    BrentInverseFunction(Function1D Func, vec2 Domain = __Whole_Line,
        bvec2 OpenDomain = {0, 0}, vec2 Range = __Whole_Line)
        : Domain(Domain), OpenDomain(OpenDomain), Range(Range) //Tolerence(Tolerence),
    {
        OriginalFunction = Func;
        CheckParameters();
    }

    float64 operator()(float64 x)const override;
};

/**
 * @brief 二分搜索反函数，可以说是最简单的寻根算法了，只是计算量大了些。
 *
 * 「一尺之棰，日取其半，万世不竭。」
 */
class BisectionRootFindingEngine : public BracketingRootFindingEngine
{
public:
    using Mybase = BracketingRootFindingEngine;

protected:
    float64 AbsoluteTolernece = 11.7; // 绝对误差的负对数，默认2E-12
    float64 RelativeTolerence = 15;   // 相对误差的负对数，默认0x1.p-50
    float64 MaxIteration      = 2;    // 最大迭代次数的对数

public:
    BisectionRootFindingEngine(Function1D Func, vec2 Domain)
    {
        OriginalFunction = Func;
        First = min(Domain[0], Domain[1]);
        Last = max(Domain[0], Domain[1]);
    }

    float64 Run(float64 x = 0, uint64* IterCount = nullptr, uint64* FCallCount = nullptr)const;
    float64 operator()(float64 x) const override;
};

/**
 * @brief 豪斯霍尔德迭代族，牛顿迭代和哈雷迭代均属于此类的子集，以美国数
 * 学家阿尔斯通·斯科特·豪斯霍尔德的名字命名。
 *
 *  此迭代族的定义为：
 *                                    (d-1)             (d)
 *      x_n+1 = x_n + d * ((1 / f(x_n))    / (1 / f(x_n))  )
 *
 *  其中d为迭代阶数，d阶迭代算法具有d + 1阶的收敛性。
 *
 *  设g(x) = 1 / f(x_n)，上式的第二项可以简化为求g(x)的n阶导函数问题。
 *  g(x)可以看作1 / x和f(x)组成的复合函数，其导数可以套用Faa di Bruno
 *  求导公式计算，Faa di Bruno公式定义如下：
 *                                            n
 *                                          ( Σ (m_k))
 *              (n)            n             k=1               n      (j)
 *      [f(g(x))] = Σ ((n! / ( Π (m_i!))) * f        (g(x)) *  Π (pow(g (x) / j!, m_j)))
 *                            i=1                             j=1
 *
 *                      n
 *  其中m_n满足约束条件： Σ (i * m_i) = n
 *                     i=1
 *
 *  由于上式中j > n - k + 1时m_j必为0，此时再设k = Σ(m_n)，得：
 *              (n)   n  (k)                       (n-k+1)
 *      [f(g(x))]  =  Σ (f (g(x)) * B_n,k(g'(x) -> g     (x)))
 *                   k=0
 *
 *  其中B_n,k为不完全贝尔多项式。
 *
 *  (1 / x)的n阶导函数为(pow(-1, n) * n!) / pow(x, n + 1)，将g(x)代
 *  入Faa di Bruno公式，得：
 *      (n)      n                                                          (n-k+1)
 *      g (x) =  Σ (((pow(-1, k) * k!) / pow(f(x), k + 1)) * B_n,k(f'(x) -> f     (x)))
 *              k=1
 *
 *  此公式即为1 / f(x)的n阶导函数的公式，其中后半部分的贝尔多项式可以使
 *  用以下递推关系构造动态规划以加速计算：(C(n, m)为组合数)
 *                  n-k
 *      B_n+1,k+1 =  Σ (C(n, i) * x_i+1 * B_n-i,k(x_1 -> x_n-k-i+1))
 *                  i=0
 *
 *  根据以上的公式，可得阶数为1时，得到的结果即为牛顿迭代，阶数为2时，得
 *  到的结果即为哈雷迭代。
 *
 *  「你越是努力盯着上面的公式，那些符号便越是在你眼前模糊成一道道虚影」
 *  「别问为什么人在梦中还会犯困，这大概就是知识的力量吧。」
 *  「云里看的不仅仅是花，还有......」
 */
class HouseholderIteratorGroup : public IterativeRootFindingEngine
{
public:
    using Mybase = IterativeRootFindingEngine;

protected:
    std::vector<Function1D> DerivativeFunctions; // n阶导函数
    float64 AbsoluteTolerence = 7.83; // 绝对误差的负对数，默认1.48e-8
    float64 RelativeTolerence = __Float64::FromBytes(POS_INF_DOUBLE); // 相对误差的负对数，默认0
    float64 MaxIteration      = 1.7;  // 最大迭代次数的对数

public:
    HouseholderIteratorGroup(std::vector<Function1D> Functions, float64 RefX)
    {
        if (Functions.size() <= 1)
        {
            throw std::logic_error(
                "Original function and its derivative is required.");
        }
        OriginalFunction = Functions[0];
        DerivativeFunctions.insert(
            DerivativeFunctions.begin(), Functions.begin() + 1, Functions.end());
        ReferencePoint = RefX;
    }

    uint64 Order()const {return DerivativeFunctions.size();}

    float64 Run(float64 x = 0, uint64* IterCount = nullptr, uint64* FCallCount = nullptr)const;
    float64 operator()(float64 x) const override;

    static float64 Newton(Function1D f, Function1D df, float64 x = 0,
        uint64* IterCount = nullptr, uint64* FCallCount = nullptr, float64 MaxIter = 1.7,
        float64 AbsTol = 7.83, float64 RelTol = __Float64::FromBytes(POS_INF_DOUBLE));
    static float64 Halley(Function1D f, Function1D df, Function1D d2f, float64 x = 0,
        uint64* IterCount = nullptr, uint64* FCallCount = nullptr, float64 MaxIter = 1.7,
        float64 AbsTol = 7.83, float64 RelTol = __Float64::FromBytes(POS_INF_DOUBLE));
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
