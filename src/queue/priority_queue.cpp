#include "queue/priority_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace dispatcher::queue {

namespace {

std::unique_ptr<IQueue> queue_factory(const QueueOptions &opt) {
    if (!opt.bounded) {
        return std::make_unique<UnboundedQueue>();
    } else if (opt.bounded && opt.capacity.has_value()) {
        return std::make_unique<BoundedQueue>(opt.capacity.value());
    } else {
        throw ConfigurationError("Bad queue option");
    }
}

}  // namespace

// здесь ваш код
PriorityQueue::PriorityQueue(const options_list_t &options) : m_finished(false) {
    m_queues.resize(options.size());
    for (TaskPriority prio : {TaskPriority::High, TaskPriority::Normal}) {
        if ((int)prio >= options.size()) {
            throw ConfigurationError("Too few options passed");
        }

        const auto &opt = options[(size_t)prio];
        m_queues[(size_t)prio] = queue_factory(opt);
    }
}

void PriorityQueue::push(TaskPriority priority, task_t task) {
    std::unique_lock lk(m_mutex);
    m_queues[(int)priority]->push(std::move(task));
    lk.unlock();
    m_pops.fetch_add(1, std::memory_order::release);
    m_pops.notify_one();
}

std::optional<task_t> PriorityQueue::pop() {
    std::unique_lock lk(m_mutex);
    while (true) {
        for (TaskPriority prio : {TaskPriority::High, TaskPriority::Normal}) {
            auto result = m_queues[(size_t)prio]->try_pop();
            if (result.has_value()) {
                return result;
            }
        }
        if (m_finished.load(std::memory_order::relaxed)) {
            return {};
        }
        auto curr_pops = m_pops.load(std::memory_order::relaxed);
        lk.unlock();
        m_pops.wait(curr_pops, std::memory_order::acquire);
        lk.lock();
    }
}

void PriorityQueue::shutdown() {
    m_finished.store(true, std::memory_order::relaxed);
    m_pops.fetch_add(1, std::memory_order::release);
    m_pops.notify_all();
}

PriorityQueue::~PriorityQueue() { shutdown(); }

}  // namespace dispatcher::queue