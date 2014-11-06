#include <iostream>
#include <thread>
#include <cstdarg>
#include <vector>
#include <mutex>

using namespace std;

template <typename T>
class container 
{
    mutex lock;
    vector<T> elements;
public:
    void add(T element) 
    {
        lock.lock();
        elements.push_back(element);
        lock.unlock();
    }
    template<typename... Args>
    void add(T first, Args... args)
    {
	add(first);
        add(args...);
    }
 
    void dump()
    {
        lock_guard<mutex> locker(lock);
        for(auto e : elements)
            cout << e << "  ";
        cout << endl;
    }
};
 
void func(container<int>& cont)
{
    cont.add(rand()%10, rand()%10, rand()%10);
}
 
int main()
{
    srand((unsigned int)time(0));
 
    container<int> cont;
 
    std::thread t1(func, ref(cont));
    std::thread t2(func, ref(cont));
    std::thread t3(func, ref(cont));
 
    t1.join();
    t2.join();
    t3.join();
 
    cont.dump();
 
    return 0;
}
