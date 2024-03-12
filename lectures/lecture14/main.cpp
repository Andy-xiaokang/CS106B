/*
 * File: main.cpp
 * --------------
 * Blank C++ project configured to use Stanford cslib and Qt
 */

#include "console.h"
#include "simpio.h"
#include "quokka.h"
#include "vector.h"
using namespace std;


int main()
{
    Vector<Quokka> v;
    Quokka q1 = Quokka("Alice", 8, 8*10);
    Quokka q2 = Quokka("Bob", 6, 6*10);
    Quokka q3 = Quokka("cyber", 8, 8*10);
    Quokka q4 = Quokka("Dog", 9, 9*10);
    q4.discount(0.75);
    v.add(q1);
    v.add(q2);
    v.add(q3);
    v.add(q4);
    for (Quokka q : v) {
        q.printInfo();
    }
    // q4.setName("xiaokang");
    return 0;
}
