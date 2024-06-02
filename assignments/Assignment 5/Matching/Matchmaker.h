#pragma once
#include <string>
#include <ostream>
#include "map.h"
#include "set.h"

/* Unordered pair of strings. */
class Pair {
public:
    /* Sort the strings. */
    Pair(const std::string& one, const std::string& two) {
        if (one < two) {
            one_ = one;
            two_ = two;
        } else {
            one_ = two;
            two_ = one;
        }
    }

    const std::string& first()  const {
        return one_;
    }
    const std::string& second() const {
        return two_;
    }

    bool operator< (const Pair& rhs) const {
        /* Compare first components. */
        auto comp = first().compare(rhs.first());
        if (comp != 0) return comp < 0;

        return second() < rhs.second();
    }

    bool operator== (const Pair& rhs) const {
        return first() == rhs.first() && second() == rhs.second();
    }

    bool operator> (const Pair& rhs) const {
        return rhs < *this;
    }
    bool operator>= (const Pair& rhs) const {
        return !(*this < rhs);
    }
    bool operator<= (const Pair& rhs) const {
        return !(*this > rhs);
    }
    bool operator!= (const Pair& rhs) const {
        return !(*this == rhs);
    }

private:
    std::string one_;
    std::string two_;
};

bool hasPerfectMatching(const Map<std::string, Set<std::string>>& possibleLinks, Set<Pair>& matching);
Set<Pair> fastMaxWeightMatching(const Map<std::string, Map<std::string, int>>& possibleLinks);
Set<Pair> fastMWMCMatching(const Map<std::string, Map<std::string, int>>& possibleLinks);

std::ostream& operator<< (std::ostream& out, const Pair& pair);
