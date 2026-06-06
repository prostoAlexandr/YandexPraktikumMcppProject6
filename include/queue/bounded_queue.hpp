#pragma once
#include "queue/queue.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
    // здесь ваш код
    std::queue<task_t> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    int m_cap;

public:
    explicit BoundedQueue(int capacity);

    void push(task_t task) override;

    std::optional<task_t> try_pop() override;

    ~BoundedQueue() override;
};

}  // namespace dispatcher::queue