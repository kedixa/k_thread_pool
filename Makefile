all: test_que.exe test_pool.exe

test_que.exe: test_que.cpp queue.hpp
	g++ -std=c++11 -pthread -O2 test_que.cpp -o test_que.exe

test_pool.exe: test_pool.cpp queue.hpp thread_pool.hpp
	g++ -std=c++11 -pthread -O2 test_pool.cpp -o test_pool.exe
