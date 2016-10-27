/*
    Author: kedixa
    E-mail: 1204837541@qq.com
    License: GPL2
*/

#include <queue>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <chrono>
#include <cassert>

template<typename T>
class k_queue
{
    using u_lock = std::unique_lock<std::mutex>;
    using milli_seconds = std::chrono::milliseconds;
private:
    std::queue<T> que;
    std::mutex mtx;
    std::condition_variable cv_not_full;
    std::condition_variable cv_not_empty;
    size_t max_size;
    bool stop_flag;
public:
    k_queue(size_t max_size = std::numeric_limits<size_t>::max()) noexcept
    {
        stop_flag = false;
        this->max_size = max_size;
    }
    k_queue(k_queue<T>&) = delete;
    k_queue operator= (k_queue<T>&) = delete;

    /*
     * Get an element from k_queue in milli, return nullptr if fail.
     */
    std::shared_ptr<T> pop(milli_seconds milli = milli_seconds(0))
    {
        u_lock lk(mtx);
        auto result = std::shared_ptr<T>(nullptr);
        if(cv_not_empty.wait_for(lk, milli, [&](){return stop_flag || !que.empty();}))
        {
            if(stop_flag) return result; // stop
            assert(!que.empty()); // DEBUG
            result = std::make_shared<T>(std::move(que.front()));
            que.pop();
            if(que.size() != max_size)
                cv_not_full.notify_one();
        }
        return result;
    }

    /*
     * Get an element from k_queue, block util get an element or stop.
     */
    std::shared_ptr<T> pop_block()
    {
        u_lock lk(mtx);
        cv_not_empty.wait(lk, [&](){return stop_flag || !que.empty();});
        if(stop_flag) return std::shared_ptr<T>(nullptr);
        assert(!que.empty()); // DEBUG
        auto result = std::make_shared<T>(std::move(que.front()));
        que.pop();
        if(que.size() != max_size)
            cv_not_full.notify_one();
        return result;
    }

    /*
     * Add an element to k_queue in milli, return false if time_out or stop.
     */
    template<typename U>
    typename std::enable_if<
            std::is_same<T, /*typename is important*/typename std::remove_reference<U>::type>::value, bool
        >::type
    push(U&& t, milli_seconds milli = milli_seconds(0))
    {
        u_lock lk(mtx);
        if(cv_not_full.wait_for(lk, milli, [&](){return stop_flag || que.size() != max_size;}))
        {
            if(stop_flag) return false;
            assert(que.size() != max_size);
            que.push(std::forward<T>(t));
            cv_not_empty.notify_one();
            return true;
        }
        else return false;
    }
    
    /*
     * Stop k_queue, after call this function, both push and pop will not work any more.
     */
    void stop()
    {
        {
            u_lock lk(mtx);
            stop_flag = true;
        }
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
    size_t size() noexcept
    {
        return que.size();
    }
    ~k_queue() noexcept
    {
        stop();
    }
};
