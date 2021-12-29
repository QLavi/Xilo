bool sphere_hit(Ray& ray, Sphere* sphere, Vec2 t_range, Hit_Info& info)
{
    Vec3 oc = ray.ro - sphere->pos;
    float a = dot(ray.rd, ray.rd);
    float b = dot(oc, ray.rd);
    float c = dot(oc, oc) - sphere->rad * sphere->rad;

    float discr = b*b - a*c;
    if(discr < 0) { return false; }


    float t = (-b - sqrt(discr)) / a;
    if(t < t_range.min || t > t_range.max)
    {
        t = (-b + sqrt(discr)) / a;
        if(t < t_range.min || t > t_range.max) { return false; }
    }

    Vec3 p = ray.ro + t * ray.rd;
    info.t = t;
    info.p = p;
    info.normal = (p - sphere->pos) * (1.0F / sphere->rad);
    info.material = sphere->material;

    return true;
}

bool quadxz_hit(Ray& ray, Quad* quad, Vec2 t_range, Hit_Info& info)
{
    Vec3 normal = Vec3{0, 1, 0};
    float n_dot_d = dot(normal, ray.rd);
    float t = dot(quad->pos - ray.ro, normal) * (1.0 / n_dot_d);

    if(t < t_range.min || t > t_range.max) { return false; }

    float x_min = quad->pos.x - quad->dim.x / 2.0F;
    float x_max = quad->pos.x + quad->dim.x / 2.0F;

    float z_min = quad->pos.z - quad->dim.y / 2.0F;
    float z_max = quad->pos.z + quad->dim.y / 2.0F;

    Vec3 p = ray.ro + t * ray.rd;
    bool not_within_region =
        p.x < x_min ||
        p.x > x_max ||
        p.z < z_min ||
        p.z > z_max;
    if(not_within_region) { return false; }

    info.t = t;
    info.p = p;
    if(n_dot_d > 0) { normal = -normal; }
    info.normal = normal;
    info.material = quad->material;
    return true;
}

const float EPSILON = 0.0001;
bool triangle_hit(Ray& ray, Triangle* triangle, Vec2 t_range, Hit_Info& info)
{
    Vec3 edge1 = triangle->v1 - triangle->v0;
    Vec3 edge2 = triangle->v2 - triangle->v0;
    Vec3 vec_p = cross(ray.rd, edge2);

    float det = dot(edge1, vec_p);
    if(det < EPSILON) { return false; }

    Vec3 vec_t = ray.ro - triangle->v0;
    float u = dot(vec_t, vec_p);
    if(u < 0 || u > det) { return false; }

    Vec3 vec_q = cross(vec_t, edge1);
    float v = dot(ray.rd, vec_q);
    if(v < 0 || u + v > det) { return false; }

    float t = dot(edge2, vec_q);
    float inv_det = 1 / det;
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;
    if(t < t_range.min || t > t_range.max) { return false; }

    info.t = t;
    info.p = ray.ro + t*ray.rd;
    info.normal = cross(edge1, edge2);
    info.material = triangle->material;
    return true;
}
