/*
    Author: kedixa
    E-mail: 1204837541@qq.com
    License: GPL2
*/

#include <queue>
#include <mutex>
#include <condition_variable>
#include <type_traits>

template<typename T>
class k_queue
{
private:
    std::queue<T> que;
    std::mutex mtx;
    std::condition_variable cv;
    size_t max_size;
public:
    k_queue(int max_size = std::numeric_limits<size_t>::max())
        : max_size(max_size) {}
    k_queue(k_queue<T>&) = delete;
    k_queue operator= (k_queue<T>&) = delete;

    std::pair<bool, T> pop()
    {
        std::unique_lock<std::mutex> lk(mtx);
        std::pair<bool, T> result;
        if(que.empty()) result = std::make_pair(false, T());
        else
        {
            result = std::make_pair(true, std::move(que.front()));
            que.pop();
        }
        return result;
    }
    T pop_block()
    {
        std::unique_lock<std::mutex> lk(mtx);
        auto f = [&](){return !que.empty();};
        cv.wait(lk, f);
        auto result = std::move(que.front());
        que.pop();
        return result;
    }
    template<typename U>
    typename std::enable_if<
            std::is_same<T, /*typename is important*/typename std::remove_reference<U>::type>::value, bool
        >::type
    push(U&& t)
    {
        std::unique_lock<std::mutex> lk(mtx);
        if(que.size() == max_size) return false;
        else
        {
            que.push(std::forward<T>(t));
            lk.unlock();
            cv.notify_one();
        }
        return true;
    }
    bool empty()
    {
        return que.empty();
    }
    size_t size()
    {
        return que.size();
    }
    ~k_queue()
    {}
};
