#include <iostream>
#include "console.h"

using namespace std;


void swap(string& s, int i, int j) {
    char tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
}

void permutation(string& s, int k) {
    if (k == s.length() - 1) {
        cout << s << endl;
        return;
    }
    for (int i = k; i < s.length(); i++) {
        swap(s, i, k);
        permutation(s, k + 1);
        swap(s, i, k);
    }
}

void permutation(string& s) {
    permutation(s, 0);
}

int main() {
    string s = "cat";
    permutation(s);
    return 0;
}


