#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/AdvMath.h"

using namespace std;

// LHopital's rule for solving limits
// Reference: https://en.wikipedia.org/wiki/L%27H%C3%B4pital%27s_rule

_CSE_BEGIN
_SCICXX_BEGIN

float64 __Calculateur_Limite_LHopital::operator()(float64 x)
{
    return _M_Fx(x) / _M_Gx(x);
}

_SCICXX_END

float64 LHopitalDiv(Function1D Numerateur, Function1D Denominateur, float64 x,
    float64 PErr, _SCICXX __Calculateur_Limite_LHopital _Engine)
{
    float64 RealErr = pow(10, -PErr);
    float64 Fx = Numerateur(x), Gx = Denominateur(x);
    if (abs(Fx) < RealErr && abs(Gx) < RealErr)
    {
        _Engine._M_Fx = [Numerateur](float64 x) {return Numerateur(x);};
        _Engine._M_Gx = [Denominateur](float64 x) {return Denominateur(x);};
        return _Engine(x);
    }
    else if (isinf(Fx) && isinf(Gx))
    {
        _Engine._M_Fx = [Denominateur](float64 x) {return 1. / Denominateur(x);};
        _Engine._M_Gx = [Numerateur](float64 x) {return 1. / Numerateur(x);};
        return _Engine(x);
    }
    else {return Fx / Gx;}
}

float64 LHopitalMul(Function1D Facteur1, Function1D Facteur2, float64 x,
    float64 PErr, _SCICXX __Calculateur_Limite_LHopital _Engine)
{
    float64 RealErr = pow(10, -PErr);
    float64 Fx = Facteur1(x), Gx = Facteur2(x);
    if (abs(Fx) < RealErr && isinf(Gx))
    {
        _Engine._M_Fx = [Facteur1](float64 x) {return Facteur1(x);};
        _Engine._M_Gx = [Facteur2](float64 x) {return 1. / Facteur2(x);};
        return _Engine(x);
    }
    else {return Fx * Gx;}
}

float64 LHopitalSub(Function1D Diminuende, Function1D Diminuteur, float64 x,
    float64 PErr, _SCICXX __Calculateur_Limite_LHopital _Engine)
{
    float64 RealErr = pow(10, -PErr);
    float64 Fx = Diminuende(x), Gx = Diminuteur(x);
    if (isinf(Fx) && isinf(Gx))
    {
        _Engine._M_Fx = [Diminuende, Diminuteur](float64 x) {return (1. / Diminuteur(x)) - (1. / Diminuende(x));};
        _Engine._M_Gx = [Diminuende, Diminuteur](float64 x) {return 1. / (Diminuende(x) * Diminuteur(x));};
        return _Engine(x);
    }
    else {return Fx - Gx;}
}

float64 LHopitalPow(Function1D Base, Function1D Exposant, float64 x,
    float64 PErr, _SCICXX __Calculateur_Limite_LHopital _Engine)
{
    float64 RealErr = pow(10, -PErr);
    float64 Fx = Base(x), Gx = Exposant(x);
    if (abs(Fx) < RealErr && abs(Gx) < RealErr)
    {
        _Engine._M_Fx = [Exposant](float64 x) {return Exposant(x);};
        _Engine._M_Gx = [Base](float64 x) {return 1. / ln(Base(x));};
        return exp(_Engine(x));
    }
    else if (Fx == 1 && isinf(Gx))
    {
        _Engine._M_Fx = [Base](float64 x) {return ln(Base(x));};
        _Engine._M_Gx = [Exposant](float64 x) {return 1. / Exposant(x);};
        return exp(_Engine(x));
    }
    else if (isinf(Fx) && Gx < RealErr)
    {
        _Engine._M_Fx = [Exposant](float64 x) {return Exposant(x);};
        _Engine._M_Gx = [Base](float64 x) {return 1. / ln(Base(x));};
        return exp(_Engine(x));
    }
    else {return pow(Fx, Gx);}
}

_CSE_END
