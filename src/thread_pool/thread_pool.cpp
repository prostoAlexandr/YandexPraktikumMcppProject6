#include "thread_pool/thread_pool.hpp"

namespace dispatcher::thread_pool {

// здесь ваш код
ThreadPool::ThreadPool(std::shared_ptr<queue::PriorityQueue> shared_queue, int thread_count)
    : m_queue(shared_queue) {
    for (int i = 0; i < thread_count; ++i) {
        m_pool.emplace_back(&ThreadPool::Worker, this);
    }
}

void ThreadPool::Worker() {
    while (true) {
        auto result = m_queue->pop();
        if (!result.has_value()) {
            return;
        }
        result.value()();
    }
}

ThreadPool::~ThreadPool() { m_queue->shutdown(); }

} // namespace dispatcher::thread_pool