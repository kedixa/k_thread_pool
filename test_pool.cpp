#include "k_thread_pool.hpp"
#include <iostream>
#include <sstream>
#include <thread>
using namespace std;

void f(int x)
{
    ostringstream oss;
    oss<<this_thread::get_id()<<" "<<x<<endl;
    cout<<oss.str();
}
int main()
{
    k_thread_pool pool(3);
    thread th1 = thread([&](){
        for(int i = 0; i < 10; i++)
            pool.add(f, i);
    });
    thread th2 = thread([&](){
        for(int i = 0; i < 10; i++)
            pool.add(f, 30 - i);
    });
    th1.join();
    th2.join();
    pool.wait();
    return 0;
}