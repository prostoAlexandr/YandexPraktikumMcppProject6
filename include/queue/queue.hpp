#pragma once
#include <functional>
#include <optional>
#include <vector>

namespace dispatcher::queue {

using task_t = std::function<void()>;

struct QueueOptions {
    bool bounded;
    std::optional<int> capacity;
};

using options_list_t = std::vector<QueueOptions>;

class IQueue {
public:
    virtual ~IQueue() = default;
    virtual void push(task_t task) = 0;
    virtual std::optional<task_t> try_pop() = 0;
};

}  // namespace dispatcher::queue