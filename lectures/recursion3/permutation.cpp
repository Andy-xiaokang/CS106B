#include <iostream>
#include "console.h"

using namespace std;

void permutation(string s, string rest) {
    if (rest == "") {
        cout << s << endl;
        return;
    }
    for (int i = 0; i < rest.length(); i++) {
        string newRest = rest.substr(0,i) + rest.substr(i+1);
        permutation(s + rest[i], newRest);
    }
}

void permutation(string s) {
    permutation("", s);
}

int main_permutation() {
    string s = "cat";
    permutation(s);
    return 0;
}
