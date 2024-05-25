/**
 * Utilities for building console-based programs.
 */
#pragma once

#include "vector.h"
#include "simpio.h"
#include <string>
#include <vector>

/* Given a list of options, prompts the user until they pick an option, then
 * returns the index of the selection.
 */
int makeSelectionFrom(const std::string& title, const Vector<std::string>& options);
int makeSelectionFrom(const std::string& title, const std::vector<std::string>& options);

/* Prompts the user for a file from the given directory. */
std::string makeFileSelection(const std::string& suffix, const std::string& directory = "res/");
