#include "CSE/Base/AdvMath.h"

_CSE_BEGIN
_SCICXX_BEGIN

//////////////////////////////// 布伦特极小值求解 ////////////////////////////////

BrentUnboundedMinimizer::BracketType BrentUnboundedMinimizer::CreateBracket
    (Function1D Function, float64 InitStart, float64 InitEnd, uint64* FCalls,
    float64 MaxIter, float64 AbsoluteTolerence, float64 GrowLimit)noexcept(0)
{
    uint64 RealMaxIter = floor(pow(10, MaxIter));
    float64 RealAbsTol = pow(10, -AbsoluteTolerence);

    float64 xa = InitStart, xb = InitEnd,
        fa = Function(InitStart), fb = Function(InitEnd);
    if (fa < fb) // 确保 fa > fb，否则交换 xa 和 xb
    {
        std::swap(xa, xb);
        std::swap(fa, fb);
    }

    float64 xc = xb + GoldenRatio2 * (xb - xa), fc = Function(xc); // 初始猜测 xc
    if (FCalls) {(*FCalls) = 3;}

    class BracketError : public std::logic_error
    {
    public:
        BracketType Data;

        BracketError(const std::string& Msg, const BracketType& Data)
            : std::logic_error(Msg), Data(Data) {}
    };

    uint64 Iter = 0;
    while (fc < fb)
    {
        // 计算抛物线插值的试探点 w
        float64 tmp1 = (xb - xa) * (fb - fc);
        float64 tmp2 = (xb - xc) * (fb - fa);
        float64 val = tmp2 - tmp1;

        float64 denom = (abs(val) < RealAbsTol) ? 2.0 * RealAbsTol : 2.0 * val;
        float64 w = xb - ((xb - xc) * tmp2 - (xb - xa) * tmp1) / denom;
        float64 wlim = xb + GrowLimit * (xc - xb);

        if (Iter >= RealMaxIter)
        {
            throw BracketError
            (
                "No valid bracket was found before the iteration limit was "
                "reached. Consider trying different initial points or "
                R"(increasing "maxiter".)",
                {.First = vec2(xa, fa), .Centre = vec2(xb, fb), .Last = vec2(xc, fc)}
            );
        }
        ++Iter;

        float64 fw;
        if ((w - xc) * (xb - w) > 0.0)
        {
            // w 在 xb 和 xc 之间
            fw = Function(w);
            if (FCalls) {++(*FCalls);}
            if (fw < fc)
            {
                // 接受 w 作为新的 xb
                xa = xb;
                xb = w;
                fa = fb;
                fb = fw;
                break;
            }
            else if (fw > fb)
            {
                // 接受 w 作为新的 xc
                xc = w;
                fc = fw;
                break;
            }
            w = xc + GoldenRatio2 * (xc - xb);
            fw = Function(w);
            if (FCalls) {++(*FCalls);}
        }
        else if ((w - wlim) * (wlim - xc) >= 0.0)
        {
            // w 超出 wlim，截断到 wlim
            w = wlim;
            fw = Function(w);
            if (FCalls) {++(*FCalls);}
        }
        else if ((w - wlim) * (xc - w) > 0.0)
        {
            // w 在 xc 和 wlim 之间
            fw = Function(w);
            if (FCalls) {++(*FCalls);}
            if (fw < fc)
            {
                xb = xc;
                xc = w;
                w = xc + GoldenRatio2 * (xc - xb);
                fb = fc;
                fc = fw;
                fw = Function(w);
                if (FCalls) {++(*FCalls);}
            }
        }
        else
        {
            // 默认情况，线性外推
            w = xc + GoldenRatio2 * (xc - xb);
            fw = Function(w);
            if (FCalls) {++(*FCalls);}
        }

        // 更新区间
        xa = xb;
        xb = xc;
        xc = w;
        fa = fb;
        fb = fc;
        fc = fw;
    }

    // 检查是否满足有效区间的条件
    bool cond1 = (fb < fc && fb <= fa) || (fb < fa && fb <= fc);
    bool cond2 = (xa < xb && xb < xc) || (xc < xb && xb < xa);
    bool cond3 = std::isfinite(xa) && std::isfinite(xb) && std::isfinite(xc);

    if (!(cond1 && cond2 && cond3))
    {
        throw BracketError
        (
            "The algorithm terminated without finding a valid bracket. "
            "Consider trying different initial points.",
            {.First = vec2(xa, fa), .Centre = vec2(xb, fb), .Last = vec2(xc, fc)}
        );
    }

    return {.First = vec2(xa, fa), .Centre = vec2(xb, fb), .Last = vec2(xc, fc)};
}

