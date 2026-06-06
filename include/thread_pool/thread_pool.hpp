#pragma once

#include "queue/priority_queue.hpp"
#include <memory>
#include <thread>
#include <vector>
namespace dispatcher::thread_pool {

class ThreadPool {
    // здесь ваш код
private:
    std::vector<std::jthread> m_pool;
    std::shared_ptr<queue::PriorityQueue> m_queue;

    void Worker();

public:
    ThreadPool(std::shared_ptr<queue::PriorityQueue> shared_queue, int thread_count);
    ~ThreadPool();
};

} // namespace dispatcher::thread_pool
