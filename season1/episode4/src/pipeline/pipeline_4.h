#ifndef PIPELINE_2_H
#define PIPELINE_2_H

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <vector>
#include <mutex>

const int SIZE = 2;

using namespace std;

template <typename T>
class Node {
 
  T val;
  mutex mux;
  
  condition_variable con_cond;
  unsigned int con_num;
  
  condition_variable pro_cond;
  bool prod;
  
  unsigned int consumed;
  bool produced;
  unsigned int total_consumer;
  
public:
  
  Node(): con_num(0), prod(false), consumed(0), produced(false), total_consumer(0) {}
  
  void addConsumer() {
    total_consumer++;
  }
  void consumerLock() {
    unique_lock<mutex> locker(mux);
    while(prod || !produced)
      pro_cond.wait(locker);
    con_num++;
    consumed++;
    
    if(consumed == total_consumer) {
      consumed = 0;
      produced = false;
    }
  }
  
    
  void consumerUnlock() {
    lock_guard<mutex> locker(mux);
    con_num--;
    
    con_cond.notify_one();
  }
  
  void producerLock() {
    unique_lock<mutex> locker(mux);
    while(con_num > 0 || produced)
      con_cond.wait(locker);
    produced = true;
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
  
  void addConsumer() {
    for (int i=0; i<size; i++) {
      nodes[i].addConsumer();
    }
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

  void log(const string& msg) const {
    io_lock->lock();
    cout << name << " " <<msg << " " << num << endl;
    io_lock->unlock();
  }
  
public:
  Filter(const string& name): name(name), num(0) {
 
  }
  void start() {
    t = new thread(&Filter::operation, this);
  }
  void startRT() {
    t = new thread(&Filter::operationRT, this);
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
  virtual void operationRT() {
    operation();
  };
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
      log("is consuming");
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
    buf->addConsumer();
  }
  
  void operation() {
    
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      buf->at(idx).producerLock();
      buf->at(idx).write(num);
      log("is producing");
      sleep();
      buf->at(idx).producerUnlock();
      idx = (idx + 1) % size;
      num++;
    }
  }
  
  void operationRT() {
    
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      bool canlock = buf->at(idx).producerRTLock();
      if (canlock) {
	buf->at(idx).write(num);
	log("is producing");
	sleep();
	buf->at(idx).producerUnlock();
	idx = (idx + 1) % size;
      }
      else {
	log("is droping");
	sleep();
      }
      num++;
    }
  }
};

class Pipeline {

private:
  bool realtime;
  mutex io_lock;
  vector<Filter *> filters;
    
public:
  
  Pipeline(): realtime(false) {}
  
  void addFilter(Filter * f) {
    f->setIOLock(&io_lock);
    filters.push_back(f);
  }
  void connectFilter(ProducerFilter& f1, ConsumerFilter& f2) {
    f1.addNextConsumer(f2);
  }
  
  void setRealTime(bool rt) {
    realtime = rt;
  }
  
  void start() {
    for (auto f : filters) {
      realtime? f->startRT() : f->start();
    }
  }
  
  void wait() {
  
    for (auto f : filters) {
      f->wait();
    }
  }
};

#endif // PIPELINE_2_H
