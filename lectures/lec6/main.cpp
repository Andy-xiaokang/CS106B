#include <iostream>
#include "console.h"
#include "map.h"
#include "vector.h"
using namespace std;

int main()
{
    Map<string, Vector<string>> map;

    map["Julie"].add("Zelenski");
    map["Julie"].add("Stanford");
    map["Chris"].add("Gregg");
    map["Chris"].add("Piech");
    map["Nick"].add("Troccoli");
    map["Nick"].add("Parlante");

    for (string s : map.keys()) {
        Vector<string> names = map[s];
        for (string name: names) {
            cout << s << " " << name << endl;
        }
    }

    return 0;
}


