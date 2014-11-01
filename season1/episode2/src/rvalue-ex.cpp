#include <iostream>
#include <string>
using namespace std;
class Foo {
public:
	Foo() {a = 5; i = 1;}
	int& foo() {i = i+1; return i;}
	int foobar() {return i;}
	void print() {cout << "i=" <<i<<", a= "<<a<<endl;}
private:
	int a;
	int i;
};
int main() {

	Foo f;
	f.print();
	f.foo() = 4;

	int * p = &f.foo();
	cout << "p=" << p << endl;
	*p = 50;
	
	f.print();
	
	//int (*Foo::fp)();
	//fp = f.foobar;
	//int *p2 = &f.foobar(); 
	return 0;

}
