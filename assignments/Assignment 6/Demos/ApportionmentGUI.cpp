#include "Apportionment.h"
#include "GUI/MiniGUI.h"
#include "Utilities/CSV.h"
#include <iomanip>
using namespace std;

namespace {
    const string kNotSelected = "-";
    const int kInvalidChoice = INT_MIN;
    const string kBaseDir = "res/apportionment/";
    const string kFileExtension = ".csv";

    /* Loads the contents of the given CSV file, reporting an error if something
     * goes wrong.
     */
    Map<string, int> contentsOf(const string& filename) {
        try {
            CSV csv = CSV::parseFile(filename);

            Map<string, int> result;
            for (size_t row = 0; row < csv.numRows(); row++) {
                result[csv[row]["State"]] = stringToInteger(csv[row]["Population"]);
            }

            return result;
        } catch (const exception& e) {
            error(format("Error loading file %s: %s", filename, e.what()));
        }
    }

    /* Returns the length of the longest key in the map. */
    int longestKeyIn(const Map<string, int>& map) {
        return static_cast<int>(max_element(map.begin(), map.end(), [](const string& lhs, const string& rhs) {
            return lhs.size() < rhs.size();
        })->size());
    }

    /* Returns the length of the largest number of seats, assuming things are converted
     * to commafied strings first.
     */
    int longestSeatSizeIn(const Map<string, int>& seats) {
        int longest = 0;
        for (string state: seats) {
            int length = static_cast<int>(addCommasTo(seats[state]).length());
            longest = max(longest, length);
        }
        return longest;
    }

    /* Apportions seats and shows the result. */
    void showApportionment(ostream& out, const string& filename, int numSeats) {
        try {
            auto populations = contentsOf(filename);
            auto seats       = apportion(populations, numSeats);

            /* To pad everything to the proper size. */
            int nameLength = longestKeyIn(populations);
            int seatLength = longestSeatSizeIn(seats);

            out << "Results from " << filename << ", with " << pluralize(numSeats, "seat") << ": \n";
            for (string state: populations) {
                out << "  " << right << setw(nameLength) << state << ": ";
                out << setw(seatLength) << addCommasTo(seats[state]) << " seat" << (seats[state] == 1? ",  " : "s, ");
                out << "population " << addCommasTo(populations[state]) << '\n';
            }

            out << endl;
        } catch (const ErrorException& e) {
            out << "Error: " << e.what() << endl;
        }
    }


    class ApportionmentGUI: public ProblemHandler {
    public:
        ApportionmentGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> console_;
        Temporary<GComboBox> selector_;
        Temporary<GLabel> seatsLabel_;
        Temporary<GTextField> seats_;
        Temporary<GButton> go_;
    };

    ApportionmentGUI::ApportionmentGUI(GWindow& window): ProblemHandler(window) {
        console_  = make_temporary<GColorConsole>(window, "CENTER");
        selector_ = Temporary<GComboBox>(makeFileSelector(kBaseDir, kNotSelected, [](const string& filename) {
            return endsWith(filename, kFileExtension);
        }), window, "SOUTH");

        seatsLabel_ = make_temporary<GLabel>(window, "SOUTH", "Number of seats: ");
        seats_      = make_temporary<GTextField>(window, "SOUTH", "435");

        go_ = make_temporary<GButton>(window, "SOUTH", "Go!");
    }

    void ApportionmentGUI::actionPerformed(GObservable* source) {
        if (source == go_) {
            /* Confirm we have a file to work with. */
            string file = selector_->getSelectedItem();
            if (file != kNotSelected) {
                /* Confirm we have a number of seats. */
                int seats = kInvalidChoice;
                try {
                    seats = stringToInteger(seats_->getText());
                } catch (const ErrorException &) {
                    *console_ << "Please enter an integer number of seats." << endl;
                }

                if (seats != kInvalidChoice) {
                    showApportionment(*console_, kBaseDir + file, seats);
                }

            } else {
                *console_ << "Please choose a populations file." << endl;
            }
        }
    }
}

GRAPHICS_HANDLER("Apportionment", GWindow& window) {
    return make_shared<ApportionmentGUI>(window);
}
