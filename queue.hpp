/*
    Author: kedixa
    E-mail: 1204837541@qq.com
    License: GPL2

    class kedixa::queue<T>
    Description:
        A thread safe queue.
*/

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <type_traits>
#include <chrono>
#include <cassert>

namespace kedixa
{
template<typename T>
class queue
{
    using u_lock = std::unique_lock<std::mutex>;
    using milli_seconds = std::chrono::milliseconds;
private:
    std::queue<T> que;
    std::mutex mtx;
    std::condition_variable cv_not_full;
    std::condition_variable cv_not_empty;
    size_t max_size;
    std::atomic<bool> stop_flag;
public:
    /*
     * Construct a queue with size max_size, maximum for default.
     */
    queue(size_t max_size =
        std::numeric_limits<size_t>::max()) noexcept
    {
        stop_flag = false;
        this->max_size = max_size;
    }

    /*
     * This class cannot be copied.
     */
    queue(queue<T>&) = delete;
    queue& operator= (queue<T>&) = delete;

    /*
     * Get an element from queue in milli, return nullptr if fail.
     */
    std::shared_ptr<T> pop(milli_seconds milli = milli_seconds(0))
    {
        if(stop_flag) return std::shared_ptr<T>(nullptr);
        u_lock lk(mtx);
        auto result = std::shared_ptr<T>(nullptr);
        auto pred = [&](){return stop_flag || !que.empty();};
        if(cv_not_empty.wait_for(lk, milli, pred))
        {
            if(stop_flag) return result; // stop
            result = std::make_shared<T>(std::move(que.front()));
            que.pop();
            if(que.size() != max_size)
                cv_not_full.notify_one();
        }
        return result;
    }

    /*
     * Get an element from queue, block util get an element or stop.
     */
    std::shared_ptr<T> pop_block()
    {
        if(stop_flag) return std::shared_ptr<T>(nullptr);
        u_lock lk(mtx);
        cv_not_empty.wait(lk, [&](){return stop_flag || !que.empty();});
        if(stop_flag) return std::shared_ptr<T>(nullptr);
        auto result = std::make_shared<T>(std::move(que.front()));
        que.pop();
        if(que.size() != max_size)
            cv_not_full.notify_one();
        return result;
    }

    /*
     * Add an element to queue in milli, return false if time_out or stop.
     */
    template<typename U>
    typename std::enable_if<std::is_same<T,
            /*typename is important*/
            typename std::remove_reference<U>::type
            >::value, bool>::type
    push(U&& t, milli_seconds milli = milli_seconds(0))
    {
        if(stop_flag) return false;
        u_lock lk(mtx);
        auto pred = [&](){return stop_flag || que.size() != max_size;};
        if(cv_not_full.wait_for(lk, milli, pred))
        {
            if(stop_flag) return false;
            que.push(std::forward<U>(t));
            cv_not_empty.notify_one();
            return true;
        }
        else return false;
    }
    
    /*
     * Stop queue, after call this function,
     * both push and pop will not work any more.
     */
    void stop()
    {
        if(stop_flag) return;
        stop_flag = true;
        cv_not_empty.notify_all();
        cv_not_full.notify_all();
    }
    bool empty() noexcept
    {
        return que.empty();
    }
    bool full() noexcept
    {
        return que.size() == max_size;
    }
    void clear() noexcept
    {
        u_lock lk(mtx);
        while(!que.empty()) que.pop();
    }
    size_t size() noexcept
    {
        return que.size();
    }
    ~queue() noexcept
    {
        stop();
        clear();
    }
}; // class queue

} // namespace
