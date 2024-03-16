#include <iostream>
#include "console.h"
using namespace std;

int main_helloworld() {
    string s = "hello world";
    int a = 1;
    for (char c : s) {
        cout << c << endl;
    }
    cout << "hello world!" << endl;
    return 0;
}
