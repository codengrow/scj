#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <exception>
#include <stdexcept>
using namespace std;

mutex g_mutex;
vector<exception_ptr> g_exceptions;

void throw_function() {
   throw exception();
}

void func() {
   try {
      throw_function();
   }
   catch(...) {
      lock_guard<mutex> lock(g_mutex);
      g_exceptions.push_back(current_exception());
   }
}

int main() {
   g_exceptions.clear();
   thread t(func);
   t.join();
   for(auto& e : g_exceptions) {
      try {
         if(e != nullptr) {
            rethrow_exception(e);
         }
      }
      catch(const exception& e) {
         cout << e.what() << std::endl;
      }
   }
   return 0;
}
