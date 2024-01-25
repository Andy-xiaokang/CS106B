// Stack Visualization Interface
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This program demonstrates basic stack operations. It uses a vector to
// implement the stack.


#include <iostream>
#include "console.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
using namespace std;

void displayMenu()
{
    cout << endl;
    cout << "Stack operations:" << endl;
    cout << endl;

    cout << "  push <elt>  -- add <elt> to top ofstack" << endl;
    cout << "  pop         -- remove element from top of stack" << endl;
    cout << "  peek        -- print sk's top element without removal" << endl;
    cout << "  top         -- alias for peek" << endl;
    cout << "  isEmpty     -- indicates whether the stack is empty" << endl;
    cout << "  isFull      -- indicates whether the stack is full" << endl;
    cout << "  size        -- get number of elements in stack" << endl;
    cout << "  clear       -- clear out the current stack" << endl;
}

void printChar(char ch, int count)
{
    for (int i = 0; i < count; i++)
    {
        cout << ch;
    }
}

void printBar(int indent, int padding, int fieldWidth)
{
    printChar(' ', indent);
    cout << "+";
    printChar('-', padding * 2 + fieldWidth);
    cout << "+" << endl;
}

void printElement(int indent, int padding, int fieldWidth, bool isTop, string elt)
{
    printChar(' ', indent);
    cout << "|";
    printChar(' ', padding);
    cout << elt;
    printChar(' ', padding + (fieldWidth - elt.length()));
    cout << "|";

    if (isTop)
    {
        cout << " <-- top";
    }

    cout << endl;
}

void printStack(Vector<string>& v, bool peeking)
{
    cout << endl;
    if (v.isEmpty())
    {
        cout << "   (empty stack)" << endl;
        return;
    }

    // Num spaces to indent each cell in the stack.
    int indent = 3;

    // Num spaces to pad with on left and right of element.
    int padding = 2;

    // Field width for individual element (not including padding).
    int fieldWidth = 4;

    for (string s : v)
    {
        if (s.length() > fieldWidth)
        {
            fieldWidth = s.length();
        }
    }

    if (peeking)
    {
        printChar(' ', indent + padding * 2 + fieldWidth);
        cout << " o/  -> peek: " << v[v.size() - 1] << endl;
        printChar(' ', indent + padding * 2 + fieldWidth);
        cout << "/|" << endl;
        printChar(' ', indent + padding * 2 + fieldWidth);
        cout << " |\\" << endl;
    }

    printBar(indent, padding, fieldWidth);
    for (int i = v.size() - 1; i >= 0; i--)
    {
        bool isTop = (i == v.size() - 1);
        printElement(indent, padding, fieldWidth, isTop, v[i]);
        printBar(indent, padding, fieldWidth);
    }
}

int main()
{
    string input;
    Vector<string> myStack;

    displayMenu();

    while (true)
    {
        bool peeking = false;

        cout << endl;
        cout << "Enter a command (\"?\" for help, \"exit\" to quit): ";
        input = toLowerCase(getLine());

        if (input == "quit" || input == "exit")
        {
            break;
        }
        else if (startsWith(input, "push"))
        {
            Vector<string> tokens = stringSplit(input, " ");
            if (tokens.size() != 2)
            {
                cout << endl;
                cout << "   Example Usage: push 25" << endl;
            }
            else
            {
                myStack.add(tokens[1]);
            }
        }
        else if (input == "pop")
        {
            if (myStack.size() == 0)
            {
                cout << endl;
                cout << "   ERROR: empty stack; cannot pop" << endl;
            }
            else
            {
                cout << endl;
                cout << "   -> Popped: " << myStack.remove(myStack.size() - 1) << endl;
            }
        }
        else if (input == "peek" || input == "top")
        {
            if (myStack.size() == 0)
            {
                cout << endl;
                cout << "   ERROR: empty stack; cannot peek" << endl;
            }
            else
            {
                //cout << endl;
                //cout << "   -> Peek: " << myStack[myStack.size() - 1] << endl;
                peeking = true;
            }
        }
        else if (input == "isempty" || input == "empty")
        {
            cout << endl;
            cout << "   -> isEmpty: " << (myStack.isEmpty() ? "true" : "false") << endl;
        }
        else if (input == "isfull" || input == "full")
        {
            cout << endl;
            cout << "   -> isFull: false" << endl;  // cheeky ;)
        }
        else if (input == "size")
        {
            cout << endl;
            cout << "   -> size: " << myStack.size() << endl;
        }
        else if (input == "clear")
        {
            myStack.clear();

            cout << endl;
            cout << "   -> clear: okay" << endl;
        }
        else if (input == "?" || input == "help")
        {
            displayMenu();
        }
        else
        {
            cout << endl;
            cout << "   ERROR: unrecognized command: \"" << input << "\"" << endl;
        }

        printStack(myStack, peeking);
    }

    return 0;
}
