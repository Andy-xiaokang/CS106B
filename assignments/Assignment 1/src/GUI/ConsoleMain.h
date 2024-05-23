#pragma once
#include <functional>

namespace MiniGUI {
    namespace Detail {
        void consoleMain(std::function<void()> initialDemo);
    }
}
