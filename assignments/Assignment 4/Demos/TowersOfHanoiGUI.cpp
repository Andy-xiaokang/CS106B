#include "TowersOfHanoi.h"
#include "GUI/MiniGUI.h"
#include "gthread.h"
#include "error.h"
#include "gobjects.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "stack.h"
#include "simpio.h"
#include <iostream>
#include <string>
#include <memory> // For unique_ptr
#include <mutex>
using namespace std;
using namespace MiniGUI;

namespace {
    /******** Constants ********/

    /* Maximum permitted disks. */
    const int kMaxDisks = 8;

    /* Number of spindles. */
    const int kNumSpindles = 3;

    /* Maximum number of disk moves printed to console at once (to always print all lines to console without pausing to ask for input, change this to -1! */
    const int kMaxLines = 25;

    /* Width of one spindle. */
    const int kSpindleWidth = 20;

    /* Pixel margin between spindle work areas. */
    const double kSpindleMarginSize = 10;

    /* Pixel margin between window boundary and content area. */
    const double kPadding = 30;

    /* Aspect ratio for the window. */
    const double kMinAspectRatio = 5.0 / 3.0;

    /* Minimum sizes of the display window. */
    const double kMinHeight = 20;
    const double kMinWidth  = kMinHeight * kMinAspectRatio;

    /* Smallest possible disk width. */
    const double kMinDiskWidth = 3 * kSpindleWidth;

    /* Spindle color: Russet! */
    const string kSpindleColor = "#80461B";

    /* Colors for each disk. */
    const string kDiskColors[] = {
       "#000000",
       "#0000ff",
       "#ff00ff",
       "#ff0000",
       "#ff8000",
       "#ffff00",
       "#00ff00",
       "#00ffff"
    };
    /* Border colors for each disk.  These are just darker versions of the disk colors
     * given above.
     */
    const string kDiskBorderColors[] = {
        "#000000",
        "#000080",
        "#800080",
        "#800000",
        "#804000",
        "#808000",
        "#008000",
        "#008080"
    };

    /* Message warning about resizing the window first. */
    const string kResizeMessage =
            "Don't forget to resize this window so that you can see both the graphics display "
            "and the Qt Creator debugger before hitting the \"Go!\" button. Once the debugger "
            "engages, you may not be able to resize or move this window.";

    /* Font info for that message. */
    const Font kResizeMessageFont(FontFamily::SERIF, FontStyle::ITALIC, 16, "#400080");

    /* Number of steps in the animation. */
    const int kNumAnimationSteps = 50;

    /* Type representing a single disk. */
    struct Disk {
        int size;    // Zero is smallest disk, sizes increase from there.
        GRect* rect; // Graphical rectangle.
    };

    struct Spindle {
        /* Which GRects are here right now. */
        Stack<Disk> disksHere;

        /* The rectangle for this spindle. */
        GRect* rect;

        /* The start and end x coordinates of the area allocated
         * to this spindle.  This is the space where disks can be
         * moved.
         */
        double startX, endX;

        /* The center line for the spindle. */
        double centerX;
    };

    class TowersOfHanoiGUI: public ProblemHandler {
    public:
        TowersOfHanoiGUI(GWindow& window);
        ~TowersOfHanoiGUI();
        void actionPerformed(GObservable* source) override;

        void initHanoiDisplay(int mNumDisks, AnimationSpeed speed);
        void moveSingleDisk(char from, char to);

        bool shuttingDown() override;

    protected:
        void repaint() override;

    private:
        GRectangle mBounds;

        Temporary<GButton> mGoButton;

        Vector<Spindle> mSpindles;

        bool mHasStarted = false;

        int mNumDisks;
        double mDiskHeight;

        double mPauseTime;

        void   setupSpindles();
        void   setupDisks();
        double diskYPosition(int spindle);
        void   computeBounds();
        void   animateDiskPath(GRect* disk, double endX, double endY, double totalTime);

        void clearGraphics();
    };

    /* Ewww, global variables! We made the decision to do this based on a careful weighing
     * of the options. You, on the other hand, should not do this. :-)
     *
     * These need to be raw pointers, not actual objects, so they aren't torn down
     * while in use during static object destruction.
     */
    auto* theGUI        = new shared_ptr<TowersOfHanoiGUI>;
    auto* theGUILock    = new mutex();
    bool  isConsoleMode = false;

    /******** Implementations ********/

