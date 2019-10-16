//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#ifndef RASTERIZER_HPP
#define RASTERIZER_HPP

#include "tty_context.hpp"
#include "screen_buffer.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include <algorithm>

namespace rst
{

template<typename VShader, typename FShader>
class Rasterizer
{
public:
    using VertexIn  = typename VShader::InType;
    using VertexOut = typename VShader::OutType;

         Rasterizer(TtyContext &context, VShader &vs, FShader &fs) noexcept;
    void RasterizeLine(const Vec3f &start, const Vec3f &end)       noexcept;
    void RasterizeTriangle(const VertexIn vertices[3])             noexcept;
    void RasterizeVertexArray(const std::vector<VertexIn> &vertices,
                              const std::vector<unsigned> &indices)     noexcept;
private:
    TtyContext  &m_context;
    FrameBuffer &m_frameBuf;
    DepthBuffer &m_depthBuf;
    VShader     &m_vertexShader;
    FShader     &m_fragmentShader;

    void SetPixel(int x, int y, float depth, const Color &color) noexcept;
};

template<typename VShader, typename FShader>
Rasterizer<VShader, FShader>::Rasterizer(TtyContext &context, VShader &vs, FShader &fs) noexcept:
    m_context{context},
    m_frameBuf{context.GetFrameBuffer()},
    m_depthBuf{context.GetDepthBuffer()},
    m_vertexShader{vs},
    m_fragmentShader{fs}
{
}

template<typename VShader, typename FShader>
void Rasterizer<VShader, FShader>::RasterizeLine(const Vec3f &start, const Vec3f &end) noexcept
{
    int startX = m_context.XNdcToScreen(Clamp(std::min(start.x, end.x), -1.0f, 1.0f));
    int startY = m_context.YNdcToScreen(Clamp(std::min(start.y, end.y), -1.0f, 1.0f));
    int endX   = m_context.XNdcToScreen(Clamp(std::max(start.x, end.x), -1.0f, 1.0f));
    int endY   = m_context.YNdcToScreen(Clamp(std::max(start.y, end.y), -1.0f, 1.0f));

    bool isMostlyHorizontal = std::abs(end.x - start.x) > std::abs(end.y - start.y);
    int pixels = isMostlyHorizontal ? endX - startX : endY - startY;

    for (int i = 0; i < pixels; i++)
    {
        float t = isMostlyHorizontal ? (m_context.XScreenToNdc(startX + i) - start.x) / (end.x - start.x) :
                  (m_context.YScreenToNdc(startY + i) - start.y) / (end.y - start.y);
        Vec3f pos = start + t * (end - start);
        SetPixel(m_context.XNdcToScreen(pos.x),
                 m_context.YNdcToScreen(pos.y),
                 pos.z,
                 Color{0xFF, 0xFF, 0xFF});
    }
}

static rst::Vec3f LIGHT_POS = {0.0f, 0.0f, 0.0f};

template<typename VShader, typename FShader>
void Rasterizer<VShader, FShader>::RasterizeTriangle(const VertexIn inputVertices[3]) noexcept
{
    VertexOut vertices[3] = {m_vertexShader(inputVertices[0]),
                             m_vertexShader(inputVertices[1]),
                             m_vertexShader(inputVertices[2])};
    Vec3f r1 = vertices[1].pos - vertices[0].pos;
    Vec3f r2 = vertices[2].pos - vertices[0].pos;

    int minX = m_context.XNdcToScreen(std::min({vertices[0].pos.x,
                                                vertices[1].pos.x,
                                                vertices[2].pos.x}));
    int minY = m_context.YNdcToScreen(std::min({vertices[0].pos.y,
                                                vertices[1].pos.y,
                                                vertices[2].pos.y}));
    int maxX = m_context.XNdcToScreen(std::max({vertices[0].pos.x,
                                                vertices[1].pos.x,
                                                vertices[2].pos.x}));
    int maxY = m_context.YNdcToScreen(std::max({vertices[0].pos.y,
                                                vertices[1].pos.y,
                                                vertices[2].pos.y}));

    const float delta = r1.x * r2.y - r1.y * r2.x;
    if (delta == 0.0f) return;

    for (int y = std::max(minY, 0); y <= std::min(maxY, SCREEN_HEIGHT - 1); ++y)
    {
        for (int x = std::max(minX, 0); x <= std::min(maxX, SCREEN_WIDTH - 1); ++x)
        {
            Vec3f ndcPixelPos{m_context.XScreenToNdc(x),
                              m_context.YScreenToNdc(y),
                              0};
            Vec3f offset = ndcPixelPos - vertices[0].pos;

            float delta1 = offset.x * r2.y - offset.y * r2.x;
            float delta2 = r1.x * offset.y - offset.x * r1.y;

            float beta = delta1 / delta;
            float gamma = delta2 / delta;
            float alpha = 1 - beta - gamma;

            // continue if the pixel is outside of the triangle
            if (beta < 0 || gamma < 0 || alpha < 0) continue;

            VertexOut interpolatedVertex;
            auto floatInterpolatedValues = reinterpret_cast<float *>(&interpolatedVertex);
            auto floatInputVertexValues  = reinterpret_cast<float *>(vertices);
            for (std::size_t i = 0; i < sizeof(interpolatedVertex) / sizeof(float); ++i)
            {
                // interpret input vertices as an array of floats and interpolate over them
                auto floatOffset = sizeof(interpolatedVertex) / sizeof(float);
                floatInterpolatedValues[i] = alpha * floatInputVertexValues[0 * floatOffset + i] +
                                             beta  * floatInputVertexValues[1 * floatOffset + i] +
                                             gamma * floatInputVertexValues[2 * floatOffset + i];
            }

            auto color = static_cast<Color>(m_fragmentShader(interpolatedVertex));
            SetPixel(x, y, interpolatedVertex.pos.z, color);
        }
    }
}

template<typename VShader, typename FShader>
void Rasterizer<VShader, FShader>::RasterizeVertexArray(const std::vector<VertexIn> &vertices,
                                                        const std::vector<unsigned> &indices) noexcept
{
    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        VertexIn triangle[3] = {
            vertices[indices[i]],
            vertices[indices[i + 1]],
            vertices[indices[i + 2]],
        };

        RasterizeTriangle(triangle);
    }
}

template<typename VShader, typename FShader>
void Rasterizer<VShader, FShader>::SetPixel(int x, int y, float depth, const Color &color) noexcept
{
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return;

    if (m_depthBuf[y][x] >= depth)
    {
        m_frameBuf[y][x] = color;
        m_depthBuf[y][x] = depth;
    }
}

}

#endif //RASTERIZER_HPP
