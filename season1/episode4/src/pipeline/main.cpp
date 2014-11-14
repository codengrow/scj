#include <iostream>
#include <string>
#include "pipeline_2.h"

using namespace std;

int main(int argc, char **argv) {
  
  std::cout << "Hello, world!" << std::endl;
  
  Pipeline pipe;
  ProducerFilter pro("producer");
  ConsumerFilter con1("consumer 1");
  ConsumerFilter con2("consumer 2");
  
  pro.setSpeed(1000);
  con1.setSpeed(1000);
  con2.setSpeed(1000);
  pipe.addFilter(&pro);
  pipe.addFilter(&con1);
  pipe.addFilter(&con2);
 
  pipe.connectFilter(pro, con1);
  pipe.connectFilter(pro, con2);
  
  pipe.setRealTime(true);
  
  pipe.start();
  pipe.wait();
  
  return 0;
}