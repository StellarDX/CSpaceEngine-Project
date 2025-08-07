#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ORBIT_BEGIN

///////////////////////////////////// BASE ////////////////////////////////////

float64 __Enhanced_Keplerian_Equation_Solver::BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)const
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

Angle __Enhanced_Keplerian_Equation_Solver::operator()(Angle MeanAnomaly)const
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

float64 __Newton_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)const
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

float64 __Markley_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)const
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
        float64 dj1 = 1. / (1. - e1 * cj1);
        float64 dj2 = dj1 * dj1;
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

uint64 __Piecewise_Quintic_Keplerian_Equation::FindInterval(float64 MRad)const
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
    (float64 MRad, float64 AbsTol, float64 RelTol)const
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

float64 __Piecewise_Quintic_Keplerian_Equation::Run(float64 MRad, float64 AbsTol, float64 RelTol)const
{
    uint64 i = FindInterval(MRad);
    float64 delM = Coefficients.at(1, i) * (MRad - Breakpoints[i].ToRadians());
    return Coefficients.at(0, i) + delM *
        (1. + delM *
        (Coefficients.at(2, i) + delM *
        (Coefficients.at(3, i) + delM *
        (Coefficients.at(4, i) + delM * Coefficients.at(5, i)))));
}

/////////////////////////////////// HKE-SDG ///////////////////////////////////
/*
    Copyright (C) 2018 by the UNIVERSIDAD POLITECNICA DE MADRID (UPM)
    AUthors: Virginia Raposo-Pulido and Jesus Pelaez

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "HKE-SDG.tbl"

__SDGH_Equacion_de_Keplerh::__SDGH_Equacion_de_Keplerh(float64 e)
{
    if (e <= 1)
    {
        throw std::logic_error("HKE-SDG is only adapt for hyperbolic orbits.");
    }
    Eccentricity = e;
    GetSegments(e, this->SegmentTable);
}

void __SDGH_Equacion_de_Keplerh::GetSegments(float64 Eccentricity, float64 *SegTable)
{
    for (uint64 i = 0; i < SegmentTableSize; ++i)
    {
        uint64 Index = (i & 1) ? (i / 2U + SegmentTableBound) : (i / 2U);
        SegTable[Index] =
            SegmentCoeffsTable[Index] * Eccentricity - 0.1 * float64(i);
    }
}

float64 __SDGH_Equacion_de_Keplerh::SingularCornerInitEstimator(float64 MRad)const
{
    float64 eps = Eccentricity - 1.;
    float64 chi = MRad / sqrt(eps * eps * eps);
    float64 Lam = sqrt(8.0L + 9.0L * chi * chi);
    float64 T   = cbrt(Lam + 3.0L * chi);
    float64 T2  = T * T;
    float64 sig = 6.0L * chi / (2.0L + T2 + 4.0L / T2);

    static auto nuh_as = [](float64 sigmao, float64 epsilon)
    {
        float64 t1 = sigmao * sigmao;
        float64 t2 = t1 * sigmao;
        float64 t5 = t1 + 2.0;
        float64 t10 = t1 * t1;
        float64 t12 = t10 * t1;
        float64 t17 = t5 * t5;
        float64 t18 = t17 * t5;
        float64 t20 = epsilon * epsilon;
        float64 t25 = t10 * t10;
        float64 t26 = t25 * t1;
        float64 t34 = t17 * t17;
        float64 t42 = t25 * t12;
        float64 t44 = t25 * t10;
        float64 t55 = t20 * t20;
        float64 t60 = t25 * t25;
        float64 t73 = t34 * t34;

        // Converted to integer math.
        float64 e1 = t2 * (t1 + 20LL) / (60LL * t5) * epsilon;
        float64 e2 = t10 * sigmao * (t12 + 25LL * t10 + 340LL * t1 + 840LL) / (1400LL * t18) * t20;
        float64 e3 = t10 * t2 * (5LL * t26 + 166LL * t25 + 2505LL * t12 + 28240LL * t10 + 124100LL * t1 + 180000LL) / (126000LL * t34) / t5 * t20 * epsilon;
        float64 e4 = t25 * sigmao * (387LL * t42 + 16172LL * t44 + 306228LL * t26 + 3619848LL * t25 + 35945312LL * t12 + 205356480LL * t10 + 568176000LL * t1 + 603680000LL) / (155232000LL * t34) / t18 * t55;
        float64 e5 = t25 * t2 * (35203LL * t60 * t1 + 1768558LL * t60 + 40930516LL * t42 + 582574720LL * t44 + 5917801960LL * t26 + 53311645600LL * t25 + 348404653600LL * t12 + 1361270272000LL * t10 + 2820160000000LL * t1 + 2400448512000LL) / (209563200000LL * t73) / t5 * t55 * epsilon;

        return sigmao - e1 + e2 - e3 + e4 - e5;
    };

    return sqrt(eps) * nuh_as(sig, eps);
}

float64 __SDGH_Equacion_de_Keplerh::SingularCornerInitEstimatorDOS(float64 MRad)const
{
    float64 S, u;
    for (int i = 0; i < 10; i++)
    {
        if (i == 0) {u = (MRad / Eccentricity);}
        else {u = (MRad + ln(S + sqrt(S * S + 1.0L))) / Eccentricity;}
        S = u;
    }
    return S;
}

float64 __SDGH_Equacion_de_Keplerh::SingularCornerInitEstimatorTRES(float64 MRad)const
{
    double S, u, z;
    double le, l2, lm, tt, phi, xi;

    le = ln(Eccentricity);
    l2 = ln(2.0L);
    lm = ln(MRad);
    tt = sqrt(Eccentricity * Eccentricity + MRad * MRad);
    phi = (tt * (ln(tt + MRad) - le)) / ((tt - 1.0) * MRad);
    xi = (-1.0L / (2.0L * pow(MRad, 3.0L))) * (pow(le - l2, 2.0L) + lm * (lm + 2.0L * (l2 - le)));
    z = (MRad / Eccentricity)*(1.0 + phi + xi);

    //printf("\n M= %020.18lf   e= %020.18lf", (double)M, (double)e);
    for (int i = 0; i < 10; i++)
    {
        if (i == 0) {u = z;}
        else {u = (MRad + ln(S + sqrt(S * S + 1.0L))) / Eccentricity;}
        S = u;
        //printf("\n S= %020.18lf", (double)S);
    }
    //printf("\n S= %020.18lf\n\n", (double)S);
    return S;
}

float64 __SDGH_Equacion_de_Keplerh::NewtonInitValue(Angle MeanAnomaly)const
{
    float64 e = Eccentricity;
    float64 M = MeanAnomaly.ToRadians();

    if (abs(e - 1.0) <= 0.5L && abs(M) < 0.25L)
    {
        return sinh(SingularCornerInitEstimator(M));
    }

    uint64 i = 0;
    while(M - SegmentTable[i] >= 0.0L && i < SegmentTableBound) {i++;}
    uint64 j = i - 1;
    uint64 k;

    if (j == SegmentTableBound - 1) {k = SegmentTableBound - 1;}
    else if (j == 0 && M < SegmentTable[SegmentTableBound]) {k = 0;}
    else if (j == SegmentTableBound - 2 &&
        M > SegmentTable[SegmentTableSize - 1])
    {
        k = SegmentTableBound - 2;
    }
    else if (M < SegmentTable[j + SegmentTableBound])
    {
        if ((M - SegmentTable[j]) <
            (SegmentTable[j + SegmentTableBound] - M)) {k = j;}
        else {k = j + SegmentTableBound - 1;}
    }
    else if (M > SegmentTable[j + SegmentTableBound])
    {
        if ((M - SegmentTable[j + SegmentTableBound]) <
            (SegmentTable[j + 1] - M))
        {
            k = j + SegmentTableBound;
        }
        else {k = j;}
    }

    return k >= PolynomTableSize ? 0.0 : TablaPolinomios[k](e, M);
}

vec4 __SDGH_Equacion_de_Keplerh::VectorizedHKE(float64 Eccentricity, float64 MRad, float64 Init)
{
    float64 ad = 1.0L + Init * Init;
    float64 a = sqrt(ad);
    float64 at = ad*a;
    float64 ac = at*ad;

    return
    {
        Init - ln(Init + a) / Eccentricity - MRad / Eccentricity,
        1.0L - 1.0L / a / Eccentricity,
        Init / at / Eccentricity,
        (3.0L - 2.0L * ad) / ac / Eccentricity
    };
}

Angle __SDGH_Equacion_de_Keplerh::Run(Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual) const
{
    float64 RealAbsTol = pow(10, -AbsoluteTolerence);
    float64 RealRelTol = pow(10, -RelativeTolerence);

    float64 M = MeanAnomaly.ToRadians();
    float64 e = Eccentricity;
    float64 eps = RealRelTol;

    float64 u = NewtonInitValue(MeanAnomaly);
    vec4 c = VectorizedHKE(e, M, u);

    if(abs(c[0]) < eps)
    {
        if (NumberOfIters) {*NumberOfIters = 0;}
        if (Residual) {*Residual = c[0];}
        return u;
    }

    uint64 MaxIter = floor(pow(10, MaxIterations));
    uint64 i;
    for(i = 1; i < MaxIter; ++i)
    {
        float64 delta = 2.0L * c[0] / (c[1] + (c[1] > 0.0L ? 1 : -1) *
            sqrt(abs(c[1] * c[1] - 2.0 * c[0] * c[2])));
        delta += -c[3] * delta * delta * delta /
            (3.0L * c[3] * delta * delta - 6.0L * c[2] * delta + 6.0L * c[1]);
        u -= delta;
        c = VectorizedHKE(e, M, u);
        if (abs(u) <= 1.0L)
        {
            if ((abs(c[0]) < eps) || (abs(delta) < RealAbsTol)) {break;}
        }
        else
        {
            if ((abs(c[0]) < eps) || (abs(delta) < abs(u * RealAbsTol))) {break;}
        }
    }

    if (NumberOfIters) {*NumberOfIters = i;}
    if (Residual) {*Residual = c[0];}
    return Angle::FromRadians(u);
}

Angle __SDGH_Equacion_de_Keplerh::operator()(Angle MeanAnomaly)const
{
    return operator()(MeanAnomaly, nullptr, nullptr);
}

Angle __SDGH_Equacion_de_Keplerh::operator()
    (Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual)const
{
    Angle MeanAnomaly0 = abs(MeanAnomaly);
    float64 ERad = Run(MeanAnomaly0, NumberOfIters, Residual).ToRadians();
    return Angle::FromRadians((MeanAnomaly < 0 ? -1 : 1) *
        ln(ERad + sqrt(1.0 + ERad * ERad)));
}

_ORBIT_END
_CSE_END
