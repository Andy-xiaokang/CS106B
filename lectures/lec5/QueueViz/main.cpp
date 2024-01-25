// Queue Visualization Interface
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This program demonstrates basic queue operations. It uses a vector to (inefficiently)
// implement a queue. See note in main() about the inefficiency of the remove() operation.


#include <iostream>
#include "console.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
using namespace std;

void displayMenu()
{
    // enqueue / enq
    // dequeue / deq
    // isEmpty
    // isFull
    // peek / front
    // size

    cout << endl;
    cout << "Queue operations:" << endl;
    cout << endl;

    cout << "  enqueue <elt>  -- add <elt> to the queue" << endl;
    cout << "  dequeue        -- remove front element from queue" << endl;
    cout << "  peek           -- print queue's front element without removal" << endl;
    cout << "  front          -- alias for peek" << endl;
    cout << "  isEmpty        -- indicates whether the queue is empty" << endl;
    cout << "  isFull         -- indicates whether the queue is full" << endl;
    cout << "  size           -- print number of elements in queue" << endl;
    cout << "  clear          -- clear out the current queue" << endl;
}

// Prints "+----" segment above and below queue element. Returns number of characters printed.
int printSegment(string elt)
{
    cout << "-";
    int n = elt.length();
    for (int i = 0; i < n; i++)
    {
        cout << "-";
    }
    cout << "-+";

    return n + 3;
}

// Prints bar above/below queue elements. Returns number of characters between start of the
// queue's front and back elements (-1 if only one element in queue).
int printTopBottomBar(Vector<string>& v)
{
    int totalWidth = -1;

    cout << "   +";
    for (int i = 0; i < v.size(); i++)
    {
        int segmentLength = printSegment(v[i]);
        if (i != v.size() - 1)
        {
            totalWidth += segmentLength;
        }
    }
    cout << endl;

    return totalWidth;
}

void printElements(Vector<string>& v)
{
    cout << "   |";
    for (string elt : v)
    {
        cout << " " << elt << " |";
    }
    cout << endl;
}

void printQueue(Vector<string>& v)
{
    cout << endl;
    if (v.isEmpty())
    {
        cout << "   (empty queue)" << endl;
        return;
    }

    int totalWidth = printTopBottomBar(v);
    printElements(v);
    printTopBottomBar(v);

    cout << "     ^";
    if (v.size() > 1)
    {
        for (int i = 0; i < totalWidth; i++)
        {
            cout << " ";
        }
        cout << "^";
    }
    cout << endl;

    cout << "   front";

    if (v.size() == 1)
    {
        cout << " / ";
    }
    else
    {
        for (int i = 0; i < totalWidth - 2; i++)
        {
            cout << " ";
        }
    }
    cout << "back";
    cout << endl;
}

int main()
{
    string input;
    Vector<string> q;

    displayMenu();

    while (true)
    {
        cout << endl;
        cout << "Enter a command (\"?\" for help, \"exit\" to quit): ";
        input = toLowerCase(getLine());

        if (input == "quit" || input == "exit")
        {
            break;
        }
        else if (startsWith(input, "enq"))
        {
            Vector<string> tokens = stringSplit(input, " ");
            if (tokens.size() != 2)
            {
                cout << endl;
                cout << "   Example Usage: enqueue 25" << endl;
            }
            else
            {
                q.add(tokens[1]);
            }
        }
        else if (input == "dequeue" || input == "deq")
        {
            if (q.size() == 0)
            {
                cout << endl;
                cout << "   ERROR: empty queue; cannot dequeue" << endl;
            }
            else
            {
                // WARNING: This removal operation is inefficient because all
                // elements after index 0 have to scoot over. Can you think of
                // a more efficient way to implement the dequeue operation
                // without making the enqueue operation less efficient?
                cout << endl;
                cout << "   -> Dequeued: " << q.remove(0) << endl;
            }
        }
        else if (input == "peek" || input == "front")
        {
            if (q.size() == 0)
            {
                cout << endl;
                cout << "   ERROR: empty queue; cannot peek" << endl;
            }
            else
            {
                cout << endl;
                cout << "   -> Peek: " << q[0] << endl;
            }
        }
        else if (input == "isempty" || input == "empty")
        {
            cout << endl;
            cout << "   -> isEmpty: " << (q.isEmpty() ? "true" : "false") << endl;
        }
        else if (input == "isfull" || input == "full")
        {
            cout << endl;
            cout << "   -> isFull: false" << endl;  // cheeky ;)
        }
        else if (input == "size")
        {
            cout << endl;
            cout << "   -> size: " << q.size() << endl;
        }
        else if (input == "clear")
        {
            q.clear();

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

        printQueue(q);
    }

    return 0;
}
