#include "ProblemHandler.h"
#include "gthread.h"
using namespace std;

/* Stores the main window. */
ProblemHandler::ProblemHandler(GWindow& window) : mainWindow(&window) {
    // Handled in member initialization list
}

/* Issues a redraw, if necessary. */
void ProblemHandler::draw() {
    if (isDirty) {
        GThread::runOnQtGuiThread([this] {
            repaint();
            window().repaint();
        });

        isDirty = false;
    }
}

/* Sets the dirty bit. */
void ProblemHandler::requestRepaint() {
    isDirty = true;
}

/* Returns the stored window. */
GWindow& ProblemHandler::window() const {
    return *mainWindow;
}

/* Default draw does nothing. */
void ProblemHandler::repaint() {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::actionPerformed(GObservable *) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::changeOccurredIn(GObservable *) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::timerFired() {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseMoved(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mousePressed(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseDragged(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseReleased(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseExited() {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseClicked(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseDoubleClicked(double, double) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::hyperlinkClicked(const string &) {
    // Do nothing
}

/* Default handler permits the shutdown. */
bool ProblemHandler::shuttingDown() {
    return true;
}

/* Default handler does nothing. */
void ProblemHandler::settingUp() {
    // Do nothing
}

/* Default handler requests a repaint. */
void ProblemHandler::windowResized() {
    requestRepaint();
}
