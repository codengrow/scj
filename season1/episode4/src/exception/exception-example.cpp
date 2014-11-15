#include<iostream>
#include<exception>
#include<string>
#include<stdexcept>

using namespace std;

class myexcept : public exception
{
  public:
    virtual const char* what() const throw()
    {
      return "Exception description";
    }

    void where()
    {
    }
} e;

int main()
{
  try
  {
    throw(1);
  }catch(int a)
  {
    cout << "1. Catch an integer: " << a << endl;
  }

  try
  {
    throw("abc");
  }catch(const char* str)
  {
    cout << "2. Catch a string: " << str << endl;
  }

  try
  {
    throw(e);
  }catch(...)
  {
    const char* message = e.what(); //message can not be changed
    cout << "3. Catch an exception with message: " << e.what() << endl;
  }

  try
  {
    e.where();
  }catch(myexcept e)
  {
    cout << "4. Catch a method inside a class: " << endl;
  }
  
}
