#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ORBIT_BEGIN

///////////////////////////////////// BASE ////////////////////////////////////

float64 __Enhanced_Keplerian_Equation_Solver::BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)
{
    // Using Bisection
    float64 fa = 2.7 * MRad;
    float64 fb = 0.301;
    float64 f = 0.154;
    float64 al = AbsTol / 1E7;
    float64 be = AbsTol / 0.3;

    while (fb - fa > al + f * be)
    {
        if (f - Eccentricity * sin(Angle::FromRadians(f)) - MRad > 0)
        {
            fb = f;
        }
        else {fa = f;}
        f = (fa + fb) / 2.;
    }

    return f;
}

Angle __Enhanced_Keplerian_Equation_Solver::operator()(Angle MeanAnomaly)
{
    if (!Eccentricity) {return MeanAnomaly;}
    float64 RealAbsTol = pow(10, -AbsoluteTolerence);
    float64 RealRelTol = pow(10, -RelativeTolerence);

    Angle M = MeanAnomaly, Mr = M;
    if (M.ToDegrees() > 360)
    {
        Mr = Angle::FromDegrees(mod(M.ToDegrees(), 360));
    }
    float64 Flip = 0;
    if (Mr.ToDegrees() > 180)
    {
        Mr = Angle::FromDegrees(360 - Mr.ToDegrees());
        Flip = 1;
    }
    else {Flip = -1;}

    float64 MRad = Mr.ToRadians();
    float64 Tmp;

    if (Eccentricity > EBoundary && MRad < MBoundary)
    {
        Tmp = BoundaryHandler(MRad, RealAbsTol, RealRelTol);
    }
    else {Tmp = Run(MRad, RealAbsTol, RealRelTol);}

    return Angle::FromRadians(M.ToRadians() + Flip * (MRad - Tmp));
}

//////////////////////////////////// ENRKE ////////////////////////////////////

__Newton_Keplerian_Equation::__Newton_Keplerian_Equation(float64 e)
{
    if (e >= 1)
    {
        throw std::logic_error("ENRKE is only adapt for elliptical orbits.");
    }
    Eccentricity = e;
}

float64 __Newton_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)
{
    float64 Delns = (2. * AbsTol) / (Eccentricity + RelTol);
    float64 Small = 0.999999; // std::nextafter(1., 0.);
    float64 ERes = MRad +
        Small * MRad * (CSE_PI - MRad) /
        (2.* MRad + Eccentricity - CSE_PI + (CSE_PI * CSE_PI / 4.) /
        (Eccentricity + RelTol));
    float64 f2 = Eccentricity * sin(Angle::FromRadians(ERes));
    float64 f3 = Eccentricity * cos(Angle::FromRadians(ERes));
    float64 f1 = 1 - f3;
    float64 f0 = ERes - f2 - MRad;
    float64 del = (-f0 / f1) * ((f1 * f1 * f1) - ((f0 * f1 * f2) / 2.) +
        (f0 * f0 * f3) / 3.) / ((f1 * f1 * f1) - (f0 * f1 * f2) + (f0 * f0 * f3) / 2.);
    while (del * del > f1 * Delns)
    {
        ERes += del;
        del = (MRad - ERes + Eccentricity * sin(Angle::FromRadians(ERes))) /
            (1. - Eccentricity * cos(Angle::FromRadians(ERes)));
    }
    return ERes + del;
}

//////////////////////////////////// ENMAKE ///////////////////////////////////

__Markley_Keplerian_Equation::__Markley_Keplerian_Equation(float64 e)
{
    if (e >= 1)
    {
        throw std::logic_error("ENMAKE is only adapt for elliptical orbits.");
    }
    Eccentricity = e;
}

