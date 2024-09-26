#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/AdvMath.h"

_CSE_BEGIN

vec2 __cdecl XYToPolar(vec2 XY)
{
    return vec2(sqrt(XY.x * XY.x + XY.y * XY.y), arctan(XY.y / XY.x));
}

vec3 __cdecl XYZToPolar(vec3 XYZ)
{
    float64 Correction = 0;
    if (XYZ.x <= 0 && XYZ.z < 0) { Correction = 0; }
    else if (XYZ.x <= 0 && XYZ.z >= 0) { Correction = 180; }
    else if (XYZ.x > 0 && XYZ.z >= 0) { Correction = -180; }
    else { Correction = 0; }
    return vec3
    (
        arctan(XYZ.x / XYZ.z) + Correction,
        arcsin(XYZ.y / sqrt(XYZ.x * XYZ.x + XYZ.y * XYZ.y + XYZ.z * XYZ.z)),
        sqrt(XYZ.x * XYZ.x + XYZ.y * XYZ.y + XYZ.z * XYZ.z)
    );
}

vec2 __cdecl PolarToXY(vec2 Polar)
{
    return vec2(Polar.x * cos(Polar.y), Polar.x * sin(Polar.y));
}

vec3 __cdecl PolarToXYZ(vec3 Polar)
{
    return vec3
    (
        -Polar.z * cos(Polar.y) * sin(Polar.x),
        +Polar.z * sin(Polar.y),
        -Polar.z * cos(Polar.y) * cos(Polar.x)
    );
}

_CSE_END
