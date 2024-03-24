#include <iostream>
#include "console.h"
#include "arraystack.h"
#include "arrayqueue.h"
#include "arrayvector.h"
using namespace std;

int main() {
    // ArrayStack stack;
    // for (int i = 0; i < 50 ; i++) {
    //     stack.push(i);
    // }
    // cout << stack.isEmpty() << endl;
    // cout << stack.peek() << endl;
    // cout << stack.size() << endl;
    // cout << stack.pop() << endl;
    // cout << stack.peek() << endl;
    // cout << stack.size() << endl;

    // ArrayQueue queue;
    // cout << queue.isEmpty() << endl;
    // cout << queue.size() << endl;
    // for (int i = 0; i < 50; i++) {
    //     queue.enqueue(i);
    //     cout << queue.size() << endl;
    // }
    // cout << queue.isEmpty() << endl;
    // cout << queue.peek() << endl;
    // cout << queue.size() << endl;
    // cout << queue.dequeue() << endl;
    // cout << queue.peek() << endl;
    // cout << queue.size() << endl;
    // int size = queue.size();
    // for (int i = 0; i < size; i++) {
    //     cout << queue.dequeue() << endl;
    // }
    // cout << queue.size() << endl;
    // cout << queue.isEmpty() << endl;

    ArrayVector v;
    cout << v.size() << v.isEmpty() << endl;
    for (int i = 0; i < 50; i++) {
        v.add(i);
    }
    cout << v.size() << endl << v.isEmpty() << endl;

    v.remove(0);
    v.insert(9, 90);
    v.set(0, 100);
    for (int i = 0; i < v.size(); i++) {
        cout << v.get(i) << endl;
    }
    // v.insert(50, 50);
    // v.get(50);
    // v.remove(50);
    return 0;
}
