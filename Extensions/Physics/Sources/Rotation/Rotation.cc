#include "CSE/Physics/Rotation.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN
_ROT_BEGIN

SimpleRotationalElems& SimpleRotationalElems::operator=(const Object::SimpleRotationModel& P) noexcept
{
    this->RotationEpoch = P.RotationEpoch;
    this->Obliquity = Angle::FromDegrees(P.Obliquity);
    this->EqAscendNode = Angle::FromDegrees(P.EqAscendNode);
    this->RotationOffset = Angle::FromDegrees(P.RotationOffset);
    this->RotationPeriod = P.RotationPeriod;
    this->Precession = P.Precession;
    return *this;
}

SimpleRotationalElems::operator Object::SimpleRotationModel()
{
    return
    {
        .RotationEpoch = this->RotationEpoch,
        .Obliquity = this->Obliquity.ToDegrees(),
        .EqAscendNode = this->EqAscendNode.ToDegrees(),
        .RotationOffset = this->RotationOffset.ToDegrees(),
        .RotationPeriod = this->RotationPeriod,
        .Precession = this->Precession
    };
}

WGCCREComplexRotationalElems& WGCCREComplexRotationalElems::operator=(const Object::IAURotationModel& P) noexcept
{
    this->Epoch = P.Epoch;
    this->PoleRA = Angle::FromDegrees(P.PoleRA);
    this->PoleRARate = Angle::FromDegrees(P.PoleRARate);
    this->PoleDec = Angle::FromDegrees(P.PoleDec);
    this->PoleDecRate = Angle::FromDegrees(P.PoleDecRate);
    this->PrimeMeridian = Angle::FromDegrees(P.PrimeMeridian);
    this->RotationRate = Angle::FromDegrees(P.RotationRate);
    this->RotationAccel = Angle::FromDegrees(P.RotationAccel);
    this->PeriodicTermsUnit = P.UsingSecular ? Secular : Diurnal;
    this->PeriodicTerms.clear();
    for (auto i : P.PeriodicTerms)
    {
        this->PeriodicTerms.push_back(
        {
            .PoleRAAmpScale = i[0],
            .PoleDecAmpScale = i[1],
            .PrimeMerAmpScale = i[2],
            .Phase = Angle::FromDegrees(i[3]),
            .Frequency = Angle::FromDegrees(i[5]),
            .FrequencyRate = Angle::FromDegrees(i[6])
        });
    }
    return *this;
}

WGCCREComplexRotationalElems::operator Object::IAURotationModel()
{
    Object::IAURotationModel Model
    {
        .Epoch = this->Epoch,
        .PoleRA = this->PoleRA.ToDegrees(),
        .PoleRARate = this->PoleRARate.ToDegrees(),
        .PoleDec = this->PoleDec.ToDegrees(),
        .PoleDecRate = this->PoleDecRate.ToDegrees(),
        .PrimeMeridian = this->PrimeMeridian.ToDegrees(),
        .RotationRate = this->RotationRate.ToDegrees(),
        .RotationAccel = this->RotationAccel.ToDegrees(),
        .UsingSecular = this->PeriodicTermsUnit == Secular
    };
    for (auto i : this->PeriodicTerms)
    {
        Model.PeriodicTerms.push_back(
        {
            i.PoleRAAmpScale,
            i.PoleDecAmpScale,
            i.PrimeMerAmpScale,
            i.Phase.ToDegrees(),
            i.Frequency.ToDegrees(),
            i.FrequencyRate.ToDegrees()
        });
    }
    return Model;
}

float64 SynodicRotationPeriod(float64 RotationPeriod, float64 OrbitalPeriod)
{
    return (RotationPeriod * OrbitalPeriod) / (OrbitalPeriod - RotationPeriod);
}

float64 StellarRotationPeriod(float64 RotationPeriod, float64 Precession)
{
    return (Precession * RotationPeriod) / (Precession - RotationPeriod);
}

float64 TropicalOrbitalPeriod(float64 SiderealOrbitalPeriod, float64 Precession)
{
    return (SiderealOrbitalPeriod * Precession) / (SiderealOrbitalPeriod + Precession);
}

_ROT_END
_CSE_END
