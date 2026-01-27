#include "CSE/Base/ConstLists.h"
#include "CSE/Physics/Rotation.h"

_CSE_BEGIN
_ROT_BEGIN

const Sexagesimal IAU_WGCCRERotationTracker::DefaultNorthEclipticPoleRA
    = __Convert_24_to_360(0, 18, 0, 0);
const Sexagesimal IAU_WGCCRERotationTracker::DefaultNorthEclipticPoleDec
    = Sexagesimal(0, 66, 33, 38.55);

IAU_WGCCRERotationTracker::BaseType IAU_WGCCRERotationTracker::CheckParams(const BaseType& InitElems)
{
    BaseType ReturnElems = InitElems;
    if (IS_NO_DATA_DBL(InitElems.Epoch)) {ReturnElems.Epoch = J2000;}
    if (IS_NO_DATA_DBL(InitElems.PoleRA.ToDegrees())) {ReturnElems.PoleRA = Angle::FromDegrees(0);}
    if (IS_NO_DATA_DBL(InitElems.PoleRARate.ToDegrees())) {ReturnElems.PoleRARate = Angle::FromDegrees(0);}
    if (IS_NO_DATA_DBL(InitElems.PoleDec.ToDegrees())) {ReturnElems.PoleDec = Angle::FromDegrees(90);}
    if (IS_NO_DATA_DBL(InitElems.PoleDecRate.ToDegrees())) {ReturnElems.PoleDecRate = Angle::FromDegrees(0);}
    if (IS_NO_DATA_DBL(InitElems.PrimeMeridian.ToDegrees())) {ReturnElems.PrimeMeridian = Angle::FromDegrees(0);}
    if (IS_NO_DATA_DBL(InitElems.RotationRate.ToDegrees())) {throw std::logic_error("Rotation rate must be provided.");}
    if (IS_NO_DATA_DBL(InitElems.RotationAccel.ToDegrees())) {ReturnElems.RotationAccel = Angle::FromDegrees(0);}
    if (InitElems.PeriodicTermsUnit > 1) {ReturnElems.PeriodicTermsUnit = WGCCREComplexRotationalElems::Diurnal;}
    return ReturnElems;
}

IAU_WGCCRERotationTracker::IAU_WGCCRERotationTracker(const BaseType &InitElems)
{
    auto IElems = CheckParams(InitElems);
    InitialState = IElems;
    CurrentState = IElems;
}

void IAU_WGCCRERotationTracker::AddMsecs(int64 Ms)
{
    SetDate(CurrentState.Epoch + Ms / (1000.0 * SynodicDay));
}

void IAU_WGCCRERotationTracker::AddSeconds(int64 Sec)
{
    SetDate(CurrentState.Epoch + Sec / SynodicDay);
}

void IAU_WGCCRERotationTracker::AddHours(int64 Hrs)
{
    SetDate(CurrentState.Epoch + Hrs * (3600.0 / SynodicDay));
}

void IAU_WGCCRERotationTracker::AddDays(int64 Days)
{
    SetDate(CurrentState.Epoch + Days);
}

void IAU_WGCCRERotationTracker::AddYears(int64 Years)
{
    CSEDateTime CurrentDateTime = JDToDateTime(CurrentState.Epoch);
    CurrentDateTime.AddYears(Years);
    float64 NewJD;
    GetJDFromDate(&NewJD,
        CurrentDateTime.date().year(), CurrentDateTime.date().month(), CurrentDateTime.date().day(),
        CurrentDateTime.time().hour(), CurrentDateTime.time().minute(),
        CurrentDateTime.time().second() + CurrentDateTime.time().msec() / 1000.0);
    SetDate(CurrentDateTime);
}

void IAU_WGCCRERotationTracker::AddCenturies(int64 Centuries)
{
    AddYears(100 * Centuries);
}

void IAU_WGCCRERotationTracker::ToCurrentDate()
{
    SetDate(GetJDFromSystem());
}

