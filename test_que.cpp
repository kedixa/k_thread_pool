#include <thread>
#include <iostream>
#include "queue.hpp"
using namespace std;
using k_queue = kedixa::queue<int>;

k_queue que(30);
const int sz = 2000000;

int main()
{
    using chrono::system_clock;
    auto start = system_clock::now();
    thread t1 = thread([](){
        for(int i = 0; i < sz; i++)
        {
            decltype(que.pop()) t;
            do
            {
                t = que.pop();
            } while(!t);
        }
    });
    thread t2 = thread([](){
        for(int i = 0; i < sz; i++)
            while(!que.push(i));
    });
    t1.join();
    t2.join();
    auto end = system_clock::now();
    auto seconds = chrono::duration_cast<chrono::seconds>(end - start);
    cout << seconds.count() << " seconds used for push and pop " <<
        sz << " integers." <<endl;
    return 0;
}
