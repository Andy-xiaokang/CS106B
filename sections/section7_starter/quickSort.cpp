/*
 * CS106B Section Handout Test Harness: Section 7
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Neel Kishnani for 
 * Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */
#include <iostream>
#include <string>
#include "testing/SimpleTest.h"
#include "cell.h"
#include "utility.h"
#include "error.h"
using namespace std;

/*
 * Function: quickSort
 * --------------------
 * Takes in a linked list and sorts it in-place using the 
 * quickSort algorithm.
 */

Cell* concatenate(Cell* one, Cell* two) {
    if (one == nullptr) {
        one = two;
        return one;
    }
    Cell* curr = one;
    while (curr->next != nullptr) {
        curr = curr->next;
    }
    curr->next = two;
    return one;
}

void insertHead(Cell*& head, Cell* Node) {
    Node->next = head;
    head = Node;
}

void partition(Cell*& pivot, Cell*& list, Cell*& smaller, Cell*& bigger) {
    Cell* curr = list;
    while (curr != nullptr) {
        Cell* next = curr->next;
        if (curr->value <= pivot->value) {
            insertHead(smaller, curr);
            // curr = curr->next;  // 这里有一个非常微妙的错误，注意在执行 insertHead 后 curr->next 已经被改变，这里的赋值会产生错误，使 curr = nullptr
                                   // 需要在执行 insertHead 之前保存 curr->next 的值。然后在循环体中遍历 list, 在遍历 list 的过程中改变 list 需要特别小心
        } else {
            insertHead(bigger, curr);
            // curr = curr->next;
        }
        curr = next;
    }
    // while (list != nullptr) {
    //     Cell* next = list->next;
    //     if (list->value <= pivot->value) {
    //         insertHead(smaller, list);
    //     } else {
    //         insertHead(bigger, list);
    //     }
    //     list = next;
    // }
}

void quickSort(Cell*& list) {
    // base case
    if (list == nullptr || list->next == nullptr) {
        return;
    }
    Cell* smaller = nullptr;
    Cell* bigger = nullptr;
    Cell* pivot = list;
    Cell* rest = list->next;
    pivot->next = nullptr;
    partition(pivot, rest, smaller, bigger);
    quickSort(smaller);
    quickSort(bigger);
    pivot = concatenate(pivot, bigger);
    smaller = concatenate(smaller, pivot);
    list = smaller;
}

/* * * * * Provided Tests Below This Point * * * * */
PROVIDED_TEST("Small test for quickSort") {
    Cell* list = createListFromVector({7, 5, 87, 41, 137, 1, 2});
    Cell* sorted = createListFromVector({1, 2, 5, 7, 41, 87, 137});
    quickSort(list);
    printList(list);
    printList(sorted);
    EXPECT(listEqual(list, sorted));
    freeList(list);
    freeList(sorted);
}
