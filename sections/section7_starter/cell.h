#pragma once
#include <string>

struct Cell {
    int value;
    Cell *next;
    Cell(int v = 0) {
        value = v;
        next = nullptr;
    }
};