void IAU_WGCCRERotationTracker::SetDate(CSEDateTime DateTime)
{
    float64 JD;
    GetJDFromDate(&JD,
        DateTime.date().year(), DateTime.date().month(), DateTime.date().day(),
        DateTime.time().hour(), DateTime.time().minute(),
        DateTime.time().second() + DateTime.time().msec() / 1000.0);
    SetDate(JD);
}

void IAU_WGCCRERotationTracker::SetDate(float64 JD)
{
    // 统一单位
    float64 LagTime = JD - InitialState.Epoch;
    float64 LagTimeCentury = LagTime / 36525.;
    float64 PoleRADeg = InitialState.PoleRA.ToDegrees();
    float64 PoleRARateDeg = InitialState.PoleRARate.ToDegrees();
    float64 PoleDecDeg = InitialState.PoleDec.ToDegrees();
    float64 PoleDecRateDeg = InitialState.PoleDecRate.ToDegrees();
    float64 PrimeMeriDeg = InitialState.PrimeMeridian.ToDegrees();
    float64 RotRateDeg = InitialState.RotationRate.ToDegrees();
    float64 RotAccelDeg = InitialState.RotationAccel.ToDegrees();

    CurrentState.Epoch = JD;

    float64 FinalPoleRA = PoleRADeg + PoleRARateDeg * LagTimeCentury;
    float64 FinalPoleDec = PoleDecDeg + PoleDecRateDeg * LagTimeCentury;
    float64 FinalPrimeMeri = PrimeMeriDeg + RotRateDeg * LagTime +
        RotAccelDeg * LagTimeCentury * LagTimeCentury;
    float64 FinalRotRate = RotRateDeg + RotAccelDeg * LagTimeCentury;

    if (!InitialState.PeriodicTerms.empty())
    {
        float64 PassTime;
        switch (InitialState.PeriodicTermsUnit)
        {
        case WGCCREComplexRotationalElems::Diurnal:
            PassTime = LagTime;
        case WGCCREComplexRotationalElems::Secular:
            PassTime = LagTimeCentury;
        }
        for (auto i : InitialState.PeriodicTerms)
        {
            float64 PhaseDeg = i.Phase.ToDegrees();
            float64 FreqDeg = i.Frequency.ToDegrees();
            float64 FreqRateDeg = i.FrequencyRate.ToDegrees();

            FinalPoleRA += i.PoleRAAmpScale * sin(Angle::FromDegrees(
                PhaseDeg + FreqDeg * PassTime + FreqRateDeg * PassTime * PassTime));
            FinalPoleDec += i.PoleDecAmpScale * cos(Angle::FromDegrees(
                PhaseDeg + FreqDeg * PassTime + FreqRateDeg * PassTime * PassTime));
            FinalPrimeMeri += i.PrimeMerAmpScale * sin(Angle::FromDegrees(
                PhaseDeg + FreqDeg * PassTime + FreqRateDeg * PassTime * PassTime));
        }
    }

    NormalizeCoord(FinalPoleRA, FinalPoleDec);
    if (FinalPrimeMeri > 360) {FinalPrimeMeri = mod(FinalPrimeMeri, 360);}
    if (FinalPrimeMeri < 0) {FinalPrimeMeri = mod(FinalPrimeMeri, 360) + 360;}

    CurrentState.PoleRA = Angle::FromDegrees(FinalPoleRA);
    CurrentState.PoleDec = Angle::FromDegrees(FinalPoleDec);
    CurrentState.PrimeMeridian = Angle::FromDegrees(FinalPrimeMeri);
    CurrentState.RotationRate = Angle::FromDegrees(FinalRotRate);
}

void IAU_WGCCRERotationTracker::Reset()
{
    CurrentState = InitialState;
}

IAU_WGCCRERotationTracker::BaseType IAU_WGCCRERotationTracker::GetCurrentState() const
{
    return CurrentState;
}

void IAU_WGCCRERotationTracker::NorthPolePos(Sexagesimal* RA, Sexagesimal* Dec) const
{
    *RA = CurrentState.PoleRA;
    *Dec = CurrentState.PoleDec;
}

Angle IAU_WGCCRERotationTracker::RotationPhase() const
{
    return CurrentState.PrimeMeridian;
}

_ROT_END
_CSE_END
