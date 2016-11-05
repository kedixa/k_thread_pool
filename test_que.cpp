#include <thread>
#include <iostream>
#include <vector>
#include <cassert>
#include "queue.hpp"

using namespace std;
template<typename T>
using k_queue = kedixa::queue<T>;
using chrono::system_clock;
const int sz = 1000000;

class test_class
{
    vector<int> v;
public:
    test_class()
    {
        cout<<"default construct.\n";
    }
    test_class(const test_class &tc)
    {
        v = tc.v;
        cout << "copy construct.\n";
    }
    test_class(test_class &&tc)
    {
        v = move(tc.v);
        cout << "move construct.\n";
    }
    test_class& operator=(const test_class &tc)
    {
        v = tc.v;
        cout << "copy.\n";
        return *this;
    }
    test_class& operator=(test_class &&tc)
    {
        v = move(tc.v);
        cout << "move copy.\n";
        return *this;
    }
    ~test_class()
    {
        cout << "destruct.\n";
    }
};

class test_cap
{
    int array[256];
public:
    test_cap(){}
    ~test_cap(){}
};

void test_push_pop()
{
    k_queue<int> que;
    auto start = system_clock::now();
    for(int i = 0; i < sz; i++)
        que.push(i);
    for(int i = 0; i < sz; i++)
        que.pop();
    auto end = system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << milliseconds.count() << " milliseconds used for push and pop " <<
        sz << " integers." <<endl;
    cout << endl;
}

void test_push_pop_thread()
{
    k_queue<int> que;
    auto start = system_clock::now();
    thread t1 = thread([&](){
        for(int i = 0; i < sz; i++)
            que.pop_block();
    });
    thread t2 = thread([&](){
        for(int i = 0; i < sz; i++)
            while(!que.push(i, chrono::milliseconds(10)));
    });
    t1.join();
    t2.join();
    auto end = system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << milliseconds.count() << " milliseconds used for push and pop " <<
        sz << " integers in 2 threads." <<endl;
    cout << endl;
}
void test_push_thread()
{
    k_queue<int> que;
    auto start = system_clock::now();
    thread t1 = thread([&](){
        for(int i = 0; i < sz; i++)
            while(!que.push(i, chrono::milliseconds(10)));
    });
    thread t2 = thread([&](){
        for(int i = 0; i < sz; i++)
            while(!que.push(i, chrono::milliseconds(10)));
    });
    t1.join();
    t2.join();
    auto end = system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << milliseconds.count() << " milliseconds used for push " <<
        sz << " * 2 integers in 2 threads." <<endl;
    cout << endl;
}

void test_move()
{
    k_queue<test_class> que(1);
    cout << "test_construct:\n--- construct ---\n";
    test_class tc;
    cout << "--- push ---\n";
    que.push(move(tc));
    assert(que.full());
    cout << "--- pop ---\n";
    que.pop();
    assert(que.empty());
    cout << "--- destruct ---\n";
}

void test_big_cap()
{
    cout << endl;
    k_queue<test_cap> que;
    test_cap tc;
    auto start = system_clock::now();
    for(int i = 0; i < sz; i++)
        que.push(tc);
    assert(que.size() == sz);
    while(!que.empty()) que.pop();
    assert(que.empty());
    auto end = system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << milliseconds.count() << " milliseconds used for push and pop " <<
        sz << " test_cap." <<endl;
    cout << endl;
}
int main()
{
    test_push_thread();
    test_push_pop();
    test_push_pop_thread();
    test_move();
    test_big_cap();
    return 0;
}