    /* Slightly hacky code: when we update the graphics display, we want to force a repaint
     * so that, if the debugger is enabled, we still see the change.
     */
    void add(GWindow& window, GObject* toAdd) {
        GThread::runOnQtGuiThread([&] {
            window.add(toAdd);
            window.repaint();
        });
    }
    void setLocation(GWindow& window, GObject* object, double x, double y) {
        GThread::runOnQtGuiThread([&] {
            object->setLocation(x, y);
            window.repaint();
        });
    }

    /* Sets up the spindles. */
    void TowersOfHanoiGUI::setupSpindles() {
        /* Calculate the total horizontal area that can be allocated to a spindle. */
        double workspaceWidth = mBounds.width / kNumSpindles;

        for (int i = 0; i < kNumSpindles; i++) {
            Spindle spindle;

            /* Set the work area appropriately. */
            spindle.startX = mBounds.x + workspaceWidth * i       + kSpindleMarginSize;
            spindle.endX   = mBounds.x + workspaceWidth * (i + 1) - kSpindleMarginSize;

            /* Create a rectangle for this spindle.  The spindle will be centered in
             * the work area and will have height equal to the number of disks plus
             * one (so the spindle is still visible).  It will also be bottom-aligned.
             */
            double height = (mNumDisks + 1) * mDiskHeight;
            double y = mBounds.y + mBounds.height - height;

            /* Determine where the center line is, then center the rectangle around that. */
            spindle.centerX = (spindle.startX + spindle.endX) / 2.0;
            spindle.rect = new GRect(spindle.centerX - kSpindleWidth / 2.0, y, kSpindleWidth, height);
            spindle.rect->setFilled(true);
            spindle.rect->setColor(kSpindleColor);

            /* Add that to the display. */
            add(window(), spindle.rect);

            /* Add this spindle to the list. */
            mSpindles += spindle;
        }
    }

    /* Creates and sets up all of the disks that will be used in the simulation. */
    void TowersOfHanoiGUI::setupDisks() {
        for (int i = 0; i < mNumDisks; i++) {
            /* We need to determine the position, color, and size of the disk.
             *
             * To size the disk, we will linearly interpolate between the workspace
             * area (the maximum possible width) and the minimum possible disk width
             * (specified as a constant).  In particular, we want the bottom disk to
             * have a size that perfectly fills the spindle workspace, and we want
             * the top disk to have size equal to kMinDiskWidth.  The formula we
             * will use for this is the following:
             *
             * Width of disk 0     = Workspace width.
             * Width of disk i - 1 = kMinDiskWidth.
             *
             * Therefore:
             *
             * width = ((kMinDiskWidth - workspaceWidth) / (numDisks - 1)) * i + workspaceWidth
             *
             * There is an edge case here when numDisks = 1, so we will special-case it.
             */
            double workspaceWidth = mSpindles[0].endX - mSpindles[0].startX;

            double width;
            if (mNumDisks == 1) {
                width = workspaceWidth;
            } else {
                width = max(kMinDiskWidth, ((kMinDiskWidth - workspaceWidth) / (mNumDisks - 1)) * i + workspaceWidth);
            }

            /* Given the width, the x coordinate can be found by taking the center line of
             * the spindle and backing off by half the width.
             */
            double x = mSpindles[0].centerX - width / 2.0;

            /* We can determine the y coordinate of the disk by using our existing function
             * for determining where the next disk should go on a spindle.
             */
            double y = diskYPosition(0);

            /* Create the rectangle. */
            GRect* rect = new GRect(x, y, width, mDiskHeight);
            rect->setColor(kDiskBorderColors[kMaxDisks - 1 - i]);
            rect->setFilled(true);
            rect->setFillColor(kDiskColors[kMaxDisks - 1 - i]);

            /* Draw the disk. */
            add(window(), rect);

            /* Add the disk to the spindle. */
            Disk disk;
            disk.rect = rect;
            disk.size = mNumDisks - i; // Inverted because we're creating the biggest disk first.

            mSpindles[0].disksHere.push(disk);
        }
    }

    /* Given a spindle number, returns the y coordinate where the next disk
     * would be placed on top of that spindle.
     */
    double TowersOfHanoiGUI::diskYPosition(int spindle) {
        if (spindle < 0 || spindle >= mSpindles.size()) {
            error("Invalid spindle number.");
        }

        /* The position is determined as follows:
         *
         * 1. Start at the bottom of the window.
         * 2. Back off by the number of disks in the stack.
         * 3. Back off once more, since we need to give the upper y coordinate.
         *
         * This works out to windowHeight - (disksInStack + 1) * diskHeight.
         */
        return mBounds.y + mBounds.height - (mSpindles[spindle].disksHere.size() + 1) * mDiskHeight;
    }

