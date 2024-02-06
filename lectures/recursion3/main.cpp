/*
 * File: main.cpp
 * --------------
 * Blank C++ project configured to use Stanford cslib and Qt
 */

#include "console.h"
#include "simpio.h"
using namespace std;


int main()
{
    string name = getLine("What is your name?");
    cout << "Hello, " << name << endl;
    return 0;
}
