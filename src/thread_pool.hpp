//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/14/19.
//

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename Callable>
class ThreadPool
{
public:
    using Params = typename Callable::ThreadParams;

    ThreadPool(std::size_t threads, std::vector<Params> &params);
    ~ThreadPool();

    void EnqueueTask(Callable &&task);
private:
    std::size_t              m_threads;
    std::vector<Params>      &m_params;
    bool                     m_shouldStop;
    std::vector<std::thread> m_workers;
    std::queue<Callable>     m_tasks;
    std::mutex               m_mutex;
    std::condition_variable  m_condition;
    std::condition_variable  m_finishCondition;
    std::size_t              m_jobSetSize;

    void Worker(Params &params);
};

template<typename Callable>
ThreadPool<Callable>::ThreadPool(std::size_t threads, std::vector<Params> &params):
    m_threads{threads},
    m_params{params},
    m_shouldStop{false}
{
    for (std::size_t i = 0; i < threads; ++i)
    {
        m_workers.emplace_back([this, i] { this->Worker(m_params[i]); });
    }
}

template<typename Callable>
ThreadPool<Callable>::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_shouldStop = true;
    }
    m_condition.notify_all();
    for(auto &worker : m_workers)
    {
        worker.join();
    }
}

template<typename Callable>
void ThreadPool<Callable>::EnqueueTask(Callable &&task)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.emplace(task);
    }
    m_condition.notify_one();
}

template<typename Callable>
void ThreadPool<Callable>::Worker(Params &params)
{
    for (;;)
    {
        Callable task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [this] { return m_shouldStop || !m_tasks.empty(); });
            if (m_shouldStop && m_tasks.empty()) {
                return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        task(params);
    }
}

#endif //THREAD_POOL_HPP
