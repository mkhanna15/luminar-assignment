#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <thread>
#include <condition_variable>
#include <deque>

template<typename T>
class threadsafe_queue_t
{
private:
    mutable std::mutex m;
    std::deque<T> data_queue;
    std::condition_variable data_cond;

    threadsafe_queue_t( threadsafe_queue_t const& ) = delete;
    threadsafe_queue_t& operator=( threadsafe_queue_t const& ) = delete;
public:
    threadsafe_queue_t() {}

    void push(T value)
    {
        std::lock_guard<std::mutex> l(m);
        data_queue.push_back(value);
        data_cond.notify_one();
    }
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> l(m);
        data_cond.wait(l,[this]{return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop_front();
    }
    size_t size() const {
        std::lock_guard<std::mutex> l(m);
        return data_queue.size();
    }
};

#endif

