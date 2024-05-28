#ifndef WhatAreYouDoing_Included
#define WhatAreYouDoing_Included

#include "vector.h"
#include "set.h"
#include <string>

/* This function is provided to you and you don't need to implement it. It takes as input
 * a string, then returns a Vector<string> representing the different parts of that string
 * in the order in which they appear.
 */
Vector<std::string> tokenize(const std::string& sentence);

/* You need to implement this function. See the assignment handout for details. */
Set<std::string> allEmphasesOf(const std::string& sentence);

#endif
