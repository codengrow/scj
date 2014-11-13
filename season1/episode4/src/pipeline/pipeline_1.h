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

#ifndef PIPELINE_1_H
#define PIPELINE_1_H

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
  int speed;
  
  void log() {
    io_lock->lock();
    cout << name << " is processing " << num << endl;
    io_lock->unlock();
  }
  
public:
  Filter(string name): name(name), num(0), speed(1000) {
    
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
private:
  FutureBuffer * buf;
public:
  ConsumerFilter(string name): Filter(name), buf(new FutureBuffer(SIZE)) {}
  FutureBuffer * getFutureBuf() {return buf;}
  void operation() {
    while(1) {
      sleep();
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
      sleep();
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
    f1.addNextConsumer(f2);
  }
};

#endif // PIPELINE_1_H
