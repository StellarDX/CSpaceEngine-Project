#include "CSE/Sexagesimal.h"
#include "CSE/Base/Algorithms.h"

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN
_EXTERN_C

_Sexagesimal80 ConstructSexagesimal(int32_t h, uint16_t m, float64 s)
{
    return _Sexagesimal80{.Seconds = abs(s), .Minutes = uint16_t(m),
        .Degrees = uint16_t(std::abs(h)), .Negative = std::signbit(h)};
}

void SexagesimalNormalize(_Sexagesimal80& _Sexagesimal)
{
    if (_Sexagesimal.Seconds >= 60)
    {
        float64 AddMinutes = int(_Sexagesimal.Seconds) / 60u;
        float64 SecondMod = mod(_Sexagesimal.Seconds, 60);
        _Sexagesimal.Seconds = SecondMod;
        _Sexagesimal.Minutes += AddMinutes;
    }

    if (_Sexagesimal.Minutes >= 60)
    {
        int AddDegrees = _Sexagesimal.Minutes / 60;
        _Sexagesimal.Minutes %= 60;
        _Sexagesimal.Degrees += AddDegrees;
    }

    if (_Sexagesimal.Degrees >= 360 && (_Sexagesimal.Minutes > 0 || _Sexagesimal.Seconds > 0))
    {
        _Sexagesimal.Degrees = 719 - _Sexagesimal.Degrees;
        _Sexagesimal.Minutes = 59 - _Sexagesimal.Minutes;
        _Sexagesimal.Seconds = 60 - _Sexagesimal.Seconds;
        _Sexagesimal.Negative ^= 1;
    }
}

float64 SexagesimalToDecimal(_Sexagesimal80 _Sexagesimal, bool TimezoneFormat)
{
    return (_Sexagesimal.Degrees + _Sexagesimal.Minutes / 60. + _Sexagesimal.Seconds / 3600.) *
           (TimezoneFormat ? 15. : 1.);
}

_Sexagesimal80 DecimalToSexagesimal(float64 Decimal, bool TimezoneFormat)
{
    float64 MaxVal = TimezoneFormat ? 24 : 360;
    if (abs(Decimal) > MaxVal) {throw std::logic_error("Input number is too big or too small.");}
    float64 h, m, s;
    m = modf(abs(Decimal), &h) * 60;
    s = modf(m, &m) * 60;
    return _Sexagesimal80{.Seconds = s, .Minutes = uint16_t(m),
        .Degrees = uint16_t(h), .Negative = std::signbit(Decimal)};
}

ustring SexagesimalToString(_Sexagesimal80 _Sexagesimal, char const* Format)
{
    auto Sign = _Sexagesimal.Negative ? "-" : "+";
    uint16_t h = _Sexagesimal.Degrees;
    uint16_t m = _Sexagesimal.Minutes;
    float64  s = _Sexagesimal.Seconds;
    return vformat(Format, make_format_args(Sign, h, m, s));
}

_END_EXTERN_C
_CSE_END
