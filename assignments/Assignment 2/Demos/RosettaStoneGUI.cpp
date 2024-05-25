#include "RosettaStone.h"
#include "filelib.h"
#include "GUI/MiniGUI.h"
#include "GUI/SimpleTest.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

namespace {
    const int KGRAM_SIZE     = 3;
    const string kBaseDir    = "res/trigrams/";
    const string kFilterList = "OfficialLanguages.txt";

    const int kTextPadLength = 25;
}

bool operator< (const Corpus& lhs, const Corpus& rhs) {
    if (lhs.name != rhs.name) return lhs.name < rhs.name;
    return lhs.profile < rhs.profile;
}

namespace {
    class RosettaStoneGUI: public ProblemHandler {
    public:
        RosettaStoneGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;
        void settingUp() override;

    private /* helpers */:
        void initChrome();
        void findBestMatch(const string& text);

    private /* state */:
        Set<Corpus> all_;

        Temporary<GColorConsole> console_;
        Temporary<GTextArea>     input_;
        Temporary<GButton>       go_;
    };

    RosettaStoneGUI::RosettaStoneGUI(GWindow& window) : ProblemHandler(window) {
        initChrome();
    }

    void RosettaStoneGUI::initChrome() {
        console_ = make_temporary<GColorConsole>(window(), "CENTER");
        input_   = make_temporary<GTextArea>(window(), "SOUTH");
        go_      = make_temporary<GButton>(window(), "SOUTH", "Go!");

        setDemoOptionsEnabled(false);
        input_->setEnabled(false);
        go_->setEnabled(false);
    }

    void RosettaStoneGUI::settingUp() {
        *console_ << "Loading language data... " << endl;
        for (string file: listDirectory(kBaseDir)) {
            if (!endsWith(file, ".3grams")) continue;

            string language = file.substr(0, file.find('.'));

            ifstream input("res/trigrams/" + file);
            if (!input) error("Bad!");

            *console_ << "  Loading data for " << setw(kTextPadLength) << left << (language + "...") << flush;

            Map<string, double> trigrams;
            input >> trigrams;
            if (!input) error("Worse!");

            Corpus corpus = { language, topKGramsIn(normalize(trigrams), kNumTrigrams) };
            all_.add(corpus);

            *console_ << " done!" << endl;
        }

        *console_ << "\nReady to guess languages." << endl;

        input_->setEnabled(true);
        go_->setEnabled(true);
        setDemoOptionsEnabled(true);
    }

    void RosettaStoneGUI::actionPerformed(GObservable* source) {
        if (source == go_) {
            findBestMatch(input_->getText());
        }
    }

    void RosettaStoneGUI::findBestMatch(const string& text) {
        try {
            string language = guessLanguageOf(topKGramsIn(normalize(kGramsIn(text, KGRAM_SIZE)), kNumTrigrams), all_);
            *console_ << "Best guess: " << language << endl;
        } catch (const exception& e) {
            console_->doWithStyle("#800000", GColorConsole::BOLD, [&] {
                *console_ << "Error: " << e.what() << endl;
            });
        }
    }
}

GRAPHICS_HANDLER("Rosetta Stone", GWindow& window) {
    return make_shared<RosettaStoneGUI>(window);
}
