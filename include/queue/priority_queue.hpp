#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace dispatcher::queue {

class ConfigurationError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class PriorityQueue {
    // здесь ваш код
    std::vector<std::unique_ptr<IQueue>> m_queues;
    std::atomic_bool m_finished;
    std::mutex m_mutex;
    std::atomic<int64_t> m_pops;

public:
    explicit PriorityQueue(const options_list_t &options);

    void push(TaskPriority priority, task_t task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<task_t> pop();

    void shutdown();

    ~PriorityQueue();
};

}  // namespace dispatcher::queue