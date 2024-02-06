#include <iostream>
#include "console.h"
#include "vector.h"
using namespace std;

void printSubsets(Vector<char>& soFar,  Vector<char>& v, int index) {
    if (index == v.size()) {
        cout << soFar << endl;
        return ;
    }
    // choose
    char choose = v[index];
    // explore
    printSubsets(soFar, v, index + 1);
    soFar.add(choose);
    printSubsets(soFar, v, index + 1);
    // unchoose
    soFar.remove(soFar.size() - 1);
}

void printSubsets(Vector<char>& v)
{
   Vector<char> soFar;
   printSubsets(soFar, v, 0);
}

int main()
{
   Vector<char> v = {'a', 'b', 'c'};
   printSubsets(v);
   return 0;
}
