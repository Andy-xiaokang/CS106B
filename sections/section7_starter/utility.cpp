#include "utility.h"

Cell* createListFromVector(Vector<int> nums) {
	if (nums.isEmpty()) return nullptr;
	Cell *head = new Cell;
    head->value = nums[0];
	head->next  = nullptr;

	Cell *cur = head;
	for (int i = 1; i < nums.size(); i++) {
		Cell *newCell = new Cell;
        newCell->value = nums[i];
		newCell->next = nullptr;
		cur->next = newCell;
		cur = newCell;
	}	
	return head;
}

void freeList(Cell* list) {
	if (list == nullptr) return;
	freeList(list->next);
	delete list;
}

bool listEqual(Cell *a, Cell *b) {
    while (a != nullptr) {
        if (b == nullptr || a->value != b->value) return false;
        a = a->next;
        b = b->next;
    }
    // make sure lists are the same length
    return a == nullptr && b == nullptr;
}

/* Prints the contents of a linked list, in order. */
void printList(Cell* list) {
    for (Cell* cur = list; cur != nullptr; cur = cur->next) {
        std::cout << cur->value << std::endl;
    }
}
