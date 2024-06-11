/* Interactive environment for testing the priority queue. */
#include "LinearProbingHashTable.h"
#include "RobinHoodHashTable.h"
#include "ChainedHashTable.h"
#include "MyOptionalHashTable.h"
#include "GUI/MiniGUI.h"
#include "Utility.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "hashmap.h"
#include "gthread.h"
#include <vector>
#include <iomanip>
using namespace std;
using namespace MiniGUI;

namespace {
    const string kButtonFont = Font(FontFamily::MONOSPACE, FontStyle::NORMAL, 12, "black").stanfordCPPLibFontString();

    /* Problem handler that lets the user interactively test the PQueue type. */
    template <typename Table> class InteractiveGUI: public ProblemHandler {
    public:
        InteractiveGUI(GWindow& window, const string& tableName);
        ~InteractiveGUI();

        void actionPerformed(GObservable* source) override;
        void changeOccurredIn(GObservable* source) override;

    private:
        Temporary<GColorConsole> console;

        string tableName;

        /* Controls. These are split into two groups: one for the constructor,
         * and one for the destructor and member functions.
         */
        Temporary<GButton> construct;
        Temporary<GButton> destruct;

        /* List of other commands. These are stored separately because we can't
         * use them as map keys.
         */
        vector<Temporary<GButton>> memberFnButtons;
        HashMap<GButton*, std::string> memberFns;

        /* Inputs for insert/delete/lookup. */
        Temporary<GLabel> nameDesc;
        Temporary<GTextField> nameInput;
        Temporary<GLabel> hashOut;

        /* Button to clear the console. */
        Temporary<GButton> clear;

        /* The actual table. */
        Table* table = nullptr;

        /* Which table this is. */
        int index = 0;

        /* Returns whether the given obserable is the source of a command. */
        bool sourceIs(GObservable* observable, const string& command) const;

        /* Performs a function, logging the output. */
        template <typename Function>
        bool performSafely(const string& command, Function fn);

        /* Performs all the appropriate actions. */
        void setTableExists(bool exists);
        void doConstruct();
        void doDestruct();
        void doSize();
        void doIsEmpty();
        void doContains();
        void doInsert();
        void doRemove();
        void doClear();
        void doPrintDebugInfo();

        /* Recomputes the displayed hash code for the current element. */
        void elemUpdated();
    };

    /* Creates a button with the specified name, installing it in the window in the given
     * spot.
     */
    Temporary<GButton> makeButton(const string& name, GWindow& window, const string& location, bool enabled) {
        Temporary<GButton> result(new GButton(name), window, location);
        result->setFont(kButtonFont);
        result->setEnabled(enabled);
        return result;
    }

