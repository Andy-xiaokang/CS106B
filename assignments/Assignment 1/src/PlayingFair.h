/******************************************************************************
 * File: PlayingFair.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Header file for the Playing Fair portion of the assignment. You should not
 * modify this file.
 */

#ifndef PlayingFair_Included
#define PlayingFair_Included

#include <string>

/**
 * Given a number n, returns the A-sequence of order n. If n is negative,
 * then this function reports an error by calling the error() function.
 *
 * @param n The order of the A-sequence to generate.
 * @return The A-sequence of order n.
 */
std::string aSequenceOfOrder(int n);

/**
 * Given a number n, returns the B-sequence of order n. If n is negative,
 * then this function reports an error by calling the error() function.
 *
 * @param n The order of the B-sequence to generate.
 * @return The B-sequence of order n.
 */
std::string bSequenceOfOrder(int n);


#endif
