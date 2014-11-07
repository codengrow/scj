#include<iostream>
#include<boost/thread.hpp>

using namespace std;


struct callable
{
	void operator()(){
		std::cout << "Thread " << boost::this_thread::get_id()<<" in a callable struct" << std::endl;
	};
};

boost::thread copies_are_safe()
{
	callable x;
	std::cout << "Thread id = " << boost::this_thread::get_id() << std::endl;
	return boost::thread(x);
}

boost::thread oops()
{
	callable x;
	std::cout << "Thread id = " << boost::this_thread::get_id() << std::endl;
	return boost::thread(x);
//	return boost::thread(boost::ref(x));

}

int find_the_question(int index)
{
	{
		boost::this_thread::disable_interruption di;
		{
			std::cout << "function's parameter" << index;
			std::cout << " Thread id = " << boost::this_thread::get_id() << std::endl;
		}
	}
}

int main()
{
	std::cout << "Native thread id=" << boost::this_thread::get_id() << std::endl;
	boost::thread t(find_the_question,432);
	t.join();
	boost::thread t2 = copies_are_safe();
	t2.join();
	boost::thread t3 = oops();
	t3.join();
}
