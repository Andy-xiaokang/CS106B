#include "RisingTides.h"
#include "GUI/MiniGUI.h"
#include "DownloadCache.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "gobjects.h"
#include "gcontainer.h"
#include "filelib.h"
#include "gthread.h"
#include <istream>
#include <fstream>
#include <vector>
#include <unordered_set>
using namespace std;

namespace {

    using GBufferedImage = GCanvas;

    const string kOutputFile = "res/terrains/result.png";
    const string kBackgroundColor = "#802040";

    const string kLoadButtonText     = "Load";
    const string kWaterHeightText    = "Water Height: ";
    const string kFloodButtonText    = "Go!";

    const string kRenderingText      = "Rendering the result...";
    const string kDownloadingMessage = "Downloading the terrain from Stanford's servers...";
    const string kBadNumberText      = "Please enter a real number.";
    const string kRunningCodeText    = " (running your code...)";

    const string kLoadingText        = "Loading the landscape...";

    /* Error message to display when failing to read a terrain. */
    const string kMalformedDataFileMessage = "Oops! Something went wrong reading that data file. If this is a terrain file you designed, double-check the syntax of the file. Otherwise, this isn't your fault.";

    /* Where to look for files. */
    const string kBasePath = "res/terrains/";
    const string kFileSuffix = ".terrain";

    /* Sentinel meaning "nothing is selected. */
    const string kNotSelected = "-";

    /* Error messages. */
    const string kRemoteDownloadErrorString = "Oops! We weren't able to download that terrain file. This isn't your fault. (Please contact the course staff.)";
    const string kVisualizationErrorString  = "Oops! We weren't able to download that terrain file. (Check your network connection?)";

    const size_t kTextInputSize = 15;

    /* Color to use for water. */
    const int kUnderwaterColor = GBufferedImage::createRgbPixel(0, 49, 83); // Prussian blue

    /* The rest of the map is colored according to the following scheme. The heights are mapped
     * to real numbers between 0 and 1. Those real numbers are then used to interpolate between
     * a fixed series of color points marked off at various intervals.
     *
     * The RGBPoint type represents a particular RGB color annotated with a threshold value
     * between 0 and 1 indicating where that color sits.
     */
    struct RGBPoint {
        int red, green, blue;
        double threshold;
    };

    /* The actual colors to use to draw the map, annotated with their threshold values. */
    const vector<RGBPoint> kColors = {
        {   0, 102,   0, 0.0  },   // Pakistan green
        { 154, 205,  50, 0.1  },   // Chartreuse
        { 251, 236,  93, 0.25 },   // Maize
        { 212, 175,  55, 0.4  },   // Metallic gold
        { 166,  60,  20, 1.01 }    // Sienna. The 1.01 here is to ensure we cover rounding errors.
    };

    /* Linearly interpolates between two quantities. The progress variable runs between 0 and 1. */
    int interpolate(int from, int to, double progress) {
        return from + (to - from) * progress;
    }

    /* Given an absolute height and the range of heights associated with a given range, returns
     * the color associated with that height.
     */
    int colorFor(double height, bool underwater, double lowest, double highest) {
        /* If the point is underwater, it's always given the water color. */
        if (underwater) return kUnderwaterColor;

        /* Map height to [0, 1]. */
        double alpha = (height - lowest) / nextafter(highest - lowest, numeric_limits<double>::infinity());

        /* Figure out which points we're between. */
        for (size_t i = 1; i < kColors.size(); i++) {
            if (alpha <= kColors[i].threshold) {
                /* Progress is measured by how far between the two points we are. 0.0 means
                 * "completely at the left end. 1.0 means "completely at the right end."
                 */
                double progress = (alpha - kColors[i - 1].threshold) /
                                  (kColors[i].threshold - kColors[i - 1].threshold);

                /* Interpolate between those color points to get our overall color. */
                int red   = interpolate(kColors[i - 1].red,   kColors[i].red,   progress);
                int green = interpolate(kColors[i - 1].green, kColors[i].green, progress);
                int blue  = interpolate(kColors[i - 1].blue,  kColors[i].blue,  progress);

                return GBufferedImage::createRgbPixel(red, green, blue);
            }
        }

        /* This code is unreachable. */
        error("Impossible alpha: " + to_string(alpha));
        return 0;
    }

