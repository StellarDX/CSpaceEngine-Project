// 求解多项式方程的方法

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/AdvMath.h"

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

_CSE_BEGIN

void __Verify_Input_Output(InputArray Coeffs, OutputArray Roots, uint64 Power) noexcept(0)
{
    using namespace std;
    if (Coeffs.size() != Power + 1) {throw std::logic_error(format("Number of Coefficients is not match. (must be {})", Power + 1));}
    if (Roots.size() < Power) {Roots.resize(Power);} //{throw std::logic_error(format("Root container is too small. (must be {} or larger)", Power));}
    if (Coeffs[0] == 0) { throw std::logic_error("Highest power of polynomial can't be zero."); }
}

int SolveLinear(InputArray Coeffs, OutputArray Roots)
{
    __Verify_Input_Output(Coeffs, Roots, 1);
    float64 a = Coeffs[0];
    float64 b = Coeffs[1];

    Roots[0] = -b / a;

    return 0;
}

int SolveQuadratic(InputArray Coeffs, OutputArray Roots)
{
    __Verify_Input_Output(Coeffs, Roots, 2);
    float64 a = Coeffs[0];
    float64 b = Coeffs[1];
    float64 c = Coeffs[2];

    float64 del = b * b - 4.0 * a * c;

    Roots[0] = (-b + sqrtc(del)[0]) / (2. * a);
    Roots[1] = (-b - sqrtc(del)[0]) / (2. * a);

    return 0;
}

int __Zero_Like(float64 x, float64 Tolerence) {return abs(x) < Tolerence;}

int SolveCubic(InputArray Coeffs, OutputArray Roots, float64 Tolerence)
{
    /***************************************************************************
     * 三次方程求解算法
     *
     * 求解三次方程有两种算法，分别为卡尔丹公式和盛金公式。但是后来有人发现这两种算法都
     * 有问题：
     *  1) 卡尔丹公式没有认真对待复数解，且算法中含有大量复数开方。
     *  2) 范盛金的算法仍有部分缺陷需要更正
     * 对此2019年田红亮基于范盛金的成果进行了修正，下文采用此方法。
     *
     * 本算法的原作者范盛金于2018年9月6日晚在东莞离世，享年63岁
     *
     * 参考文献：
     *  [1] 范盛金.一元三次方程的新求根公式与新判别法[J].海南师范学院学报(自然科学版),
     *      1989,2(2):91-98.
     *  [2] 田红亮.一元三次方程根的解法[J].湖北工程学院学报,2019,39(06):97-105.
     *  [3] 雪鳕.卡丹公式欺骗了五百年所有数学家[EB/OL].(2013-11-08)[2019-08-18].
     *      http://blog.sina.com.cn/s/blog_6aa947af0101dy4f.html.
     *  [4] 沈天珩.一元高次方程寻根记杂谈[EB/OL].(2020-02-03)[2025-05-09].
     *      https://zhuanlan.zhihu.com/p/104832975
     *  [5] 毕式明,潘俊宇.“盛金公式”发明者、著名数学家范盛金辞世[EB/OL].南方Plus,
     *      (2018-09-08)[2025-05-09]
     *      https://www.sohu.com/a/252667814_100116740
     ***************************************************************************/

    __Verify_Input_Output(Coeffs, Roots, 3);

    float64 a = Coeffs[0];
    float64 b = Coeffs[1];
    float64 c = Coeffs[2];
    float64 d = Coeffs[3];
    float64 RTol = pow(10, -Tolerence);

    float64 A = b * b - 3. * a * c;
    float64 B = b * c - 9. * a * d;
    float64 C = c * c - 3. * b * d;

    float64 DEL = B * B - 4. * A * C;

    if (__Zero_Like(DEL, RTol)) // Δ = 0，3个实数根
    {
        if (__Zero_Like(A, RTol)) // A = 0，3个相等实数根
        {
            Roots[0] = -b / (3. * a);
            Roots[1] = Roots[0];
            Roots[2] = Roots[0];

            return 1;
        }
        else // A > 0，3个实数根中有2个相等
        {
            Roots[0] = (B / A) - (b / a);
            Roots[1] = -B / (2. * A);
            Roots[2] = Roots[1];

            return 2;
        }
    }

    if (DEL > 0) // Δ > 0，1个实数根和2个共轭复数根
    {
        float64 cbrtY1 = cbrt(((3. * a * B) - (2. * A * b) + (3. * a * sqrt(DEL))) / 2.);
        float64 cbrtY2 = cbrt(((3. * a * B) - (2. * A * b) - (3. * a * sqrt(DEL))) / 2.);

        Roots[0] = (cbrtY1 + cbrtY2 - b) / (3. * a);
        Roots[1] = (-(2. * b) - cbrtY1 - cbrtY2 + (1i * sqrt(3) * (cbrtY1 - cbrtY2))) / (6. * a);
        Roots[2] = (-(2. * b) - cbrtY1 - cbrtY2 + (1i * sqrt(3) * (cbrtY2 - cbrtY1))) / (6. * a);

        return 3;
    }

    if (DEL < 0) // Δ < 0，A > 0，3个不相等实数根
    {
        float64 tet = arccos(((3. * a * B) - (2. * A * b)) / (2. * A * sqrt(A))).ToDegrees();

        Roots[0] = ((2. * sqrt(A) * cos(Angle::FromDegrees(tet / 3.))) - b) / (3. * a);
        Roots[1] = ((2. * sqrt(A) * cos(Angle::FromDegrees((tet + 360) / 3.))) - b) / (3. * a);
        Roots[2] = ((2. * sqrt(A) * cos(Angle::FromDegrees((tet + 720) / 3.))) - b) / (3. * a);

        return 4;
    }

    return -1;
}

