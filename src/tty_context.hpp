//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#ifndef TTY_CONTEXT_HPP
#define TTY_CONTEXT_HPP

#include <cstdint>
#include "screen_buffer.hpp"
#include "math.hpp"

namespace rst
{

constexpr int   SCREEN_WIDTH{1920};
constexpr int   SCREEN_HEIGHT{1080};
constexpr float ASPECT_RATIO{SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT};

struct Color
{
    std::uint8_t b, g, r, a;

    Color(std::uint8_t x = 0, std::uint8_t y = 0,
          std::uint8_t z = 0, std::uint8_t w = 0) noexcept :
        b{x}, g{y}, r{z}, a{w} {}
    explicit Color(const Vec4f &vec) noexcept :
        b(0xFF * vec.y), g(0xFF * vec.y), r(0xFF * vec.x), a(0xFF * vec.w) {}
};

using FrameBuffer = ScreenBuffer<Color>;
using DepthBuffer = ScreenBuffer<float>;

class TtyContext
{
public:
                      TtyContext()            noexcept;
    FrameBuffer       &GetFrameBuffer()       noexcept { return m_frameBuffer; }
    const FrameBuffer &GetFrameBuffer() const noexcept { return m_frameBuffer; }
    DepthBuffer       &GetDepthBuffer()       noexcept { return m_depthBuffer; }
    const DepthBuffer &GetDepthBuffer() const noexcept { return m_depthBuffer; }

    void FlushFb() const noexcept;
    void Clear()         noexcept;

    float XScreenToNdc(int x)   const noexcept;
    float YScreenToNdc(int y)   const noexcept;
    int   XNdcToScreen(float x) const noexcept;
    int   YNdcToScreen(float y) const noexcept;
private:
    ScreenBuffer<Color> m_frameBuffer;
    ScreenBuffer<float> m_depthBuffer;
};

}

#endif //TTY_CONTEXT_HPP
