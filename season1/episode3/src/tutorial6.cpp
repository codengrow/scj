#include <cstddef>
#include <future>
#include <vector>
#include <iostream>

using namespace std;

template<typename Iterator,typename Func>
void parallel_for_each(Iterator first,Iterator last,Func f)
{
    ptrdiff_t const range_length=last-first;
    if(!range_length)
        return;
    if(range_length==1)
    {
        f(*first);
        return;
    }

    Iterator const mid=first+(range_length/2);

    future<void> bgtask = async(&parallel_for_each<Iterator,Func>,
                                        first,mid,f);
    try
    {
        parallel_for_each(mid,last,f);
    }
    catch(...)
    {
        bgtask.wait();
        throw;
    }
    bgtask.get();   
}

int main() {
    vector<int> vec {1,2,3,4,5};
    parallel_for_each(vec.begin(), vec.end(), [](int i){cout << i << " "; });
    cout << endl;
}
