#include<iostream>
#include<boost/thread.hpp>

using namespace std;


struct callable
{
	void operator()(){
		std::cout << "Thread " 
              << boost::this_thread::get_id()
              <<" in a callable struct" 
              << std::endl;
	};
};

boost::thread copies_are_safe()
{
	callable x;
	std::cout << "Thread " 
            << boost::this_thread::get_id() 
            << std::endl;
	return boost::thread(x);
}

boost::thread oops()
{
	callable x;
	std::cout << "Thread " 
            << boost::this_thread::get_id() 
            << std::endl;
	return boost::thread(boost::ref(x));
}

int func(int para)
{
	{
		boost::this_thread::disable_interruption di;
		{
			std::cout << "Thread " 
                << boost::this_thread::get_id() 
                << " with para =" 
                << para 
                << std::endl;
		}
	}
}

int main()
{
  /***** main thread *****/	
  std::cout << "Native thread " << boost::this_thread::get_id() << std::endl;

  /***** launch thread from a function with (unlimited) parameters *****/	
  boost::thread t1( func, 432 );
  t1.join();

  /***** thread as a returned object ****/
  boost::thread t2 = copies_are_safe();
  //std::cout << t2::get_id() << std::endl;
 // t2.detach();
	t2.join();

  /***** thread as a returned object *****/
	boost::thread t3 = oops();
	t3.join();
}
