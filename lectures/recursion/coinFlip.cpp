#include<iostream>
#include "console.h"

using namespace std;


void coinFlip(string s, int order) {
    if (order == 0) {
        cout << s << endl;
        return;
    }
    coinFlip(s + "正", order - 1);
    coinFlip(s + "反", order - 1);
}

void coinFlip(int n) {
    coinFlip("", n);
}

int main_coinFlip() {
    coinFlip(5);
    return 0;
}
