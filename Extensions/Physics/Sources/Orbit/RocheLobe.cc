// Roche Lobe functions is based on PyAstronomy
// PyAstronomy is released under the MIT license

/*
    Copyright (c) 2011, PyA group
    Adapted for CSpaceEngine, 2025, By StellarDX

    Permission is hereby granted, free of charge, to any person obtaining a copy of this
    software and associated documentation files (the "Software"), to deal in the Software
    without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
    to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies
    or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ORBIT_BEGIN

vec3 RocheLobe::BarycenterPos()const
{
    vec3 Result = vec3(Separation * (CompanionMass / (PrimaryMass + CompanionMass)), 0, 0);
    return (InvAxisMapper * matrix<1, 3>{Result})[0];
}

float64 RocheLobe::__Dimensionless_Potential_Impl(vec3 Pos)const
{
    float64 MassRatio = CompanionMass / PrimaryMass;
    float64 R1 = linalg::EuclideanNorm(Pos);
    vec3 Pos2 = vec3(Pos.x - 1, Pos.y, Pos.z);
    float64 R2 = linalg::EuclideanNorm(Pos2);
    return 2 / ((1 + MassRatio) * R1)
        + 2 * MassRatio / ((1 + MassRatio) * R2)
        + pow(Pos.x - MassRatio / (1 + MassRatio), 2) + Pos.y * Pos.y;
}

float64 RocheLobe::DimensionlessPotential(vec3 Pos)const
{
    Pos = (AxisMapper * matrix<1, 3>{Pos})[0];
    return __Dimensionless_Potential_Impl(Pos);
}

float64 RocheLobe::Potential(vec3 Pos)const
{
    return -(GravConstant * (PrimaryMass + CompanionMass)) / (2. * Separation)
        * DimensionlessPotential(Pos / Separation);
}

float64 RocheLobe::__Dimensionless_Potential_X_Derivative_Impl(vec3 Pos)const
{
    float64 MassRatio = CompanionMass / PrimaryMass;
    float64 R1 = linalg::EuclideanNorm(Pos);
    vec3 Pos2 = vec3(Pos.x - 1, Pos.y, Pos.z);
    float64 R2 = linalg::EuclideanNorm(Pos2);
    return 2 / (1 + MassRatio) * (- Pos.x / (R1 * R1 * R1))
        + 2 * MassRatio / (1 + MassRatio) * (-(Pos.x - 1) / (R2 * R2 * R2))
         + 2 * (Pos.x - MassRatio / (1 + MassRatio));
}

float64 RocheLobe::DimensionlessPotentialXDerivative(vec3 Pos)const
{
    Pos = (AxisMapper * matrix<1, 3>{Pos})[0];
    return __Dimensionless_Potential_X_Derivative_Impl(Pos);
}

float64 RocheLobe::__Dimensionless_Potential_Y_Derivative_Impl(vec3 Pos)const
{
    float64 MassRatio = CompanionMass / PrimaryMass;
    float64 R1 = linalg::EuclideanNorm(Pos);
    vec3 Pos2 = vec3(Pos.x - 1, Pos.y, Pos.z);
    float64 R2 = linalg::EuclideanNorm(Pos2);
    return 2 / (1 + MassRatio) * (-Pos.y / (R1 * R1 * R1))
        + 2 * MassRatio / (1 + MassRatio) * (-Pos.y / (R2 * R2 * R2))
        + 2 * Pos.y;
}

float64 RocheLobe::DimensionlessPotentialYDerivative(vec3 Pos)const
{
    Pos = (AxisMapper * matrix<1, 3>{Pos})[0];
    return __Dimensionless_Potential_Y_Derivative_Impl(Pos);
}

float64 RocheLobe::__Dimensionless_Potential_Z_Derivative_Impl(vec3 Pos)const
{
    float64 MassRatio = CompanionMass / PrimaryMass;
    float64 R1 = linalg::EuclideanNorm(Pos);
    vec3 Pos2 = vec3(Pos.x - 1, Pos.y, Pos.z);
    float64 R2 = linalg::EuclideanNorm(Pos2);
    return 2 / (1 + MassRatio) * (-Pos.z / (R1 * R1 * R1))
        + 2 * MassRatio / (1 + MassRatio) * (-Pos.z / (R2 * R2 * R2));
}

float64 RocheLobe::DimensionlessPotentialZDerivative(vec3 Pos)const
{
    Pos = (AxisMapper * matrix<1, 3>{Pos})[0];
    return __Dimensionless_Potential_Z_Derivative_Impl(Pos);
}

float64 RocheLobe::__Eggelton_1983_Effective_Lobe_Radius(float64 MassRatio)const
{
    float64 CbrtMassRatio = cbrt(MassRatio);
    return (0.49 * CbrtMassRatio * CbrtMassRatio) /
        ((0.6 * CbrtMassRatio * CbrtMassRatio) + ln(1 + CbrtMassRatio));
}

float64 RocheLobe::PrimaryEffectiveLobeRadius()const
{
    return __Eggelton_1983_Effective_Lobe_Radius(PrimaryMass / CompanionMass);
}

float64 RocheLobe::CompanionEffectiveLobeRadius()const
{
    return __Eggelton_1983_Effective_Lobe_Radius(CompanionMass / PrimaryMass);
}

std::array<vec3, 5> RocheLobe::LagrangePoints(const SolvePolyRoutine& Routine)const
{
    float64 Mu = CompanionMass / (PrimaryMass + CompanionMass);
    std::vector<complex64> LRoots;
    float64 RealRoot;
    std::array<vec3, 5> Results;

    // L1
    SolvePoly({1, Mu - 3, 3 - 2 * Mu, -Mu, 2 * Mu, -Mu}, LRoots, Routine);
    RealRoot = std::find_if(LRoots.begin(), LRoots.end(), [](complex64 x)
    {
        return abs(x.imag()) < 1e-13;
    })->real();
    Results[0] = vec3(Separation - RealRoot * Separation, 0, 0);

    // L2
    SolvePoly({1, 3 - Mu, 3 - 2 * Mu, -Mu, -2 * Mu, -Mu}, LRoots, Routine);
    RealRoot = std::find_if(LRoots.begin(), LRoots.end(), [](complex64 x)
    {
        return abs(x.imag()) < 1e-13;
    })->real();
    Results[1] = vec3(Separation + RealRoot * Separation, 0, 0);

    // L3
    // Unnormalized equation:
    // M_1 / (R - x)^2 + M_2 / (2 * R - x)^2 = (M_2 / (M_1 + M_2) * R + R - x) * ((M_1 + M_2) / R^3)
    // => (4 * M_1 * R^2 - 4 * M_1 * R * x + M_1 * x^2 + M_2 * R^2 - 2M_2 * R * x + M_2 * x^2) / (4 * R^4 - 12 * R^3 * x + 13 * R^2 * x^2 - 6 * R * x^3 + x^4) = (M_1 * R - M_1 * x + 2 * M_2 * R - M_2 * x) / R^3
    // => (4 * M_1 * R^5) - (4 * M_1 * R^4 * x) + (M_1 * R^3 * x^2) + (M_2 * R^5) - (2 * M_2 * R^4 * x) + (M_2 * R^3 * x^2) = (4 * M_1 * R^5) - (16 * M_1 * R^4 * x) + (25 * M_1 * R^3 * x^2) - (19 * M_1 * R^2 * x^3) + (7 * M_1 * R * x^4) - (M_1 * x^5) + (8 * M_2 * R^5) - (28 * M_2 * R^4 * x) + (38 * M_2 * R^3 * x^2) - (25 * M_2 * R^2 * x^3) + (8 * M_2 * R * x^4) - (M_2 * x^5)
    // => ((M_1 * R^3) + (M_2 * R^3)) * x^2 - ((4 * M_1 * R^4) + (2 * M_2 * R^4)) * x + (M_2 * R^5) = -(M_1 + M_2) * x^5 + ((7 * M_1 * R) + (8 * M_2 * R)) * x^4 - ((19 * M_1 * R^2) + (25 * M_2 * R^2)) * x^3 + ((25 * M_1 * R^3) + (38 * M_2 * R^3)) * x^2 - ((16 * M_1 * R^4) + (28 * M_2 * R^4)) * x + (8 * M_2 * R^5)
    // =>
    //    +(M_1 + M_2) * x^5
    //    -((7 * M_1) + (8 * M_2)) * R * x^4
    //    +((19 * M_1) + (25 * M_2)) * R^2 * x^3
    //    -((24 * M_1) + (37 * M_2)) * R^3 * x^2
    //    +((12 * M_1) + (26 * M_2)) * R^4 * x
    //    -(7 * M_2) * R^5
    float64 C5 = PrimaryMass + CompanionMass;
    float64 C4 = -(7 * PrimaryMass + 8 * CompanionMass)
        * Separation;
    float64 C3 = (19 * PrimaryMass + 25 * CompanionMass)
        * Separation * Separation;
    float64 C2 = -(24 * PrimaryMass + 37 * CompanionMass)
        * Separation * Separation * Separation;
    float64 C1 = (12 * PrimaryMass + 26 * CompanionMass)
        * Separation * Separation * Separation * Separation;
    float64 C0 = -(7 * CompanionMass)
        * Separation * Separation * Separation * Separation * Separation;
    SolvePoly({C5, C4, C3, C2, C1, C0}, LRoots, Routine);
    RealRoot = std::find_if(LRoots.begin(), LRoots.end(), [](complex64 x)
    {
        return abs(x.imag()) < 1e-13;
    })->real();
    Results[2] = vec3(-(Separation - RealRoot), 0, 0);

    // L4
    //float64 Correction = (PrimaryMass - CompanionMass) / (PrimaryMass + CompanionMass);
    Results[3] = Separation * vec3(0.5 /** Correction*/, 0.86602540378443864676372317075294, 0);

    // L5
    Results[4] = Separation * vec3(0.5 /** Correction*/, -0.86602540378443864676372317075294, 0);

    for (int i = 0; i < 5; ++i)
    {
        Results[i] = (InvAxisMapper * matrix<1, 3>{Results[i]})[0];
    }

    return Results;
}

