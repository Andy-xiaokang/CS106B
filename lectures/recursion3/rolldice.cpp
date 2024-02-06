#include <iostream>
#include "vector.h"
#include "console.h"
#include "strlib.h"

using namespace std;

void rolldice(string s, int n, Vector<string>& v) {
    if (n == 0) {
        v.add(s);
        return;
    }
    for (int i = 1; i <= 6; i++) {
        rolldice(s + integerToString(i), n - 1, v);
    }
}

void rolldice(int n) {
    Vector<string> results;
    rolldice("", n, results);
    for (string result : results) {
        cout << result << endl;
    }
    cout << results.size() << endl;
}

int main() {
    rolldice(4);
    return 0;
}
