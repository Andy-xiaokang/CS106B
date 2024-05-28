RUN_TESTS_MENU_OPTION()

WINDOW_TITLE("Recursion!")

MENU_ORDER("SierpinskiGUI.cpp",
           "SierpinskiBungeeJumpGUI.cpp",
           "HumanPyramidsGUI.cpp",
           "WhatAreYouDoingGUI.cpp",
           "ShiftSchedulingGUI.cpp")

TEST_ORDER("HumanPyramids.cpp",
           "WhatAreYouDoing.cpp",
           "ShiftScheduling.cpp")

TEST_BARRIER("HumanPyramidsGUI.cpp", "HumanPyramids.cpp")
TEST_BARRIER("WhatAreYouDoingGUI.cpp", "WhatAreYouDoing.cpp")
TEST_BARRIER("ShiftSchedulingGUI.cpp", "ShiftScheduling.cpp")
