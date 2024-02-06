#include <iostream>
#include "console.h"
#include "strlib.h"
#include "vector.h"

using namespace std;

void fancyPrint(string v) {
    cout << "{";
    for (int i = 0; i < v.size(); i++) {
        cout << v[i];
        if (i != v.size() - 1) {
            cout << ',';
        }
    }
    cout << "}" << endl;
}

int printSubset(string sofar, string rest) {
    if (rest.empty()) {
        fancyPrint(sofar);
        return 1;
    }
    char choose = rest[0];
    string newRest = rest.substr(1);
    return printSubset(sofar, newRest) + printSubset(sofar + choose, newRest);
}

int printSubset(string s) {
    return printSubset("", s);
}

int main_printSubset() {
    cout << printSubset("abc") << endl;
    return 0;
}
