/**
* eat() is called in the intermediate function
* If do not use virtual function in Animal, 
* you need to overload func(Animal *animal) function.
* i.e. void func(Cat *cat){ cat->eat();}
* Thing goes complicated when you have many subclasses of Animal.
* 
* "late binding"  vs. "early binding" 
* with "virtual", we have "late binding"
* without "virtual", we have "early binding"
*/

#include<iostream>

using namespace std;
class Animal
{
    public:
        virtual void eat()
	{
	    cout << "I am eating generic food.\n";
	}
};

class Cat : public Animal
{
    public:
        void eat()
        {
	    cout << "I am eating a rat.\n";
        }
};

/* Intermediate function */
void func(Animal *animal)
{
    animal->eat();
}

int main()
{
    Animal *animal = new Animal;
    Cat *cat = new Cat;

    func(animal);
    func(cat); 
}
