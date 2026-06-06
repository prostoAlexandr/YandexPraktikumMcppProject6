#pragma once

#include <memory>
#include <optional>

#include "queue/priority_queue.hpp"
#include "queue/queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {

class TaskDispatcher {
    // здесь ваш код
    std::shared_ptr<queue::PriorityQueue> m_queue;
    thread_pool::ThreadPool m_tp;

public:
    TaskDispatcher(size_t thread_count, const queue::options_list_t &options = {
                                            {.bounded = true, .capacity = 1'000},
                                            {.bounded = false, .capacity = std::nullopt}});

    void schedule(TaskPriority priority, std::function<void()> task);
    ~TaskDispatcher();
};

}  // namespace dispatcher