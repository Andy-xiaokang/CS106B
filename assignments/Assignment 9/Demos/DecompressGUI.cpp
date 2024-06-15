#include "HuffmanResult.h"
#include "Huffman.h"
#include "GUI/MiniGUI.h"
#include "ginteractors.h"
#include "filelib.h"
#include "set.h"
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

namespace {
    const string kOutputDir = "res/decompressed/";
    const string kSampleDir = "res/";
    const string kNoFileSelected = "(choose a file to decompress)";

    #if   defined(_WIN32) || defined(_WIN64)

        void openFile(const string& filename) {
            string command = "start \"Decompressed File\" " + quotedVersionOf(filename);
            system(command.c_str());
        }
        #define HAS_OPEN_COMMAND

    #elif defined(__APPLE__) || defined(__MACH__)

        void openFile(const string& filename) {
            string command = "open " + quotedVersionOf(filename);
            system(command.c_str());
        }
        #define HAS_OPEN_COMMAND

    #elif defined(__linux__)

        void openFile(const string& filename) {
            string command = "xdg-open " + quotedVersionOf(filename);
            system(command.c_str());
        }
        #define HAS_OPEN_COMMAND

    #endif

    const string kFileSuffix = ".huff"; // Must be lower-case

    class DecompressGUI: public ProblemHandler {
    public:
        DecompressGUI(GWindow& window);
        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> mDisplay;
        Temporary<GContainer> mChrome;

        GLabel*  selectedFile;
        GButton* chooseFileButton;
        GButton* decompressButton;

        #ifdef HAS_OPEN_COMMAND
        GCheckBox*     openOption;
        #endif

        void doDecompress();
        void doChooseFile();
    };

    DecompressGUI::DecompressGUI(GWindow& window) : ProblemHandler(window) {
        mDisplay = make_temporary<GColorConsole>(window, "CENTER");

        /* Main chrome box. */
        mChrome = make_temporary<GContainer>(window, "SOUTH", GContainer::LAYOUT_FLOW_VERTICAL);

        /* Currently-selected file. */
        selectedFile = new GLabel(kNoFileSelected);
        mChrome->add(selectedFile);

        /* Row box for buttons. */
        GContainer* buttons = new GContainer();
        chooseFileButton = new GButton("Choose file");
        decompressButton   = new GButton("Decompress!");
        decompressButton->setEnabled(false);

        buttons->add(chooseFileButton);
        buttons->add(decompressButton);

        #ifdef HAS_OPEN_COMMAND
            openOption = new GCheckBox("Open file after decompressing", false);
            buttons->add(openOption);
        #endif

        mChrome->add(buttons);
    }

    HuffmanResult contentsOf(const string& filename) {
        ifstream input(filename, ios::binary);
        if (!input) {
            error("Could not open file " + filename + " for reading.");
        }

        return readHuffmanFile(input);
    }

    void decompressFile(ostream& log, const string& inFile, const string& outFile, bool thenOpen) {
        log << "Reading file " << inFile << "... " << flush;
        HuffmanResult hf = contentsOf(inFile);
        log << "done!" << endl;

        log << "Decompressing... " << flush;
        string contents = decompress(hf);
        log << "done!" << endl;

        log << "Writing result to disk... " << flush;
        ofstream out(outFile, ios::binary);
        out << contents;
        out.close();
        log << "done!" << endl;
        log << endl;

        log << "File was saved as " << getCurrentDirectory() << "/" << outFile << endl;
        log << endl;

        log << "Statistics: " << endl;
        uint64_t oldSize = fileSize(inFile);
        uint64_t newSize = contents.size();
        log << "  Compressed file size:   " << oldSize << endl;
        log << "  Decompressed file size: " << newSize << endl;
        log << "  Compression ratio:      " << (100.0 * oldSize) / newSize << "%" << endl;
        log << endl;

        #ifdef HAS_OPEN_COMMAND
            if (thenOpen) {
                openFile(outFile);
            }
        #else
            (void) thenOpen;
        #endif
    }

    void DecompressGUI::actionPerformed(GObservable* source) {
        if (source == decompressButton) {
            doDecompress();
        } else if (source == chooseFileButton) {
            doChooseFile();
        }
    }

    void DecompressGUI::doDecompress() {
        string inFile = selectedFile->getText();
        if (inFile != kNoFileSelected) {
            setDemoOptionsEnabled(false);
            decompressButton->setEnabled(false);
            chooseFileButton->setEnabled(false);

            #ifdef HAS_OPEN_COMMAND
                openOption->setEnabled(false);
                bool shouldOpen = openOption->isChecked();
            #else
                const bool shouldOpen = false;
            #endif

            string baseFile = getTail(inFile);
            string outFile = kOutputDir + baseFile.substr(0, baseFile.size() - kFileSuffix.size());

            try {
                decompressFile(*mDisplay, inFile, outFile, shouldOpen);
            } catch (const ErrorException& e) {
                mDisplay->doWithStyle("#800000", GColorConsole::BOLD, [&] {
                    *mDisplay << "\n\n";
                    *mDisplay << "Error: error() was called: " << e.what () << endl;
                });
            } catch (const exception& e) {
                mDisplay->doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                    *mDisplay << "\n\n";
                    *mDisplay << "Error: Exception generated: " << e.what () << endl;
                });
            }

            decompressButton->setEnabled(true);
            chooseFileButton->setEnabled(true);

            #ifdef HAS_OPEN_COMMAND
                openOption->setEnabled(true);
            #endif

            setDemoOptionsEnabled(true);
        }
    }

    void DecompressGUI::doChooseFile() {
        string filename = GFileChooser::showOpenDialog(window().getWidget(), "Choose file to decompress", kSampleDir, "*" + kFileSuffix);
        if (filename == "" || !endsWith(filename, kFileSuffix)) {
            selectedFile->setText(kNoFileSelected);
            decompressButton->setEnabled(false);
        } else {
            selectedFile->setText(filename);
            decompressButton->setEnabled(true);
        }
    }
}

GRAPHICS_HANDLER("Decompress a File", GWindow& window) {
    return make_shared<DecompressGUI>(window);
}

namespace {
    Vector<string> huffFilesIn(const string& directory) {
        Vector<string> result;
        for (string file: listDirectory(directory)) {
            if (endsWith(file, kFileSuffix)) result += file;
        }
        return result;
    }

    string makeDecompressFileSelection() {
        Vector<string> compressed = huffFilesIn("res/compressed");
        Vector<string> samples    = huffFilesIn("res/samples");

        int option = makeSelectionFrom("Choose file to decompress", compressed + samples);
        return option < compressed.size()? "res/compressed/" + compressed[option] :
                                           "res/samples/" + samples[option - compressed.size()];
    }

    bool getThenOpen() {
        #ifdef HAS_OPEN_COMMAND
            return getYesOrNo("Open the file once it's decompressed? ");
        #else
            return false;
        #endif
    }
}

CONSOLE_HANDLER("Decompress a File") {
    do {
        string inFile   = makeDecompressFileSelection();
        bool thenOpen   = getThenOpen();
        string baseFile = getTail(inFile);
        string outFile  = kOutputDir + baseFile.substr(0, baseFile.size() - kFileSuffix.size());

        try {
            decompressFile(cout, inFile, outFile, thenOpen);
        } catch (const ErrorException& e) {
            cerr << "\n\n";
            cerr << "Error: error() was called: " << e.what() << endl;
        } catch (const exception& e) {
            cerr << "\n\n";
            cerr << "Error: Exception generated: " << e.what() << endl;
        }
    } while (getYesOrNo("Decompress another file? "));
}
