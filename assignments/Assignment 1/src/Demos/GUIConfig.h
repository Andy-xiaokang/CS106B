WINDOW_TITLE("Welcome to C++!")

RUN_TESTS_MENU_OPTION()
MENU_ORDER("StackOverflowGUI.cpp",
           "OnlyConnectGUI.cpp",
           "PlayingFairGUI.cpp",
           "SandpileGUI.cpp",
           "PlotterGUI.cpp")
TEST_ORDER("StackOverflow.cpp",
           "OnlyConnect.cpp",
           "PlayingFair.cpp",
           "Sandpiles.cpp",
           "Plotter.cpp")

TEST_BARRIER("OnlyConnectGUI.cpp", "OnlyConnect.cpp")
TEST_BARRIER("PlayingFairGUI.cpp", "PlayingFair.cpp")
TEST_BARRIER("SandpileGUI.cpp",    "Sandpiles.cpp")