    /* Given a character, maps that character to a spindle number. */
    int charToSpindle(char ch) {
        switch (ch) {
            case 'A': case 'a': return 0;
            case 'B': case 'b': return 1;
            case 'C': case 'c': return 2;
            default: error("Unknown spindle."); return -1;
        }
    }

    /* Given a time through the animation, represented by a number between 0 (start)
     * and 1 (end), interpolates where along the trajectory the disk would be at that
     * time using a cubic Hermitian spline.  This makes the animation look significantly
     * smoother than before.
     *
     * http://en.wikipedia.org/wiki/Cubic_Hermite_spline
     */
    double interpolate(double t) {
        return -2 * t * t * t + 3 * t * t;
    }

    /* Animates a disk moving from its current position to the destination. */
    void TowersOfHanoiGUI::animateDiskPath(GRect* disk, double endX, double endY, double totalTime) {
        const double startX = disk->getX(), startY = disk->getY();

        /* Animate the motion! */
        for (int i = 0; i < kNumAnimationSteps; i++) {
            /* Interpolate between the start and end positions.  To determine
             * how much to interpolate, we use a cubic Hermite spline to map
             * from the fraction of the animation completed to a smoother
             * animation point.
             */
            double x = startX + (endX - startX) * interpolate(double(i) / (kNumAnimationSteps - 1));
            double y = startY + (endY - startY) * interpolate(double(i) / (kNumAnimationSteps - 1));
            setLocation(window(), disk, x, y);

            /* Pause to let the animation progress.  This will not work out to pausing
             * for exactly the right amount of time because there's some latency involved
             * in the graphics calls, but it's "close enough."
             */
            pause(totalTime / kNumAnimationSteps);
        }
    }

    void TowersOfHanoiGUI::computeBounds() {
        /* Compute the maximum possible width and height of the display. */
        double maxWidth  = max(kMinWidth,  window().getCanvasWidth()  - 2 * kPadding);
        double maxHeight = max(kMinHeight, window().getCanvasHeight() - 2 * kPadding);

        /* Determine the current aspect ratio. */
        double empiricalAspectRatio = maxWidth / maxHeight;

        /* Never drop below the required aspect ratio. */
        double width, height;
        if (empiricalAspectRatio < kMinAspectRatio) {
            width  = maxWidth;
            height = width / kMinAspectRatio;
        } else {
            width  = maxWidth;
            height = maxHeight;
        }

        /* Compute amount of padding needed. */
        double xPadding = (window().getCanvasWidth()  - width)  / 2.0;
        double yPadding = (window().getCanvasHeight() - height) / 2.0;

        mBounds = {
            xPadding,
            yPadding,
            width,
            height
        };

        /* Determine the height of a single disk.  We want to size the disks such that
         * each spindle can have all the disks on top of it, then some vertical space to
         * show the top of each spindle, then two blank spaces above it.  This works
         * out to needing a number of "virtual disks" equal to the total number of disks
         * actually used, plus four extras.
         */
        int numVirtualDisks = mNumDisks + 4;
        mDiskHeight = mBounds.height / numVirtualDisks;
    }

    /* Returns the animation speed associated with a given constant. */
    double pauseTimeFor(AnimationSpeed speed) {
        switch (speed) {
        case AnimationSpeed::MOLASSES:
            return 1000;
        case AnimationSpeed::CHEETAH:
            return 500;
        case AnimationSpeed::PEREGRINE_FALCON:
            return 100;
        default:
            error("Unknown speed?");
            return -1;
        }
    }

    TowersOfHanoiGUI::TowersOfHanoiGUI(GWindow& window) : ProblemHandler(window) {
        mGoButton = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");
    }
}

/* Congratulations! You've found the function that triggered the call to
 * solveTowersOfHanoi!
 */
void makeTheMagicHappen() {
    solveTowersOfHanoi(5, 'A', 'C', 'B');
}

namespace {
    void TowersOfHanoiGUI::actionPerformed(GObservable* source) {
        if (source == mGoButton) {
            /* Yep, the demo has started! */
            mHasStarted = true;

            setDemoOptionsEnabled(false);
            mGoButton->setEnabled(false);

            makeTheMagicHappen();

            mGoButton->setEnabled(true);
            setDemoOptionsEnabled(true);
        }
    }

