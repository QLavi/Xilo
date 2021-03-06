#include <stdint.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "pt_math.h"
#include "pt_random.h"
#define CHECK_BOUNDS
#include "pt_utils.cpp"

struct Camera { Vec3 look_from, cu, cv, cw; Vec2 fov; };
struct Material { Vec3 color; float emissive_strength, metallic, specular, roughness; };
struct Hit_Info { float t; Vec3 p, normal; Material* material; };
struct Sphere { float rad; Vec3 pos; Material* material; };
struct Quad { Vec2 dim; Vec3 pos; Material* material; };
struct Triangle { Vec3 v0, v1, v2; Material* material; };
struct AABB { Vec3 min, max; };
struct BVH_Node { Triangle* triangle; AABB bbox; BVH_Node *left, *right; };
struct Render_Context { Array<Quad> quads; Array<BVH_Node*> bvhs; Camera camera; };

const int WIDTH = 1280;
const int HEIGHT = 768;
constexpr float ASPECT = WIDTH / float(HEIGHT);
const int MAX_SAMPLES = 1024;
const int MAX_LIGHT_BOUNCES = 64;
const int TILE_SIZE = 64;
const int THREAD_COUNT = 8;

#include "objects.cpp"
#include "bvh.cpp"
#include "render.cpp"
#include "task_launch.cpp"
#include "mesh_loader.cpp"

int main()
{
    uint32_t* image = MEM_ALLOC(uint32_t, WIDTH * HEIGHT);
    Render_Context* render_ctx = MEM_ALLOC(Render_Context, 1);

    float Fov = 70.0F;
    float v_Fov = tan((Fov * PI / 180) / 2.0);
    float h_Fov = v_Fov * ASPECT;

    Vec3 look_from = Vec3{20, 15, 17};
    Vec3 look_at   = Vec3{0, 0, 0};
    Vec3 up        = Vec3{0, 1, 0};

    Vec3 cw = normalize(look_from - look_at);
    Vec3 cu = normalize(cross(up, cw));
    Vec3 cv = cross(cw, cu);
    render_ctx->camera = Camera{look_from, cu, cv, cw, Vec2{h_Fov, v_Fov}};

    Array<Material> materials; materials.allocate(4);
    materials[0] = Material{Vec3{0.800, 0.093, 0.148}, 0, 1, 0, 0}; // Ground Plane
    materials[1] = Material{Vec3{0.050, 0.169, 0.646}, 0, 0, 0, 1}; // Dragon
    materials[2] = Material{Vec3{0.216, 1.000, 0.463}, 0, 0, 0, 1}; // Wabbit
    materials[3] = Material{Vec3{1, 1, 1}, 7, 0, 0, 0};

    Array<Quad> quads; quads.allocate(2);
    quads[0] = Quad {Vec2{500, 500}, Vec3{0.0, 0.0, 0.0}, &materials[0]};
    quads[1] = Quad {Vec2{15, 15}, Vec3{0.0, 15.0, 0.0}, &materials[3]};
    render_ctx->quads = quads;

    Array<Triangle> bunny = load_mesh("data/bunny.obj", 34835, 69666, &materials[2]);
    Array<Triangle> dragon = load_mesh("data/dragon.obj", 50000, 100000, &materials[1]);

    Array<BVH_Node*> bvhs; bvhs.allocate(2);
    for(int x = 0; x < bvhs.size; x++) { bvhs[x] = create_bvh_node(); }
    build_BVH(bvhs[0], bunny.data, 0, bunny.size);
    build_BVH(bvhs[1], dragon.data, 0, dragon.size);
    render_ctx->bvhs = bvhs;

#if 1
    Array<Thread_Info> thread_infos = create_thread_infos(image, render_ctx);
    Array<pthread_t> threads = launch_threads(thread_infos);

    wait_for_threads(threads);

    stbi_write_png("render.png", WIDTH, HEIGHT, 4, image, WIDTH * 4);

    destroy_threads(threads);
    destroy_thread_infos(thread_infos);
#endif

    destroy_BVHs(bvhs);
    bunny.deallocate();
    dragon.deallocate();
    materials.deallocate();
    quads.deallocate();
    MEM_FREE(render_ctx);
    MEM_FREE(image);
    return 0;
}
