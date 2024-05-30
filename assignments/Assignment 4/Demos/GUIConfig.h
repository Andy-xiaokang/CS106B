WINDOW_TITLE("Recursion to the Rescue!")

RUN_TESTS_MENU_OPTION()
MENU_ORDER("TowersOfHanoiGUI.cpp",
           "PermutationGUI.cpp",
           "MatchmakerGUI.cpp",
           "DisasterGUI.cpp")
           
TEST_ORDER("Matchmaker.cpp",
           "DisasterPlanning.cpp")

TEST_BARRIER("MatchmakerGUI.cpp", "Matchmaker.cpp")
TEST_BARRIER("DisasterGUI.cpp",   "DisasterPlanning.cpp")
