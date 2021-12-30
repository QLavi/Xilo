#include <algorithm>

bool aabb_hit(Ray& ray, AABB* aabb, Vec2 t_range)
{
    for(int x = 0; x < 3; x++)
    {
        float invD = ray.inv_rd.d[x];
        float t0 = (aabb->min.d[x] - ray.ro.d[x]) * invD;
        float t1 = (aabb->max.d[x] - ray.ro.d[x]) * invD;
        if(invD < 0) { float tmp=t0; t0=t1; t1=tmp; }

        t_range.min = t0 > t_range.min ? t0 : t_range.min;
        t_range.max = t1 < t_range.max ? t1 : t_range.max;

        if(t_range.max <= t_range.min) { return false; }
    }
    return true;
}

bool bvh_node_hit(Ray& ray, BVH_Node* node, Vec2 t_range, Hit_Info& info)
{
    if(!aabb_hit(ray, &node->bbox, t_range)) { return false; }

    if(node->triangle != nullptr)
    {
        bool hit = triangle_hit(ray, node->triangle, t_range, info);
        return hit;
    }

    bool hit_left = bvh_node_hit(ray, node->left, t_range, info);
    float max_t = hit_left ? info.t : t_range.max;
    bool hit_right = bvh_node_hit(ray, node->right, Vec2{t_range.min, max_t}, info);
    return hit_left || hit_right;
}

AABB triangle_bbox(Triangle* triangle)
{
    AABB bbox;
    bbox.min = Vec3{
        fmin(fmin(triangle->v0.x, triangle->v1.x), triangle->v2.x),
        fmin(fmin(triangle->v0.y, triangle->v1.y), triangle->v2.y),
        fmin(fmin(triangle->v0.z, triangle->v1.z), triangle->v2.z),
    };
    bbox.max = Vec3{
        fmax(fmax(triangle->v0.x, triangle->v1.x), triangle->v2.x),
        fmax(fmax(triangle->v0.y, triangle->v1.y), triangle->v2.y),
        fmax(fmax(triangle->v0.z, triangle->v1.z), triangle->v2.z),
    };
    return bbox;
}

AABB sphere_bbox(Sphere* sphere)
{
    AABB bbox;
    bbox.min = sphere->pos - Vec3{sphere->rad};
    bbox.max = sphere->pos + Vec3{sphere->rad};
    return bbox;
}

AABB enclose_bboxes(AABB& a, AABB& b)
{
    AABB bbox;
    bbox.min = Vec3{fmin(a.min.x, b.min.x), fmin(a.min.y, b.min.y), fmin(a.min.z, b.min.z)};
    bbox.max = Vec3{fmax(a.max.x, b.max.x), fmax(a.max.y, b.max.y), fmax(a.max.z, b.max.z)};
    return bbox;
}

void enclose_primitives_inside_node(BVH_Node* node, Triangle* triangles, int start, int end)
{
    AABB bbox;
    bool first_bbox = true;

    for(int x = start; x < end; x++)
    {
        AABB object_box = triangle_bbox(&triangles[x]);
        bbox = first_bbox ? object_box : enclose_bboxes(bbox, object_box);
        first_bbox = false;
    }
    node->bbox = bbox;
}

BVH_Node* create_bvh_node()
{
    BVH_Node* node = MEM_ALLOC(BVH_Node, 1);
    node->triangle = nullptr;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

bool compare_bboxes(Triangle& x, Triangle& y, int axis)
{
    AABB a = triangle_bbox(&x);
    AABB b = triangle_bbox(&y);

    return a.min.d[axis] < b.min.d[axis];
}

void build_BVH(BVH_Node* node, Triangle* triangles, int start, int end)
{
    enclose_primitives_inside_node(node, triangles, start, end);
    Vec3 net_dist = node->bbox.max - node->bbox.min;
    int axis = 
        fmax(net_dist.x, net_dist.y) == net_dist.x ?
        fmax(net_dist.x, net_dist.z) == net_dist.x ? 0 : 2 : 1;

    auto comparing_fn = [axis](Triangle& x, Triangle& y) { return compare_bboxes(x, y, axis); };

    node->left = create_bvh_node();
    node->right = create_bvh_node();
    int span = end - start;

    if(span == 1)
    {
        node->left->triangle = node->right->triangle = &triangles[start];
    }
    else if(span == 2)
    {
        if(comparing_fn(triangles[start], triangles[start +1]))
        {
            node->left->triangle = &triangles[start];
            node->right->triangle = &triangles[start +1];
        }
        else
        {
            node->left->triangle = &triangles[start +1];
            node->right->triangle = &triangles[start];
        }
    }
    else
    {
        std::sort(triangles + start, triangles + end, comparing_fn);
        int mid = start + span / 2;

        build_BVH(node->left, triangles, start, mid);
        build_BVH(node->right, triangles, mid, end);
    }

    if(node->left->triangle != nullptr || node->right->triangle != nullptr)
    {
        node->left->bbox = triangle_bbox(node->left->triangle);
        node->right->bbox = triangle_bbox(node->right->triangle);
    }
}

void destroy_BVH(BVH_Node* node)
{
    if(node == nullptr) { return; }
    destroy_BVH(node->left);
    destroy_BVH(node->right);
    MEM_FREE(node);
}

void destroy_BVHs(Array<BVH_Node*> bvhs)
{
    for(int x = 0; x < bvhs.size; x++)
    {
        destroy_BVH(bvhs[x]);
    }
    bvhs.deallocate();
}
