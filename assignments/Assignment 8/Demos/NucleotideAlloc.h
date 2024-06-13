#pragma once
#include "../GUI/MemoryDiagnostics.h"

/* Faster allocation of the Nucleotide type; used to speed up performance on
 * Windows. You shouldn't need to investigate how this works.
 */
namespace NucleotideAlloc {
    void* alloc(std::size_t numBytes);
    void  free(void* memory);

    int instances();
}

struct Nucleotide;
namespace MemoryDiagnostics {
    template <> struct Allocator<Nucleotide> {
        static void* scalarAlloc(std::size_t numBytes) {
            return NucleotideAlloc::alloc(numBytes);
        }
        static void* vectorAlloc(std::size_t numBytes) {
            return ::operator new[](numBytes);
        }
        static void scalarFree(void* memory) {
            NucleotideAlloc::free(memory);
        }
        static void vectorFree(void* memory) {
            ::operator delete[](memory);
        }
    };
}
