#pragma once

#include "vector.h"
#include "queue.h"

void printC(int n);
void printH(int n);
void printI(int n);
void printP(int n);
void printChip(int n);

int countTriples(const Vector<int>& values, int target);

void printCycle_v1(Vector<int>& values);
void printCycle_v2(Vector<int>& values);
void printCycle_v3(Queue<int>& values);

int recursivePuzzle(int n);
int recursiveEnigma(int n);

int maximumSingleSellProfit_v1(const Vector<int>& values);
int maximumSingleSellProfit_v2(const Vector<int>& values);
