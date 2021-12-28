#pragma once
#include <math.h>
#define PT_MATH_FN static inline

#define PI 3.14159265359F

struct Vec2 {
    Vec2() : x{0}, y{0} {}
    Vec2(float m): x{m}, y{m} {}
    Vec2(float x, float y): x{x}, y{y} {}
    union {
        float d[2];
        struct { float x, y; };
        struct { float u, v; };
        struct { float min, max; };
    };
};

struct Vec3 {
    Vec3() : x{0}, y{0}, z{0} {}
    Vec3(float m): x{m}, y{m}, z{m} {}
    Vec3(float x, float y, float z): x{x}, y{y}, z{z} {}
    union {
        float d[3];
        struct { float x, y, z; };
        struct { float r, g, b; };
    };
};
struct Ray { Vec3 ro, rd, inv_rd; };

PT_MATH_FN Vec3 operator + (Vec3 u, Vec3 v) { return Vec3{u.x + v.x, u.y + v.y, u.z + v.z}; }
PT_MATH_FN Vec3 operator + (float t, Vec3 u) { return Vec3{u.x + t, u.y + t, u.z + t}; }
PT_MATH_FN Vec3 operator + (Vec3 u, float t) { return t + u; }
PT_MATH_FN Vec3 operator - (Vec3 u, Vec3 v) { return Vec3{u.x - v.x, u.y - v.y, u.z - v.z}; }
PT_MATH_FN Vec3 operator - (Vec3 u) { return Vec3{-u.x, -u.y, -u.z}; }
PT_MATH_FN Vec3 operator * (Vec3 u, Vec3 v) { return Vec3{u.x * v.x, u.y * v.y, u.z * v.z}; }
PT_MATH_FN Vec3 operator * (Vec3 u, float t) { return Vec3{u.x * t, u.y * t, u.z * t}; }
PT_MATH_FN Vec3 operator * (float t, Vec3 u) { return u * t; }
PT_MATH_FN Vec3 operator / (float t, Vec3 u) { return Vec3{t / u.x, t / u.y, t / u.z}; }

PT_MATH_FN Vec2 operator + (Vec2 u, Vec2 v) { return Vec2{u.x + v.x, u.y + v.y}; }
PT_MATH_FN Vec2 operator + (float t, Vec2 u) { return Vec2{u.x + t, u.y + t}; }
PT_MATH_FN Vec2 operator + (Vec2 u, float t) { return t + u; }
PT_MATH_FN Vec2 operator - (Vec2 u, Vec2 v) { return Vec2{u.x - v.x, u.y - v.y}; }
PT_MATH_FN Vec2 operator - (Vec2 u) { return Vec2{-u.x, -u.y}; }
PT_MATH_FN Vec2 operator * (Vec2 u, Vec2 v) { return Vec2{u.x * v.x, u.y * v.y}; }
PT_MATH_FN Vec2 operator * (Vec2 u, float t) { return Vec2{u.x * t, u.y * t}; }
PT_MATH_FN Vec2 operator * (float t, Vec2 u) { return u * t; }
PT_MATH_FN Vec2 operator / (float t, Vec2 u) { return Vec2{t / u.x, t / u.y}; }

PT_MATH_FN float dot(Vec3 u, Vec3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }
PT_MATH_FN float len(Vec3 u) { return sqrt(dot(u, u)); }
PT_MATH_FN float len2(Vec3 u) { return dot(u, u); }
PT_MATH_FN Vec3 normalize(Vec3 u) { return u * (1.0 / len(u)); }
PT_MATH_FN Vec3 rgb(int r, int g, int b) { return Vec3{r/255.0F, g/255.0F, b/255.0F}; }

PT_MATH_FN Vec3 cross(Vec3 u, Vec3 v) {
    return Vec3{
    u.y * v.z - u.z * v.y,
    u.z * v.x - u.x * v.z,
    u.x * v.y - u.y * v.x};
}

PT_MATH_FN float dot(Vec2 u, Vec2 v) { return (u.x * v.x) + (u.y * v.y); }
PT_MATH_FN float len(Vec2 u) { return sqrt(dot(u, u)); }
PT_MATH_FN float len2(Vec2 u) { return dot(u, u); }
PT_MATH_FN Vec2 normalize(Vec2 u) { return u * (1.0 / len(u)); }

PT_MATH_FN float clamp(float x, float min, float max) { return x < max ? x > min ? x : min : max; }

static Vec3 reflect(Vec3 d, Vec3 n)
{
    Vec3 reflected_dir = d - 2.0 * dot(d, n) * n;
    return reflected_dir;
}

static Vec3 refract(Vec3 d, Vec3 n, float ratio, bool& refraction_possible)
{
    float n_dot_d = dot(d, n);
    float discr = 1.0 - (ratio * ratio) * (1.0 - (n_dot_d * n_dot_d));
    refraction_possible = (discr < 0) ? false : true;
    if(refraction_possible)
    {
        Vec3 refracted_dir = ratio * (d - (n_dot_d * n)) - sqrt(discr) * n;
        return refracted_dir;
    }
    return Vec3{0};
}

static float schlick_approx(float cosine, float ratio)
{
    float r0 = (1.0 - ratio) / (1.0 + ratio);
    r0 *= r0;
    float res = r0 + (1 - r0) * pow(1 - cosine, 5);
    return res;
}

