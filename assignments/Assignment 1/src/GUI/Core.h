/* Shared configuration information. This is meant for use by the
 * program drivers and isn't meant to be used by clients.
 */

#pragma once

/* Macro: GRAPHICS_HANDLER("Name of demo", GWindow& window) {
 *    ...
 * }
 *
 * Defines a graphics handler that appears in the menu when in graphics mode.
 * This function should return a std::shared_ptr<T> type, where T is some
 * derived class of ProblemHandler.
 */
#define GRAPHICS_HANDLER(name, argument) /* ... something internal ... */

/* Macro: CONSOLE_HANDLER("Name of demo") {
 *    ...
 * }
 *
 * Defines a demo that appears in the menu when in console mode. This function
 * should interact with stdin/stdout as appropriate.
 */
#define CONSOLE_HANDLER(name) /* ... something internal ... */







/***** Implementation Below This Point *****/

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "ProblemHandler.h"

namespace MiniGUI {
    /* Named menu callback functions. */
    struct MenuOption {
        std::string name;
        std::function<void()> callback;
    };

    namespace Config {
        /* Title of the program. */
        std::string programTitle();

        /* List of menu options. */
        std::vector<MenuOption> menuOptions();

        /* Order of files to use in test cases. */
        std::vector<std::string> testOrder();

        /* Whether we're in console mode. */
        bool isConsoleMode();
    }

    namespace Detail {
        struct FileKey {
            const char* filename;
            std::size_t line;
        };

        struct GraphicsInstaller {
            GraphicsInstaller(FileKey key,
                              const char* name,
                              std::function<std::shared_ptr<ProblemHandler>(GWindow&)>);
        };
        struct ConsoleInstaller {
            ConsoleInstaller(FileKey key,
                             const char* name,
                             std::function<void()>);
        };
    }
}

#undef GRAPHICS_HANDLER

#define GRAPHICS_HANDLER(title, arg) MAKE_GRAPHICS_HANDLER(__FILE__, __LINE__, title, arg)
#define MAKE_GRAPHICS_HANDLER(file, line, title, arg) DO_MAKE_GRAPHICS_HANDLER(file, line, title, arg)
#define DO_MAKE_GRAPHICS_HANDLER(file, line, title, arg)                  \
    static std::shared_ptr<ProblemHandler> _graphicsHandler##line(arg);   \
    static ::MiniGUI::Detail::GraphicsInstaller _installer##line(         \
        { file, line }, title, _graphicsHandler##line                     \
    );                                                                    \
    static std::shared_ptr<ProblemHandler> _graphicsHandler##line(arg)

#undef CONSOLE_HANDLER

#define CONSOLE_HANDLER(title) MAKE_CONSOLE_HANDLER(__FILE__, __LINE__, title)
#define MAKE_CONSOLE_HANDLER(file, line, title) DO_MAKE_CONSOLE_HANDLER(file, line, title)
#define DO_MAKE_CONSOLE_HANDLER(file, line, title)                        \
    static void _consoleHandler##line();                                  \
    static ::MiniGUI::Detail::ConsoleInstaller _installer##line(          \
        { file, line }, title, _consoleHandler##line                      \
    );                                                                    \
    static void _consoleHandler##line()
