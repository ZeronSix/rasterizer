//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/15/19.
//

#include <iostream>
#include <memory>
#include <chrono>
#include "tty_context.hpp"
#include "rasterizer.hpp"
#include "texture.hpp"

using namespace rst;

struct MyVertexShader
{
    using InType = Mesh::Vertex;
    struct OutType
    {
        Vec4f pos;
        Vec2f tex;
        Vec3f norm;
    };

    Mat4f projection;

    OutType operator()(const InType &in)
    {
        Vec4f outPos = projection * Vec4f{in.pos, 1.0f};

        return OutType{outPos, in.tex, in.norm};
    }
};

struct MyFragmentShader
{
    using InType = MyVertexShader::OutType;
    MyVertexShader::OutType vert;

    Vec3f camPos;
    Texture *tex;

    Vec4f operator()(const InType &in)
    {
        Vec3f light = Normalize(Vec3f{1.f, 1.f, 3.f});
        Vec3f camera = Normalize(camPos - Vec3f{in.pos});
        Vec3f halfway = Normalize(light + camera);
        float NH = std::max(0.f, Dot(in.norm, halfway));
        float spec = NH;
        for(int i = 0; i < 3; i++)
            spec *= spec;
        float diff = std::max(0.f, Dot(in.norm, light));
        Vec4f c = (0.2f + 0.4f * diff + 0.4f * spec) * tex->Fetch(in.tex);
        return c;
    }
};

int main()
{
    Mat4f perspProj = Persp(1.0f, ASPECT_RATIO, 0.1f, 10.0f);
    Vec3f up(0.f, 1.f, 0.f);
    Vec3f at(0.f, 1.f, 0.f);

    TtyContext context;

    MyVertexShader vs;
    MyFragmentShader fs;
    Rasterizer<MyVertexShader, MyFragmentShader> pipe{context, vs, fs, 4};

    Mesh cat = Mesh::LoadFromObj("cat.obj");
    Texture tex{"cat.ppm"};
    fs.tex = &tex;

    float theta = 0.4f;
    int iter = 0;
    int iters = 2000;

    while (iter < iters)
    {
        context.Clear();

        float phi = 2.f / iters * M_PI * iter++;
        Vec3f dir(std::sin(phi) * std::cos(theta), std::sin(theta), std::cos(phi) * std::cos(theta));
        Vec3f camPos = 1.5f * dir;
        Mat4f lookAt = LookAt(camPos + at, at, up);

        vs.projection = perspProj * lookAt;
        fs.camPos = camPos;

        auto t0 = std::chrono::system_clock::now();

        pipe.RasterizeVertexArray(cat.vertices, cat.indices);

        auto t1 = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> const dt = t1 - t0;
        std::cout << dt.count() << std::endl;
        context.FlushFb();
    }

    return 0;
}
