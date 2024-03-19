#include "CSE/Base/ConstLists.h"
#include "CSE/Base/MathFuncs.h"

_CSE_BEGIN

namespace consts
{
    const double Equation21 = AvogadroConst * BoltzmannConst;
    const double Equation22 = (2.0 * (CSE_PI * CSE_PI * CSE_PI * CSE_PI * CSE_PI) *
                                    (BoltzmannConst * BoltzmannConst * BoltzmannConst * BoltzmannConst)) /
                                   (15.0 * (PlanckConst * PlanckConst * PlanckConst) *
                                    (double(SpeedOfLight) * SpeedOfLight));
    const double Equation23 = 1. / (VacPermeability * (double(SpeedOfLight) * SpeedOfLight));
    const double Equation24 = (ElemCharge * ElemCharge) / (2. * VacPermittivity * PlanckConst * SpeedOfLight);

    const double Equation41 = (180.0 * 60.0 * 60.0 * AU) / CSE_PI;

    const double Equation81 = PlanckConst / (2.0 * CSE_PI);
    const double Equation82 = sqrt((DiracConstant * GravConstant) / pow(SpeedOfLight, 3));
    const double Equation83 = sqrt((DiracConstant * SpeedOfLight) / GravConstant);
    const double Equation84 = sqrt((DiracConstant * GravConstant) / pow(SpeedOfLight, 5));
    const double Equation85 = sqrt((DiracConstant * pow(SpeedOfLight, 5)) / (GravConstant * pow(BoltzmannConst, 2)));
    const double Equation86 = (HubbleParam * 1000.0) / (1000000.0 * Parsec);
    const double Equation87 = 3.0 * pow(HubbleParamSI / SpeedOfLight, 2) * CosmoDensityRatio;
    const double Equation88 = SpeedOfLight / HubbleParamSI;
}

_CSE_END
