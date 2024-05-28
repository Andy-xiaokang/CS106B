#ifndef ProblemHandler_Included
#define ProblemHandler_Included

#include "gwindow.h"
#include "gobjects.h"
#include <memory>
#include <string>
#include <utility>

/* Interface representing a type that's capable of displaying a problem
 * graphically and responding to events.
 *
 * The intent is that each problem handler will be spun up and torn down
 * based on what the user chooses to do, so don't assume you will have
 * access to long-running state in the course of implementing these types.
 *
 * Each ProblemHandler must also export the following functions:
 *
 *      Constructor(GWindow&): Construct a new handler in the associated window.
 */
class ProblemHandler {
public:
    /* Polymorphic classes need virtual destructors. */
    virtual ~ProblemHandler() = default;

    /* Respond to component events. */
    virtual void actionPerformed(GObservable* source);
    virtual void changeOccurredIn(GObservable* source);

    /* Respond to mouse motion. */
    virtual void mouseMoved(double x, double y);
    virtual void mousePressed(double x, double y);
    virtual void mouseDragged(double x, double y);
    virtual void mouseReleased(double x, double y);
    virtual void mouseExited();
    virtual void mouseClicked(double x, double y);
    virtual void mouseDoubleClicked(double x, double y);

    /* Respond to timer events. */
    virtual void timerFired();

    /* Respond to changes in the window. */
    virtual void windowResized();

    /* Respond to links events. */
    virtual void hyperlinkClicked(const std::string& url);

    /* Respond to construction. Feel free to run UI-enabled
     * code here.
     */
    virtual void settingUp();

    /* Respond to shutdown. Return true to proceed with the shutdown, and
     * false to halt it.
     */
    virtual bool shuttingDown();

    /* Redraws the window. */
    void draw();

protected:
    /* Constructor stores the window.
     *
     * The constructor for each ProblemHandler is run inside the Qt GUI thread. Use the
     * constructor to handle initial chrome setup, but don't perform any tasks that
     * require UI refreshes.
     */
    ProblemHandler(GWindow& window);

    /* Draw the current state of things. */
    virtual void repaint();

    /* Marks the region as dirty. */
    void requestRepaint();

    /* The window that we're working with. */
    GWindow& window() const;

private:
    /* Dirty bit. We're initially dirty because nothing's been drawn yet. */
    bool isDirty = true;

    /* Pointer to the main window. */
    GWindow* mainWindow;
};

#endif
