#include "../SplicingAndDicing.h"
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;

namespace {
    /* Allocator magic number, used to internally validate that we didn't
     * do something silly with a pointer (and to make sure that deallocated
     * pointers are actually valid!)
     */
    const uint32_t kMagicHeader = 0xC5106BA7; // CS106B, A7
    const uint32_t kMagicFooter = 0x13710642; // 137, 106, 42

    /* One block of memory, large enough to hold a Nucleotide. */
    struct Block {
        uint32_t header = kMagicHeader;
        bool isFree = true;
        alignas(Nucleotide) char buffer[sizeof(Nucleotide)];
        uint32_t footer = kMagicFooter;
    };

    /* List of all available blocks.
     *
     * TODO: This should probably be replaced with a std::vector rather than raw
     * arrays.
     */
    Block* theQueue;
    Block* theHead;
    const size_t kNumObjects = 1 << 22; // 4M nucleotides, way more than needed.

    /* Number of instances allocated. */
    int theNumInstances = 0;

    /* Set up and tear down the memory blocks. */
    struct MemoryInitializer {
        MemoryInitializer() {
            theQueue = new Block[kNumObjects]();
            theHead  = theQueue;

            /* Fill all blocks with garbage. */
            for (size_t i = 0; i < kNumObjects; i++) {
                fill(begin(theQueue[i].buffer), end(theQueue[i].buffer), 0xBA);
            }
        }

        ~MemoryInitializer() {
            delete[] theQueue;
        }
    } theInitializer;
}

void* NucleotideAlloc::alloc(size_t numBytes) {
    ++theNumInstances;

    /* If we are allocating any number of bytes other than what we expect, something
     * is wrong.
     */
    if (numBytes != sizeof(Nucleotide)) {
        abort();
    }

    /* Walk until we find a free block. */
    Block* orig = theHead;
    do {
        /* If we're free, claim this space. */
        if (theHead->isFree) {
            /* Claim this spot. */
            theHead->isFree = false;
            void* result = theHead->buffer;

            /* Advance to the next free slot. */
            theHead++;
            if (theHead - theQueue == kNumObjects) {
                theHead = theQueue;
            }

            return result;
        }
    } while (theHead != orig);

    /* Oops, failed. */
    return ::operator new(numBytes);
}

void NucleotideAlloc::free(void* memory) {
    --theNumInstances;

    /* If we're within the queue, great! Reclaim this space. */
    if (memory >= theQueue && memory < theQueue + kNumObjects) {
        /* Map this back to a block, and confirm that we aren't misaligned. */
        Block* theBlock = reinterpret_cast<Block*>(static_cast<char*>(memory) - offsetof(Block, buffer));
        if (theBlock->header != kMagicHeader || theBlock->footer != kMagicFooter) {
            abort();
        }

        /* This block should not be free at this point. If it is, this is a double-free. */
        if (theBlock->isFree) {
            cerr << "You have attempted to delete a Nucleotide object that is no longer allocated. "
                    "This may indicate that you are trying to delete the same pointer multiple times, "
                    "which is not allowed. Run your program with the debugger engaged and walk up the "
                    "call stack to identify the code that's deleting the same Nucleotide many times." << endl;
            abort();
        }

        /* Mark the block as free, and fry the memory in the block so that using it
         * later is more likely to trigger an error.
         */
        theBlock->isFree = true;
        fill(begin(theBlock->buffer), end(theBlock->buffer), 0xBA);
    } else {
        ::operator delete(memory);
    }
}

int NucleotideAlloc::instances() {
    return theNumInstances;
}
