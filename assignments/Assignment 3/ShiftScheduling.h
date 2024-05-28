#ifndef ShiftScheduling_Included
#define ShiftScheduling_Included

#include "Shift.h"
#include "set.h"

/**
 * Given a set of potential shifts for a part-time employee to fill, the number of hours
 * that employee is allowed to work, and the estimated value for each shift in the week,
 * returns a set of shifts that maximize the value produced by that employee.
 *
 * You can assume that the number of hours the employee is allowed to work is nonnegative,
 * but it could be zero (for example, if they have the week off.)
 *
 * You are not allowed to assign an employee to work on overlapping shifts. For example,
 * an employee who picks a Monday 8:00AM - 4:00PM shift can't also take a 2:00PM - 6:00PM
 * shift.
 *
 * It is possible that some shifts might have zero value or even negative value, which might
 * happen, for example, if the cost of paying the employee for their work is less than the
 * expected revenue that would be brought in.
 *
 * If there is a tie between multiple different schedules that would all produce the same
 * value, you can return any of those schedules.
 *
 * You are not required to use up all of the employee's available hours.
 *
 * maxHours may be zero, but it should not be negative. If the client passes in a negative
 * value for maxHours, you should use the error() function to report an error.
 *
 * @param shifts All the potential shifts that could be assigned.
 * @param maxHours The maximum number of hours that the employee is allowed to work.
 * @return A schedule for the worker that maximizes the value brought in. This schedule cannot
 *         have overlapping shifts, nor can it exceed the maximum number of hours.
 */
Set<Shift> highestValueScheduleFor(const Set<Shift>& shifts, int maxHours);

#endif
