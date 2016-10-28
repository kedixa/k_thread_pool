#include "k_thread_pool.hpp"
#include <iostream>
#include <sstream>
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
    for(int i = 0; i < 10; ++i)
    {
        pool.add(f, i);
    }
    pool.wait();
    return 0;
}