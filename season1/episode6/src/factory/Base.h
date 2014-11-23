
#ifndef BASE_H
#define BASE_H

#include <map>
#include <string>
#include <functional>

using namespace std;

class Base {
public:
  Base() {
  }
  virtual void print()=0;
};


template<typename T>
Base * createT() { return new T; }


struct BaseFactory {
  
  typedef map<string, function<Base * ()>> map_type;
  static Base * createInstance(string const& s) {
    map_type::iterator it = mymap->find(s);
    if(it == mymap->end())
      return 0;
    return it->second();
  }
  
protected:
  static map_type * getMap() {
    // never delete'ed. (exist until program termination)
    // because we can't guarantee correct destruction order 
    if(!mymap) { mymap = new map_type(); } 
    return mymap; 
  }
  
private:
  static map_type * mymap;
  
};


template<typename T>
struct DerivedRegister : BaseFactory { 
  DerivedRegister(string const& s) { 
    getMap()->insert(make_pair(s, &createT<T>));
  }
};

#endif // BASE_H
