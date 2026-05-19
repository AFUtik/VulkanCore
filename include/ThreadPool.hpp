#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <barrier> 
#include <cassert>

using u32 = std::uint32_t;

class ThreadPool
{
public:
    explicit ThreadPool(u32 nThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename F> 
    void parallel_for(u32 begin, u32 end, F&& f)
    {
        if (begin >= end) return;

        const u32 total   = end - begin;
        const u32 nT      = static_cast<u32>(threads_.size());
        const u32 chunk   = (total + nT - 1) / nT;   // ceiling div

        task_begin_ = begin;
        task_end_   = end;
        task_chunk_ = chunk;
        task_fn_    = [&f](u32 b, u32 e) {
            if constexpr (std::is_invocable_v<F, u32, u32>)
                f(b, e);
            else
                for (u32 i = b; i < e; ++i) f(i);
        };

        // Синхронна фаза 1: головний потік відпускає воркерів
        barrier_.arrive_and_wait();

        // Воркери виконують роботу...

        // Синхронна фаза 2: чекаємо завершення всіх
        barrier_.arrive_and_wait();
    }

    u32 threadCount() const { return static_cast<u32>(threads_.size()); }
private:
    void workerLoop(u32 tid);

    std::vector<std::thread>                          threads_;
    std::barrier<>                                    barrier_;
    std::atomic<bool>                                 stop_{ false };

    u32                                               task_begin_{};
    u32                                               task_end_{};
    u32                                               task_chunk_{};
    std::function<void(u32, u32)>                     task_fn_;
};

extern ThreadPool threadPool;