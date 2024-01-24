/************************************************************
  CSpaceEngine Geometry Algorithms.
***********************************************************/

#include "CSE/CSEBase/Geometry.h"

_CSE_BEGIN

inline float64 DistancePolar(vec2 p0, vec2 p1)
{
    return sqrt(pow(p0.x, 2) + pow(p1.x, 2) - 2. * p0.x * p1.x * _CSE cos(p0.y - p1.y));
}

inline float64 DistancePolar(vec3 p0, vec3 p1)
{
    float64 CosSep = cse::cos(p0.y) * cse::cos(p1.y) * cse::cos(p0.x - p1.x) + cse::sin(p0.y) * cse::sin(p1.y);
    return sqrt(pow(p0.z, 2) + pow(p1.z, 2) - (2. * p0.z * p1.z * CosSep));
}

inline float64 AngularSeparation(vec3 p0, vec3 p1, float64 RABase)
{
    p0.x *= 360. / RABase;
    p1.x *= 360. / RABase;
    return arccos(cse::cos(p0.y) * cse::cos(p1.y) * cse::cos(p0.x - p1.x) + cse::sin(p0.y) * cse::sin(p1.y));
}

inline vec3 cross(vec3 a, vec3 b)
{
    return vec3
    (
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x)
    );
}

inline vec4 cross(vec4 u, vec4 v, vec4 t)
{
    //mat4 U
    //({
    //    {0, -u.w * v.z + u.z * v.w, +u.w * v.y - u.y * v.w, +u.y * v.z - u.z * v.y},
    //    {+u.w * v.z - u.z * v.w, 0, -u.w * v.x + u.x * v.w, -u.x * v.z + u.z * v.x},
    //    {-u.w * v.y + u.y * v.w, +u.w * v.x - u.x * v.w, 0, +u.x * v.y - u.y * v.x},
    //    {-u.y * v.z + u.z * v.y, +u.x * v.z - u.z * v.x, -u.x * v.y + u.y * v.x, 0}
    //});
    //matrix<float64, 1, 4> T({t.x, t.y, t.z, t.w});
    //return vec4((U * T)[0]);

    // Expand
    return vec4
    (
        - (t.w * u.y * v.z) + (t.w * u.z * v.y) + (t.y * u.w * v.z) - (t.y * u.z * v.w) - (t.z * u.w * v.y) + (t.z * u.y * v.w),
        + (t.w * u.x * v.z) - (t.w * u.z * v.x) - (t.x * u.w * v.z) + (t.x * u.z * v.w) + (t.z * u.w * v.x) - (t.z * u.x * v.w),
        - (t.w * u.x * v.y) + (t.w * u.y * v.x) + (t.x * u.w * v.y) - (t.x * u.y * v.w) - (t.y * u.w * v.x) + (t.y * u.x * v.w),
        + (t.x * u.y * v.z) - (t.x * u.z * v.y) - (t.y * u.x * v.z) + (t.y * u.z * v.x) + (t.z * u.x * v.y) - (t.z * u.y * v.x)
    );
}

_CSE_END
