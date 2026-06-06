#include "queue/unbounded_queue.hpp"

#include <functional>
#include <mutex>
#include <queue>
#include <semaphore>

namespace dispatcher::queue {

// здесь ваш код
UnboundedQueue::UnboundedQueue() {}

void UnboundedQueue::push(task_t task) {
    std::scoped_lock lk(m_mutex);
    m_queue.push(task);
}

std::optional<task_t> UnboundedQueue::try_pop() {
    std::scoped_lock lk(m_mutex);
    if (m_queue.empty()) {
        return {};
    }

    std::optional<task_t> result = std::move(m_queue.front());
    m_queue.pop();
    return result;
}

} // namespace dispatcher::queue