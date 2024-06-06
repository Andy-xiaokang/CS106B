#pragma once

#include "map.h"
#include <string>

/* Apportions numSeats House seats to the given collection of states using the
 * Huntington-Hill method. It's assumed that there are at least enough seats
 * for each of the states to get one seat; if that isn't the case, this function
 * reports an error(). The function then returns a Map whose keys are the states
 * and whose values are the number of seats allocated to that state.
 */
Map<std::string, int> apportion(const Map<std::string, int>& populations, int numSeats);
