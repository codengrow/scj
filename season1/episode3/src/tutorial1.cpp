#include <iostream>
#include <thread>

using namespace std;

void func(int x, int y, int& z) {
    this_thread::sleep_for(chrono::seconds{5});
    cout << "Inside thread " << x << ", " << y << ", " << z << endl;
    x = 100; y = 200; z = 300;
}


int main() {

    int x = 2, y = 12, z = 102;
    thread th {&func,x, ref(y), ref(z)};
    x++; y++; z++;
    cout << "Before join " << x << ", " << y << ", " << z << endl;
    th.join();
    cout << "After join " << x << ", " << y << ", " << z << endl;
    return 0;

}