float64 __Markley_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)
{
    float64 alf = ((3. * CSE_PI * CSE_PI) +
        (1.6 * CSE_PI) * (CSE_PI - MRad) /
        (1. + Eccentricity)) / (CSE_PI * CSE_PI - 6);
    float64 d = 3. * (1. - Eccentricity) + alf * Eccentricity;
    float64 M2 = MRad * MRad;
    float64 q = 2. * alf * d * (1. - Eccentricity) - M2;
    float64 q2 = q * q;
    float64 r = 3. * alf * d * (d - 1. + Eccentricity) * MRad + M2 * MRad;
    float64 w0 = abs(r) + sqrt(q2 * q + r * r);
    float64 w = cbrt(w0 * w0);
    float64 ERes = (MRad + 2. * r * w / (w * w + w * q + q2)) / d;
    float64 f2 = Eccentricity * sin(Angle::FromRadians(ERes));
    float64 f3 = Eccentricity * cos(Angle::FromRadians(ERes));
    float64 f1 = 1 - f3;
    float64 f0 = ERes - f2 - MRad;
    float64 del0 = -f0 / (f1 - (f0 * f2 / 2.) / f1);
    float64 del1 = -f0 / (f1 + (del0 * f2 / 2.) + del0 * del0 * f3 / 6.);
    float64 del2 = del1 * del1;
    ERes += -f0 / (f1 + (del1 * f2 / 2.) + del2 * f3 / 6. - del1 * del2 * f2 / 24.);
    return ERes;
}

//////////////////////////////////// ENP5KE ///////////////////////////////////

__Piecewise_Quintic_Keplerian_Equation::__Piecewise_Quintic_Keplerian_Equation(float64 e)
{
    if (e >= 1)
    {
        throw std::logic_error("ENMAKE is only adapt for elliptical orbits.");
    }
    Eccentricity = e;
    GetCoefficients(Eccentricity, pow(10, -AbsoluteTolerence),
        &BlockBoundaries, &Breakpoints, &Coefficients);
}

void __Piecewise_Quintic_Keplerian_Equation::GetCoefficients(float64 Eccentricity, float64 Tolerence, std::vector<int64> *kvec, std::vector<Angle> *bp, SciCxx::DynamicMatrix<float64> *coeffs)
{
    std::vector<Angle> EGrid;
    GetCoefficients1(Eccentricity, Tolerence, &EGrid);
    kvec->resize(EGrid.size() + 1);
    std::fill(kvec->begin(), kvec->end(), 0);
    bp->resize(EGrid.size() + 1);
    coeffs->resize({6, EGrid.size()});
    GetCoefficients2(Eccentricity, EGrid, kvec, bp, coeffs);
}

void __Piecewise_Quintic_Keplerian_Equation::GetCoefficients1
    (float64 Eccentricity, float64 Tolerence, std::vector<Angle> *Grid)
{
    float64 E2 = 1. - Eccentricity;
    float64 h0 = yroot(Tolerence, 6) * (0.86 + 1.1* E2 + 1.5 * E2 * E2);
    // 以下两项是针对C数组使用的预测大小，但此处使用了动态数组，所以可以不需要了。
    //*n = ::llrint((CSE_PI - ln(E2) / CSE_SQRT2) / h0) + 2;
    //*Grid = std::vector<float64>(*n);

    // Multistep routine begin
    Angle Ei = 0;
    while (Ei.ToDegrees() < 180)
    {
        Grid->push_back(Ei);
        Ei = Angle::FromRadians(Ei.ToRadians() +
            h0 * sqrt(1. - Eccentricity * cos(Ei)));
    }
    Grid->back() = Angle::FromDegrees(180);
}

