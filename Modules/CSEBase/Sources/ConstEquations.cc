#include "CSE/CSEBase/ConstLists.h"
#include "CSE/CSEBase/MathFuncs.h"

_CSE_BEGIN

namespace consts
{
	using cse::pow;
	using cse::sqrt;

    const long double Equation21 = GasConstant / AvogadroConst;
    const long double Equation22 = (2.0 * pow(CSE_PI, 5) * pow(BoltzmannConst, 4)) / (15.0 * pow(PlanckConst, 3) * pow(SpeedOfLight, 2));
    const long double Equation23 = (2. * FineStructConst * PlanckConst) / (pow(ElemCharge, 2) * SpeedOfLight);
    const long double Equation24 = 1. / (MagnetConstant * pow(SpeedOfLight, 2));

    const long double Equation41 = (180.0 * 60.0 * 60.0 * AU) / CSE_PI;

    const long double Equation81 = PlanckConst / (2.0 * CSE_PI);
    const long double Equation82 = sqrt((DiracConstant * GravConstant) / pow(SpeedOfLight, 3));
    const long double Equation83 = sqrt((DiracConstant * SpeedOfLight) / GravConstant);
    const long double Equation84 = sqrt((DiracConstant * GravConstant) / pow(SpeedOfLight, 5));
    const long double Equation85 = sqrt((DiracConstant * pow(SpeedOfLight, 5)) / (GravConstant * pow(BoltzmannConst, 2)));
    const long double Equation86 = (HubbleParam * 1000.0) / (1000000.0 * Parsec);
    const long double Equation87 = 3.0 * pow(HubbleParamSI / SpeedOfLight, 2) * CosmoDensityRatio;
    const long double Equation88 = SpeedOfLight / HubbleParamSI;
}

_CSE_END
