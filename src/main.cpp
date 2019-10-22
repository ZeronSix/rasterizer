#include <iostream>
#include <memory>
#include "tty_context.hpp"
#include "rasterizer.hpp"
#include "mouse.hpp"
#include "shaders/shaders.hpp"

using namespace rst;

int main()
{
    auto projection = ProjectionMatrix(120.0f * DEG2RAD, ASPECT_RATIO, 0.01f, 1000.0f);
    TtyContext context;
    PhongVertexShader vs {
        Mat4f::Id(),
        projection,
        Mat4f::Id()
    };
    PhongFragmentShader fs;
    Rasterizer<PhongVertexShader, PhongFragmentShader> rast{context, vs, fs};
    Mesh cube = Mesh::LoadFromObj("../../assets/cube.obj");
    Mouse mouse;

    float theta = M_PI_2;
    float phi = 0.0f;

    while (1)
    {
        Mouse::Event e{0, 0, 0};
        while (mouse.Poll(e))
        {
            if (e.IsLeftButton())
            {
                return 0;
            }
            phi -= e.dx * 1e-3;
            theta += e.dy * 1e-3;
        }

        Vec3f direction{std::sin(theta) * std::sin(phi),
                       std::cos(theta),
                       std::sin(theta) * std::cos(phi)};

        context.Clear();

        Mat4f rot = LookRotation(direction, Vec3f{0, 1, 0});
        Mat4f trans = TranslationMatrix(Vec3f{0, 0, 10});
        vs.modelMatrix = trans * rot;
        vs.normalMatrix = rot;
        rast.RasterizeVertexArray(cube.vertices, cube.indices);
        context.FlushFb();
    }

    return 0;
}