void __Piecewise_Quintic_Keplerian_Equation::GetCoefficients2
    (float64 Eccentricity, const std::vector<Angle> &Grid, std::vector<int64> *kvec,
    std::vector<Angle> *bp, SciCxx::DynamicMatrix<float64> *coeffs)
{
    uint64 n = Grid.size();
    float64 e1 = Eccentricity;
    float64 e2 = e1 * e1;
    float64 e3 = e2 * e1;
    float64 e4 = e2 * e2;

    for (uint64 jx = 0; jx < n; ++jx)
    {
        Angle Ej = Grid[jx];
        float64 sj = sin(Ej);
        float64 cj1 = cos(Ej);
        float64 cj2 = cj1 * cj1;
        float64 cj3 = cj2 * cj1;
        float64 cj4 = cj2 * cj2;
        float64 dj1 = 1. / (1. - e1 * cj1);
        float64 dj2 = dj1 * dj1;
        float64 dj3 = dj2 * dj1;
        float64 dj4 = dj2 * dj2;
        float64 dej1 = sj* dj1;
        float64 dej2 = dej1 * dej1;
        float64 dej3 = dej2 * dej1;
        float64 dej4 = dej2 * dej2;

        bp->at(jx) = Angle::FromRadians(Ej.ToRadians() - e1 * sj);
        coeffs->at(0, jx) = Ej.ToRadians();
        coeffs->at(1, jx) = dj1;
        coeffs->at(2, jx) = -e1 * dej1 / 2.;
        coeffs->at(3, jx) = -e1 * cj1 * dj1 / 6. + e2 * dej2 / 2.;
        coeffs->at(4, jx) = (e1 * dej1 + 10. * e2 * cj1 * dej1 * dj1 -
            15. * e3 * dej3) / 24.;
        coeffs->at(5, jx) = (e1 * cj1 * dj1 + 10. * e2 * cj2 * dj2 -
            15. * e2 * dej2 - 105. * e3 * cj1 * dj1 * dej2 +
            105. * e4 * dej4) / 120.;

        int64 jy = int64(bp->at(jx).ToDegrees() * n / 180.) + 1U;
        if (jy > 0 && jy < n) {++(kvec->at(jy));}
    }

    bp->back() = Angle::FromDegrees(180);
    int64 kvj = 0;
    kvec->front() = 0;
    kvec->back() = n + 1;
    for (uint64 jx = 1; jx < n; ++jx)
    {
        kvj += kvec->at(jx);
        kvec->at(jx) = kvj;
    }
}

uint64 __Piecewise_Quintic_Keplerian_Equation::FindInterval(float64 MRad)
{
    uint64 ny = Breakpoints.size() - 1;
    uint64 Left = 0;
    uint64 Right = ny;
    uint64 i1 = uint64(Angle::FromRadians(MRad).ToDegrees() * ny / 180.);
    uint64 q = BlockBoundaries[i1] - 1;
    if (q > Left) {Left = q;}
    q = BlockBoundaries[i1 + 1] + 1;
    if (q < Right) {Right = q;}
    if (Breakpoints[Left + 1].ToRadians() > MRad)
    {
        return Left;
    }
    ++Left;
    while (Left < Right - 1)
    {
        uint64 Mid = (Left + Right) / 2ULL;
        if (Breakpoints[Mid].ToRadians() > MRad)
        {
            Right = Mid;
        }
        else
        {
            Left = Mid;
        }
    }
    return Left;
}

float64 __Piecewise_Quintic_Keplerian_Equation::BoundaryHandler
    (float64 MRad, float64 AbsTol, float64 RelTol)
{
    uint64 i = FindInterval(MRad);
    float64 Left = Coefficients.at(0, i);
    float64 Right = Coefficients.at(0, i + 1);
    float64 Mid = (Left + Right) / 2.;
    float64 al = AbsTol / 1E7;
    float64 be = AbsTol / 0.3;
    while (Right - Left > al + Mid * be)
    {
        if (Mid - Eccentricity * sin(Angle::FromRadians(Mid)) - MRad > 0)
        {
            Right = Mid;
        }
        else {Left = Mid;}
        Mid = (Left + Right) / 2.;
    }
    return Mid;
}

float64 __Piecewise_Quintic_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)
{
    uint64 i = FindInterval(MRad);
    float64 delM = Coefficients.at(1, i) * (MRad - Breakpoints[i].ToRadians());
    return Coefficients.at(0, i) + delM *
        (1. + delM *
        (Coefficients.at(2, i) + delM *
        (Coefficients.at(3, i) + delM *
        (Coefficients.at(4, i) + delM * Coefficients.at(5, i)))));
}

_ORBIT_END
_CSE_END