    void TowersOfHanoiGUI::moveSingleDisk(char startCh, char finishCh) {
        /* Convert the start and end spindles to indices. */
        int start = charToSpindle(startCh), end = charToSpindle(finishCh);

        /* Confirm that the start spindle isn't empty. */
        if (mSpindles[start].disksHere.isEmpty()) {
            error("No disks at the start spindle.");
        }

        /* Get the disk to move. */
        Disk disk = mSpindles[start].disksHere.pop();

        /* Make sure we can legally move the disk from the start spindle to the
         * end spindle.
         */
        if (!mSpindles[end].disksHere.isEmpty() &&
            mSpindles[end].disksHere.peek().size <= disk.size) {
            error("Cannot move a larger disk atop a smaller one.");
        }

        /* Determine how long the animation must take for each of the three parts of the
         * animation.
         */
        double eachAnimationTime = mPauseTime / 3;

        /* Move the disk up. */
        animateDiskPath(disk.rect, disk.rect->getX(), mBounds.y, eachAnimationTime);

        /* Move the disk over.  We need to compute the new x coordinate by centering
         * the disk over the new midline.
         */
        double newX = mSpindles[end].centerX - disk.rect->getWidth() / 2.0;
        animateDiskPath(disk.rect, newX, mBounds.y, eachAnimationTime);

        /* Move the disk down. */
        animateDiskPath(disk.rect, disk.rect->getX(), diskYPosition(end), eachAnimationTime);

        /* Update internal state: make sure that the disk is now on the destination
         * spindle.
         */
        mSpindles[end].disksHere.push(disk);
    }



    /* Initializes the display. */
    void TowersOfHanoiGUI::initHanoiDisplay(int numDisks, AnimationSpeed speed) {
        /* Validate input. */
        if (numDisks > kMaxDisks) {
            error("Sorry, but we can't support that many disks.");
        }

        if (numDisks <= 0) {
            error("Sorry, but we need a positive number of disks.");
        }

        clearGraphics();
        mNumDisks = numDisks;
        computeBounds();

        /* Initialize the spindles and disks. */
        setupSpindles();
        setupDisks();

        mPauseTime = pauseTimeFor(speed);
    }

    void TowersOfHanoiGUI::clearGraphics() {
        window().clearCanvas();
        for (auto& spindle: mSpindles) {
            window().remove(spindle.rect);
            delete spindle.rect;

            while (!spindle.disksHere.isEmpty()) {
                auto disk = spindle.disksHere.pop();
                window().remove(disk.rect);
                delete disk.rect;
            }
        }
        mSpindles.clear();
    }

    TowersOfHanoiGUI::~TowersOfHanoiGUI() {
        clearGraphics();
    }

    void TowersOfHanoiGUI::repaint() {
        /* If we've already done the demo, don't display the resize message. */
        if (!mHasStarted) {
            window().clearCanvasPixels();

            double width  = window().getCanvasWidth()  - 2 * kPadding;
            double height = window().getCanvasHeight() - 2 * kPadding;
            GRectangle bounds = {
                kPadding, kPadding, width, height
            };
            try {
                auto text = TextRender::construct(kResizeMessage, bounds, kResizeMessageFont);
                text->alignCenterHorizontally();
                text->alignCenterVertically();
                text->draw(window());
            } catch (const ErrorException &) {
                // Oops, can't fit text in the box. Don't attempt to.
            }
        }
    }

    bool TowersOfHanoiGUI::shuttingDown() {
        unique_lock<mutex> lock(*theGUILock);

        /* Free the shared pointer stored here so that the destructor fires
         * once the GUI is done with us.
         */
        theGUI->reset();

        return true;
    }



    /******** Console Implementation ********/
    class TowersOfHanoiConsole {
    public:
        bool condensedOutput = false;
        void initHanoiDisplay(int mNumDisks, AnimationSpeed speed);
        void moveSingleDisk(char from, char to);

    private:
        int mNumDisks;
        int mNumCalls = 0;
        Map<char, Vector<int>> mSpindles;

        double mPauseTime;

        void setup();
        void listDisks(char spindle);
        void listSpires();

    };

    TowersOfHanoiConsole console; // global instance

    /* Convert int to alphabetic char for spindle labelling */
    char convert(int i) {
        return (i % 26) + 'A';
    }

    /* Initialize mSpindles to have all mNumDisks disks on leftmost spindle, remaining spindles are empty */
    void TowersOfHanoiConsole::setup() {
        // initialize spindles according to alphabet (left is 'A')
        for (int i = 0; i < kNumSpindles; i++){
            mSpindles[convert(i)] = { };
        }

        // add disks to leftmost spindle so that disks go from 1 to mNumDisks, from top to bottom
        for (int i = 0; i < mNumDisks; i++) {
            mSpindles['A'].add(mNumDisks - i);
        }

    }

