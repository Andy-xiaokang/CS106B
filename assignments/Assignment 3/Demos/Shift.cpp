#include "ShiftScheduling.h"
#include "hashcode.h"
#include <sstream>
#include <iomanip>
using namespace std;

namespace {
    int threeWayCompareShifts(const Shift& lhs, const Shift& rhs) {
        return stanfordcpplib::collections::compareTo(static_cast<unsigned>(lhs.day), static_cast<unsigned>(rhs.day),
                                                      lhs.startHour, rhs.startHour,
                                                      lhs.endHour,   rhs.endHour,
                                                      lhs.value,     rhs.value);
    }
}

/* Implementation of relational operators and hash codes. You're welcome to read over this
 * if you'd like, but it's certainly not necessary.
 */
bool operator< (const Shift& lhs, const Shift& rhs) {
    return threeWayCompareShifts(lhs, rhs) < 0;
}
bool operator== (const Shift& lhs, const Shift& rhs) {
    return threeWayCompareShifts(lhs, rhs) == 0;
}
int hashCode(const Shift& shift) {
    return hashCode(static_cast<unsigned>(shift.day), shift.startHour, shift.endHour, shift.value);
}

ostream& operator<< (ostream& out, Day day) {
    switch (day) {
    case Day::SUNDAY:    return out << "Sunday";
    case Day::MONDAY:    return out << "Monday";
    case Day::TUESDAY:   return out << "Tuesday";
    case Day::WEDNESDAY: return out << "Wednesday";
    case Day::THURSDAY:  return out << "Thursday";
    case Day::FRIDAY:    return out << "Friday";
    case Day::SATURDAY:  return out << "Saturday";
    default:             return out << "(unknown day)";
    }
}

bool overlapsWith(const Shift& one, const Shift& two) {
    return one.day == two.day &&
           ((one.startHour <= two.startHour && two.startHour < one.endHour) ||
            (two.startHour <= one.startHour && one.startHour < two.endHour));
}

int lengthOf(const Shift& shift) {
    return shift.endHour - shift.startHour;
}
int valueOf(const Shift& shift) {
    return shift.value;
}

ostream& operator<< (ostream& out, const Shift& shift) {
    ostringstream builder;
    builder << "{ " << shift.day << ", "
            << setfill('0') << setw(2) << shift.startHour << ":00 - "
            << setw(2) << shift.endHour << ":00,"
            << " value $" << valueOf(shift)
            << " }";
    return out << builder.str();
}
