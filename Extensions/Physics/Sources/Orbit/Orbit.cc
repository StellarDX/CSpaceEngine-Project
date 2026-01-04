#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ORBIT_BEGIN

KeplerianOrbitElems& KeplerianOrbitElems::operator=
    (const Object::OrbitParams& P)noexcept
{
    this->Epoch           = P.Epoch;
    this->GravParam       = P.GravParam;
    this->PericenterDist  = P.PericenterDist;
    this->Period          = P.Period;
    this->Eccentricity    = P.Eccentricity;
    this->Inclination     = Angle::FromDegrees(P.Inclination);
    this->AscendingNode   = Angle::FromDegrees(P.AscendingNode);
    this->ArgOfPericenter = Angle::FromDegrees(P.ArgOfPericenter);
    this->MeanAnomaly     = Angle::FromDegrees(P.MeanAnomaly);
    return *this;
}

KeplerianOrbitElems::operator Object::OrbitParams()
{
    return
    {
        .Epoch            = Epoch,
        .Period           = Period,
        .PericenterDist   = PericenterDist,
        .GravParam        = GravParam,
        .Eccentricity     = Eccentricity,
        .Inclination      = Inclination.ToDegrees(),
        .AscendingNode    = AscendingNode.ToDegrees(),
        .ArgOfPericenter  = ArgOfPericenter.ToDegrees(),
        .MeanAnomaly      = MeanAnomaly.ToDegrees(),
    };
}

// ---------------------------------------- 开普勒方程 ---------------------------------------- //

_KE_BEGIN

Angle __Elliptical_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity >= 1)
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
    if (Eccentricity <= 1)
    {
        throw std::logic_error("This function is only adapt for hyperbolic orbit.");
    }
    float64 EARadians = EccentricAnomaly.ToRadians();
    return Angle::FromRadians
        (Eccentricity * sinh(EARadians) - EARadians).ToDegrees();
}

_KE_END

// ---------------------------------------------------------------------------------------------

