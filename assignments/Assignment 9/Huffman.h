#pragma once

#include "Demos/Bit.h"
#include "GUI/MemoryDiagnostics.h"
#include "queue.h"
#include <string>
#include <ostream>

/**
 * Type representing a node in an encoding tree. Each node stores two pointers,
 * one to the child labeled 0 and one to the child labeled 1.
 *
 * Each node in the tree has an associated character. However, this field is
 * only meaningful if the node is a leaf (that is, a node with no children).
 * You should not touch the character in the node unless you are sure that the
 * node is a leaf.
 *
 * Do not modify this type when working on this assignment.
 */
struct EncodingTreeNode {
    char ch;
    EncodingTreeNode* zero;
    EncodingTreeNode* one;

    TRACK_ALLOCATIONS_OF(EncodingTreeNode); // For testing
};

/**
 * Type representing a Huffman-coded file.
 *
 * Do not modify this type when working on this assignment.
 */
struct HuffmanResult {
    Queue<Bit>  treeBits;
    Queue<char> treeLeaves;
    Queue<Bit>  messageBits;
};

/* For debugging purposes, you can print HuffmanResult objects to cout to see
 * what they contain.
 */
std::ostream& operator<< (std::ostream& out, const HuffmanResult& file);

/**
 * End-to-end processing routines.
 */
HuffmanResult compress(const std::string& text);
std::string decompress(HuffmanResult& file);
