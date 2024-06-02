#ifndef EmptyProblemHandler_Included
#define EmptyProblemHandler_Included

#include "ProblemHandler.h"

/* A problem handler that just prompts the user to pick a problem to solve. This
 * gets installed whenever there's nothing else to display.
 */
class EmptyProblemHandler: public ProblemHandler {
public:
    EmptyProblemHandler(GWindow& window): ProblemHandler(window) {}

protected:
    void repaint() override;
};



#endif
