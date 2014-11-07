#include <condition_variable>
#include <mutex>
#include <future>
#include <iostream>
#include <chrono>
#include <queue>

const int size = 10; 
std::mutex mutex;
std::condition_variable condvar;

struct Data {
    int d;
    int error;
};
 
std::queue<Data> messageQ;
 
void Producer()
{
    int i = 0;
    while(i<=size) {
        int d = rand()%10;
        int error = (i==size)?1:0;
        Data data {d, error};
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%500));
        std::lock_guard<std::mutex> guard(mutex);
        std::cout << "Producing message: " << data.d << std::endl;
        messageQ.push(data);
        condvar.notify_one();
        i++;
    }
 
}
 
void Consumer()
{
    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%500));
        std::unique_lock<std::mutex> ulock(mutex);
        condvar.wait(ulock, [] {return !messageQ.empty(); });
       
        Data data = messageQ.front();
        std::cout << "Consuming message: " << data.d  << std::endl;
        messageQ.pop();
        if (data.error)
            break;
    }
}
 
 
int main()
{
   std::thread t1 {Producer};
 
   std::thread t2 {Consumer};
 
   t1.join();
 
   t2.join(); 
}
