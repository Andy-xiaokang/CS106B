RUN_TESTS_MENU_OPTION()

MENU_ORDER("RosettaStoneGUI.cpp",
           "RisingTidesGUI.cpp")

WINDOW_TITLE("Fun With Collections")

TEST_ORDER("RosettaStone.cpp",
           "RisingTides.cpp")

TEST_BARRIER("RosettaStoneGUI.cpp", "RosettaStone.cpp")
TEST_BARRIER("RisingTidesGUI.cpp",  "RisingTides.cpp")
