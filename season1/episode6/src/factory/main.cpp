#include <iostream>

#include <map>
#include <functional>
#include <string>

#include "Base.h"

using namespace std;

int main(int argc, char **argv) {
    std::cout << "Hello, world!" << std::endl;
    
    Base* b = BaseFactory::createInstance("DerivedA");
    b->print();
    return 0;
}