    /* Draws the given image to the output file. This is done because GBufferedImage is an efficient
     * way to push the image to the backend, but doesn't support resizing (TODO: validate this).
     * We therefore dump it to a file and reload it later as a GImage, which does support resizing.
     */
    void renderToFile(const Grid<double>& heights, const Grid<bool>& underwater) {
        double lowest  = *min_element(heights.begin(), heights.end());
        double highest = *max_element(heights.begin(), heights.end());

        Grid<int> pixels(heights.numRows(), heights.numCols());
        for (int row = 0; row < heights.numRows(); row++) {
            for (int col = 0; col < heights.numCols(); col++) {
                pixels[row][col] = colorFor(heights[row][col], underwater[row][col], lowest, highest);
            }
        }

        GThread::runOnQtGuiThread([&] {
            GBufferedImage image;
            image.fromGrid(pixels);
            image.save(kOutputFile);
        });
    }

    /* Generates a semi-humorous message to pass the time as everything computes. */
    string floodMessage() {
        switch (rand() % 4) {
            case 0:  return "Forecasting the flood";
            case 1:  return "Watering the world";
            case 2:  return "Deploying the deluge";
            default: return "Simulating the surge";
        }
    }

    /* Type: Terrain
     * ----------------------------------------------------------------------------------
     * Type representing a terrain as a Grid<double> subdivided into individual cells,
     * each with an associated height, along with a list of sources from which the water
     * floods out.
     *
     * The GridLocation type is a simple struct that contains just a row and a column.
     * It's used as a way of tracking a position in the Grid as a single object.
     */
    struct Terrain {
        Grid<double> heights;              // Height of each point on the map, in meters.
        Vector<GridLocation> waterSources; // Which locations, if any, are water sources.
    };

    /* Loads a terrain from a data file. */
    Terrain loadTerrain(istream& input, GLabel* statusLine) {
        /* The first line of the input is either a URL to download or the string "local."
         * If it's a remote download, we need to fetch the file first.
         */
        string url;
        if (!getline(input, url)) {
            error(kMalformedDataFileMessage);
        }

        if (url != "local") {
            auto data = webContentsOf(url, ".terrain", [&] (DownloadStatus status) {
                if (status == DownloadStatus::DOWNLOADING) {
                    statusLine->setText(kDownloadingMessage);
                } else if (status == DownloadStatus::FINISHED) {
                    statusLine->setText(" ");
                }
            });

            /* Go download that data instead. */
            return loadTerrain(*data, statusLine);
        }

        statusLine->setText(kLoadingText);

        int numRows, numCols;
        if (input >> numRows >> numCols, !input) {
            error(kMalformedDataFileMessage);
        }

        int numSources;
        if (input >> numSources, !input) {
            error(kMalformedDataFileMessage);
        }

        /* Read the flooding sources. */
        Terrain result;
        for (int i = 0; i < numSources; i++) {
            int row, col;
            if (input >> row >> col, !input) {
                error(kMalformedDataFileMessage);
            }

            result.waterSources.add({ row, col });
        }

        /* Read the height data. */
        result.heights.resize(numRows, numCols);
        for (int row = 0; row < result.heights.numRows(); row++) {
            for (int col = 0; col < result.heights.numCols(); col++) {
                if (input >> result.heights[row][col], !input) {
                    error(kMalformedDataFileMessage);
                }
            }
        }

        char leftover;
        if (input >> leftover) {
            error(kMalformedDataFileMessage);
        }

        return result;
    }