std::array<vec3, 20> RocheLobe::EquipotentialDimensions(float64 PotentialOffset, const SolvePolyRoutine& SPRoutine)const
{
    /*
        已知归一化势函数为：
        Φ(p:vec3, q) =
            2 / ((1 + q) * r1) + (2 * q) / ((1 + q) * r2) + (p.x - q / (1 + q))^2 + p.y^2
        其中：
        r1 = sqrt(p.x^2 + p.y^2 + p.z^2)
        r2 = sqrt((p.x - 1)^2 + p.y^2 + p.z^2)
        q = M2 / M1

        (1) 要得到在X方向上，给定等势面Φ的洛希瓣半径，可以令y = z = 0，此时上式简化为：
            2 / ((1 + q) * abs(x)) + (2 * q) / ((1 + q) * abs(x - 1)) + (x - q / (1 + q))^2
            由于上式中出现了绝对值，故此时会出现一个大的分段函数。事实上在x = 0和1时刚好就是两个物体的位置，引力势为-inf
            (i) 当x > 1时，abs(x)和abs(x-1)为本身，整理得：
                (q^2 * x^4 - 3q^2 * x^3 + 3q^2 * x^2 + q^2 * x + 2q * x^4 - 4q * x^3 + 2q * x^2 + 4q * x - 2q + x^4 - x^3 + 2x - 2) / (q^2 * x^2 - q^2 * x + 2q * x^2 - 2q * x + x^2 - x)
                可以得到分母的二次式的解必然为0和1，也对应了两个物体的位置。带入等势面Φ，得：
                (q^2 * x^4 - 3q^2 * x^3 + 3q^2 * x^2 + q^2 * x + 2q * x^4 - 4q * x^3 + 2q * x^2 + 4q * x - 2q + x^4 - x^3 + 2x - 2) - (q^2 * x^2 - q^2 * x + 2q * x^2 - 2q * x + x^2 - x) * Φ = 0
                整理后可以得到一个4次方程：
                - (q^2 * Φ * x^2) + (q^2 * Φ * x) + (q^2 * x^4) - (3q^2 * x^3) + (3q^2 * x^2) + (q^2 * x) - (2q * Φ * x^2) + (2q * Φ * x) + (2q * x^4) - (4q * x^3) + (2q * x^2) + (4q * x) - (2q) - (Φ * x^2) + (Φ * x) + (x^4) - (x^3) + (2x) - (2) = 0
                => (q^2 + 2q + 1) * x^4
                 - (3q^2 + 4q + 1) * x^3
                 - (q^2 * Φ - 3q^2 + 2q * Φ - 2q + Φ) * x^2
                 + (q^2 * Φ + q^2 + 2q * Φ + 4q + Φ + 2) * x
                 - (2q + 2) = 0
                解此方程可得到两个符合条件的解，洛希瓣半径取较小的解
            (ii) 当0 < x < 1时，abs(x)为本身，abs(x-1)为1-x，整理得：
                (q^2 * x^4 - 3q^2 * x^3 + 3q^2 * x^2 - 3q^2 * x + 2q * x^4 - 4q * x^3 + 2q * x^2 - 2q + x^4 - x^3 + 2x - 2) - (q^2 * x^2 - q^2 * x + 2q * x^2 - 2q * x + x^2 - x) * Φ = 0
                => - (q^2 * Φ * x^2) + (q^2 * Φ * x) + (q^2 * x^4) - (3q^2 * x^3) + (3q^2 * x^2) - (3q^2 * x) - (2q * Φ * x^2) + (2q * Φ * x) + (2q * x^4) - (4q * x^3) + (2q * x^2) - (2q) - (Φ * x^2) + (Φ * x) + (x^4) - (x^3) + (2x) - (2)
                => (q^2 + 2q + 1) * x^4
                 - (3q^2 + 4q + 1) * x^3
                 - (q^2 * Φ - 3q^2 + 2q * Φ - 2q + Φ) * x^2
                 + (q^2 * Φ - 3q^2 + 2q * Φ + Φ + 2) * x
                 - (2q + 2) = 0
                解此方程可得到两个符合条件的解，如果取L1点的引力势则解为重根
            (iii) 当x < 0时，abs(x)为-x，abs(x-1)为1-x，整理得：
                (q^2 * x^4 - 3q^2 * x^3 + 3q^2 * x^2 - 3q^2 * x + 2q * x^4 - 4q * x^3 + 2q * x^2 - 4q * x + 2q + x^4 - x^3 - 2x + 2) - (q^2 * x^2 - q^2 * x + 2q * x^2 - 2q * x + x^2 - x) * Φ = 0
                => - (q^2 * Φ * x^2) + (q^2 * Φ * x) + (q^2 * x^4) - (3q^2 * x^3) + (3q^2 * x^2) - (3q^2 * x) - (2q * Φ * x^2) + (2q * Φ * x) + (2q * x^4) - (4q * x^3) + (2q * x^2) - (4q * x) + (2q) - (Φ * x^2) + (Φ * x) + (x^4) - (x^3) - (2x) + (2)
                => (q^2 + 2q + 1) * x^4
                 - (3q^2 + 4q + 1) * x^3
                 - (q^2 * Φ - 3q^2 + 2q * Φ - 2q + Φ) * x^2
                 + (q^2 * Φ - 3q^2 + 2q * Φ - 4q + Φ - 2) * x
                 + (2q + 2) = 0
                解此方程可得到两个符合条件的解，洛希瓣半径取较大的解

        (2) Y轴方向需要分别求解两个物体的位置，也就是分别令x = 0或1，z = 0：
            1)  x = 1时，求的是伴星的洛希瓣半径，此时上式简化为：
                2 / ((1 + q) * sqrt(1 + y^2)) + (2 * q) / ((1 + q) * abs(y)) + (1 - q / (1 + q))^2 + y^2
                此方程为关于X轴对称的偶函数且在y=0时分段，所以只需要求解其中一个分支，另一分支可以直接镜像过去，
                当y > 0时，方程简化为：
                2 / ((1 + q) * sqrt(1 + y^2)) + (2 * q) / ((1 + q) * y) + (1 - q / (1 + q))^2 + y^2
                此方程为代数-根式混合超越方程，无初等解析解，但是可以用一些其他的近似求解算法，
                本文利用洛希瓣的几何性质接牛顿迭代求解：
                (i) 以伴星位置为圆心，过U2点作圆交直线x=1于点U8'，作为洛希瓣半径的初值
                (ii) 以系统质心C为圆心，线段C-U3的长度为半径作圆交直线x=1于点U9'，作为另一等势点的初值
                (iii) 对这两个点分别接牛顿迭代以获取近似值
                以上求解算法实测仅需要不到10次迭代就可以到达1倍精度极限
            2)  x = 0时，求的是主星的洛希瓣半径，此时上式简化为：
                2 / ((1 + q) * abs(y)) + (2 * q) / ((1 + q) * sqrt(1 + y^2)) + (-q / (1 + q))^2 + y^2
                此方程为关于X轴对称的偶函数且在y=0时分段，所以只需要求解其中一个分支，另一分支可以直接镜像过去，
                当y > 0时，方程简化为：
                2 / ((1 + q) * y) + (2 * q) / ((1 + q) * sqrt(1 + y^2)) + (-q / (1 + q))^2 + y^2
                此方程无初等解析解，使用以下的求解方式近似求解：
                (i) 以主星位置为圆心，过U7点作圆交直线x=1于点U12'，作为洛希瓣半径的初值
                (ii) 以系统质心C为圆心，线段C-U6的长度为半径作圆交直线y轴于点U13'，作为另一等势点的初值
                (iii) 对这两个点分别接牛顿迭代以获取近似值
                以上求解算法实测仅需要不到10次迭代就可以到达1倍精度极限

        (3) Z轴方向也需要分别求解两个物体的位置，也就是分别令x = 0或1，y = 0：
            1)  x = 1时，求的是伴星的洛希瓣半径，此时上式简化为：
                2 / ((1 + q) * sqrt(1 + z^2)) + (2 * q) / ((1 + q) * abs(z)) + (1 - q / (1 + q))^2
                此方程为关于X轴对称的偶函数且在z=0时分段，所以只需要求解其中一个分支，另一分支可以直接镜像过去，
                当z > 0时，方程简化为：
                2 / ((1 + q) * sqrt(1 + z^2)) + (2 * q) / ((1 + q) * z) + (1 - q / (1 + q))^2
                此方程无初等解析解，此处以伴星位置为圆心，过U8点作圆交直线x=1于点U16'，作为洛希瓣半径的初值，
                随后接牛顿迭代求解。
            2)  x = 0时，求的是主星的洛希瓣半径，此时上式简化为：
                2 / ((1 + q) * abs(z)) + (2 * q) / ((1 + q) * sqrt(1 + z^2)) + (-q / (1 + q))^2
                此方程为关于X轴对称的偶函数且在z=0时分段，所以只需要求解其中一个分支，另一分支可以直接镜像过去，
                当z > 0时，方程简化为：
                2 / ((1 + q) * z) + (2 * q) / ((1 + q) * sqrt(1 + z^2)) + (-q / (1 + q))^2
                此方程无初等解析解，此处以主星位置为圆心，过U12点作圆交直线x=1于点U18'，作为洛希瓣半径的初值，
                随后接牛顿迭代求解。
    */

    /*
        返回值结构如下：
        0和1分别为主星和伴星的洛希瓣尺寸
        其余返回值为计算过程中产生的等势点，大致位置分布如下，A和B为主星和伴星，L1-L5为拉格朗日点，Ui为等势点
        X-Y平面：
                      U13           U9
                             L4

                      U12           U8

            U6 L3 U7  A   U4 L1 U5  B   U2 L2 U3

                      U14           U10

                             L5
                      U15           U11

        X-Z平面：(Z方向因为离心势不起作用，在只有引力势的情况下每一边只有1个解)

                      U18           U16

            U6 L3 U7  A   U4 L1 U5  B   U2 L2 U3

                      U19           U17
    */

    std::array<vec3, 20> Results;
    float64 L1Pot = __Dimensionless_Potential_Impl(LagrangePoints(SPRoutine)[0] / Separation);
    float64 PHI = L1Pot + PotentialOffset;

    float64 q = CompanionMass / PrimaryMass;
    std::vector<complex64> FRoots;

    SolveQuartic(
    {
        q * q + 2 * q + 1,
        -(3 * q * q + 4 * q + 1),
        -(q * q * PHI - 3 * q * q + 2 * q * PHI - 2 * q + PHI),
        q * q * PHI + q * q + 2 * q * PHI + 4 * q + PHI + 2,
        -(2 * q + 2)
    }, FRoots);
    std::erase_if(FRoots, [](complex64 x){return x.real() <= 1;});
    float64 U8InitValue = min(FRoots[0].real(), FRoots[1].real());
    float64 U9InitValue = max(FRoots[0].real(), FRoots[1].real());
    Results[2] = vec3(U8InitValue, 0, 0) * Separation;
    Results[3] = vec3(U9InitValue, 0, 0) * Separation;

    SolveQuartic(
    {
        q * q + 2 * q + 1,
        -(3 * q * q + 4 * q + 1),
        -(q * q * PHI - 3 * q * q + 2 * q * PHI - 2 * q + PHI),
        q * q * PHI - 3 * q * q + 2 * q * PHI + PHI + 2,
        -(2 * q + 2)
    }, FRoots);
    std::erase_if(FRoots, [](complex64 x){return x.real() <= 0 || x.real() >= 1;});
    Results[4] = vec3(min(FRoots[0].real(), FRoots[1].real()), 0, 0) * Separation;
    Results[5] = vec3(max(FRoots[0].real(), FRoots[1].real()), 0, 0) * Separation;

    SolveQuartic(
    {
        q * q + 2 * q + 1,
        -(3 * q * q + 4 * q + 1),
        -(q * q * PHI - 3 * q * q + 2 * q * PHI - 2 * q + PHI),
        q * q * PHI - 3 * q * q + 2 * q * PHI - 4 * q + PHI - 2,
        2 * q + 2
    }, FRoots);
    std::erase_if(FRoots, [](complex64 x){return x.real() >= 0;});
    float64 U13InitValue = min(FRoots[0].real(), FRoots[1].real());
    float64 U12InitValue = max(FRoots[0].real(), FRoots[1].real());
    Results[6] = vec3(U13InitValue, 0, 0) * Separation;
    Results[7] = vec3(U12InitValue, 0, 0) * Separation;

    Results[1].x = Results[2].x - Results[5].x;
    Results[0].x = Results[4].x - Results[7].x;

    U8InitValue -= 1;
    float64 Barycen = BarycenterPos().x / Separation;
    U9InitValue = sqrt(pow(U9InitValue - Barycen, 2) - pow(1 - Barycen, 2));
    float64 NRoot1 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 y){return __Dimensionless_Potential_Impl({1, y, 0}) - PHI;},
        [this](float64 y){return __Dimensionless_Potential_Y_Derivative_Impl({1, y, 0});},
        U8InitValue);
    float64 NRoot2 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 y){return __Dimensionless_Potential_Impl({1, y, 0}) - PHI;},
        [this](float64 y){return __Dimensionless_Potential_Y_Derivative_Impl({1, y, 0});},
        U9InitValue);
    float64 U16InitValue = NRoot1;
    Results[8] = vec3(1, NRoot1, 0) * Separation;
    Results[9] = vec3(1, NRoot2, 0) * Separation;
    Results[10] = vec3(1, -NRoot1, 0) * Separation;
    Results[11] = vec3(1, -NRoot2, 0) * Separation;

    Results[1].z = Results[8].y * 2;

    U12InitValue = abs(U12InitValue);
    U13InitValue = sqrt(pow(Barycen - U13InitValue, 2) - pow(Barycen, 2));
    NRoot1 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 y){return __Dimensionless_Potential_Impl({0, y, 0}) - PHI;},
        [this](float64 y){return __Dimensionless_Potential_Y_Derivative_Impl({0, y, 0});},
        U12InitValue);
    NRoot2 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 y){return __Dimensionless_Potential_Impl({0, y, 0}) - PHI;},
        [this](float64 y){return __Dimensionless_Potential_Y_Derivative_Impl({0, y, 0});},
        U13InitValue);
    float64 U18InitValue = NRoot1;
    Results[12] = vec3(0, NRoot1, 0) * Separation;
    Results[13] = vec3(0, NRoot2, 0) * Separation;
    Results[14] = vec3(0, -NRoot1, 0) * Separation;
    Results[15] = vec3(0, -NRoot2, 0) * Separation;

    Results[0].z = Results[12].y * 2;

    NRoot1 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 z){return __Dimensionless_Potential_Impl({1, 0, z}) - PHI;},
        [this](float64 z){return __Dimensionless_Potential_Z_Derivative_Impl({1, 0, z});},
        U16InitValue);
    Results[16] = vec3(1, 0, NRoot1) * Separation;
    Results[17] = vec3(1, 0, -NRoot1) * Separation;

    Results[1].y = Results[16].z * 2;

    NRoot1 = SciCxx::HouseholderIteratorGroup::Newton(
        [this, PHI](float64 z){return __Dimensionless_Potential_Impl({0, 0, z}) - PHI;},
        [this](float64 z){return __Dimensionless_Potential_Z_Derivative_Impl({0, 0, z});},
        U18InitValue);
    Results[18] = vec3(0, 0, NRoot1) * Separation;
    Results[19] = vec3(0, 0, -NRoot1) * Separation;

    Results[0].y = Results[18].z * 2;

    for (int i = 2; i < 20; ++i)
    {
        Results[i] = (InvAxisMapper * matrix<1, 3>{Results[i]})[0];
    }

    return Results;
}

std::array<vec3, 2> RocheLobe::PhysicalDimensions(float64 PrimaryEffectiveRadius, float64 CompanionEffectiveRadius, const SolvePolyRoutine& SPRoutine)const
{
    return std::array<vec3, 2>(); // TODO
}

_ORBIT_END
_CSE_END