    template <typename Table>
    InteractiveGUI<Table>::InteractiveGUI(GWindow& window, const string& tableName) : ProblemHandler(window) {
        this->tableName = tableName;

        /* Standard buttons. */
        construct      = makeButton(tableName + "()", window, "WEST", true);
        destruct       = makeButton("~" + tableName + "()", window, "WEST", false);

        /* Member functions. */
        memberFnButtons.push_back(makeButton("size();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("isEmpty();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("contains(elem);", window, "WEST", false));
        memberFnButtons.push_back(makeButton("insert(elem);", window, "WEST", false));
        memberFnButtons.push_back(makeButton("remove(elem);", window, "WEST", false));
        memberFnButtons.push_back(makeButton("printDebugInfo();", window, "WEST", false));

        nameDesc = make_temporary<GLabel>(window, "SOUTH", "elem: ");
        nameDesc->setEnabled(false);
        nameDesc->setFont(kButtonFont);

        nameInput = Temporary<GTextField>(new GTextField(), window, "SOUTH");
        nameInput->setFont(kButtonFont);
        nameInput->setEnabled(false);

        hashOut   = make_temporary<GLabel>(window, "SOUTH", "");
        hashOut->setFont(kButtonFont);
        hashOut->setEnabled(false);

        /* Output pane. */
        console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
        console->doWithStyle("#008000", [&] {
             *console << "/* Welcome to the interactive testing environment!\n"
                      << " * This will let you test your hash table using a simple\n"
                      << " * hash function that treats the input string as a\n"
                      << " * number, then uses the last digit of the number as\n"
                      << " * the hash code for the string. (If the string does\n"
                      << " * not represent a number, the hash code will be zero.)\n"
                      << " */" << endl;
        });

        /* Clear button. */
        clear = makeButton("Clear Console", window, "EAST", true);

        /* Assemble all the commands into the command list. */
        for (const auto& button: memberFnButtons) {
            string command = button->getText();
            command = command.substr(0, command.find("("));

            memberFns[button.get()] = command;
        }

        elemUpdated();
    }

    /* Given an observable, reports whether that observable is associated with the given named
     * function.
     */
    template <typename Table>
    bool InteractiveGUI<Table>::sourceIs(GObservable* observable, const string& name) const {
        auto* button = dynamic_cast<GButton *>(observable);
        return button && memberFns.get(button) == name;
    }

    template <typename T> struct Box {};

    template <typename Function, typename T>
    void logEffectsOf(GColorConsole& g, Function fn, Box<T>) {
        auto value = fn();
        g.doWithStyle("#008000", [&] {
             g << " // Returned " << boolalpha << value << endl;
        });
    }

    template <typename Function>
    void logEffectsOf(GColorConsole& g, Function fn, Box<void>) {
        fn();
        g << endl;
    }

    /* Performs the given action and logs the result. */
    template <typename Table>
    template <typename Function>
    bool InteractiveGUI<Table>::performSafely(const string& command, Function fn) {
        *console << stringReplace(command, "%s", "table" + to_string(index));
        try {
            logEffectsOf(*console, fn, Box<decltype(fn())>());
            return true;
        } catch (const ErrorException& e) {
            console->doWithStyle("#800000", [&, this] {
                *console << " // Triggered a call to error(\"" << e.what() << "\");" << endl;
            });
            return false;
        } catch (const exception& e) {
            console->doWithStyle("#FF0000", GColorConsole::BOLD_ITALIC, [&, this] {
                *console << " // !! runtime exception generated: \"" << e.what() << "\"" << endl;
            });
            return false;
        } catch (...) {
            console->doWithStyle("#FF0000", GColorConsole::BOLD_ITALIC, [&, this] {
                *console << " // !! unknown exception generated" << endl;
            });
            return false;
        }
    }

    /* Updates the controls in response to a table being created / destroyed. */
    template <typename Table>
    void InteractiveGUI<Table>::setTableExists(bool exists) {
        construct->setEnabled(!exists);
        destruct->setEnabled(exists);
        nameDesc->setEnabled(exists);
        nameInput->setEnabled(exists);
        hashOut->setEnabled(exists);
        for (auto* button: memberFns) {
            button->setEnabled(exists);
        }
        elemUpdated();
    }

    template <typename Table>
    void InteractiveGUI<Table>::doConstruct() {
        /* Make the priority queue. */
        performSafely(tableName + " %s(Hash::identity(10));", [&, this] {
            table = new Table(Hash::identity(10));

            setTableExists(true);
        });
    }

    template <typename Table>
    void InteractiveGUI<Table>::doDestruct() {
        /* Tear down the table queue. */
        performSafely("/* %s goes out of scope */", [&, this] {
            delete table;
            table = nullptr;
            index++;

            setTableExists(false);
            *console << endl;
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doInsert() {
        string name = nameInput->getText();
        ostringstream command;
        command << "%s.insert(" << quotedVersionOf(name) << ");";
        performSafely(command.str(), [&, this] {
            return table->insert(name);
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doRemove() {
        string name = nameInput->getText();
        ostringstream command;
        command << "%s.remove(" << quotedVersionOf(name) << ");";
        performSafely(command.str(), [&, this] {
            return table->remove(name);
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doContains() {
        string name = nameInput->getText();
        ostringstream command;
        command << "%s.contains(" << quotedVersionOf(name) << ");";
        performSafely(command.str(), [&, this] {
            return table->contains(name);
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doSize() {
        performSafely("%s.size();", [&, this] {
            return table->size();
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doIsEmpty() {
        performSafely("%s.isEmpty();", [&, this] {
            return table->isEmpty();
        });
    }
    template <typename Table>
    void InteractiveGUI<Table>::doPrintDebugInfo() {
        /* Temporarily hijack cout to direct to our own buffer. */
        stringstream result;
        auto* oldBuf = cout.rdbuf(result.rdbuf());

        /* Call printDebugInfo to see what we get back. */
        performSafely("%s.printDebugInfo();", [&, this] {
            table->printDebugInfo();
        });

        /* Restore the old buffer. */
        cout.rdbuf(oldBuf);

        /* Show that got printed out. */
        console->doWithStyle("#000080", [&, this] {
            for (string line; getline(result, line); ) {
                *console << "  // " << line << endl;
            }
            *console << endl;
        });
    }

    template <typename Table>
    void InteractiveGUI<Table>::actionPerformed(GObservable* source) {
        /* If this is the clear button, then clear the log. */
        if (source == clear) {
            console->clearDisplay();
            console->flush();
        }
        /* We have two basic modes - one for when there is a queue, and one for where there isn't. */
        else if (table == nullptr) {
            if (source == construct) {
                doConstruct();
            }
        } else {
            if (source == destruct) {
                doDestruct();
            } else if (sourceIs(source, "insert")) {
                doInsert();
            } else if (sourceIs(source, "remove")) {
                doRemove();
            } else if (sourceIs(source, "contains")) {
                doContains();
            } else if (sourceIs(source, "size")) {
                doSize();
            } else if (sourceIs(source, "isEmpty")) {
                doIsEmpty();
            } else if (sourceIs(source, "printDebugInfo")) {
                doPrintDebugInfo();
            }
        }
    }

    template <typename Table>
    void InteractiveGUI<Table>::elemUpdated() {
        ostringstream builder;
        builder << "(hash code for elem: " << Hash::identity(10)(nameInput->getText()) << ")";
        hashOut->setText(builder.str());
    }

    template <typename Table>
    InteractiveGUI<Table>::~InteractiveGUI() {
        delete table;
    }

    template <typename Table>
    void InteractiveGUI<Table>::changeOccurredIn(GObservable* observable) {
        if (observable == nameInput) {
            elemUpdated();
        }
    }
}

GRAPHICS_HANDLER("Interactive Linear Probing", GWindow& window) {
    return make_shared<InteractiveGUI<LinearProbingHashTable>>(window, "LinearProbingHashTable");
}

GRAPHICS_HANDLER("Interactive Robin Hood Hashing", GWindow& window) {
    return make_shared<InteractiveGUI<RobinHoodHashTable>>(window, "RobinHoodHashTable");
}

namespace {
    /* Hash traits. */
    template <typename Table> struct HashTraits;

    /* X Macros to import definition. */
    #define HASH_TABLE(type, desc)                \
        template <> struct HashTraits<type> {     \
            static string name() { return desc; } \
        };
    #include "TimeTestConfig.h"
    #undef HASH_TABLE

    /* Function: printReplInstructions
     * ------------------------------------------------------------------
     * Prints instructions on how to use the REPL environment.
     */
    template <typename Table> void printReplInstructions() {
        cout << "Interactive " << HashTraits<Table>::name() << " Test" << endl;
        cout << "This environment allows you to type in commands that will be" << endl;
        cout << "executed on your hash table.  The interpreter knows the" << endl;
        cout << "following commands:" << endl;
        cout << endl;
        cout << "   isEmpty:         Reports whether the priority queue is empty." << endl;
        cout << "   size:            Reports the size of the priority queue" << endl;
        cout << "   insert <str>:    Inserts the string into the data point." << endl;
        cout << "   contains <str>:  Returns whether the table contains the string." << endl;
        cout << "   remove <str>:    Removes the element from the table." << endl;
        cout << "   printDebugInfo:  Prints debug information." << endl;
        cout << "   quit:            Quits the interpret and returns to the menu." << endl;
        cout << endl;
        cout << "The first letter of any command can be used as a substitute" << endl;
        cout << "for any command name." << endl;
        cout << endl;
        cout << "Elements are hashed by treating the string as a number and using" << endl;
        cout << "the number's last digit as the hash code. Non-number inputs will be" << endl;
        cout << "hashed to zero." << endl;
    }

    template <typename Table> void demoTable() {
        printReplInstructions<Table>();

        /* These curly braces introduce a new layer of scoping.  This ensures that if
         * your priority queue's destructor causes an error, the error occurs before
         * this function returns.
         */
        {
            Table table(Hash::identity(10));
            while (true) {
                /* Get a command from the user. */
                istringstream command(getLine("Enter command: "));

                /* Extract the action. */
                string action;
                command >> action >> ws;
                action = toLowerCase(action);

                if (!command) {
                    cout << "Please enter a command." << endl;
                } else {
                    try {
                        if (action == "isempty") {
                            cout << boolalpha << table.isEmpty() << endl;
                        } else if (action == "size" || action == "s") {
                            cout << table.size() << endl;
                        } else if (action == "quit" || action == "q") {
                            cout << "Leaving test environment...   " << flush;
                            break;
                        } else if (action == "insert" || action == "i") {
                            string toInsert;
                            char leftover;
                            if (!(command >> toInsert) || (command >> ws >> leftover)) {
                                cout << "Please specify a string to insert." << endl;
                            } else {
                                bool result = table.insert(toInsert);
                                cout << "Attempted to add " << toInsert << " to the table. ";
                                cout << "Result: " << boolalpha << result << endl;
                            }
                        } else if (action == "contains" || action == "c") {
                            string toCheck;
                            char leftover;
                            if (!(command >> toCheck) || (command >> ws >> leftover)) {
                                cout << "Please specify a string to check." << endl;
                            } else {
                                bool result = table.contains(toCheck);
                                cout << "Is " << toCheck << " in the table? " << boolalpha << result << endl;
                            }
                        } else if (action == "remove" || action == "r") {
                            string toRemove;
                            char leftover;
                            if (!(command >> toRemove) || (command >> ws >> leftover)) {
                                cout << "Please specify a string to remove." << endl;
                            } else {
                                bool result = table.remove(toRemove);
                                cout << "Attempted to remove " << toRemove << " from the table. ";
                                cout << "Result: " << boolalpha << result << endl;
                            }
                        } else if (action == "printdebuginfo" || action == "p") {
                            table.printDebugInfo();
                        } else {
                            cout << "Unknown command." << endl;
                        }

                    } catch (ErrorException& e) {
                        cout << "A call to error() was triggered: " << e.getMessage() << endl;
                    } catch (exception& e) {
                        cout << "An error occurred: " << e.what() << endl;
                    } catch (...) {
                        cout << "An unknown error occurred." << endl;
                    }
                }
            }
        }

        /* If we made it here, the destructor didn't crash. */
        cout << "success." << endl;
        cout << endl;
    }
}

CONSOLE_HANDLER("Interactive Linear Probing") {
    demoTable<LinearProbingHashTable>();
}

CONSOLE_HANDLER("Interactive Robin Hood Hashing") {
    demoTable<RobinHoodHashTable>();
}
