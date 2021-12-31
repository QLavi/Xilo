bool scene_hit(Ray& ray, Vec2 t_range, Hit_Info& info, Render_Context* render_ctx)
{
    bool hit_anything = false;
    Array<Quad> quads = render_ctx->quads;
    Array<BVH_Node*> bvhs = render_ctx->bvhs;

    for(int x = 0; x < quads.size; x++)
    {
        bool hit = quadxz_hit(ray, &quads[x], t_range, info);
        if(hit) { t_range.max = info.t; hit_anything = true; }
    }

    for(int x = 0; x < bvhs.size; x++)
    {
        bool hit = bvh_node_hit(ray, bvhs[x], t_range, info);
        if(hit) { t_range.max = info.t; hit_anything = true; }
    }

    return hit_anything;
}

Vec3 trace(Ray& ray, int bounces, Render_Context* render_ctx)
{
    Vec3 bg_color = Vec3{0};
    if(bounces > MAX_LIGHT_BOUNCES) { return bg_color; }

    Hit_Info info;
    Vec2 t_range = Vec2{0.0001, 1000};
    bool hit = scene_hit(ray, t_range, info, render_ctx);
    if(!hit) { return bg_color; }

    Vec3 new_rd = info.material->metallic ? 
        reflect(ray.rd, info.normal) : unorm_sample_hemisphere(info.normal);

    Ray new_ray = Ray{info.p, new_rd, 1 / new_rd};
    Vec3 color = info.material->emissive_strength + 
        info.material->color * trace(new_ray, bounces +1, render_ctx);

    return color;
}

uint32_t render_tile(int x, int y, Render_Context* ctx)
{
    Vec3 color = Vec3{0};
    for(int i = 0; i < MAX_SAMPLES; i++)
    {
        float u = (x + randf()) / (WIDTH -1.0F);
        float v = (y + randf()) / (HEIGHT -1.0F);

        u = u*2 -1;
        v = v*2 -1;
        u = u * ctx->camera.fov.u;
        v = v * ctx->camera.fov.v;
        Vec3 ro = ctx->camera.look_from;
        Vec3 rd = ctx->camera.cu * u - ctx->camera.cv * v - ctx->camera.cw;
        rd = normalize(rd);
        Vec3 inv_rd = 1.0F / rd;

        Ray ray = { ro, rd, inv_rd };
        color = color + trace(ray, 0, ctx);
    }

    float scale = 1.0 / MAX_SAMPLES;
    color.r = sqrt(scale * color.r);
    color.g = sqrt(scale * color.g);
    color.b = sqrt(scale * color.b);

    uint32_t r = (uint32_t)(256 * clamp(color.r, 0.0, 0.999));
    uint32_t g = (uint32_t)(256 * clamp(color.g, 0.0, 0.999));
    uint32_t b = (uint32_t)(256 * clamp(color.b, 0.0, 0.999));
    uint32_t pixel_color = 0xFF000000 | (b << 16) | (g << 8) | r;
    return pixel_color;
}
