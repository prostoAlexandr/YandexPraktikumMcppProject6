#pragma once
#include "queue/queue.hpp"
#include <mutex>
#include <queue>

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
    // здесь ваш код
    std::queue<task_t> m_queue;
    std::mutex m_mutex;

public:
    explicit UnboundedQueue();

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override = default;
};

}  // namespace dispatcher::queue