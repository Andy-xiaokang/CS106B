#include "HuffmanResult.h"
#include "Huffman.h"
#include "GUI/MiniGUI.h"
#include "ginteractors.h"
#include "filelib.h"
#include "set.h"
#include <string>
#include <fstream>
using namespace std;

namespace {
    const string kOutputDir = "res/compressed/";
    const string kSampleDir = "res/samples/";
    const string kNoFileSelected = "(choose a file to compress)";

    class CompressGUI: public ProblemHandler {
    public:
        CompressGUI(GWindow& window);
        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> mDisplay;

        /* Layout:
         * +--------------------------------------+
         * |            selected file             |
         * |     [ choose file ] [ compress ]     |
         * +--------------------------------------+
         */
        Temporary<GContainer> mChrome;
        GButton* chooseFileButton;
        GButton* compressButton;
        GLabel*  selectedFile;

        void doCompress();
        void doChooseFile();
    };

    CompressGUI::CompressGUI(GWindow& window) : ProblemHandler(window) {
        mDisplay = make_temporary<GColorConsole>(window, "CENTER");

        /* Main chrome box. */
        mChrome = make_temporary<GContainer>(window, "SOUTH", GContainer::LAYOUT_FLOW_VERTICAL);

        /* Currently-selected file. */
        selectedFile = new GLabel(kNoFileSelected);
        mChrome->add(selectedFile);

        /* Row box for buttons. */
        GContainer* buttons = new GContainer();
        chooseFileButton = new GButton("Choose file");
        compressButton   = new GButton("Compress!");
        compressButton->setEnabled(false);

        buttons->add(chooseFileButton);
        buttons->add(compressButton);
        mChrome->add(buttons);
    }

    string contentsOf(const string& filename) {
        ifstream input(filename, ios::binary);
        if (!input) {
            error("Could not open file " + filename + " for reading.");
        }

        ostringstream buffer;
        buffer << input.rdbuf();
        return buffer.str();
    }

    void compressFile(ostream& log, const string& filename) {
        log << "Reading file " << filename << "... " << flush;
        string contents = contentsOf(filename);
        log << "done!" << endl;

        log << "Compressing... " << flush;
        HuffmanResult hf = compress(contents);
        log << "done!" << endl;

        log << "Writing result to disk... " << flush;
        string resultFilename = kOutputDir + getTail(filename) + ".huff";

        ofstream out(resultFilename, ios::binary);
        writeHuffmanFile(hf, out);
        out.close();
        log << "done!" << endl;
        log << endl;

        log << "File was saved as " << getCurrentDirectory() << getDirectoryPathSeparator() << resultFilename << endl;
        log << endl;

        log << "Statistics: " << endl;
        uint64_t oldSize = contents.size();
        uint64_t newSize = fileSize(resultFilename);
        log << "  Original file size: " << oldSize << endl;
        log << "  New file size:      " << newSize << endl;
        log << "  Compression ratio:  " << (100.0 * newSize) / oldSize << "%" << endl;
        log << endl;
    }

    void CompressGUI::actionPerformed(GObservable* source) {
        if (source == compressButton) {
            doCompress();
        } else if (source == chooseFileButton) {
            doChooseFile();
        }
    }

    void CompressGUI::doCompress() {
        string file = selectedFile->getText();
        if (file != kNoFileSelected) {
            setDemoOptionsEnabled(false);
            compressButton->setEnabled(false);
            chooseFileButton->setEnabled(false);

            try {
                compressFile(*mDisplay, file);
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

            chooseFileButton->setEnabled(true);
            compressButton->setEventsEnabled(true);
            setDemoOptionsEnabled(true);
        }
    }

    void CompressGUI::doChooseFile() {
        string filename = GFileChooser::showOpenDialog(window().getWidget(), "Choose file to compress", kSampleDir);
        if (filename == "") {
            selectedFile->setText(kNoFileSelected);
            compressButton->setEnabled(false);
        } else {
            selectedFile->setText(filename);
            compressButton->setEnabled(true);
        }
    }
}

GRAPHICS_HANDLER("Compress a File", GWindow& window) {
    return make_shared<CompressGUI>(window);
}

CONSOLE_HANDLER("Compress a File") {
    do {
        string file = makeFileSelection("", kSampleDir);
        try {
            compressFile(cout, file);
        } catch (const ErrorException& e) {
            cerr << "\n\n";
            cerr << "Error: error() was called: " << e.what() << endl;
        } catch (const exception& e) {
            cerr << "\n\n";
            cerr << "Error: Exception generated: " << e.what() << endl;
        }
    } while (getYesOrNo("Compress another file? "));
}
