//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#include "tty_context.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cmath>

namespace rst
{

TtyContext::TtyContext() noexcept:
    m_frameBuffer{SCREEN_WIDTH, SCREEN_HEIGHT},
    m_depthBuffer{SCREEN_WIDTH, SCREEN_HEIGHT}
{
    Clear();
}

void TtyContext::FlushFb() const noexcept
{
    int fd = open("/dev/fb0", O_WRONLY);
    if (fd < 0)
    {
        std::perror("open fb0");
        return;
    }

    if (write(fd, m_frameBuffer, sizeof(Color) * SCREEN_WIDTH * SCREEN_HEIGHT) < 0)
    {
        std::perror("write");
    }
    close(fd);
}

void TtyContext::Clear() noexcept
{
    m_frameBuffer.Clear(Color{0x0, 0x0, 0x0, 0x0});
    m_depthBuffer.Clear(1.0f);
}

float TtyContext::XScreenToNdc(int x) const noexcept
{
    return -1.0f + (2.0f * x + 1.0f) / SCREEN_WIDTH;
}

float TtyContext::YScreenToNdc(int y) const noexcept
{
    return -1.0f + (2.0f * y + 1.0f) / SCREEN_HEIGHT;
}

int TtyContext::XNdcToScreen(float x) const noexcept
{
    return std::lround(-0.5f + SCREEN_WIDTH / 2.0f * (x + 1));
}

int TtyContext::YNdcToScreen(float y) const noexcept
{
    return std::lround(-0.5f + SCREEN_HEIGHT / 2.0f * (y + 1));
}

}
