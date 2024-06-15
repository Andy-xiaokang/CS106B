#ifndef GUIMain_Included
#define GUIMain_Included

#include "ProblemHandler.h"
#include <memory>

/* Locks/unlocks all demo option buttons. Should only be called from the main GUI thread. */
void setDemoOptionsEnabled(bool isEnabled);

/* For internal use only. */
namespace MiniGUI {
    namespace Detail {
        /* Sets which demo is active. This is not meant to be used by MiniGUI clients. */
        void setActiveDemo(std::shared_ptr<ProblemHandler> handler);

        /* Retrieves the MiniGUI window. This is not meant to be used by MiniGUI clients. */
        GWindow& graphicsWindow();

        /* Graphical main loop. */
        [[ noreturn ]] void graphicsMain(std::function<void()> initialDemo);
    }
}



#endif
