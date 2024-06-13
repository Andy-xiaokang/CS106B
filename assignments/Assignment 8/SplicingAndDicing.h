#pragma once
#include "Demos/NucleotideAlloc.h"

/**
 * Type representing a nucleotide. Please do not make any changes to this
 * struct in the course of solving this problem; then again, you shouldn't
 * find yourself needing to do this, either.
 */
struct Nucleotide {
    char value;
    Nucleotide* next;
    Nucleotide* prev;

    /* This custom macro assists with memory leak detection. */
    TRACK_ALLOCATIONS_OF(Nucleotide);
};

