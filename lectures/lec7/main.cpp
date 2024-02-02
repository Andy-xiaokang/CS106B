#include <iostream>
#include "console.h"
using namespace std;

void printStringHelper(string& s, int k)
{
   if ( k == s.length() )
   {
      cout << endl;
      return;
   }

   cout << s[ k++ ];
   printStringHelper(s,  k );
}

void printString(string& s)
{
   printStringHelper(s, 0);
}


void printStringReverseHelper(string s, bool originalCall)
{
    if (s.length() == 0)
    {
        return;
    }

    // You can modify the following lines, but do not swap their order. So, the
    // cout statement must come after the printStringReverseHelper() statement.
    // You can, however, add lines before or after the following, or even change
    // how the printStringReverseHelper() function is called.
    printStringReverseHelper(s.substr(1), false);
    cout << s[0];
    if (originalCall) {
        cout << endl;
    }
}

void printStringReverse(string s)
{
    // You can modify the following line, but it must call printStringReverseHelper(),
    // and you cannot add any other lines of code to this function.
    printStringReverseHelper(s, true);
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

bool isPalindrome(string s) {
    if (s.length() <= 1) {
        return true;
    }
    if (s[0] != s[s.length() - 1]) {
        return false;
    } else {
        return isPalindrome(s.substr(1, s.length() - 2));
    }
}

int main()
{
    // Calling twice to ensure proper placement of line break in output.
    string s = "hello";
    printStringReverse(s);
    printStringReverse("hello");
    cout << endl;
    cout << factorial(5) << endl;
    cout << isPalindrome("issi") << endl;
    cout << isPalindrome("hello") << endl;

    // printString(s);

    return 0;
}
