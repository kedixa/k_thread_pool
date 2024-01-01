#include "thread_pool.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cassert>
using namespace std;
using kedixa::thread_pool;
const int sz = 100000;

void func(int x)
{
    ostringstream oss;
    oss<<this_thread::get_id()<<" "<<x<<endl;
    cout<<oss.str();
}

void test_work()
{
    thread_pool pool(3);
    cout << "test work:\n";
    thread th1 = thread([&](){
        for(int i = 0; i < 8; i+=2)
            while(!pool.add(func, i));
    });
    thread th2 = thread([&](){
        for(int i = 1; i < 8; i+=2)
            while(!pool.add(func, i));
    });
    th1.join();
    th2.join();
    pool.wait();
    cout << endl;
}
void test_wait_stop()
{
    thread_pool pool1(3);
    cout << "test wait:\n";
    for(int i = 0; i < 8; i++)
        pool1.add(func, i);
    pool1.wait();

    cout << "test stop:\n";
    thread_pool pool2(3);
    for(int i = 0; i < 8; i++)
        pool2.add(func, i);
    for(int i = 0; i < 1000000; i++);
    pool2.stop();
    cout << endl;
}

void vf(){}
void test_functions()
{
    thread_pool pool(3);
    cout << "test functions:\n";
    pool.add([](){}); // lambda
    pool.add(vf); // function
    pool.add(&vf); // function pointer
    pool.add(negate<int>(), -1); // function object
    cout << "passed without error.\n";
    cout << endl;
}

void test_ref()
{
    cout << "test reference:\n";
    int x = 0;
    auto f = [](int &y){y++;};
    thread_pool pool1(1);
    pool1.add(f, x);
    pool1.wait();
    assert(x == 0);

    thread_pool pool2(1);
    pool2.add(f, ref(x));
    pool2.wait();
    assert(x == 1);
    cout << "passed without error.\n";
    cout << endl;
}

void test_efficiency()
{
    cout << "test efficiency:\n";
    using chrono::system_clock;
    auto f = [](){};
    thread_pool pool1;
    auto start = system_clock::now();
    for(int i = 0; i < sz; ++i)
        pool1.add(f);
    pool1.wait();
    auto end = system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << milliseconds.count() << " milliseconds used for run " << sz <<
        " empty functions.\n";
    cout << endl;
}

void test_destroy()
{
    thread_pool *pool1 = new thread_pool();
    cout << "add destroy function" << endl;
    // you can stop but not wait for yourself
    pool1->add([pool1] { pool1->stop(); delete pool1; });
    cout << "after add destroy function" << endl;
}

int main()
{
    test_work();
    test_wait_stop();
    test_functions();
    test_ref();
    test_destroy();
    test_efficiency();
    return 0;
}
