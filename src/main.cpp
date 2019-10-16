#include <iostream>
#include <memory>
#include "tty_context.hpp"
#include "rasterizer.hpp"
#include "mouse.hpp"
#include "shaders/basic_vertex_shader.hpp"
#include "shaders/basic_fragment_shader.hpp"

using namespace rst;

int main()
{
    auto projection = ProjectionMatrix(120.0f * DEG2RAD, ASPECT_RATIO, 0.01f, 1000.0f);
        TtyContext context;
    BasicVertexShader vs {
        Mat4f::Id(),
        projection,
        Mat3f::Id()
    };
    BasicFragmentShader fs;
    Rasterizer<BasicVertexShader, BasicFragmentShader> rast{context, vs, fs};
    Mesh cube = Mesh::LoadFromObj("../assets/cube.obj");
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
        rast.RasterizeVertexArray(cube.vertices, cube.indices);
        context.FlushFb();
    }

    return 0;
}