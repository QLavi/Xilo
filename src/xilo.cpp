#include <stdint.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "pt_math.h"
#include "pt_random.h"
#define CHECK_BOUNDS
#include "pt_utils.cpp"

struct Camera { Vec3 look_from, cu, cv, cw; Vec2 fov; };
struct Hit_Info { float t; Vec3 p, normal, color; };
struct Sphere { float rad; Vec3 pos, color; };
struct Quad { Vec2 dim; Vec3 pos, color; };
struct Render_Context { Array<Sphere> spheres; Array<Quad> quads; Camera camera; };

#include "objects.cpp"

uint32_t color_pixel(float x, float y, Render_Context* ctx);
Vec3 trace(Ray& ray, Render_Context* ctx);

const int WIDTH = 768;
const int HEIGHT = 768;
constexpr float ASPECT = WIDTH / float(HEIGHT);
const int MAX_SAMPLES = 1;

int main()
{
    uint32_t* image = MEM_ALLOC(uint32_t, WIDTH * HEIGHT);
    Render_Context* render_ctx = MEM_ALLOC(Render_Context, 1);

    float Fov = 90.0F;
    float v_Fov = tan((Fov * PI / 180) / 2.0);
    float h_Fov = v_Fov * ASPECT;

    Vec3 look_from = Vec3{0, 2, 5};
    Vec3 look_at   = Vec3{0, 0, 0};
    Vec3 up        = Vec3{0, 1, 0};

    Vec3 cw = normalize(look_from - look_at);
    Vec3 cu = normalize(cross(up, cw));
    Vec3 cv = cross(cw, cu);
    render_ctx->camera = Camera{look_from, cu, cv, cw, Vec2{h_Fov, v_Fov}};

    Array<Sphere> spheres; spheres.allocate(1);
    spheres[0] = Sphere {0.5, Vec3{0.0, 0.5, 0.0}, rgb(249, 153, 183)};
    render_ctx->spheres = spheres;

    Array<Quad> quads; quads.allocate(1);
    quads[0] = Quad {Vec2{5, 5}, Vec3{0.0, 0.0, 0.0}, rgb(23, 23, 23)};
    render_ctx->quads = quads;

    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            image[WIDTH*y + x] = color_pixel(x, y, render_ctx);
        }
    }

    stbi_write_png("render.png", WIDTH, HEIGHT, 4, image, WIDTH * 4);
    spheres.deallocate();
    quads.deallocate();
    MEM_FREE(render_ctx);
    MEM_FREE(image);
    return 0;
}

uint32_t color_pixel(float x, float y, Render_Context* ctx)
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
