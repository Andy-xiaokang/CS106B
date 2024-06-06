/* Interactive environment for testing the priority queue. */
#include "GUI/MiniGUI.h"
#include "HeapPQueue.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "hashmap.h"
#include "gthread.h"
#include "simpio.h"
#include <vector>
#include <iomanip>
using namespace std;

namespace {
    const string kButtonFont = "Monospaced-12";

    /* Problem handler that lets the user interactively test the PQueue type. */
    class InteractivePQueueGUI: public ProblemHandler {
    public:
        InteractivePQueueGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> console;

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

        /* Two inputs for enqueue. */
        Temporary<GTextField> name, weight;

        /* Chrome around those inputs. */
        Temporary<GLabel> open, comma, close;

        /* Button to clear the console. */
        Temporary<GButton> clear;

        /* The actual heap. */
        HeapPQueue* pq = nullptr;

        /* Which queue this is. */
        int index = 0;

        /* Returns whether the given obserable is the source of a command. */
        bool sourceIs(GObservable* observable, const string& command) const;

        /* Performs a function, logging the output. */
        template <typename Function>
        bool performSafely(const string& command, Function fn);

        /* Performs all the appropriate actions. */
        void setQueueExists(bool exists);
        void doConstruct();
        void doDestruct();
        void doSize();
        void doIsEmpty();
        void doPeek();
        void doEnqueue();
        void doDequeue();
        void doClear();
        void doPrintDebugInfo();
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

