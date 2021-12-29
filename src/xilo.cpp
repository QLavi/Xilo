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

const int WIDTH = 768;
const int HEIGHT = 768;
constexpr float ASPECT = WIDTH / float(HEIGHT);
const int MAX_SAMPLES = 32;
const int TILE_SIZE = 64;
const int THREAD_COUNT = 8;

#include "objects.cpp"
#include "render.cpp"
#include "task_launch.cpp"

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

    Array<Thread_Info> thread_infos = create_thread_infos(image, render_ctx);
    Array<pthread_t> threads = launch_threads(thread_infos);

    wait_for_threads(threads);

    destroy_threads(threads);
    destroy_thread_infos(thread_infos);

    stbi_write_png("render.png", WIDTH, HEIGHT, 4, image, WIDTH * 4);
    spheres.deallocate();
    quads.deallocate();
    MEM_FREE(render_ctx);
    MEM_FREE(image);
    return 0;
}
