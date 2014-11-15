#include<iostream>
#include<exception>
#include<stdexcept>
#include<ctime>

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
	srand(time(0));
	if (rand()%2 == 0)
		throw "Where";
	else
		throw 42;
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
  }catch(const char * str)
  {
    cout << "4. Catch a method inside a class: "  << str << endl;
  }
  catch(int i) {
    cout << "4. Catch an integer: " << i << endl;
  }
  
}
