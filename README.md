# k_thread_pool

## queue
1. queue(size_t max_size)  
构造最大容量为max_size的队列。

2. std::shared_ptr<T> pop(std::chrono::milliseconds milli)  
从队列中取出一个元素，最多等待milli毫秒，取出失败则返回std::shared_ptr<T>(nullptr)。

3. std::shared_ptr<T> pop_block()  
从队列中取出一个元素，若队列为空则等待。

4. bool push(T, std::chrono::milliseconds)  
向队列添加一个元素并返回真，若添加失败则返回假。

5. void stop()  
停止队列，若有push/pop在等待，则返回；调用函数后，队列不再可用。

6. void wait()  
等待队列，不可push，允许pop；调用函数后，当队列为空后不再可用。

7. bool empty()  
返回队列是否为空。

8. bool full()  
返回队列是否为满。

9. void clear()  
清理占用的内存，调用后队列不再可用。

10. size_t size()  
返回当前队列的大小。

11. ~queue()  
析构。


## thread_pool
1. thread_pool(size_t thread_num)  
构造线程数量为thread_num的线程池。

2. bool add(Fn, Args)  
向线程池添加函数，其中Fn为可调用对象，Args为参数。

3. void stop()  
当所有线程完成当前任务后，不再执行新的任务，并退出。

4. void wait()  
不再允许添加任务，当已经添加的任务执行完后退出。

5. ~thread_pool()  
析构线程池。

## 编译 运行
 - gcc版本在4.9以上
 - 执行make生成测试程序
 - 使用Mingw GCC时运行效率较低
 - visual studio未测试
