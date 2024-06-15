#include "ConsoleUtils.h"
#include "filelib.h"
#include "simpio.h"
#include "strlib.h"
#include <iostream>
using namespace std;

int makeSelectionFrom(const string& title, const Vector<string>& options) {
    return makeSelectionFrom(title, vector<string>(options.begin(), options.end()));
}

int makeSelectionFrom(const string& title, const vector<string>& options) {
    if (options.empty()) {
        error("Internal error: Requesting the user to pick an item from an empty list.");
    }

    cout << title << endl;

    for (int i = 0; i < options.size(); i++) {
        cout << i << " " <<  options[i] << endl;
    }

    while (true) {
        int result = getInteger("Your choice: ");
        if (result >= 0 && size_t(result) < options.size()) return result;

        cout << "Please enter a number between 0 and " << options.size() - 1 << endl;
    }
}

/* Lists all files with the given suffix and asks the user to pick one. */
string makeFileSelection(const string& suffix, const string& directory) {
    Vector<string> options;
    for (const string& file: listDirectory(directory)) {
        if (endsWith(file, suffix)) {
            options += file;
        }
    }

    string effectiveDirectory = directory;
    if (effectiveDirectory == "") effectiveDirectory = ".";
    if (effectiveDirectory.back() != '/') effectiveDirectory += '/';

    return effectiveDirectory + options[makeSelectionFrom("Please choose a demo file from this list:", options)];
}
