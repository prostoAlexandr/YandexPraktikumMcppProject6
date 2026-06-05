#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include <mutex>
#include <optional>

namespace dispatcher::queue {

// здесь ваш код
BoundedQueue::BoundedQueue(int capacity) : m_cap(capacity), m_finished(false) {}

void BoundedQueue::push(task_t task) {
    std::unique_lock lk(m_mutex);
    m_cv.wait(lk, [this] { return m_queue.size() < m_cap || m_finished; });
    if (m_finished) {
        return;
    }
    m_queue.push(task);
}

std::optional<task_t> BoundedQueue::try_pop() {
    std::unique_lock lk(m_mutex);
    if (m_queue.empty() || m_finished) {
        return {};
    }

    std::optional<task_t> result = std::move(m_queue.front());
    m_queue.pop();
    lk.unlock();
    m_cv.notify_one();
    return result;
}

BoundedQueue::~BoundedQueue() {
    m_finished = true;
    m_cv.notify_all();
}

} // namespace dispatcher::queue