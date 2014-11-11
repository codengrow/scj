#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <mutex>

#define SIZE 10

using namespace std;

class Buffer {
protected:
  int size;
  int current_index;
  
public:
  Buffer(int s): size(s), current_index(0) {
  }
  
  /*
  void insert(int b) {
    buf[current_index] = b;
    current_index = (current_index + 1) % size;
  }
  
  int get() {
    return buf[current_index];
  }*/
  
};

class FutureBuffer : public Buffer {
private:
  future<int> * buf;
  
public:
  FutureBuffer(int s): Buffer(s), buf(new future<int>[s]) {}

  future<int> * getFutureBuf() { return buf; }
  
  int get() {
    int res = buf[current_index].get();
    current_index = (current_index + 1) % size;
    return res;
  }
};

class PromiseBuffer : public Buffer {
private:
  promise<int> * buf;
  
public:
  PromiseBuffer(int s): Buffer(s), buf(new promise<int>[s]) {}
  
  void setFuture(FutureBuffer * fb) {
    future<int> * buffuture = fb->getFutureBuf();
    for (int i=0; i < size; i++) {
      buffuture[i] = buf[i].get_future();
    }
  }
  void insert(int b) {
    buf[current_index].set_value(b);
    current_index = (current_index + 1) % size;
  }
};

class Filter {

protected:
  string name;
  int num;
  thread * t;
  mutex* io_lock;
  
  void log() {
    io_lock->lock();
    cout << name << " is processing " << num << endl;
    io_lock->unlock();
  }
  
public:
  Filter(string name): name(name), num(0) {
    
  }
  void start() {
    t = new thread(&Filter::operation, this);
  }
  void wait() {
    t->join();
  }
  void setIOLock(mutex* m) {
    io_lock = m;
  }
  virtual void operation() = 0;
};


class ConsumerFilter: public Filter {
private:
  FutureBuffer * buf;
public:
  ConsumerFilter(string name): Filter(name), buf(new FutureBuffer(SIZE)) {}
  FutureBuffer * getFutureBuf() {return buf;}
  void operation() {
    while(1) {
      //this_thread::sleep_for(chrono::milliseconds{rand()%1000});
      num = buf->get();
      log(); 
    }
  }
};

class ProducerFilter: public Filter {
private:
  PromiseBuffer * buf;
public:
  ProducerFilter(string name): Filter(name), buf(new PromiseBuffer(SIZE)) {}
  void addNextConsumer(ConsumerFilter& cf) {
    buf->setFuture(cf.getFutureBuf());
  }
  void operation() {
    while(1) {
      this_thread::sleep_for(chrono::milliseconds{rand()%1000});
      log();
      buf->insert(num);
      num++;
    }
  }
};

class Pipeline {

private:
  mutex io_lock;
    
public:
  void addFilter(Filter& f) {
    f.setIOLock(&io_lock); 
  }
  void connectFilter(ProducerFilter& f1, ConsumerFilter& f2) {
    //f1.addNext(f2);
    f1.addNextConsumer(f2);
    //f2.setIOLock(&io_lock);
  }
};

int main(int argc, char **argv) {
  
  std::cout << "Hello, world!" << std::endl;
  
  Pipeline pipe;
  ProducerFilter pro("producer");
  ConsumerFilter con1("consumer 1");
  ConsumerFilter con2("consumer 2");
  
  pipe.addFilter(pro);
  pipe.addFilter(con1);
  pipe.addFilter(con2);
 
  pipe.connectFilter(pro, con1);
  pipe.connectFilter(pro, con2);
  
  
  pro.start();
  con1.start();
  con2.start();
  
  pro.wait();
  con1.wait();
  con2.wait();
  
  return 0;
}