int SolveQuartic(InputArray Coeffs, OutputArray Roots, float64 Tolerence)
{
    /***************************************************************************
     * 四次方程求解算法
     *
     * 历史上最先提出一般形式的四次方程的解法的是卡尔丹的学生费拉里。但是其公式复杂度
     * 极高，对此，沈天珩对公式进行了简化。同样的，相比费拉里算法，沈天珩算法中不会出
     * 现复数开方。
     *
     * 沈天珩在知乎上另一篇四次方程求根公式证明文章下的评论说，他并没有对此公开发表过
     * 论文，仅仅只是在他本科辅修的毕设中提到过这个算法。对于计算机求解代码而言，不该
     * 出现根号时应尽量避免，万不得已出现根号最好避免复数开方，这些都是优势所在。毕竟
     * 像沈天珩算法这种除了求解以外还能判虚实判重根的算法太少了。
     *
     * 参考文献：
     *  [1] 沈天珩.一元高次方程寻根记杂谈[EB/OL].(2020-02-03)[2025-05-09].
     *      https://zhuanlan.zhihu.com/p/104832975
     ***************************************************************************/

    __Verify_Input_Output(Coeffs, Roots, 4);

    float64 a = Coeffs[0];
    float64 b = Coeffs[1];
    float64 c = Coeffs[2];
    float64 d = Coeffs[3];
    float64 e = Coeffs[4];
    float64 RTol = pow(10, -Tolerence);

    float64 D = 3. * b * b - 8. * a * c;
    float64 E = -b * b * b + 4. * a * b * c - 8. * a * a * d;
    float64 F = 3. * b * b * b * b + 16. * a * a * c * c - 16. * a * b * b * c + 16. * a * a * b * d - 64. * a * a * a * e;

    float64 A = D * D - 3. * F;
    float64 B = D * F - 9. * E * E;
    float64 C = F * F - 3. * D * E * E;

    float64 DEL = B * B - 4. * A * C;

    if (__Zero_Like(D, RTol) && __Zero_Like(E, RTol) && __Zero_Like(F, RTol)) // D = E = F = 0，一个四重实根
    {
        Roots[0] = -b / (4. * a);
        Roots[1] = Roots[0];
        Roots[2] = Roots[0];
        Roots[3] = Roots[0];

        return 0;
    }

    if (!__Zero_Like(D * E * F, RTol) && __Zero_Like(A, RTol) && __Zero_Like(B, RTol) && __Zero_Like(C, RTol)) // D * E * F != 0，A = B = C = 0，一个三重实根和一个独立实根
    {
        Roots[0] = (-(b * D) + (9. * E)) / (4. * a * D);
        Roots[1] = (-(b * D) - (3. * E)) / (4. * a * D);
        Roots[2] = Roots[1];
        Roots[3] = Roots[1];

        return 1;
    }

    if (__Zero_Like(E, RTol) && __Zero_Like(F, RTol) && !__Zero_Like(D, RTol)) // E = F = 0，D != 0，两个二重根
    {
        Roots[0] = (-b + sqrtc(D)[0]) / (4. * a);
        Roots[1] = Roots[0];
        Roots[2] = (-b - sqrtc(D)[0]) / (4. * a);
        Roots[3] = Roots[2];

        return 2;
    }

    if (!__Zero_Like(A * B * C, RTol) && __Zero_Like(DEL, RTol)) // A * B * C != 0，Δ = 0，一个二重实根，若A * B > 0，则其余两根为两个不等实根；若A * B < 0，则其余两根为一对共轭虚根。
    {
        Roots[0] = (-b + ((2. * A * E) / B) + sqrtc((2. * B) / A)[0]) / (4. * a);
        Roots[1] = (-b + ((2. * A * E) / B) - sqrtc((2. * B) / A)[0]) / (4. * a);
        Roots[2] = (-b - ((2. * A * E) / B)) / (4. * a);
        Roots[3] = Roots[2];

        return 3;
    }

    if (DEL > 0) // Δ > 0，两个不等实根和一对共轭虚根
    {
        float64 cbrtz1 = cbrt((A * D) + (3. * ((-B + sqrt(DEL)) / 2.)));
        float64 cbrtz2 = cbrt((A * D) + (3. * ((-B - sqrt(DEL)) / 2.)));
        float64 sqrtz = sqrt((D * D) - (D * (cbrtz1 + cbrtz2)) + ((cbrtz1 + cbrtz2) * (cbrtz1 + cbrtz2)) - (3. * A));

        Roots[0] = ((-b + (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) + (sqrt(((+2. * D) - (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a));
        Roots[1] = ((-b + (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) - (sqrt(((+2. * D) - (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a));
        Roots[2] = ((-b - (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) + (sqrt(((-2. * D) + (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a)) * 1i;
        Roots[3] = ((-b - (sgn(E) * sqrt((D + cbrtz1 + cbrtz2) / 3.))) / (4. * a)) - (sqrt(((-2. * D) + (cbrtz1 + cbrtz2) + (2. * sqrtz)) / 3.) / (4. * a)) * 1i;

        return 4;
    }

    if (DEL < 0) // Δ < 0，若D与F均为正数，则为四个不等实根；否则为两对不等共轭虚根。
    {
        if (__Zero_Like(E, RTol))
        {
            if (F > 0)
            {
                Roots[0] = (-b + sqrtc(D + (2. * sqrt(F)))[0]) / (4. * a);
                Roots[1] = (-b - sqrtc(D + (2. * sqrt(F)))[0]) / (4. * a);
                Roots[2] = (-b + sqrtc(D - (2. * sqrt(F)))[0]) / (4. * a);
                Roots[3] = (-b - sqrtc(D - (2. * sqrt(F)))[0]) / (4. * a);

                return 5;
            }

            if (F < 0)
            {
                Roots[0] = (((-2. * b) + sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) + (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[1] = (((-2. * b) + sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) - (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[2] = (((-2. * b) - sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) + (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;
                Roots[3] = (((-2. * b) - sqrt((2. * D) + (2. * sqrt(A - F)))) / (8. * a)) - (sqrt((-2. * D) + (2. * sqrt(A - F))) / (8. * a)) * 1i;

                return 6;
            }
        }

        else
        {
            float64 tet = arccos(((3. * B) - (2. * A * D)) / (2. * A * sqrt(A))).ToDegrees();
            float64 y1 = (D - (2. * sqrt(A) * cos(Angle::FromDegrees(tet / 3.)))) / 3.;
            float64 y2 = (D + (sqrt(A) * (cos(Angle::FromDegrees(tet / 3.)) + sqrt(3) * sin(Angle::FromDegrees(tet / 3.))))) / 3.;
            float64 y3 = (D + (sqrt(A) * (cos(Angle::FromDegrees(tet / 3.)) - sqrt(3) * sin(Angle::FromDegrees(tet / 3.))))) / 3.;

            float64 sqrty1 = sqrt(abs(y1));
            float64 sqrty2 = sqrt(y2); // 此时y2恒>0
            float64 sqrty3 = sqrt(abs(y3));

            if (D > 0 && F > 0)
            {
                Roots[0] = (-b + (sgn(E) * sqrty1) + (sqrty2 + sqrty3)) / (4. * a);
                Roots[1] = (-b + (sgn(E) * sqrty1) - (sqrty2 + sqrty3)) / (4. * a);
                Roots[2] = (-b - (sgn(E) * sqrty1) + (sqrty2 - sqrty3)) / (4. * a);
                Roots[3] = (-b - (sgn(E) * sqrty1) - (sqrty2 - sqrty3)) / (4. * a);

                return 7;
            }

            else
            {
                Roots[0] = ((-b - sqrty2) / (4. * a)) + (((sgn(E) * sqrty1) + sqrty3) / (4. * a)) * 1i;
                Roots[1] = ((-b - sqrty2) / (4. * a)) - (((sgn(E) * sqrty1) + sqrty3) / (4. * a)) * 1i;
                Roots[2] = ((-b + sqrty2) / (4. * a)) + (((sgn(E) * sqrty1) - sqrty3) / (4. * a)) * 1i;
                Roots[3] = ((-b + sqrty2) / (4. * a)) - (((sgn(E) * sqrty1) - sqrty3) / (4. * a)) * 1i;

                return 8;
            }
        }
    }

    return -1;
}

std::vector<complex64> DurandKernerSolvePoly::GetExponentialInitValue(float64 Power, complex64 IValue)
{
    std::vector<complex64> InitValue;
    for (size_t i = 0; i < Power; i++)
    {
        InitValue.push_back(powc(Power, i));
    }
    return InitValue;
}

std::vector<complex64> DurandKernerSolvePoly::GetCircularInitValue(InputArray Coeffs)
{
    int N = Coeffs.size() - 1;
    float64 R = pow(abs(Coeffs.back()), 1.0 / N);
    std::vector<complex64> x0(N);
    for (int j = 0; j < N; ++j)
    {
        double tet = 2.0 * CSE_PI * j / N;
        x0[j] = std::polar(R, tet);
    }
    return x0;
}

std::vector<complex64> DurandKernerSolvePoly::GetHomotopicInitValue(InputArray Coeffs, float64 a)
{
    // 使用同伦加速
    // 来源：江源.应用同伦方法加速DURAND-KERNER算法[D].中国科学技术大学,2011.

    int N = Coeffs.size() - 1;
    float64 R = pow(abs(Coeffs.back()), 1.0 / N);

    std::vector<float64> Q(N + 1, 0.0);
    Q[0] = 1;
    Q[N] = -abs(Coeffs.back());

    std::vector<float64> dQ(N, 0.0);
    dQ[0] = N;
    std::vector<float64> P(Coeffs);
    std::vector<float64> dP = SciCxx::Polynomial(P).Derivative().GetCoefficients();

    std::vector<complex64> x0(N);
    for (int j = 0; j < N; ++j)
    {
        double tet = 2.0 * CSE_PI * j / N;
        x0[j] = std::polar(R, tet);
    }

    int64 M = ceil(a * N);
    float64 h = 1.0 / M;
    float64 t = 0.0;

    auto f = [](InputArray Coeffs, complex64 x)->complex64
    {
        complex64 SIG = 0;
        uint64 N = Coeffs.size() - 1;
        for (size_t i = 0; i < Coeffs.size(); i++)
        {
            SIG += Coeffs[i] * powc(x, N - i);
        }
        return SIG;
    };

    std::vector<complex64> x1(N);
    for (int k = 0; k < M; ++k)
    {
        for (int j = 0; j < N; ++j)
        {
            complex64 Q_val = f(Q, x0[j]);
            complex64 P_val = f(P, x0[j]);
            complex64 dQ_val = f(dQ, x0[j]);
            complex64 dP_val = f(dP, x0[j]);

            complex64 denominator = (1.0 - t) * dQ_val + t * dP_val;
            x1[j] = x0[j] + (Q_val - P_val) / denominator * h;
        }
        t += h;
        x0 = x1;
    }

    return x0;
}

int DurandKernerSolvePoly::Run(InputArray _Coeffs, OutputArray _Roots)const
{
    if (_Coeffs.empty()) { return 0;}
    uint64 Power = _Coeffs.size() - 1;
    __Verify_Input_Output(_Coeffs, _Roots, Power);
    std::vector<float64> Coeffs(_Coeffs);
    std::vector<complex64> Roots(Power);

    float64 Base = Coeffs[0];
    for (size_t i = 0; i < Coeffs.size(); i++)
    {
        Coeffs[i] /= Base;
    }

    if (!InitValue.empty())
    {
        if (InitValue.size() != Power)
        {
            throw std::logic_error("Initial value is too few.");
        }
        std::copy(InitValue.begin(), InitValue.end(), Roots.begin());
    }
    else {Roots = GetCircularInitValue(Coeffs);}

    auto f = [&](complex64 x)->complex64
    {
        complex64 SIG = 0;
        for (size_t i = 0; i < Coeffs.size(); i++)
        {
            SIG += Coeffs[i] * powc(x, Power - i);
        }
        return SIG;
    };

    auto g = [&](complex64 x, uint64 Ignore)->complex64
    {
        complex64 PI = 1;
        for (size_t i = 0; i < Roots.size(); i++)
        {
            if (i != Ignore) { PI *= x - Roots[i]; }
        }
        return PI;
    };

    uint64 it = 0;
    float64 RealAbsError = pow(10, -AbsoluteTolerence);
    float64 RealRelError = pow(10, -RelativeTolerence);
    float64 LastError;
    uint64 MaxIterR = pow(10, MaxIter);

    while (it < MaxIterR)
    {
        std::vector<float64> Diffs;
        std::vector<complex64> LastSolution(Roots);
        for (size_t i = 0; i < Power; i++)
        {
            complex64 Diff = f(Roots[i]) / g(Roots[i], i);
            Diffs.push_back(abs(Diff));
            Roots[i] -= Diff;
        }

        float64 MaxDiff = 0;
        for (uint64 i = 0; i < Power; ++i)
        {
            float64 LastSolution0 = abs(LastSolution[i]);
            float64 AbsoluteError = abs(Diffs[i]);
            float64 Error = (LastSolution0 > RealAbsError) ? (AbsoluteError / LastSolution0) : AbsoluteError;
            MaxDiff = max(MaxDiff, Error);
        }
        if (MaxDiff < RealRelError) {break;}
        //if (it && MaxDiff > LastError) {break;}

        LastError = MaxDiff;
        ++it;
    }

    std::copy(Roots.begin(), Roots.end(), _Roots.begin());
    return it;
}

uint64 SolvePoly(InputArray Coeffs, OutputArray Roots, const SolvePolyRoutine& Routine)
{
    return Routine.Run(Coeffs, Roots);
}

_CSE_END
