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
    bool stop_running;
    bool stop_add;
    k_queue<std::function<void()>> task_que;
    std::vector<std::shared_ptr<std::thread>> threads;
    std::once_flag one_flag;
    
    /*
     * Add std::function<void()> to task_que,
     * only called by member functions.
     */
    bool _add(std::function<void()> &&fn)
    {
        if(stop_add) return false;
        return task_que.push(fn);
    }

    /*
     * The function to run tasks.
     */
    void run()
    {
        while(!stop_running && !(stop_add && task_que.empty()))
        {
            auto task = task_que.pop_block();
            if(task) (*task)();
        }
    }

    /*
     * Wait all threads to return.
     */
    void join_all_threads()
    {
        for(auto &t : threads)
            if(t) t->join();
        threads.clear();
    }
public:
    k_thread_pool(size_t thread_num =
        std::thread::hardware_concurrency()) noexcept
    {
        stop_running = false;
        stop_add = false;
        this->thread_num = thread_num;
        for(size_t i = 0; i < this->thread_num; ++i)
            threads.push_back(std::make_shared<std::thread>(
                &k_thread_pool::run, this));
    }

    /*
     * Add task function(args) to task_que,
     * enable only if fn is a function.
     */
    template<class Fn, class... Args>
    typename std::enable_if<std::is_function<Fn>::value, bool>::type
    add(Fn &fn, Args&&... args)
    {
        _add(std::bind(fn, args...));
    }
    template<class Fn, class... Args>
    typename std::enable_if<std::is_function<Fn>::value, bool>::type
    add(Fn &&fn, Args&&... args)
    {
        _add(std::bind(std::forward<Fn>(fn), args...));
    }

    /*
     * Tell all threads do not start new task and stop,
     * unless the current task is not completed.
     * After calling this function, all other functions will not work.
     */
    void stop()
    {
        stop_running = true;
        stop_add = true;
        task_que.stop(); // stop task que to let pop_block return.
        std::call_once(one_flag, [this]{join_all_threads();});
        task_que.clear();
    }

    /*
     * Stop add tasks, work until the task_que is empty.
     * After calling this function, all other functions will not work.
     */
    void wait()
    {
        stop_add = true;
        std::call_once(one_flag, [this]{join_all_threads();});
        stop_running = true;
        task_que.stop();
    }

    ~k_thread_pool()
    {
        stop();
    }
};