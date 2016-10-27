#include <thread>
#include <iostream>
#include "k_queue.hpp"
using namespace std;
class cc
{
    int a;
    public:
    cc(){}
};
k_queue<cc> que(1);
const int sz = 10;

int main()
{
    thread t1 = thread([&](){
        for(int i = 0; i < sz; i++)
        {
            auto t = que.pop(chrono::milliseconds(0));
            if(t.first) cout<<"thread 1 pop: "<<endl;
            else 
            {
                cout<<"wait"<<endl;
                --i;
            }
        }
    });
    thread t2 = thread([&](){
        for(int i = 0; i < sz; i++)
        {
            auto f=que.push(cc());
            if(f)cout<<"thread 2 push"<<endl;
            else
            {
                cout<<"wait 2"<<endl;
                --i;
            }
        }
    });
    t1.join();
    t2.join();
    return 0;
}