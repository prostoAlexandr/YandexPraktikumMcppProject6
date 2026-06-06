#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include <atomic>
#include <mutex>
#include <optional>

namespace dispatcher::queue {

// здесь ваш код
BoundedQueue::BoundedQueue(int capacity) : m_cap(capacity), m_pops(0) {}

void BoundedQueue::push(task_t task) {
    std::unique_lock lk(m_mutex);
    while (m_queue.size() >= m_cap) {
        auto curr_pops = m_pops.load(std::memory_order::relaxed);
        lk.unlock();
        m_pops.wait(curr_pops, std::memory_order::acquire);
        lk.lock();
    }
    m_queue.push(task);
}

std::optional<task_t> BoundedQueue::try_pop() {
    std::unique_lock lk(m_mutex);
    if (m_queue.empty()) {
        return {};
    }

    std::optional<task_t> result = std::move(m_queue.front());
    m_queue.pop();
    lk.unlock();

    m_pops.fetch_add(1, std::memory_order::release);
    m_pops.notify_one();
    return result;
}

}  // namespace dispatcher::queue