    /* Returns all sample problems found in the example directory. */
    vector<string> sampleProblems() {
        vector<string> result;
        for (const auto& file: listDirectory(kBasePath)) {
            if (endsWith(file, kFileSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }

    /* Problem class corresponding to the water level problem. */
    class FindWaterLevel: public ProblemHandler {
    public:
        /* Construct handler, assuming the input stream contains the data to read. */
        FindWaterLevel(GWindow& window);

        /* TODO: We leak the memory for the interactors. Fix this! */

        /* File extension. */
        static std::string fileExtension();

        /* Respond to action events. */
        void actionPerformed(GObservable* source) override;

    protected:
        /* Draw the current state of things. */
        void repaint() override;

    private:
        /* Two-row grid of items. The layout looks like this
         *
         *    Dropdown   Load     Water Height:    Input Field     Go!
         *    ------------------- status line ------------------------
         */
        Temporary<GContainer> container;

        /* Load file. */
        GComboBox*  terrainChooser;
        GButton*    loadButton;

        /* Input fields. */
        GLabel*     heightDesc;
        GTextField* heightField;
        GButton*    solveButton;

        /* Status reporting. */
        GLabel*     statusLine;

        /* The floodplain and what's currently under water. */
        Terrain plain;
        Grid<bool> underwater;

        /* Name of the current terrain. */
        string currTerrain = kNotSelected;

        /* Runs a flood simulation. */
        void runFlood(double height);

        /* Sets which terrain is currently active. */
        void setActiveTerrain(const string& terrainFile, bool clearHeight);
    };

    FindWaterLevel::FindWaterLevel(GWindow& window) : ProblemHandler(window) {
        auto* rawContainer = new GContainer(GContainer::LAYOUT_GRID);

        terrainChooser = new GComboBox();
        terrainChooser->addItem(kNotSelected); // Initially, nothing shows
        for (const string& file: sampleProblems()) {
            terrainChooser->addItem(file);
        }
        terrainChooser->setEditable(false);

        loadButton  = new GButton(kLoadButtonText);
        heightDesc  = new GLabel(kWaterHeightText);
        heightField = new GTextField(kTextInputSize);
        solveButton = new GButton(kFloodButtonText);
        statusLine  = new GLabel("");

        rawContainer->addToGrid(terrainChooser, 0, 0);
        rawContainer->addToGrid(loadButton,     0, 1);
        rawContainer->addToGrid(heightDesc,     0, 2);
        rawContainer->addToGrid(heightField,    0, 3);
        rawContainer->addToGrid(solveButton,    0, 4);
        rawContainer->addToGrid(statusLine,     1, 0, 1, 5);

        heightField->setText("0.0");

        container= Temporary<GContainer>(rawContainer, window, "SOUTH");

        setActiveTerrain(kNotSelected, true);
    }

    /* Runs a flood starting from the given height. */
    void FindWaterLevel::runFlood(double height) {
        statusLine->setText(floodMessage() + kRunningCodeText);
        underwater = floodedRegionsIn(plain.heights, plain.waterSources, height);

        /* Stash the rendered image to disk. */
        statusLine->setText(kRenderingText);
        renderToFile(plain.heights, underwater);
        statusLine->setText(" ");

        requestRepaint();
    }

    /* Renders the result of the flood. */
    void FindWaterLevel::repaint() {
        /* Clear the display. */
        clearDisplay(window(), kBackgroundColor);

        /* If the world is empty, don't draw anything. */
        if (plain.heights.isEmpty()) return;

        GImage image(kOutputFile);

        /* Rescale the image to fit into the window. */
        double aspectRatio = image.getWidth() / image.getHeight();
        double scale;
        if (aspectRatio > window().getCanvasWidth() / window().getCanvasHeight()) {
            scale = window().getCanvasWidth() / image.getWidth();
        } else {
            scale = window().getCanvasHeight() / image.getHeight();
        }
        image.scale(scale);

        /* Draw the image. */

        /* TODO: As of now (2/27/19) there is a bug where scaling a GBufferedImage causes the effect of the
         * transform to be applied to its x and y coordinate in addition to its width and height. We therefore
         * have to invert the transformation here.
         *
         * Once this bug is fixed, remove the logic to scale the x and y coordinates.
         */
        window().draw(&image, (window().getCanvasWidth()  - image.getWidth() * scale)  / (2 * scale),
                              (window().getCanvasHeight() - image.getHeight() * scale) / (2 * scale));
    }

    void FindWaterLevel::actionPerformed(GObservable* source) {
        if (source == heightField || source == solveButton) {
            double height;
            bool heightValid = true;

            try {
                height = stringToReal(heightField->getText());
            } catch (const exception& e) {
                statusLine->setText(kBadNumberText);
                heightValid = false;
            }

            if (heightValid) {
                container->setEnabled(false);
                if (currTerrain != terrainChooser->getSelectedItem()) {
                    setActiveTerrain(terrainChooser->getSelectedItem(), false);
                } else if (terrainChooser->getSelectedItem() != kNotSelected) {
                    runFlood(height);
                }
                container->setEnabled(true);
            }
        } else if (source == loadButton) {
            setActiveTerrain(terrainChooser->getSelectedItem(), true);
        }
    }

    void FindWaterLevel::setActiveTerrain(const string& terrainFile, bool clearHeight) {
        if (terrainFile == kNotSelected) {
            plain.heights.clear();
            plain.waterSources.clear();
            currTerrain = kNotSelected;
        } else {
            setDemoOptionsEnabled(false);
            container->setEnabled(false);
            try {
                ifstream input(kBasePath + terrainFile);
                if (!input) error("Cannot open file " + kBasePath + terrainFile);
                plain = loadTerrain(input, statusLine);
                if (clearHeight) heightField->setText("0.0");
                currTerrain = terrainFile;

                double height = 0.0;
                try {
                    height = stringToReal(heightField->getText());
                } catch (const exception &) {

                }
                runFlood(height);
            } catch (const DownloadError& e) {
                if (e.errorCode() < 0) {
                    statusLine->setText(kVisualizationErrorString);
                } else {
                    statusLine->setText(kRemoteDownloadErrorString);
                }
            }
            container->setEnabled(true);
            setDemoOptionsEnabled(true);
        }
    }
}

GRAPHICS_HANDLER("Rising Tides", GWindow& window) {
    return make_shared<FindWaterLevel>(window);
}
