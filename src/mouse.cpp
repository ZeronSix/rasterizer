//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 10/1/19.
//

#include "mouse.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

rst::Mouse::Mouse():
    m_fd{open("/dev/input/mice", O_RDONLY | O_NONBLOCK)}
{
//    assert(m_fd > 0 && "Failed to open mouse");
    Mouse::Event e{0, 0, 0};
    while (Poll(e));
}

rst::Mouse::~Mouse()
{
    close(m_fd);
}

bool rst::Mouse::Poll(rst::Mouse::Event &e)
{
    return read(m_fd, &e, sizeof(e)) > 0;
}

bool rst::Mouse::Event::IsLeftButton() const
{
    return (flags & 0x1) != 0;
}
