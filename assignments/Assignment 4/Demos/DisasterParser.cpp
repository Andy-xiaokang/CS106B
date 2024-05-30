#include "DisasterParser.h"
#include "strlib.h"
#include <regex>
using namespace std;

/* Everything in here is private to this file. */
namespace {
    enum NameComponents {
        WholeString,
        CityName,
        XCoord,
        YCoord,
        NumComponents
    };

    /* Given city information in the form
     *
     *     CityName (X, Y)
     *
     * Parses out the name and the X/Y coordinate, returning the
     * name, and filling in the DisasterTest with what's found.
     */
    string parseCity(const string& cityInfo, DisasterTest& result) {
        /* Split on all the delimiters and confirm we've only got
         * three components.
         */
        regex  pattern("^([A-Za-z0-9 .\\-]+)\\(\\s*(-?[0-9]+(?:\\.[0-9]+)?)\\s*,\\s*(-?[0-9]+(?:\\.[0-9]+)?)\\s*\\)$");
        smatch components;
        string toMatch = trim(cityInfo);

        if (!regex_match(toMatch, components, pattern)) {
            error("Can't parse this data; is it city info? " + cityInfo);
        }

        /* There are four components here, actually: the whole match,
         * plus each subexpression we care about.
         */
        if (components.size() != NumComponents) {
            error("Could not find all components?");
        }

        /* We're going to get back some extra leading or trailing
         * whitespace here, so peel it off.
         */
        string name = trim(components[CityName]);
        if (name.empty()) error("City names can't be empty.");

        /* Insert the city location */
        result.cityLocations[name] = {
            stringToReal(components[XCoord]),
            stringToReal(components[YCoord])
        };

        /* Insert an entry for the city into the road network. */
        result.network[name] = {};
        return name;
    }

    /* Reads the links out of the back half of the line of a file,
     * adding them to the road network.
     */
    void parseLinks(const string& cityName, const string& linksStr,
                    DisasterTest& result) {
        /* It's possible that there are no outgoing links. */
        if (trim(linksStr) == "") {
            result.network[cityName] = {};
            return;
        }

        auto components = stringSplit(linksStr, ",");
        for (const string& dest: components) {
            /* Clean up all whitespace and make sure that we didn't
             * discover an empty entry.
             */
            string cleanName = trim(dest);
            if (cleanName.empty()) {
                error("Blank name in list of outgoing cities?");
            }

            /* Confirm this isn't a dupe. */
            if (result.network[cityName].contains(cleanName)) {
                error("City appears twice in outgoing list?");
            }

            result.network[cityName] += cleanName;
        }
    }

    /* Parses one line out of the file and updates the network with what
     * it found. This will only add edges in the forward direction as
     * a safety measure; edges are reversed later on.
     */
    void parseCityLine(const string& line, DisasterTest& result) {
        /* Search for a colon on the line. The split function will only return a
         * single component if there are no outgoing links specified.
         */
        auto numColons = count(line.begin(), line.end(), ':');
        if (numColons != 1) {
            error("Each data line should have exactly one colon on it.");
        }

        /* Split the line into the city name/location and the list
         * of outgoing cities.
         */
        auto components = stringSplit(line, ":");
        if (components.isEmpty()) {
            error("Data line appears to have no city information.");
        }

        /* Create a dummy list of outgoing cities if one doesn't already exist. */
        if (components.size() == 1) components.add({});

        string name = parseCity(components[0], result);

        parseLinks(name, components[1], result);
    }

    /* Given a graph in which all forward edges have been added, adds
     * the reverse edges to the graph.
     */
    void addReverseEdges(DisasterTest& result) {
        for (const string& source: result.network) {
            for (const string& dest: result.network[source]) {
                if (!result.network.containsKey(dest)) {
                    error("Outgoing link found to nonexistent city '" + dest + "'");
                }
                result.network[dest] += source;
            }
        }
    }

    /* Given a graph, confirms all nodes are at distinct locations. */
    void validateLocations(const DisasterTest& test) {
        Map<GPoint, string> locations;
        for (auto loc: test.cityLocations) {
            if (locations.containsKey(test.cityLocations[loc])) {
                throw runtime_error(loc + " is at the same location as " + locations[test.cityLocations[loc]]);
            }
            locations[test.cityLocations[loc]] = loc;
        }
    }
}

/**
 * Given a stream pointing at a test case for Disaster Preparation,
 * pulls the data from that test case.
 *
 * @param source The stream containing the test case.
 * @return A test case from the file.
 * @throws ErrorException If an error occurs or the file is invalid.
 */
DisasterTest loadDisaster(istream& source) {
    DisasterTest result;

    for (string line; getline(source, line); ) {
        /* Skip blank lines or comments. */
        if (trim(line).empty() || startsWith(line, "#")) continue;

        parseCityLine(line, result);
    }

    addReverseEdges(result);
    validateLocations(result);
    return result;
}
