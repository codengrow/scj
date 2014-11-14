/*
 * Copyright 2014 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#ifndef PIPELINE_2_H
#define PIPELINE_2_H

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <vector>
#include <mutex>
#include "semaphore.h"

#define SIZE 10

using namespace std;

template <typename T>
class Node {
 
  T val;
  mutex con_mux;
  condition_variable con_cond;
  unsigned long con_num;
  
  mutex pro_mux;
  condition_variable pro_cond;
  
public:
  
  Node(): con_num(0) {}
  
  void consumerLock() {
    unique_lock<mutex> pro_locker(pro_mux);
    pro_cond.wait(pro_locker);
   
    lock_guard<mutex> con_locker(con_mux);
    con_num++;
  }
  
  void consumerUnlock() {
    lock_guard<mutex> con_locker(con_mux);
    con_num--;
    con_cond.notify_one();
  }
  
  void producerLock() {
    unique_lock<mutex> con_locker(con_mux);
    while(con_num > 0)
      con_cond.wait(con_locker);
  }
  
  void producerUnlock() {
    lock_guard<mutex> pro_locker(pro_mux);
    pro_cond.notify_all();
  }
  
  void write(T val) {
    this->val = val; 
  }
  
  T read() {
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
  
  int getSize() {return size;}
  Node<T>& getNode(int idx) const { return nodes[idx]; }
};


class Filter {
 
protected:
  string name;
  int num;
  thread * t;
  mutex* io_lock;
  int speed;
  Buffer<int> * buf;

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
  
  void setSpeed(int ms) { speed = ms; }
  
  void sleep() {
    this_thread::sleep_for(chrono::milliseconds{rand()%speed});
  }
  
  virtual void operation() = 0;
};


class ConsumerFilter: public Filter {
public:
  ConsumerFilter(string name): Filter(name) {
    buf = nullptr;
  }
  
  void setBuf(Buffer<int> * b) { buf = b; }
  
  void operation() {
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      buf->getNode(idx).consumerLock();
      num = buf->getNode(idx).read();
      log();
      sleep();
      buf->getNode(idx).consumerUnlock();
      idx = (idx + 1) % size;
    }
  }
};

class ProducerFilter: public Filter {
public:
  ProducerFilter(string name): Filter(name) {
    buf = new Buffer<int>(SIZE);
  }
  
  void addNextConsumer(ConsumerFilter& cf) {
    cf.setBuf(buf);
  }
  
  void operation() {
    
    int size = buf->getSize();
    
    int idx = 0;
    while(1) {
      buf->getNode(idx).producerLock();
      buf->getNode(idx).write(num);
      log();
      sleep();
      buf->getNode(idx).producerUnlock();
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
