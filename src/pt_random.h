#pragma once
#include <stdlib.h>
#include "pt_math.h"

PT_MATH_FN float randf() { return drand48(); }
PT_MATH_FN float randf_range(float min, float max)
{ return min + (max - min) * randf(); }

PT_MATH_FN Vec3 randf_vec3(float min, float max)
{ return Vec3{randf_range(min, max), randf_range(min, max), randf_range(min, max)}; }

PT_MATH_FN Vec3 unorm_sample_hemisphere(Vec3 n)
{
    float u = randf();
    float v = randf();

    float theta = 2.0F * PI * u;
    float phi = acos(2.0F * v - 1.0F);

    float sin_phi = sin(phi);

    Vec3 p = Vec3{
        cos(theta) * sin_phi,
        sin(theta) * sin_phi,
        cos(phi)
    };

    float p_dot_n = dot(p, n);
    if(p_dot_n <= 0) { p = -p; }

    return p;
}

PT_MATH_FN Vec3 sample_unit_sphere()
{
    while(true)
    {
        Vec3 p = randf_vec3(-1, 1);
        if(len2(p) >= 1) { continue; }
        return p;
    }
}

PT_MATH_FN bool near_zero(Vec3 u)
{ float eps = 1e-6; return (fabs(u.x) < eps) && (fabs(u.y) < eps) && (fabs(u.z) < eps); }
