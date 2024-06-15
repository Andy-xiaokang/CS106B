RUN_TESTS_MENU_OPTION()

MENU_ORDER("CompressGUI.cpp", "DecompressGUI.cpp")

WINDOW_TITLE("Huffman Coding")

TEST_BARRIER("CompressGUI.cpp", "Huffman.cpp")
TEST_BARRIER("DecompressGUI.cpp", "Huffman.cpp")
