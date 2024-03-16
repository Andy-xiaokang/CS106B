// test pass by reference
#include <iostream>
#include "random.h"
using namespace std;

void passByReference(int& c) {
    cout << "the location of reference is " << &c << endl;
}

int* createRandomInt() {
    int* p = new int(randomInteger(1, 100));
    return p;
}

int main() {
    int a = 0;
    cout << "the location of variable is " << &a << endl;
    passByReference(a);

    for (int i = 0; i < 1000; i++) {
        int* q = createRandomInt();
        cout <<"location: " << q << " value: " << *q << endl;
        delete q;
    }
    return 0;
}
