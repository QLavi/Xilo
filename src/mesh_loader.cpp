#include <stdio.h>
#include <string.h>

Array<Triangle> load_mesh(const char* filename, int vertex_count, int face_count, Material* material)
{
    Array<Vec3> vertices; vertices.allocate(vertex_count);
    Array<int> indices; indices.allocate(face_count * 3);

    FILE* fptr = fopen(filename, "r");
    while(true)
    {
        char line[128];
        int res = fscanf(fptr, "%s", line);
        if(res == EOF) { break; }

        if(strcmp(line, "v") == 0)
        {
            Vec3 vertex;
            fscanf(fptr, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        else if(strcmp(line, "f") == 0)
        {
            int vertex_index[3];
            fscanf(fptr, "%i %i %i\n", &vertex_index[0], &vertex_index[1], &vertex_index[2]);
            indices.push_back(vertex_index[0]);
            indices.push_back(vertex_index[1]);
            indices.push_back(vertex_index[2]);
        }
    }
    fclose(fptr);

    int triangle_count = indices.size / 3;
    Array<Triangle> triangles; triangles.allocate(triangle_count);

    for(int x = 0; x < indices.size; x+=3)
    {
        Triangle triangle = Triangle
        {
            vertices[indices[x] -1],
            vertices[indices[x +1] -1],
            vertices[indices[x +2] -1],
            material
        };
        triangles.push_back(triangle);
    }

    vertices.deallocate();
    indices.deallocate();
    return triangles;
}

void destroy_meshes(Array<Array<Triangle>> meshes)
{
    for(int x = 0; x < meshes.size; x++)
    {
        meshes[x].deallocate();
    }
    meshes.deallocate();
}
