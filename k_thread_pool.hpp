/*
    Author: kedixa
    E-mail: 1204837541@qq.com
    License: GPL2
*/

#include <thread>
#include "k_queue.hpp"

class k_thread_pool
{
private:
    size_t thread_num; // the number of threads.
    bool stop_flag;
    k_queue<std::function<void()>> task_que;
    std::vector<std::shared_ptr<std::thread>> threads;
    std::once_flag one_flag;
    
    bool _add(std::function<void()> &&fn)
    {
        task_que.push(fn);
    }
    void run()
    {
        while(!stop_flag)
        {
            auto task = task_que.pop();
            if(task) (*task)();
        }
    }
    void join_all_threads()
    {
        stop_flag = true;
        for(auto &t : threads)
            if(t) t->join();
    }
public:
    k_thread_pool(size_t thread_num = std::thread::hardware_concurrency()) noexcept
    {
        stop_flag = false;
        this->thread_num = thread_num;
        for(size_t i = 0; i < this->thread_num; ++i)
            threads.push_back(std::make_shared<std::thread>(&k_thread_pool::run, this));
    }

    template<class Fn, class... Args>
    bool add(Fn &fn, Args&&... args)
    {
        _add(std::bind(fn, args...));
    }
    template<class Fn, class... Args>
    bool add(Fn &&fn, Args&&... args)
    {
        _add(std::bind(std::forward<Fn>(fn), args...));
    }

    void stop()
    {
        std::call_once(one_flag, [this]{join_all_threads();});
    }

    ~k_thread_pool()
    {
        stop();
    }
};