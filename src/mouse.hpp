//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 10/1/19.
//

#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <cstdint>

namespace rst
{

class Mouse
{
public:
    struct Event
    {
        std::int8_t flags;
        std::int8_t dx;
        std::int8_t dy;

        bool IsLeftButton() const;
    };

    Mouse();
    ~Mouse();
    bool Poll(Event &e);
private:
    int m_fd;
};

}

#endif //MOUSE_HPP