    InteractivePQueueGUI::InteractivePQueueGUI(GWindow& window): ProblemHandler(window) {
        /* Standard buttons. */
        construct      = makeButton("HeapPQueue()", window, "WEST", true);
        destruct       = makeButton("~HeapPQueue()", window, "WEST", false);

        /* Member functions. */
        memberFnButtons.push_back(makeButton("size();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("isEmpty();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("peek();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("dequeue();", window, "WEST", false));
        memberFnButtons.push_back(makeButton("printDebugInfo();", window, "WEST", false));

        /* Enqueue. */
        memberFnButtons.push_back(makeButton("enqueue", window, "SOUTH", false));

        open = Temporary<GLabel>(new GLabel("( { \""), window, "SOUTH");
        open->setFont(kButtonFont);

        name = Temporary<GTextField>(new GTextField(), window, "SOUTH");
        name->setFont(kButtonFont);
        name->setEnabled(false);

        comma = Temporary<GLabel>(new GLabel("\", "), window, "SOUTH");
        comma->setFont(kButtonFont);

        weight = Temporary<GTextField>(new GTextField(), window, "SOUTH");
        weight->setFont(kButtonFont);
        weight->setEnabled(false);

        close = Temporary<GLabel>(new GLabel(" } );"), window, "SOUTH");
        close->setFont(kButtonFont);

        /* Output pane. */
        console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");

        /* Clear button. */
        clear = makeButton("Clear Console", window, "EAST", true);

        /* Assemble all the commands into the command list. */
        for (const auto& button: memberFnButtons) {
            string command = button->getText();
            command = command.substr(0, command.find("("));

            memberFns[button.get()] = command;
        }
    }

    /* Given an observable, reports whether that observable is associated with the given named
     * function.
     */
    bool InteractivePQueueGUI::sourceIs(GObservable* observable, const string& name) const {
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
    template <typename Function>
    bool InteractivePQueueGUI::performSafely(const string& command, Function fn) {
        *console << stringReplace(command, "%s", "hpq" + to_string(index));
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

    /* Updates the controls in response to a queue being created / destroyed. */
    void InteractivePQueueGUI::setQueueExists(bool exists) {
        construct->setEnabled(!exists);
        destruct->setEnabled(exists);
        name->setEnabled(exists);
        weight->setEnabled(exists);
        for (auto* button: memberFns) {
            button->setEnabled(exists);
        }
    }

    void InteractivePQueueGUI::doConstruct() {
        /* Make the priority queue. */
        performSafely("HeapPQueue %s;", [&, this] {
            pq = new HeapPQueue();

            setQueueExists(true);
        });
    }

    void InteractivePQueueGUI::doDestruct() {
        /* Make the priority queue. */
        performSafely("/* %s goes out of scope */", [&, this] {
            delete pq;
            pq = nullptr;
            index++;

            setQueueExists(false);
            *console << endl;
        });
    }
    void InteractivePQueueGUI::doEnqueue() {
        /* Get the name/weight pair, and validate that the weight is legit. */
        string name = this->name->getText();
        double weight;
        try {
            weight = stringToReal(this->weight->getText());
        } catch (const ErrorException &) {
            GOptionPane::showMessageDialog("Please enter a valid weight.", "Invalid weight");
            return;
        }

        /* Enqueue it. */
        ostringstream command;
        command << "%s.enqueue(" << DataPoint{name, weight} << ");";
        performSafely(command.str(), [&, this] {
            pq->enqueue({ name, weight });
        });
    }
    void InteractivePQueueGUI::doDequeue() {
        performSafely("%s.dequeue();", [&, this] {
            return pq->dequeue();
        });
    }
    void InteractivePQueueGUI::doPeek() {
        performSafely("%s.peek();", [&, this] {
            return pq->peek();
        });
    }
    void InteractivePQueueGUI::doSize() {
        performSafely("%s.size();", [&, this] {
            return pq->size();
        });
    }
    void InteractivePQueueGUI::doIsEmpty() {
        performSafely("%s.isEmpty();", [&, this] {
            return pq->isEmpty();
        });
    }
    void InteractivePQueueGUI::doPrintDebugInfo() {
        /* Temporarily hijack cout to direct to our own buffer. */
        stringstream result;
        auto* oldBuf = cout.rdbuf(result.rdbuf());

        /* Call printDebugInfo to see what we get back. */
        performSafely("%s.printDebugInfo();", [&, this] {
            pq->printDebugInfo();
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

    void InteractivePQueueGUI::actionPerformed(GObservable* source) {
        /* If this is the clear button, then clear the log. */
        if (source == clear) {
            console->clearDisplay();
            console->flush();
        }
        /* We have two basic modes - one for when there is a queue, and one for where there isn't. */
        else if (pq == nullptr) {
            if (source == construct) {
                doConstruct();
            }
        } else {
            if (source == destruct) {
                doDestruct();
            } else if (source == name || source == weight || sourceIs(source, "enqueue")) {
                doEnqueue();
            } else if (sourceIs(source, "dequeue")) {
                doDequeue();
            } else if (sourceIs(source, "peek")) {
                doPeek();
            } else if (sourceIs(source, "size")) {
                doSize();
            } else if (sourceIs(source, "isEmpty")) {
                doIsEmpty();
            } else if (sourceIs(source, "printDebugInfo")) {
                doPrintDebugInfo();
            }
        }
    }
}

GRAPHICS_HANDLER("Interactive PQueue", GWindow& window) {
    return make_shared<InteractivePQueueGUI>(window);
}

namespace {
    /* Function: printReplInstructions
     * ------------------------------------------------------------------
     * Prints instructions on how to use the REPL environment.
     */
    void printReplInstructions() {
        cout << "Interactive Priority Queue Test" << endl;
        cout << "This environment allows you to type in commands that will be" << endl;
        cout << "executed on your priority queue.  The interpreter knows the" << endl;
        cout << "following commands:" << endl;
        cout << endl;
        cout << "   isEmpty:            Reports whether the priority queue is empty." << endl;
        cout << "   size:               Reports the size of the priority queue" << endl;
        cout << "   enqueue <str> <w>:  Enqueues a DataPoint with the given string and weight." << endl;
        cout << "   peek:               Peeks at the minimum element of the priority queue." << endl;
        cout << "   dequeue:            Dequeues the minimum element of the priority queue." << endl;
        cout << "   printDebugInfo:     Prints debug information." << endl;
        cout << "   quit:               Quits the interpret and returns to the menu." << endl;
        cout << endl;
        cout << "The first letter of any command can be used as a substitute" << endl;
        cout << "for any command name." << endl;
    }
}

CONSOLE_HANDLER("Interactive PQueue") {
    printReplInstructions();

            /* These curly braces introduce a new layer of scoping.  This ensures that if
             * your priority queue's destructor causes an error, the error occurs before
             * this function returns.
             */
            {
                HeapPQueue queue;
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
                            if (action == "peek" || action == "p") {
                                cout << queue.peek() << endl;
                            } else if (action == "isempty" || action == "i") {
                                cout << boolalpha << queue.isEmpty() << endl;
                            } else if (action == "size" || action == "s") {
                                cout << queue.size() << endl;
                            } else if (action == "dequeue" || action == "d") {
                                cout << queue.dequeue() << endl;
                            } else if (action == "quit" || action == "q") {
                                cout << "Leaving test environment...   " << flush;
                                break;
                            } else if (action == "enqueue" || action == "e") {
                                string toEnqueue;
                                double weight;
                                char leftover;
                                if (!(command >> toEnqueue >> weight) || (command >> ws >> leftover)) {
                                    cout << "Please specify a string and a weight." << endl;
                                } else {
                                    DataPoint pt{ toEnqueue, weight };
                                    queue.enqueue(pt);
                                    cout << "Enqueued " << pt << "." << endl;
                                }
                            } else if (action == "printdebuginfo") {
                                queue.printDebugInfo();
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