bool KeplerCompute(KeplerianOrbitElems& InitElems)
{
    // 检查三个关键参数中有几个已提供
    int ProvidedParams = 0;
    if (!IS_NO_DATA_DBL(InitElems.PericenterDist)) {ProvidedParams++;}
    if (!IS_NO_DATA_DBL(InitElems.Period)) {ProvidedParams++;}
    if (!IS_NO_DATA_DBL(InitElems.GravParam)) {ProvidedParams++;}

    // 如果提供的参数不足两个，返回false
    if (ProvidedParams < 2) {return false;}

    // 根据开普勒第三定律计算缺失的参数
    // 开普勒第三定律: T^2 = (4π^2a^3)/μ
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

float64 GetSemiMajorAxisFromPericenterDist(float64 Eccentricity, float64 PericenterDist)
{
    return PericenterDist / (1 - Eccentricity);
}

float64 GetPericenterDistFromSemiMajorAxis(float64 Eccentricity, float64 SemiMajorAxis)
{
    return abs(SemiMajorAxis - SemiMajorAxis * Eccentricity);
}

Angle KeplerianEquation(float64 Eccentricity, Angle EccentricAnomaly)
{
    if (Eccentricity == 0) {return EccentricAnomaly;}
    else if (Eccentricity < 1)
    {
        return _KE __Elliptical_Keplerian_Equation(Eccentricity, EccentricAnomaly);
    }
    else if (Eccentricity == 1)
    {
        return _KE __Parabolic_Keplerian_Equation(EccentricAnomaly);
    }
    else
    {
        return _KE __Hyperbolic_Keplerian_Equation(Eccentricity, EccentricAnomaly);
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

float64 GetSemiLatusRectumFromPericenterDist(float64 Eccentricity, float64 PericenterDist)
{
    return PericenterDist * (1. + Eccentricity);
}

Angle GetArgOfLatitude(Angle ArgOfPericen, Angle Anomaly)
{
    return Angle::FromDegrees(ArgOfPericen.ToDegrees() + Anomaly.ToDegrees());
}

Angle PeriodToAngularVelocity(float64 Period)
{
    return Angle::FromDegrees(360. / Period);
}

Angle PericenterDistToAngularVelocity(float64 Eccentricity, float64 PericenterDist, float64 GravParam)
{
    float64 A;
    if (Eccentricity == 1)
    {
        A = GetSemiLatusRectumFromPericenterDist(1, PericenterDist);
    }
    else
    {
        A = GetSemiMajorAxisFromPericenterDist(Eccentricity, PericenterDist);
    }
    A = abs(A);
    float64 A3 = A * A * A;
    return Angle::FromRadians(sqrt(GravParam / A3));
}

float64 RigidRocheLimit(float64 PrimaryRadius, float64 PrimaryDensity, float64 CompanionDensity)
{
    return PrimaryRadius * cbrt(2 * (PrimaryDensity / CompanionDensity));
}

float64 FluidRocheLimit(float64 PrimaryMass, float64 PrimaryRadius, float64 PrimaryFlattening, float64 CompanionMass, float64 CompanionDensity)
{
    float64 PrimaryDensity = PrimaryMass / ((4. / 3) * CSE_PI * PrimaryRadius * PrimaryRadius * PrimaryRadius);
    return 2.423 * PrimaryRadius * cbrt(PrimaryDensity / CompanionDensity) *
        cbrt(((1 + CompanionMass / (3 * PrimaryMass)) + PrimaryFlattening / 3 * (1 + CompanionMass / PrimaryMass)) / (1 - PrimaryFlattening));
}

float64 ApproxHillSphere(float64 PrimaryMass, float64 CompanionMass, float64 Separation)
{
    return Separation * cbrt(CompanionMass / (3 * (PrimaryMass + CompanionMass)));
}

float64 HillSphere(float64 PrimaryMass, float64 CompanionMass, float64 Separation, const SolvePolyRoutine& SPRoutine)
{
    // (G * M_2) / x^2 - (G * M_1) / (R - x)^2 + Ω^2 * (R - x) = 0
    // => (-G * M_1 * x^2 + G * M_2 * R^2 - 2 * G * M_2 * R * x + G * M_2 * x^2 + R^3 * Ω^2 * x^2 - 3 * R^2 * Ω^2 * x^3 + 3 * R * Ω^2 * x^4 - Ω^2 * x^5) / (R^2 * x^2 - 2 * R * x^3 + x^4) = 0
    // => - (Ω^2) * x^5
    //    + (3 * R * Ω^2) * x^4
    //    - (3 * R^2 * Ω^2) * x^3
    //    + ((-G * M_1) + (G * M_2) + (R^3 * Ω^2)) * x^2
    //    - (2 * G * M_2 * R) * x
    //    + (G * M_2 * R^2)

    float64 Separation3 = Separation * Separation * Separation;
    float64 AngularVelocity2 = (GravConstant * (PrimaryMass + CompanionMass)) / Separation3;
    float64 GravParamPrim = GravConstant * PrimaryMass;
    float64 GravParamComp = GravConstant * CompanionMass;
    std::vector<float64> Coeffs
    {
        -AngularVelocity2,
        3 * Separation * AngularVelocity2,
        -3 * Separation * Separation * AngularVelocity2,
        GravParamComp - GravParamPrim + Separation3 * AngularVelocity2,
        -2 * GravParamComp * Separation,
        GravParamComp * Separation * Separation
    };
    std::vector<complex64> Roots;
    SolvePoly(Coeffs, Roots, SPRoutine);
    return std::find_if(Roots.begin(), Roots.end(), [](complex64 x)
    {
        return abs(x.imag()) < 1e-13;
    })->real();
}

void __cdecl MakeOrbit(Object* Primary, Object* Companion, std::optional<float64> Separation, KeplerianOrbitElems Args)
{
    if (!Separation && IS_NO_DATA_DBL(Args.PericenterDist))
    {
        throw std::logic_error("Failed to make orbit: Separation is not provided.");
    }

    try {Companion->ParentBody = Primary->Name.at(0);}
    catch(...) {throw std::logic_error("Failed to make orbit: Primary has no available name.");}

    KeplerianOrbitElems FinalOrbit = Args;
    if (Separation)
    {
        float64 Eccentricity = IS_NO_DATA_DBL(Args.Eccentricity) ? 0 : Args.Eccentricity;
        FinalOrbit.PericenterDist = GetPericenterDistFromSemiMajorAxis(Eccentricity, Separation.value());
    }
    Companion->Orbit = FinalOrbit;
}

std::shared_ptr<Object> MakeBinary(Object* Primary, Object* Companion, std::optional<float64> Separation, KeplerianOrbitElems Args)
{
    if (!Separation && IS_NO_DATA_DBL(Args.PericenterDist))
    {
        throw std::logic_error("Failed to make binary: Separation is not provided.");
    }
    if (IS_NO_DATA_DBL(Primary->Mass) || IS_NO_DATA_DBL(Companion->Mass))
    {
        throw std::logic_error("Failed to make binary: Masses of primary and companion must be provided.");
    }

    Object Barycenter
    {
        .Type = "Barycenter",
        .Name = {"Barycenter"},
        .ParentBody = Primary->ParentBody,
        .Orbit = Primary->Orbit
    };

    Primary->ParentBody = Barycenter.Name.at(0);
    Companion->ParentBody = Barycenter.Name.at(0);

    float64 Separation0, Eccentricity = 0;
    if (Separation) {Separation0 = Separation.value();}
    else
    {
        Eccentricity = IS_NO_DATA_DBL(Args.Eccentricity) ? 0 : Args.Eccentricity;
        Separation = GetSemiMajorAxisFromPericenterDist(Eccentricity, Args.PericenterDist);
    }

    float64 BarycenterPos = RocheLobe
        {Primary->Mass, Companion->Mass, Separation0, mat3(1), mat3(1)}
        .BarycenterPos().x;
    KeplerianOrbitElems PFinalOrbit = Args;
    KeplerianOrbitElems CFinalOrbit = Args;
    PFinalOrbit.PericenterDist =
        GetPericenterDistFromSemiMajorAxis(Eccentricity, BarycenterPos);
    CFinalOrbit.PericenterDist =
        GetPericenterDistFromSemiMajorAxis(Eccentricity, Separation0 - BarycenterPos);
    PFinalOrbit.ArgOfPericenter = Angle::FromDegrees(PFinalOrbit.ArgOfPericenter.ToDegrees() - 180);

    Primary->Orbit = PFinalOrbit;
    Companion->Orbit = CFinalOrbit;

    return std::make_shared<Object>(Barycenter);
}

_ORBIT_END
_CSE_END
