//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/18/19.
//

#ifndef BATCH_TASKS_HPP
#define BATCH_TASKS_HPP

#include "thread_pool.hpp"
#include "tty_context.hpp"
#include <vector>
#include <atomic>

namespace rst
{

template<typename FShader>
struct FragmentBatchTask
{
    using FsIn = typename FShader::InType;

    FShader     &shader;
    FsIn        *fragments;
    std::size_t batchSize;

    FrameBuffer &frameBuf;
    DepthBuffer &depthBuf;

    void operator()(int threadId)
    {
        for (auto i = 0ul; i < batchSize; ++i)
        {
            FsIn p = fragments[i];
            if (p.depth < -1.0f || p.depth > depthBuf[p.y][p.x])
            {
                continue;
            }

            frameBuf[p.y][p.x] = Color{shader(p)};
        }
    }
};

enum class Culling
{
    Ccw,
    Cw,
    None
};

template<typename VShader>
struct VertexBatchTask
{
    using VsIn  = typename VShader::InType;
    using VsOut = typename VShader::OutType;
    struct ThreadParams
    {
        VShader &shader;

        std::vector<VsOut> &output;
    };

    const VsIn  *vertices;
    std::size_t startIndex;
    std::size_t endIndex;

    void operator()(ThreadParams &params)
    {
        auto &shader = params.shader;
        auto &output = params.output;

        for (auto i = startIndex; i < endIndex; ++i)
        {
            output[i] = shader(vertices[i]);
        }
    }
};

template<typename VShader>
struct RasterBatchTask
{
    using VsIn  = typename VShader::InType;
    using VsOut = typename VShader::OutType;

    struct Output
    {
        int x;
        int y;
        float depth;
        float b;
        float c;

        VsOut v1, v2, v3;
    };

    const unsigned *indices;
    std::size_t    start;
    std::size_t    end;

    struct ThreadParams
    {
        std::vector<VsOut>  &vsOutput;
        Culling             &culling;
        FrameBuffer  &fb;
        std::vector<Output> output;
    };

    void operator()(ThreadParams &params)
    {
        auto &vsOutput = params.vsOutput;
        for (auto i = start; i < end; i += 3)
        {
            ProcessTriangle(vsOutput[indices[i]],
                            vsOutput[indices[i + 1]],
                            vsOutput[indices[i + 2]], params);
        }
    }
private:
    void ProcessTriangle(VsOut &p1, VsOut &p2, VsOut &p3, ThreadParams &params)
    {
        auto &output = params.output;
        if (p1.pos.w > 0.0f || p2.pos.w > 0.0f || p3.pos.w > 0.0f)
        {
            return;
        }

        Vec3f v[3] = {Vec3f{p1.pos}, Vec3f{p2.pos}, Vec3f{p3.pos}};

        float dx1 = v[1].x - v[0].x;
        float dx2 = v[2].x - v[0].x;
        float dy1 = v[1].y - v[0].y;
        float dy2 = v[2].y - v[0].y;
        float det = dx1 * dy2 - dy1 * dx2;

        // Determine the winding of the triangle
        Vec3f cross = Cross(Vec3f{dx1, dy1, 0}, Vec3f{dx2, dy2, 0});
        bool cw = cross.z > 0.0f;
        if (cw && params.culling == Culling::Cw) return;
        if (!cw && params.culling == Culling::Ccw) return;

        auto clamp = [](float x) {
            constexpr float eps = 1e-6;
            return x >= 1.f ? 1.f - eps : (x <= -1.f ? -1.f + eps : x);
        };

        int minX = XNdcToScreen(clamp(std::min(std::min(v[0].x, v[1].x), v[2].x)));
        int maxX = XNdcToScreen(clamp(std::max(std::max(v[0].x, v[1].x), v[2].x)));
        int minY = YNdcToScreen(clamp(std::min(std::min(v[0].y, v[1].y), v[2].y)));
        int maxY = YNdcToScreen(clamp(std::max(std::max(v[0].y, v[1].y), v[2].y)));

        for (int y = minY; y <= maxY; ++y)
        {
            float ndcY = YScreenToNdc(y);
            float dy = ndcY - v[0].y;
            for (int x = minX; x <= maxX; ++x)
            {
                float ndcX = XScreenToNdc(x);
                float dx = ndcX - v[0].x;
                float det1 = dx * dy2 - dy * dx2;
                float det2 = dx1 * dy - dy1 * dx;

                float b0 = det1 / det;
                float c0 = det2 / det;
                float a0 = 1.f - b0 - c0;
                if(a0 < 0.f || b0 < 0.f || c0 < 0.f)
                    continue;
                float depth = v[0].z * a0 + v[1].z * b0 + v[2].z * c0;
                float a = a0 / p1.pos.w;
                float b = b0 / p2.pos.w;
                float c = c0 / p3.pos.w;
                float sum = a + b + c;

                output.emplace_back(Output{x, y, depth, b / sum, c / sum, p1, p2, p3});
            }
        }
    }
};

template<typename Fragment, typename FShader>
struct FragBatchTask
{
    using FsIn = typename FShader::InType;

    struct ThreadParams
    {
        FShader     &shader;
        FrameBuffer &frameBuf;
        DepthBuffer &depthBuf;
        ScreenLock  &screenLock;
    };

    Fragment    *fragments;
    std::size_t start;
    std::size_t end;

    void operator()(ThreadParams &params)
    {
        auto &shader = params.shader;
        auto &frameBuf = params.frameBuf;
        auto &depthBuf = params.depthBuf;

        for (auto i = start; i < end; ++i)
        {
            auto &frag = fragments[i];

            FsIn v;
            auto vf  = reinterpret_cast<float *>(&v);
            auto v1f = reinterpret_cast<float *>(&frag.v1);
            auto v2f = reinterpret_cast<float *>(&frag.v2);
            auto v3f = reinterpret_cast<float *>(&frag.v3);

            if (depthBuf[frag.y][frag.x] < frag.depth)
            {
                continue;
            }

            float a = 1 - frag.b - frag.c;
            for (std::size_t j = 0; j < sizeof(FsIn) / sizeof(float); ++j)
            {
                // interpret input vertices as an array of floats and interpolate over them
                vf[j] = a * v1f[j] + frag.b * v2f[j] + frag.c * v3f[j];
            }

            params.screenLock.Lock(frag.x, frag.y);
            if (depthBuf[frag.y][frag.x] >= frag.depth)
            {
                frameBuf[frag.y][frag.x] = static_cast<Color>(shader(v));
                depthBuf[frag.y][frag.x] = frag.depth;
            }
            params.screenLock.Unlock(frag.x, frag.y);
        }
    }
};


}

#endif //BATCH_TASKS_HPP
