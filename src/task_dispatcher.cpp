#include "task_dispatcher.hpp"
#include "queue/priority_queue.hpp"

namespace dispatcher {

// здесь ваш код
TaskDispatcher::TaskDispatcher(size_t thread_count, const queue::options_list_t &options)
    : m_queue(std::make_shared<queue::PriorityQueue>(options)), m_tp(m_queue, thread_count) {}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) {
    m_queue->push(priority, task);
}

TaskDispatcher::~TaskDispatcher() { m_queue->shutdown(); }

}  // namespace dispatcher