#include "ThreadPool.hpp"

ThreadPool::ThreadPool(u32 nThreads)
        : threads_(nThreads)
        , barrier_(nThreads + 1)
{
    for (u32 t = 0; t < nThreads; ++t) 
        threads_[t] = std::thread(&ThreadPool::workerLoop, this, t);
}

ThreadPool::~ThreadPool()
{
    stop_.store(true, std::memory_order_relaxed);

    barrier_.arrive_and_drop();
    for (auto& th : threads_) th.join();
}

void ThreadPool::workerLoop(u32 tid)
{
    while (true)
    {
        // Чекаємо старту задачі
        barrier_.arrive_and_wait();

        if (stop_.load(std::memory_order_relaxed)) return;

        // Кожен потік бере свій шматок
        const u32 b = std::min(task_begin_ + tid * task_chunk_, task_end_);
        const u32 e = std::min(b + task_chunk_, task_end_);

        if (b < e) task_fn_(b, e);

        // Сигналізуємо завершення
        barrier_.arrive_and_wait();
    }
}

ThreadPool threadPool;