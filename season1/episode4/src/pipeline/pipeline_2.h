#ifndef PIPELINE_2_H
#define PIPELINE_2_H

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <vector>
#include <mutex>

const int SIZE = 10;

using namespace std;

template <typename T>
class Node {
 
  T val;
  mutex mux;
  
  condition_variable con_cond;
  unsigned int con_num;
  
  condition_variable pro_cond;
  bool prod;
  
public:
  
  Node(): con_num(0), prod(false) {}
  
  void consumerLock() {
    unique_lock<mutex> locker(mux);
    while(prod)
      pro_cond.wait(locker);
    
    con_num++;
  }
  
    
  void consumerUnlock() {
    lock_guard<mutex> locker(mux);
    con_num--;
    con_cond.notify_one();
  }
  
  void producerLock() {
    unique_lock<mutex> locker(mux);
    while(con_num > 0)
      con_cond.wait(locker);
    
    prod = true;
  }
  
  bool producerRTLock() {
    lock_guard<mutex> locker(mux);
    if(con_num > 0)
      return false;
    
    prod = true;
    return true;
  }
  
  void producerUnlock() {
    lock_guard<mutex> locker(mux);
    prod = false;
    pro_cond.notify_all();
  }
  
  void write(T val) {
    this->val = val; 
  }
  
  T read() const {
    return val;
  }
};

template<typename T>
class Buffer {
protected:
  Node<T> * nodes;
  int size;
  
public:
  Buffer(int s): size(s), nodes(new Node<T>[s]())  {
  }
  
  int getSize() const {return size;}
  
  Node<T>& at(int idx) const  { return nodes[idx]; }
};


class Filter {
 
protected:
  string name;
  int num;
  thread * t;
  mutex* io_lock;
  int speed;
  Buffer<int> * buf;

  void log() const {
    io_lock->lock();
    cout << name << " is processing " << num << endl;
    io_lock->unlock();
  }
  
public:
  Filter(const string& name): name(name), num(0) {
 
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
  
  void setSpeed(int ms) { speed = ms; }
  
  void sleep() const {
    this_thread::sleep_for(chrono::milliseconds{rand()%speed});
  }
  
  virtual void operation() = 0;
};


class ConsumerFilter: public Filter {
public:
  ConsumerFilter(const string& name): Filter(name) {
    buf = nullptr;
  }
  
  void setBuf(Buffer<int> * b) { buf = b; }
  
  void operation() {
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      buf->at(idx).consumerLock();
      num = buf->at(idx).read();
      log();
      sleep();
      buf->at(idx).consumerUnlock();
      idx = (idx + 1) % size;
    }
  }
};

class ProducerFilter: public Filter {
public:
  ProducerFilter(const string& name): Filter(name) {
    buf = new Buffer<int>(SIZE);
  }
  
  void addNextConsumer(ConsumerFilter& cf) {
    cf.setBuf(buf);
  }
  
  void operation() {
    
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      buf->at(idx).producerLock();
      buf->at(idx).write(num);
      log();
      sleep();
      buf->at(idx).producerUnlock();
      num++;
      idx = (idx + 1) % size;
    }
  }
};

class Pipeline {

private:
  mutex io_lock;
  vector<Filter *> filters;
    
public:
  void addFilter(Filter * f) {
    f->setIOLock(&io_lock);
    filters.push_back(f);
  }
  void connectFilter(ProducerFilter& f1, ConsumerFilter& f2) {
    f1.addNextConsumer(f2);
  }
  
  void start() {
  
    for (auto f : filters) {
      f->start();
    }
  }
  
  void wait() {
  
    for (auto f : filters) {
      f->wait();
    }
  }
};

#endif // PIPELINE_2_H
