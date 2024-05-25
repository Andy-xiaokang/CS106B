#pragma once

#include "map.h"
#include "set.h"
#include <string>

const int kNumTrigrams = 2000;

struct Corpus {
    std::string name;
    Map<std::string, double> profile;
};

bool operator< (const Corpus& lhs, const Corpus& rhs);

Map<std::string, double> kGramsIn(const std::string& str, int k);

Map<std::string, double> topKGramsIn(const Map<std::string, double>& profile, int maxNum);

Map<std::string, double> normalize(const Map<std::string, double>& profile);

std::string guessLanguageOf(const Map<std::string, double>& profile,
                            const Set<Corpus>& corpora);
