//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 10/15/19.
//

#ifndef SCREEN_BUFFER_HPP
#define SCREEN_BUFFER_HPP

#include <cstddef>
#include <stdexcept>

namespace rst
{

constexpr int SCREEN_WIDTH{1920};
constexpr int SCREEN_HEIGHT{1080};

template<typename T>
class ScreenBuffer
{
public:
                    ScreenBuffer(std::size_t width, std::size_t height);
                    ScreenBuffer(const ScreenBuffer<T> &) = delete;
    ScreenBuffer<T> &operator=(const ScreenBuffer<T> &) = delete;
                    ~ScreenBuffer() noexcept;

    T    *operator[](std::size_t y) noexcept;
    T    *operator[](std::size_t y) const noexcept;
    void Clear(const T& value);

    operator const T*() const { return m_memory; }
private:
    std::size_t m_width;
    std::size_t m_height;
    T           *m_memory;
};

template<typename T>
ScreenBuffer<T>::ScreenBuffer(std::size_t width, std::size_t height):
    m_width{width},
    m_height{height},
    m_memory{new T[width * height]}
{
    if (!m_memory) throw std::runtime_error("Memory allocation");
}

template<typename T>
ScreenBuffer<T>::~ScreenBuffer() noexcept
{
    delete[] m_memory;
}

template<typename T>
T *ScreenBuffer<T>::operator[](std::size_t y) noexcept
{
    return m_memory + m_width * (m_height - 1 - y);
}

template<typename T>
T *ScreenBuffer<T>::operator[](std::size_t y) const noexcept
{
    return m_memory + m_width * (m_height - 1 - y);
}

template<typename T>
void ScreenBuffer<T>::Clear(const T &value)
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            m_memory[m_width * y + x] = value;
        }
    }
}

}

#endif //SCREEN_BUFFER_HPP
