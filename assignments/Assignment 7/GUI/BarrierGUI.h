#pragma once

#include "MiniGUI.h"
#include <string>
#include <set>
#include <functional>

/* Runs all the tests in the given files. If they all pass, then uses the
 * specified factory function to create a GUI. If not, returns a GUI that
 * reports that the GUI can't be run until the tests are all passing.
 */
std::function<std::shared_ptr<ProblemHandler>(GWindow&)>
ifPassedThen(const std::set<std::string>& filenames,
             std::function<std::shared_ptr<ProblemHandler>(GWindow&)> constructor);

/* Console version of ifPassedThen. */
std::function<void()>
ifPassedThen(const std::set<std::string>& filenames,
             std::function<void()> demo);
