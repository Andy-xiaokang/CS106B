#pragma once

#include "../Huffman.h"
#include <iostream>

/**
 * Routines for reading and writing HuffmanResult objects to disk. The code in
 * here is what actually touches files on disk. You're welcome to peek around
 * and see how this works, but you won't need to call these functions and won't
 * need to understand how they're put together.
 */
void writeHuffmanFile(HuffmanResult& file, std::ostream& out);
HuffmanResult readHuffmanFile(std::istream& in);