BrentUnboundedMinimizer::BracketType
    BrentUnboundedMinimizer::CreateBracketFromArray(Function1D Func, std::vector<float64> Points, uint64* FuncCallCount)
{
    if (Points.empty())
    {
        return CreateBracket(Func, 0, 1, FuncCallCount);
    }
    else if (Points.size() == 2)
    {
        return CreateBracket(Func, Points[0], Points[1], FuncCallCount);
    }
    else if (Points.size() == 3)
    {
        std::sort(Points.begin(), Points.end());
        return
        {
            .First  = vec2(Points[0], Func(Points[0])),
            .Centre = vec2(Points[1], Func(Points[1])),
            .Last   = vec2(Points[2], Func(Points[2])),
        };
        if (FuncCallCount) {(*FuncCallCount) = 3;}
    }
    throw std::logic_error("Bracketing interval must be "
        "length 2 or 3 sequence.");
}

vec2 BrentUnboundedMinimizer::Run(Function1D Func, std::vector<float64> Points)const
{
     // 此处使用Boost的实现，本质上Boost的实现和SciPy的实现是一样的
    float64 RTolerance = pow(10, -Tolerence);
    float64 MinTol = pow(10, -MinTolerence);
    float64 MaxIterCount = pow(10, MaxIter);
    float64 x;              // 当前最优解（最小值点）
    float64 w;              // 次优解
    float64 v;              // 第三优解（w的前一个值）
    float64 u;              // 最新评估点
    float64 delta;          // 上一步移动距离
    float64 delta2;         // 上上步移动距离
    float64 fu, fv, fw, fx; // u, v, w, x 处的函数值
    float64 mid;            // 搜索区间[a,b]的中点
    float64 fract1, fract2; // x的最小相对移动量

    BracketType Bracket = CreateBracketFromArray(Func, Points);

    x = w = v = Bracket.Centre.x;
    fw = fv = fx = Bracket.Centre.y;
    delta2 = delta = 0;

    float64 a = min(Bracket.First.x, Bracket.Last.x),
        b = max(Bracket.First.x, Bracket.Last.x);
    uint64 Count = MaxIterCount;
    do
    {
        // 计算当前区间中点
        mid = (a + b) / 2;
        // 检查是否满足收敛条件：
        fract1 = RTolerance * abs(x) + MinTol;
        fract2 = 2 * fract1;
        if(abs(x - mid) <= (fract2 - (b - a) / 2)) {break;}

        // SciPy说明：首次迭代时，delta仅在条件成立时绑定，
        // 这在源Python程序中会导致UnboundLocalError错误。
        // 所以它应在if语句前设置（但应设为何值？）
        // 丹霞：C++中可能不会存在上述问题，但我从未对别的语言翻译来的代码有足够的信心。
        if(abs(delta2) > fract1)
        {
            // 尝试构造抛物线拟合：
            float64 r = (x - w) * (fx - fv);
            float64 q = (x - v) * (fx - fw);
            float64 p = (x - v) * q - (x - w) * r;
            q = 2 * (q - r);
            if(q > 0) {p = -p;}
            q = abs(q);
            float64 td = delta2;
            delta2 = delta;
            // 判断抛物线步长是否可接受：
            if((abs(p) >= abs(q * td / 2)) || (p <= q * (a - x)) || (p >= q * (b - x)))
            {
                // 不，试试黄金分割
                delta2 = (x >= mid) ? a - x : b - x;
                delta = GRatioConj * delta2;
            }
            else
            {
                // 可以，抛物线拟合:
                delta = p / q;
                u = x + delta;
                if(((u - a) < fract2) || ((b - u) < fract2))
                    delta = (mid - x) < 0 ? -abs(fract1) : abs(fract1);
            }
        }
        else
        {
            // 黄金分割：
            delta2 = (x >= mid) ? a - x : b - x;
            delta = GRatioConj * delta2;
        }

        // 更新当前点：
        u = (abs(delta) >= fract1) ? (x + delta) : (delta > 0 ? (x + abs(fract1)) : (x - abs(fract1)));
        fu = Func(u);

        if(fu <= fx)
        {
            // 发现更优解!
            // 更新搜索区间:
            if(u >= x) {a = x;}
            else {b = x;}
            // 更新控制点:
            v = w;
            w = x;
            x = u;
            fv = fw;
            fw = fx;
            fx = fu;
        }
        else
        {
            // 哦，天哪，新的点比我们现有的还要糟糕，
            // 即便如此，它“一定”比我们的一个端点要好:
            // (丹霞：Boost库的程序员在编写这段代码时是在抖M吗?)
            if(u < x) {a = u;}
            else {b = u;}
            if((fu <= fw) || (w == x))
            {
                // 然而，它至少是第二优解：
                v = w;
                w = u;
                fv = fw;
                fw = fu;
            }
            else if((fu <= fv) || (v == x) || (v == w))
            {
                // 第三优解：
                v = u;
                fv = fu;
            }
        }
    }
    while(--Count);

    return vec2(x, fx);
}

vec2 BrentUnboundedMinimizer::operator()(Function1D Func)const
{
    return Run(Func);
}

_SCICXX_END
_CSE_END
