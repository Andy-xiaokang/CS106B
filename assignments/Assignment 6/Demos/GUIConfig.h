RUN_TESTS_MENU_OPTION()

MENU_ORDER("PerformanceGUI.cpp",
           "ExploreArraysGUI.cpp",
           "InteractivePQueueGUI.cpp",
           "ApportionmentGUI.cpp")
           
TEST_ORDER("HeapPQueue.cpp",
           "Apportionment.cpp")
           
TEST_BARRIER("ApportionmentGUI.cpp", "HeapPQueue.cpp", "Apportionment.cpp")
WINDOW_TITLE("Data Sagas")
