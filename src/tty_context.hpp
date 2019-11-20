//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#ifndef TTY_CONTEXT_HPP
#define TTY_CONTEXT_HPP

#include <cstdint>
#include "screen_buffer.hpp"
#include "math.hpp"
#include "screen_lock.hpp"

namespace rst
{

constexpr float ASPECT_RATIO{SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT};

struct Color
{
    std::uint8_t b, g, r, a;

    Color(std::uint8_t x = 0, std::uint8_t y = 0,
          std::uint8_t z = 0, std::uint8_t w = 0) noexcept :
        b{x}, g{y}, r{z}, a{w} {}
    explicit Color(const Vec4f &vec) noexcept :
        b(0xFF * vec.x), g(0xFF * vec.y), r(0xFF * vec.z), a(0xFF * vec.w) {}
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
    ScreenLock        &GetScreenLock()        noexcept { return m_screenLock; }
    const ScreenLock  &GetScreenLock()  const noexcept { return m_screenLock; }


    void FlushFb() const noexcept;
    void Clear()         noexcept;

private:
    ScreenBuffer<Color> m_frameBuffer;
    ScreenBuffer<float> m_depthBuffer;
    ScreenLock          m_screenLock;
};

float XScreenToNdc(int x)   noexcept;
float YScreenToNdc(int y)   noexcept;
int   XNdcToScreen(float x) noexcept;
int   YNdcToScreen(float y) noexcept;

}

#endif //TTY_CONTEXT_HPP