    /* Prints list of disks on spindle from top (end of vector) to bottom (beginning of vector) */
    void TowersOfHanoiConsole::listDisks(char spindle) {
        for (int i = mSpindles[spindle].size() - 1; i >= 0; i--){
            cout << mSpindles[spindle][i];
            if (i > 0) {
                cout << " ";
            }
        }
    }

    /* Lists all spires and their disks */
    void TowersOfHanoiConsole::listSpires() {
        for (int i = 0; i < kNumSpindles; i++){
            cout << " " << convert(i) << " [";
            listDisks(convert(i));
            cout << "]";
            if (i < kNumSpindles - 1){
                cout << ",";
            }
        }
        cout << endl;
    }

    /* Waits for the user to enter a blank line. */
    void pressEnterToContinue(const string& message) {
        string line;
        do {
            line = getLine(message);
        } while (!line.empty());
    }

    /**
     * Initialize console "display" by setting up numDisks disks on leftmost spindle (spindle 'A') and printing text-based explanation of setup
     *
     * @param numDisks Number of disks to use in problem; all disks start on leftmost spindle
     * @param speed Animation speed dictates length of pause between print statements
     */
    void TowersOfHanoiConsole::initHanoiDisplay(int numDisks, AnimationSpeed speed){
        if (numDisks > kMaxDisks) {
            error("Sorry, but we can't support that many disks.");
        }

        if (numDisks <= 0) {
            error("Sorry, but we need a positive number of disks.");
        }
        mNumDisks = numDisks;

        setup(); // initialize spindles and disks

        mPauseTime = pauseTimeFor(speed);

        // explanation of setup
        cout << "The movement of the disks will be abbreviated, and at each move the state of all the spires will be printed. ";
        cout << "'1 from A to C:  A [2 3 4 5], B [], C [1]' translates to 'Disk 1 moves from A to C, and as a result the spires look like the following: 2, 3, 4, and 5 are on spire A, no disks are on spire B, and disk 1 is on spire C.'" << endl;

        pressEnterToContinue("Press enter when you're ready to begin.");

        cout << "Initial state: ";
        listSpires();

    }

    /* Prints description of movement and updates internal state to reflect disk update */
    void TowersOfHanoiConsole::moveSingleDisk(char from, char to){
        // Check to see if kMaxLines have been printed already, if so, prompt user before continuing to print
        if (kMaxLines >= 0 && mNumCalls >= kMaxLines) {
            pressEnterToContinue("Press enter to continue printing lines.");
            mNumCalls = 0;
        }

        if (mSpindles[from].isEmpty()) {
            error("No disks at the start spire.");
        }

        // do movement -- update mSpindles
        int currDisk = mSpindles[from][mSpindles[from].size() - 1];
        mSpindles[from].remove(mSpindles[from].size() - 1);

        if (!mSpindles[to].isEmpty() &&
            mSpindles[to][mSpindles[to].size() - 1] <= currDisk) {
            error("Cannot move a larger disk atop a smaller one.");
        }

        mSpindles[to].add(currDisk);

        cout << currDisk << " from " << from << " to " << to << ": ";
        listSpires();

        mNumCalls++;
    }
}

GRAPHICS_HANDLER("Towers of Hanoi", GWindow& window) {
    isConsoleMode = false;

    unique_lock<mutex> lock(*theGUILock);
    if (!*theGUI) {
        theGUI->reset(new TowersOfHanoiGUI(window));
    }
    return *theGUI;
}

void initHanoiDisplay(int n, AnimationSpeed speed) {
    if (!isConsoleMode) {
        unique_lock<mutex> lock(*theGUILock);
        if (!*theGUI) {
            error("No display was detected. This can happen if you tried calling one of the "
                  "Towers of Hanoi graphics functions from a test case, which isn't permitted.");
        }

        (*theGUI)->initHanoiDisplay(n, speed);
    } else {
        /* DONE: Set up Hanoi graphics. */
        console.initHanoiDisplay(n, speed);
    }
}

void moveSingleDisk(char start, char finish) {
    if (!isConsoleMode) {
        unique_lock<mutex> lock(*theGUILock);
        if (!*theGUI) {
            error("No display was detected. This can happen if you tried calling one of the "
                  "Towers of Hanoi graphics functions from a test case, which isn't permitted.");
        }

        (*theGUI)->moveSingleDisk(start, finish);
    } else {
        console.moveSingleDisk(start, finish);
    }
}

CONSOLE_HANDLER("Towers of Hanoi") {
    /* Set the global mode to console so the handlers know we're in console mode. */
    isConsoleMode = true;

    makeTheMagicHappen();
}
