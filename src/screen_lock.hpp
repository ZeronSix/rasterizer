//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/19/19.
//

#ifndef SCREEN_LOCK_HPP
#define SCREEN_LOCK_HPP

#include "aligning_mallocator.hpp"
#include <atomic>

namespace rst
{

constexpr std::size_t CACHE_LINE_SIZE{64};
constexpr std::size_t PIXELS_PER_LOCK{16};

class ScreenLock
{
public:
    ScreenLock(): m_buf{SCREEN_WIDTH * SCREEN_HEIGHT / PIXELS_PER_LOCK} {}
    void Lock(std::size_t x, std::size_t y) { m_buf[Index(x, y)].Lock(); }
    void Unlock(std::size_t x, std::size_t y) { m_buf[Index(x, y)].Unlock(); }
private:
    struct SpinLock
    {
        std::atomic_flag lock;

        void Lock() { while (lock.test_and_set(std::memory_order_acquire)); }
        void Unlock() { lock.clear(std::memory_order_release); }
    };// __attribute__((aligned(CACHE_LINE_SIZE)));
    AlignedVec<SpinLock, CACHE_LINE_SIZE> m_buf;

    std::size_t Index(std::size_t x, std::size_t y) { return (SCREEN_WIDTH * y + x) / PIXELS_PER_LOCK; }
};

}

#endif //SCREEN_LOCK_HPP
