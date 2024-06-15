#include "HuffmanResult.h"
#include <cstdint>
using namespace std;

namespace {
    /**
     * Validates that the given HuffmanFile obeys all the invariants we expect it to.
     */
    void checkIntegrityOf(const HuffmanResult& file) {
        /* Number of distinct characters must be at least two. */
        if (file.treeLeaves.size() < 2) {
            error("File must contain at least two distinct characters.");
        }

        /* Number of tree bits should be exactly 2c - 1, where c is the number of
         * distinct characters.
         */
        if (file.treeBits.size() != file.treeLeaves.size() * 2 - 1) {
            error("Wrong number of tree bits for the given leaves.");
        }
    }

    /* Utility types for reading/writing individual bits. Inspired by a
     * similar implementation by Julie Zelenski.
     */
    class BitWriter {
    public:
        explicit BitWriter(ostream& out) : out(out) {}
        ~BitWriter() {
            if (bitIndex != 0) flush();
        }

        void put(Bit b) {
            if (b != 0) {
                bitBuffer |= (1u << bitIndex);
            }

            bitIndex++;
            if (bitIndex == 8) {
                flush();
            }
        }

    private:
        void flush() {
            out.put(bitBuffer);
            bitBuffer = 0;
            bitIndex = 0;
        }

        ostream& out;
        uint8_t bitBuffer = 0;
        uint8_t bitIndex  = 0;
    };

    class BitReader {
    public:
        explicit BitReader(istream& in) : in(in) {

        }

        Bit get() {
            if (bitIndex == 8) readMore();

            Bit result = !!(bitBuffer & (1u << bitIndex));
            bitIndex++;
            return result;
        }

    private:
        istream& in;
        uint8_t bitBuffer = 0;
        uint8_t bitIndex  = 8;

        void readMore() {
            char read;
            if (!in.get(read)) {
                error("Unexpected end of file when reading bits.");
            }

            bitBuffer = read;
            bitIndex = 0;
        }
    };

    /* "CS106B A8" */
    const uint32_t kFileHeader = 0xC5106BA8;
}

/**
 * We store HuffmanResult data on disk as follows:
 *
 *
 * 1 byte:  number of distinct characters, minus one.
 * c bytes: the leaves of the tree, in order.
 * 1 byte:  number of valid bits in the last byte.
 * n bits:  tree bits, followed by message bits.
 *
 * We don't need to store how many bits are in the tree, since it's always given
 * by 2*c - 1, as this is the number of nodes in a full binary tree with c leaves.
 */
void writeHuffmanFile(HuffmanResult& file, ostream& out) {
    /* Validate invariants. */
    checkIntegrityOf(file);

    /* Write magic header. */
    out.write(reinterpret_cast<const char *>(&kFileHeader), sizeof kFileHeader);

    /* Number of characters. */
    const uint8_t charByte = file.treeLeaves.size() - 1;
    out.put(charByte);

    /* Tree leaves. */
    while (!file.treeLeaves.isEmpty()) out.put(file.treeLeaves.dequeue());

    /* Number of bits in the last byte to read. */
    uint8_t modulus = (file.treeBits.size() + file.messageBits.size()) % 8;
    if (modulus == 0) modulus = 8;
    out.put(modulus);

    /* Bits themselves. */
    BitWriter writer(out);
    while (!file.treeBits.isEmpty()) writer.put(file.treeBits.dequeue());
    while (!file.messageBits.isEmpty()) writer.put(file.messageBits.dequeue());
}

/**
 * Decodes a Huffman file from disk.
 */
HuffmanResult readHuffmanFile(istream& in) {
    /* Read back the magic header and make sure it matches. */
    uint32_t header;
    if (!in.read(reinterpret_cast<char *>(&header), sizeof header) ||
        header != kFileHeader) {
        error("Chosen file is not a Huffman-compressed file.");
    }

    HuffmanResult result;

    /* Read the character count. */
    char skewCharCount;
    if (!in.get(skewCharCount)) {
        error("Error reading character count.");
    }

    /* We offset this by one - add the one back. */
    int charCount = uint8_t(skewCharCount);
    charCount++;

    if (charCount < 2) {
        error("Character count is too low for this to be a valid file.");
    }

    /* Read in the leaves. */
    vector<char> leaves(charCount);
    if (!in.read(leaves.data(), leaves.size())) {
        error("Could not read in all tree leaves.");
    }
    for (char leaf: leaves) {
        result.treeLeaves.enqueue(leaf);
    }

    /* Read in the modulus. */
    char signedModulus;
    if (!in.get(signedModulus)) {
        error("Error reading modulus.");
    }
    uint8_t modulus = signedModulus;

    /* See how many bits we need to read. To do this, jump to the end of the file
     * and back to where we are to count the bytes, then transform that to a number
     * of bits.
     *
     * Thanks to Julie Zelenski for coming up with this technique!
     */
    auto currPos = in.tellg();
    if (!in.seekg(0, istream::end)) {
        error("Error seeking to end of file.");
    }
    auto endPos  = in.tellg();
    if (!in.seekg(currPos, istream::beg)) {
        error("Error seeking back to middle of file.");
    }

    /* Number of bits to read = (#bytes - 1) * 8 + modulus. */
    uint64_t bitsToRead = (endPos - currPos - 1) * 8 + modulus;

    /* Read in the tree bits. */
    BitReader reader(in);
    for (int i = 0; i < 2 * charCount - 1; i++) {
        result.treeBits.enqueue(reader.get());
        bitsToRead--;
    }

    /* Read in the message bits. */
    while (bitsToRead > 0) {
        result.messageBits.enqueue(reader.get());
        bitsToRead--;
    }

    return result;
}

/* For debugging purposes. */
ostream& operator<< (ostream& out, const HuffmanResult& file) {
    ostringstream builder;
    builder << "{treeBits:" << file.treeBits
            << ",treeLeaves:" << file.treeLeaves
            << ",messageBits:" << file.messageBits
            << "}";
    return out << builder.str();
}
