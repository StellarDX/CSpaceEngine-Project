// Trigonometric functions from OpenCV

#include "CSE/Base/CSEBase.h"
#include "CSE/Base/MathFuncs.h"

// Sine function is based on taylor series and existing formulae
// the taylor series if sine is: SUM[0, inf]((pow(-1, n) / (2 * n + 1)!) * pow(x, 2 * n + 1))
// and cosine is: SUM[0, inf]((pow(-1, n) / (2 * n)!) * pow(x, 2 * n))

// This table lists the sine value of (360 / 64) * n (n from 0 to 63)

_CSE_BEGIN

const float64 __CV_SinCos_Tab[]
(
    +0.00000000000000000000000000000000L,
    +0.09801714032956060199419556388864L,
    +0.19509032201612826784828486847702L,
    +0.29028467725446236763619237581740L,
    +0.38268343236508977172845998403040L,
    +0.47139673682599764855638762590525L,
    +0.55557023301960222474283081394853L,
    +0.63439328416364549821517161322549L,
    +0.70710678118654752440084436210485L,
    +0.77301045336273696081090660975847L,
    +0.83146961230254523707878837761791L,
    +0.88192126434835502971275686366039L,
    +0.92387953251128675612818318939679L,
    +0.95694033573220886493579788698027L,
    +0.98078528040323044912618223613424L,
    +0.99518472667219688624483695310948L,
    +1.00000000000000000000000000000000L,
    +0.99518472667219688624483695310948L,
    +0.98078528040323044912618223613424L,
    +0.95694033573220886493579788698027L,
    +0.92387953251128675612818318939679L,
    +0.88192126434835502971275686366039L,
    +0.83146961230254523707878837761791L,
    +0.77301045336273696081090660975847L,
    +0.70710678118654752440084436210485L,
    +0.63439328416364549821517161322549L,
    +0.55557023301960222474283081394853L,
    +0.47139673682599764855638762590525L,
    +0.38268343236508977172845998403040L,
    +0.29028467725446236763619237581740L,
    +0.19509032201612826784828486847702L,
    +0.09801714032956060199419556388864L,
    -0.00000000000000000000000000000000L,
    -0.09801714032956060199419556388864L,
    -0.19509032201612826784828486847702L,
    -0.29028467725446236763619237581740L,
    -0.38268343236508977172845998403040L,
    -0.47139673682599764855638762590525L,
    -0.55557023301960222474283081394853L,
    -0.63439328416364549821517161322549L,
    -0.70710678118654752440084436210485L,
    -0.77301045336273696081090660975847L,
    -0.83146961230254523707878837761791L,
    -0.88192126434835502971275686366039L,
    -0.92387953251128675612818318939679L,
    -0.95694033573220886493579788698027L,
    -0.98078528040323044912618223613424L,
    -0.99518472667219688624483695310948L,
    -1.00000000000000000000000000000000L,
    -0.99518472667219688624483695310948L,
    -0.98078528040323044912618223613424L,
    -0.95694033573220886493579788698027L,
    -0.92387953251128675612818318939679L,
    -0.88192126434835502971275686366039L,
    -0.83146961230254523707878837761791L,
    -0.77301045336273696081090660975847L,
    -0.70710678118654752440084436210485L,
    -0.63439328416364549821517161322549L,
    -0.55557023301960222474283081394853L,
    -0.47139673682599764855638762590525L,
    -0.38268343236508977172845998403040L,
    -0.29028467725446236763619237581740L,
    -0.19509032201612826784828486847702L,
    -0.09801714032956060199419556388864L
);

