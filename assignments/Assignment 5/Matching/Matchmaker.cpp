#include "Matchmaker.h"
#include "Matching/graphtypes.h"
#include <limits>
using namespace std;

Set<Pair> fastMaxWeightMatching(const Map<string, Map<string, int>>& links) {
    /* Linearize links into a predictable order. */
    Map<int, string> order;
    Map<string, int> toIndex;
    for (string name: links) {
        int index = order.size() + 1;
        order[index] = name;
        toIndex[name] = index;
    }

    /* Build the graph. */
    auto graph = EdRothberg::NewGraph(links.size());
    for (const auto& src: links) {
        for (const auto& dst: links[src]) {
            if (toIndex[src] < toIndex[dst]) {
                EdRothberg::AddEdge(graph, toIndex[src], toIndex[dst], links[src][dst]);
            }
        }
    }

    /* Run the matching. */
    auto matching = EdRothberg::WeightedMatch(graph);
    EdRothberg::FreeGraph(graph);

    /* Convert to pairs. */
    Set<Pair> result;
    for (const auto& entry: matching) {
        result.add({ order[entry.first], order[entry.second] });
    }

    return result;
}

namespace {
    Set<Pair> fastMaxCardinalityMatching(const Map<string, Set<string>>& links) {
        /* Transform to a graph where each edge has cost 1. */
        Map<string, Map<string, int>> newLinks;
        for (const auto& src: links) {
            for (const auto& dst: links[src]) {
                newLinks[src][dst] = 1;
            }
        }

        /* Now find a maximum-weight matching in this new graph. */
        return fastMaxWeightMatching(newLinks);
    }
}

bool hasPerfectMatching(const Map<string, Set<string>>& possibleLinks, Set<Pair>& matching) {
    matching = fastMaxCardinalityMatching(possibleLinks);
    return matching.size() * 2 == possibleLinks.size();
}

Set<Pair> fastMWMCMatching(const Map<string, Map<string, int>>& possibleLinks) {
    /* Artificially boost all the edge costs by an amount such that it is always better to
     * have more edges than fewer edges.
     */
    int maxEdge = 0;
    for (const string& src: possibleLinks) {
        for (const string& dst: possibleLinks[src]) {
            maxEdge = max(maxEdge, possibleLinks[src][dst]);
        }
    }

    Map<string, Map<string, int>> newLinks;
    for (const string& src: possibleLinks) {
        for (const string& dst: possibleLinks[src]) {
            /* Each edge is boosted by the score you'd get if everything was paired at the
             * maximum value. Now, any matching has to include the maximum number of edges.
             */
            newLinks[src][dst] = possibleLinks[src][dst] + (maxEdge + 1) * possibleLinks.size();
        }
    }

    return fastMaxWeightMatching(newLinks);
}
