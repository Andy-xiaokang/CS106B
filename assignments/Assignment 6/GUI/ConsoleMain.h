#pragma once
#include <functional>

namespace MiniGUI {
    namespace Detail {
        [[ noreturn ]] void consoleMain(std::function<void()> initialDemo);
    }
}