float64 __cdecl __CV_SIN_DEGREES(float64 _X)
{
    int Sign = 1;
    if (_X < 0)
    {
        Sign = -1;
        _X = -_X;
    }
    if (_X > 360)
    {
        float64 fDec = _X - (uint64)_X;
        _X = (uint64)_X % 360 + fDec;
    }

    static const size_t TABSIZE = 64;
    static const float64 k1 = TABSIZE / 360.; // Base on angle
    static const float64 k2 = 6.283185307179586476925286766559L / TABSIZE;

    // OpenCV only use the first 2 terms for calculation in 32-bif format
    // For 64-bit, maybe this number need to increase to more than 5
    static const float64 As0 = +2.7557319223985890652557319223986e-6L * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 As1 = -1.9841269841269841269841269841270e-4L * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 As2 = +0.00833333333333333333333333333333L * k2 * k2 * k2 * k2 * k2;
    static const float64 As3 = -0.16666666666666666666666666666667L * k2 * k2 * k2;
    static const float64 As4 = +1.00000000000000000000000000000000L * k2;

    static const float64 Ac0 = +2.4801587301587301587301587301587e-5L * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 Ac1 = -0.00138888888888888888888888888889L * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 Ac2 = +0.04166666666666666666666666666667L * k2 * k2 * k2 * k2;
    static const float64 Ac3 = -0.50000000000000000000000000000000L * k2 * k2;
    static const float64 Ac4 = +1.00000000000000000000000000000000L;

    // Separate number
    float64 t = _X * k1;
    int64 it = std::llround(t);
    t -= it;

    // Locate parameters
    int64 IndexS = it & (TABSIZE - 1);
    int64 IndexC = (TABSIZE / 4 - IndexS) & (TABSIZE - 1);

    // Finding solution with table value and taylor series
    float64 sina = __CV_SinCos_Tab[IndexS];
    float64 sinb = ((((As0 * t * t + As1) * t * t + As2) * t * t + As3) * t * t + As4) * t;
    float64 cosa = __CV_SinCos_Tab[IndexC];
    float64 cosb = (((Ac0 * t * t + Ac1) * t * t + Ac2) * t * t + Ac3) * t * t + Ac4;

    // Merge data using Sum formula
    return Sign * (sina * cosb + cosa * sinb);
}

// cos(x) = sin(90 - x)

float64 __cdecl __CV_COS_DEGREES(float64 _X)
{
    if (_X < 0) { _X = -_X; }
    if (_X > 360)
    {
        float64 fDec = _X - (uint64)_X;
        _X = (uint64)_X % 360 + fDec;
    }

    // Almost same as the sine function, but different return.
    size_t TABSIZE = 64;
    float64 k1 = TABSIZE / 360.;
    static const float64 k2 = 6.283185307179586476925286766559L / TABSIZE;

    static const float64 As0 = +2.7557319223985890652557319223986e-6L * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 As1 = -1.9841269841269841269841269841270e-4L * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 As2 = +0.00833333333333333333333333333333L * k2 * k2 * k2 * k2 * k2;
    static const float64 As3 = -0.16666666666666666666666666666667L * k2 * k2 * k2;
    static const float64 As4 = +1.00000000000000000000000000000000L * k2;

    static const float64 Ac0 = +2.4801587301587301587301587301587e-5L * k2 * k2 * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 Ac1 = -0.00138888888888888888888888888889L * k2 * k2 * k2 * k2 * k2 * k2;
    static const float64 Ac2 = +0.04166666666666666666666666666667L * k2 * k2 * k2 * k2;
    static const float64 Ac3 = -0.50000000000000000000000000000000L * k2 * k2;
    static const float64 Ac4 = +1.00000000000000000000000000000000L;

    float64 t = _X * k1;
    int64 it = std::llround(t);
    t -= it;

    int64 IndexS = it & (TABSIZE - 1);
    int64 IndexC = (TABSIZE / 4 - IndexS) & (TABSIZE - 1);

    float64 sina = __CV_SinCos_Tab[IndexS];
    float64 sinb = ((((As0 * t * t + As1) * t * t + As2) * t * t + As3) * t * t + As4) * t;
    float64 cosa = __CV_SinCos_Tab[IndexC];
    float64 cosb = (((Ac0 * t * t + Ac1) * t * t + Ac2) * t * t + Ac3) * t * t + Ac4;

    return cosa * cosb - sina * sinb;
}

float64 __cdecl __CV_TAN_DEGREES(float64 _X)
{
    return __CV_SIN_DEGREES(_X) / __CV_COS_DEGREES(_X);
}

_CSE_END
