#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ORBIT_BEGIN

bool KeplerCompute(OrbitElems& InitElems)
{
    // 检查三个关键参数中有几个已提供
    int ProvidedParams = 0;
    if (!IS_NO_DATA_DBL(InitElems.PericenterDist)) {ProvidedParams++;}
    if (!IS_NO_DATA_DBL(InitElems.Period)) {ProvidedParams++;}
    if (!IS_NO_DATA_DBL(InitElems.GravParam)) {ProvidedParams++;}

    // 如果提供的参数不足两个，返回false
    if (ProvidedParams < 2) {return false;}

    // 根据开普勒第三定律计算缺失的参数
    // 开普勒第三定律: T^2 = (4π²a³)/μ
    // 其中 a 是半长轴，μ 是重力参数，T 是周期

    // 计算半长轴 (a = PericenterDist / (1 - Eccentricity))
    float64 a = InitElems.PericenterDist,
        e = IS_NO_DATA_DBL(InitElems.Eccentricity) ? 0 : InitElems.Eccentricity;
    if (!IS_NO_DATA_DBL(a)) { a /= (1.0 - e); }

    // 情况1: 缺失重力参数
    if (IS_NO_DATA_DBL(InitElems.GravParam))
    {
        if (!IS_NO_DATA_DBL(InitElems.Period) && !IS_NO_DATA_DBL(a))
        {
            InitElems.GravParam = (4.0 * CSE_PI * CSE_PI * a * a * a) /
                                  (InitElems.Period * InitElems.Period);
        }
    }

    // 情况2: 缺失周期
    else if (IS_NO_DATA_DBL(InitElems.Period))
    {
        if (!IS_NO_DATA_DBL(InitElems.GravParam) && !IS_NO_DATA_DBL(a))
        {
            InitElems.Period = 2.0 * CSE_PI * sqrt(a * a * a / InitElems.GravParam);
        }
    }

    // 情况3: 缺失近地点距离
    else if (IS_NO_DATA_DBL(InitElems.PericenterDist))
    {
        if (!IS_NO_DATA_DBL(InitElems.GravParam) && !IS_NO_DATA_DBL(InitElems.Period))
        {
            // 先计算半长轴
            a = cbrt(InitElems.GravParam * InitElems.Period * InitElems.Period / (4.0 * CSE_PI * CSE_PI));
            // 然后计算近地点距离
            InitElems.PericenterDist = a - a * e;
        }
    }

    // 检查是否成功计算了缺失的参数
    if (IS_NO_DATA_DBL(InitElems.PericenterDist) ||
        IS_NO_DATA_DBL(InitElems.Period) ||
        IS_NO_DATA_DBL(InitElems.GravParam))
    {
        return false;
    }

    return true;
}

_ORBIT_END
_CSE_END
