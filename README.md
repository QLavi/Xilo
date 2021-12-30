# Xilo

Xilo is a Path Tracer written in C++ [Mostly C Style]

How to build the project:
1. All the Rendering Configuration is inside `src/xilo.cpp`
2. OBJ Loading using `load_mesh` function in the same `src/xilo.cpp` implemented in `src/mesh_loader.cpp`
3. Run `make` in the terminal

What I have implemented in Xilo
- OBJ Loader
- Bounding Volume Heirarchy
- Simple Diffuse Material

What I have to implement in Xilo
- Properly using Kajiya's Rendering Equation
- Reflective/Specular and Refractive Materials
- Importance Sampling
- Next Event Estimation
- Depth of Field For Camera

Resources that helped me
- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [_Atrix256's Path Tracing Blog_](https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/)
- [_1000 Forms of Bunnies_](https://viclw17.github.io/)
