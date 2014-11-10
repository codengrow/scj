#include <functional>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class AddressBook
{

public:
    template<typename Func>
    vector<string> findAddr (vector<string> _addresses, Func func)
    { 
        vector<string> results;
        for ( auto itr = _addresses.begin(), end = _addresses.end(); itr != end; ++itr )
        {
            // call the function passed into findAddr and see if it matches
            if ( func( *itr ) )
            {
                results.push_back( *itr );
            }
        }
        return results;
    }

private:
    vector<string> _addresses;

};

class Comparator {

public:
    bool operator()(const string & str) {
        if (str.find("@") != string::npos)
            return true;
        else
            return false;
   }
};

bool Compare(const string & str) {
    if (str.find("@") != string::npos)
        return true;
    else
        return false;
}


int main() {
    
    int n;

    cout << "Email address lookup program using:\n";
    cout << "1. Function pointer\n2. Function object\n";
    cout << "3. Lambda function\nPress a number>";
    cin >> n;

    vector<string> addresses = {"Arash Shafiei", 
				"a.shafiei@tue.nl", 
				"arash.shafiei@gmail.com", 
				"arash.shafiei@alumni.enseeiht.fr"}; 
    AddressBook adrbook;
    vector<string> results;

    switch(n) {
    case 1:
        // First solution with function pointers
        results = adrbook.findAddr(addresses, &Compare);
        // or
        //results = adrbook.findAddr<bool (const string &)>(addresses, Compare);
        break;
 
    case 2:
        // Second solution with functor
        Comparator comp;    
        results = adrbook.findAddr(addresses, comp);
        // or 
        //results = adrbook.findAddr<Comparator>(addresses, comp);
        break;

    case 3:
        // Third solution with lambda function
        results = adrbook.findAddr(addresses, 
				   [](const string & str) {return str.find("@") != string::npos;});
        // or 
        //results = adrbook.findAddr<bool (const string &)>(addresses, [](const string & str) {return str.find("@") != string::npos;});
        // or       
        //results = adrbook.findAddr<function<bool (const string &)>>(addresses, [](const string & str) {return str.find("@") != string::npos;});
       
        break;

    default:
        cout << "Sorry the number is invalid.\n";
    
    }

    for_each(results.begin(), results.end(), [](string val){cout << val << endl;});
}

