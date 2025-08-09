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

void TruncateTo360(Angle& Ang)
{
    float64 ADeg = Ang.ToDegrees();
    if (abs(ADeg) > 360) {ADeg = mod(ADeg, 360);}
    if (ADeg < 0) {ADeg += 360;}
    Ang = Angle::FromDegrees(ADeg);
}

// ---------------------------------------- 开普勒方程 ---------------------------------------- //

_KE_BEGIN

Angle __Elliptical_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity >= 0)
    {
        throw std::logic_error("This function is only adapt for elliptical orbit.");
    }
    float64 EARadians = EccentricAnomaly.ToRadians();
    return Angle::FromRadians
        (EARadians - Eccentricity * sin(EccentricAnomaly)).ToDegrees();
}

Angle __Parabolic_Keplerian_Equation(Angle EccentricAnomaly)
{
    float64 EARadians = EccentricAnomaly.ToRadians();
    return Angle::FromRadians
        (EARadians / 2. + EARadians * EARadians * EARadians / 6.);
}

Angle __Hyperbolic_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity <= 0)
    {
        throw std::logic_error("This function is only adapt for hyperbolic orbit.");
    }
    float64 EARadians = EccentricAnomaly.ToRadians();
    return Angle::FromRadians
        (Eccentricity * sinh(EARadians) - EARadians).ToDegrees();
}

_KE_END

// ---------------------------------------------------------------------------------------------

Angle KeplerianEquation(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity == 0) {return EccentricAnomaly;}
    else if (Eccentricity < 1)
    {
        return _KE __Elliptical_Keplerian_Equation(EccentricAnomaly, Eccentricity);
    }
    else if (Eccentricity == 1)
    {
        return _KE __Parabolic_Keplerian_Equation(EccentricAnomaly);
    }
    else
    {
        return _KE __Hyperbolic_Keplerian_Equation(EccentricAnomaly, Eccentricity);
    }
}

Angle InverseKeplerianEquation(float64 Eccentricity, Angle MeanAnomaly)
{
    if (Eccentricity == 0) {return MeanAnomaly;}
    else if (Eccentricity < 1)
    {
        _KE DefaultEllipticalIKE IKE(Eccentricity);
        return IKE(MeanAnomaly);
    }
    else if (Eccentricity == 1)
    {
        _KE DefaultParabolicIKE IKE;
        return IKE(MeanAnomaly);
    }
    else
    {
        _KE DefaultHyperbolicIKE IKE(Eccentricity);
        return IKE(MeanAnomaly);
    }
}

Angle GetTrueAnomalyFromEccentricAnomaly(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity == 0) {return EccentricAnomaly;}
    else if (Eccentricity < 1)
    {
        float64 EDeg = EccentricAnomaly.ToDegrees();
        float64 bet = Eccentricity /
            (1. + sqrt(1. - Eccentricity * Eccentricity));
        return Angle::FromDegrees(EDeg +
            2. * arctan((bet * sin(EccentricAnomaly)) /
            (1. - bet * cos(EccentricAnomaly))));
    }
    else if (Eccentricity == 1)
    {
        float64 ERad = EccentricAnomaly.ToRadians();
        return Angle::FromDegrees(2. * arctan(ERad).ToDegrees());
    }
    else
    {
        float64 ERad = EccentricAnomaly.ToRadians();
        float64 TanPhi = tanh(ERad / 2.) *
            sqrt((Eccentricity + 1) / (Eccentricity - 1));
        return Angle::FromDegrees(2. * arctan(TanPhi).ToDegrees());
    }
}

Angle GetEccentricAnomalyFromTrueAnomaly(float64 Eccentricity, Angle TrueAnomaly)
{
    if (Eccentricity == 0) {return TrueAnomaly;}
    else if (Eccentricity < 1)
    {
        float64 y = sqrt(1. - Eccentricity * Eccentricity) * sin(TrueAnomaly);
        float64 x = Eccentricity + cos(TrueAnomaly);
        float64 r = Arctan2(y, x).ToDegrees();
        return Angle::FromDegrees(r + ((r < 0) ? 360 : 0));
    }
    else if (Eccentricity == 1)
    {
        float64 TDeg = TrueAnomaly.ToDegrees() / 2.;
        return Angle::FromRadians(tan(Angle::FromDegrees(TDeg)));
    }
    else
    {
        float64 y = sqrt(Eccentricity * Eccentricity - 1) * sin(TrueAnomaly);
        float64 x = Eccentricity + cos(TrueAnomaly);
        return Angle::FromRadians(artanh(y / x));
    }
}

OrbitState KeplerianElementsToStateVectors(OrbitElems Elems)
{

}

_ORBIT_END
_CSE_END
