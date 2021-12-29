Vec3 trace(Ray& ray, Render_Context* render_ctx)
{
    Vec3 color = Vec3{0.5, 0.7, 1.0};
    Array<Sphere> spheres = render_ctx->spheres;
    Array<Quad> quads = render_ctx->quads;

    Hit_Info info;
    Vec2 t_range = Vec2{0.0001, 1000};
    for(int x = 0; x < spheres.size; x++)
    {
        bool hit = sphere_hit(ray, &spheres[x], t_range, info);
        if(hit) { t_range.max = info.t; color = info.color; }
    }

    for(int x = 0; x < quads.size; x++)
    {
        bool hit = quadxz_hit(ray, &quads[x], t_range, info);
        if(hit) { t_range.max = info.t; color = info.color; }
    }

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
        color = color + trace(ray, ctx);
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
