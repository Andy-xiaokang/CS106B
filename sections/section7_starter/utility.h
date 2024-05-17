#pragma once

#include "cell.h"
#include "vector.h"

Cell* createListFromVector(Vector<int> nums);
void freeList(Cell* list);
bool listEqual(Cell* a, Cell *b);
void printList(Cell *list);

