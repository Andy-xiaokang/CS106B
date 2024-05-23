/******************************************************************************
 * File: OnlyConnect.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Header file for the Only Connect portion of the assignment. You should not
 * modify this file.
 */

#ifndef OnlyConnect_Included
#define OnlyConnect_Included

#include <string>

/**
 * Given a string, converts that string into an Only Connect puzzle by deleting all
 * non-consonants and making all characters upper-case.
 *
 * @param phrase The phrase to transform.
 * @return That string, rendered as an Only Connect puzzle.
 */
std::string onlyConnectize(std::string phrase);

#endif
