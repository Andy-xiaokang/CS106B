#include "Matchmaker.h"
#include "GUI/MiniGUI.h"
#include "GraphEditor/GraphEditor.h"
#include <fstream>
#include <algorithm>
#include "filelib.h"
using namespace std;
using namespace MiniGUI;

ostream& operator<< (ostream& out, const Pair& pair) {
    ostringstream builder;
    builder << "{ " << quotedVersionOf(pair.first()) << ", " << quotedVersionOf(pair.second()) << " }";
    return out << builder.str();
}

namespace {
    const string kUnsavedChanges = "You have unsaved changes.\n\nDo you want to save?";
    const string kUnsavedChangesTitle = "Unsaved Changes";

    const string kUntitledGraph = "(Untitled)";

    const string kBaseDir = "res/matchmaker/";
    const string kFileExtension = "";

    const string kWelcome = R"(Click "Load Graph" to choose a graph, or "New Graph" to make a new graph.)";

    const Font   kWelcomeFont(FontFamily::SERIF, FontStyle::BOLD_ITALIC, 24, "#4C5866"); // Marengo

    const string kInstructions = R"(Double-click to add a node.)";
    const Font&  kInstructionsFont  = kWelcomeFont;

    const string kBackgroundColor = "white";

    const string kNoPerfectMatching = "No perfect matching exists.";
    const string kNoPerfectMatchingTitle = "No Perfect Matching";

    const int kMinWeight = 1;
    const int kMaxWeight = 10;
    const int kDefaultWeight = 1;

    class MatchmakerGUI: public ProblemHandler {
    public:
        MatchmakerGUI(GWindow& window);

        /* Forward to the relevant listener. */
        void mouseDoubleClicked(double x, double y) override;
        void mouseMoved(double x, double y) override;
        void mousePressed(double x, double y) override;
        void mouseDragged(double x, double y) override;
        void mouseReleased(double x, double y) override;

        void actionPerformed(GObservable* source) override;
        void changeOccurredIn(GObservable* source) override;
        void windowResized() override;

        bool shuttingDown() override;

    protected:
        void repaint() override;

    private:
        shared_ptr<GraphEditor::Editor<>> editor;

        /* Current matching, if any. */
        unique_ptr<Set<Pair>> currMatching;

        /* Panel layout:
         *
         * [name of file] [              delete button                  ]
         * [new  button ] [              weight slider                  ]
         * [save button ]
         * [load button ]
         *
         * [ find perfect matching ][ max-weight matching  ]
         */
        Temporary<GContainer> controls;
        GLabel*  fileLabel;
        GButton* newButton;
        GButton* saveButton;
        GButton* loadButton;
        GButton* deleteButton;

        GContainer* graphControls;
        GButton* perfectMatchButton;
        GButton* maxWeightMatchButton;
        GSlider* edgeWeightSlider;

        void dirty(bool isDirty = true);
        void entitySelected(GraphEditor::Entity* entity);
        void entityCreated(GraphEditor::Entity* entity);

        void initEditor(shared_ptr<GraphEditor::Viewer<>> viewer);
        void initChrome();

        void deleteSelected();
        void setEditorBounds();

        void drawWelcomeMessage();
        void drawInstructions();

        GRectangle contentArea();

        void userLoad();
        void load(const string& filename);

        void newGraph();
        void userNewGraph();

        bool userSave();
        void save();

        bool handleUnsavedChanges();

        void findPerfectMatching();
        void findMaxWeightMatching();

        void drawGraph();

        /* Is anything selected? */
        bool somethingSelected = false;

        /* Dirty bit. */
        bool isDirty = false;

        /* Current filename. */
        string currFilename;

        /* Whether this is a new file. */
        bool isNew = false;
    };

    MatchmakerGUI::MatchmakerGUI(GWindow& window) : ProblemHandler(window) {
        initChrome();
        dirty(false);
    }

    void MatchmakerGUI::initChrome() {
        GContainer* leftPanel = new GContainer(GContainer::LAYOUT_FLOW_VERTICAL);
        fileLabel = new GLabel("Choose a Graph");
        newButton  = new GButton("New Graph");
        saveButton = new GButton("Save Graph");
        saveButton->setEnabled(false);
        loadButton = new GButton("Load Graph");

        leftPanel->add(fileLabel);
        leftPanel->add(newButton);
        leftPanel->add(saveButton);
        leftPanel->add(loadButton);

        deleteButton = new GButton("Delete");
        perfectMatchButton   = new GButton("Find Perfect Matching");
        maxWeightMatchButton = new GButton("Find Max-Weight Matching");

        edgeWeightSlider = new GSlider(kMinWeight, kMaxWeight, kDefaultWeight);

        controls = make_temporary<GContainer>(window(), "SOUTH", GContainer::LAYOUT_GRID);
        controls->addToGrid(leftPanel, 0, 0, 3, 1);
        controls->addToGrid(deleteButton, 1, 1);

        graphControls = new GContainer();
        graphControls->add(perfectMatchButton);
        graphControls->add(maxWeightMatchButton);
        graphControls->add(new GLabel("Edge weight: "));
        graphControls->add(edgeWeightSlider);
        controls->addToGrid(graphControls, 4, 0, 1, 2);

        graphControls->setEnabled(false);

        controls->setWidth(window().getWidth() * 0.9);

        /* Clarify that nothing is currently selected. */
        entitySelected(nullptr);
    }

    void MatchmakerGUI::initEditor(shared_ptr<GraphEditor::Viewer<>> viewer) {
        editor = make_shared<GraphEditor::Editor<>>(viewer);

        class Listener: public GraphEditor::Listener {
        public:
            Listener(MatchmakerGUI* owner) : owner(owner) {

            }

            void needsRepaint() {
                owner->requestRepaint();
            }
            void isDirty() {
                owner->dirty();
            }
            void entitySelected(GraphEditor::Entity* entity) {
                owner->entitySelected(entity);
            }
            void entityCreated(GraphEditor::Entity* entity) {
                owner->entityCreated(entity);
            }

        private:
            MatchmakerGUI* owner;
        };

        editor->addListener(make_shared<Listener>(this));
        setEditorBounds();
        entitySelected(nullptr);
        dirty(false);
        currMatching.reset();
        graphControls->setEnabled(true);
        requestRepaint();
    }

    void MatchmakerGUI::mouseDoubleClicked(double x, double y) {
        if (editor) editor->mouseDoubleClicked(x, y);
    }
    void MatchmakerGUI::mouseMoved(double x, double y) {
        if (editor) editor->mouseMoved(x, y);
    }
    void MatchmakerGUI::mousePressed(double x, double y) {
        if (editor) editor->mousePressed(x, y);
    }
    void MatchmakerGUI::mouseDragged(double x, double y) {
        if (editor) editor->mouseDragged(x, y);
    }
    void MatchmakerGUI::mouseReleased(double x, double y) {
        if (editor) editor->mouseReleased(x, y);
    }

    void MatchmakerGUI::repaint() {
        clearDisplay(window(), kBackgroundColor);
        if (editor) {
            if (editor->viewer()->numNodes() == 0) {
                drawInstructions();
            } else {
                drawGraph();
            }
        } else {
            drawWelcomeMessage();
        }
    }

    void MatchmakerGUI::entitySelected(GraphEditor::Entity* entity) {
        somethingSelected = !!entity;
        deleteButton->setEnabled(somethingSelected);

        /* If this is an edge, adjust the weight slider. */
        if (auto* edge = dynamic_cast<GraphEditor::Edge*>(entity)) {
            edgeWeightSlider->setValue(stringToInteger(edge->label()));
        }

        /* Clear any displayed matching when something is selected so that
         * the colors are correct.
         */
        currMatching.reset();
    }

    void MatchmakerGUI::entityCreated(GraphEditor::Entity* entity) {
        /* Nodes need names. */
        if (auto* node = dynamic_cast<GraphEditor::Node*>(entity)) {
            node->label(string(1, 'a' + node->index()));
        }
        /* Edges need weights. */
        if (auto* edge = dynamic_cast<GraphEditor::Edge*>(entity)) {
            edge->label(to_string(edgeWeightSlider->getValue()));
        }
    }

    void MatchmakerGUI::actionPerformed(GObservable* source) {
        if (source == loadButton) {
            userLoad();
        } else if (source == newButton) {
            userNewGraph();
        } else if (editor && source == saveButton) {
            userSave();
        } else if (editor && source == deleteButton) {
            if (somethingSelected) deleteSelected();
        } else if (editor && source == perfectMatchButton) {
            findPerfectMatching();
        } else if (editor && source == maxWeightMatchButton) {
            findMaxWeightMatching();
        }
    }

    /* Dragging the slider changes the edge weight. */
    void MatchmakerGUI::changeOccurredIn(GObservable* source) {
        if (editor && editor->selectedEdge() && source == edgeWeightSlider) {
            /* See if this changed the value. If so, we need to mark the graph as
             * dirty.
             */
            int nextValue = edgeWeightSlider->getValue();
            int currValue = stringToInteger(editor->selectedEdge()->label());

            if (nextValue != currValue) {
                editor->selectedEdge()->label(to_string(edgeWeightSlider->getValue()));
                requestRepaint();
                dirty();
            }
        }
    }

    void MatchmakerGUI::deleteSelected() {
        if (editor->selectedNode()) {
            editor->deleteNode(editor->selectedNode());
        } else if (editor->selectedEdge()) {
            editor->deleteEdge(editor->selectedEdge());
        } else {
            error("Something is selected, but no node or edge is selected?");
        }
    }

    void MatchmakerGUI::setEditorBounds() {
        if (editor) editor->viewer()->setBounds(contentArea());
    }

    void MatchmakerGUI::windowResized() {
        setEditorBounds();
        ProblemHandler::windowResized();
    }

    void MatchmakerGUI::drawWelcomeMessage() {
        auto render = TextRender::construct(kWelcome, contentArea(), kWelcomeFont);
        render->alignCenterVertically();
        render->alignCenterHorizontally();
        render->draw(window());
    }

    void MatchmakerGUI::drawInstructions() {
        auto render = TextRender::construct(kInstructions, contentArea(), kInstructionsFont);
        render->alignCenterVertically();
        render->alignCenterHorizontally();
        render->draw(window());
    }

    GRectangle MatchmakerGUI::contentArea() {
        return { 0, 0, window().getCanvasWidth(), window().getCanvasHeight() };
    }

    /* Serializes the current state of things. */
    void MatchmakerGUI::save() {
        /* TODO: Don't overwrite the source until the write has finished.
         * Use mkstemp, write there, and then do a move when done.
         */
        ofstream output(currFilename);
        if (!output) error("Cannot open " + currFilename + " for writing.");

        output << editor->viewer()->toJSON();
    }

    bool MatchmakerGUI::userSave() {
        if (isNew) {
            string newName = GFileChooser::showSaveDialog(&window(), "Save File", kBaseDir, "*" + kFileExtension);
            if (newName == "") return false;

            currFilename = newName;
            fileLabel->setLabel(getTail(currFilename));

            /* Not a new file anymore - we just picked the file! */
            isNew = false;
        }

        save();

        dirty(false);
        GOptionPane::showMessageDialog(&window(), "Graph " + currFilename + " was saved!");
        return true;
    }

    bool MatchmakerGUI::handleUnsavedChanges() {
        if (!isDirty) return true;

        auto result = GOptionPane::showConfirmDialog(&window(), kUnsavedChanges, kUnsavedChangesTitle, GOptionPane::CONFIRM_YES_NO_CANCEL);

        /* A firm "no" means "okay, I want to discard things. */
        if (result == GOptionPane::CONFIRM_NO) {
            return true;
        }

        /* "Cancel" means "wait, hold on, I didn't mean to do that. */
        if (result == GOptionPane::CONFIRM_CANCEL) {
            return false;
        }

        /* Otherwise, they intend to save. */
        return userSave();
    }

    void MatchmakerGUI::userNewGraph() {
        if (!handleUnsavedChanges()) return;

        newGraph();
        saveButton->setEnabled(true);
    }

    void MatchmakerGUI::newGraph() {
        currFilename = kUntitledGraph;

        auto viewer = make_shared<GraphEditor::Viewer<>>();
        viewer->type(GraphEditor::Type::UNDIRECTED);

        initEditor(viewer);

        isNew = true;
        dirty();
    }

    void MatchmakerGUI::load(const string& filename) {
        currFilename = filename;
        fileLabel->setLabel(getTail(currFilename));

        ifstream input(filename);
        if (!input) throw runtime_error("Error opening file: " + filename);
        initEditor(make_shared<GraphEditor::Viewer<>>(JSON::parse(input)));

        isNew = false;
    }

    void MatchmakerGUI::userLoad() {
        /* Warn user about unsaved changes. */
        if (!handleUnsavedChanges()) {
            return;
        }

        /* Ask user to pick a file; don't do anything if they don't pick one. */
        string filename = GFileChooser::showOpenDialog(&window(), "Choose Graph", kBaseDir, "*" + kFileExtension);
        if (filename == "") return;

        load(filename);
        saveButton->setEnabled(true);
    }

    void MatchmakerGUI::dirty(bool dirtyBit) {
        if (dirtyBit) {
            if (!isDirty) {
                isDirty = true;
                fileLabel->setText(getTail(currFilename) + "*");
            }
        } else {
            if (isDirty) {
                isDirty = false;
                fileLabel->setText(getTail(currFilename));
            }
        }
    }

    bool MatchmakerGUI::shuttingDown() {
        return handleUnsavedChanges();
    }

    void MatchmakerGUI::findPerfectMatching() {
        /* Extract the graph. */
        Map<string, Set<string>> graph;
        auto g = editor->viewer();

        /* Install nodes. */
        g->forEachNode([&](GraphEditor::Node* node) {
            graph[node->label()] = {};
        });

        /* Install edges. */
        g->forEachEdge([&](GraphEditor::Edge* edge) {
            auto src = edge->from()->label();
            auto dst = edge->to()->label();

            graph[src] += dst;
            graph[dst] += src;
        });

        /* Deselect everything; we aren't working on nodes/edges right now.
         *
         * This must come first, since this has the effect of clearing any
         * existing matching.
         */
        editor->setActive(nullptr);

        Set<Pair> matching;
        if (hasPerfectMatching(graph, matching)) {
            /* Store this matching. */
            currMatching.reset(new Set<Pair>(matching));

            /* We need to redraw. */
            requestRepaint();
        } else {
            currMatching.reset();
            requestRepaint();

            GOptionPane::showMessageDialog(&window(), kNoPerfectMatching, kNoPerfectMatchingTitle);
        }
    }

    void MatchmakerGUI::findMaxWeightMatching() {
        /* Extract the graph. */
        Map<string, Map<string, int>> graph;
        auto g = editor->viewer();

        /* Install nodes. */
        g->forEachNode([&](GraphEditor::Node* node) {
            graph[node->label()] = {};
        });

        /* Install edges. */
        g->forEachEdge([&](GraphEditor::Edge* edge) {
            auto src = edge->from()->label();
            auto dst = edge->to()->label();

            graph[src][dst] = stringToInteger(edge->label());
            graph[dst][src] = stringToInteger(edge->label());
        });

        /* Deselect everything; we aren't working on nodes/edges right now.
         *
         * This must come first, since this has the effect of clearing any
         * existing matching.
         */
        editor->setActive(nullptr);

        /* Store this matching. */
        currMatching.reset(new Set<Pair>(maximumWeightMatching(graph)));

        /* We need to redraw. */
        requestRepaint();
    }

    const string kMatchedColor   = "#ffd320";     // Slide highlight color
    const string kMatchedBorderColor = "#000000"; // Black

    const string kUnmatchedColor = "#e0e0e0";       // Gray
    const string kUnmatchedBorderColor = "#c0c0c0"; // Darker Gray


    void MatchmakerGUI::drawGraph() {
        unordered_map<GraphEditor::Node*, GraphEditor::NodeStyle> nodeStyles;
        unordered_map<GraphEditor::Edge*, GraphEditor::EdgeStyle> edgeStyles;

        /* If we have a matching, color-code nodes appropriately. */
        if (currMatching) {
            auto graph = editor->viewer();

            /* Initially, all nodes draw as unmatched. */
            GraphEditor::NodeStyle unmatchedNodeStyle;
            unmatchedNodeStyle.borderColor = kUnmatchedBorderColor;
            unmatchedNodeStyle.fillColor   = kUnmatchedColor;
            unmatchedNodeStyle.textColor   = kUnmatchedBorderColor;

            graph->forEachNode([&](GraphEditor::Node* node) {
                nodeStyles[node] = unmatchedNodeStyle;
            });

            /* Initially, all edges are unmatched. */
            GraphEditor::EdgeStyle unmatchedEdgeStyle;
            unmatchedEdgeStyle.lineColor     = kUnmatchedBorderColor;
            unmatchedEdgeStyle.labelColor = kUnmatchedBorderColor;

            graph->forEachEdge([&](GraphEditor::Edge* edge) {
                edgeStyles[edge] = unmatchedEdgeStyle;
            });

            /* Highlight matched nodes and edges. */
            GraphEditor::NodeStyle matchedNodeStyle;
            matchedNodeStyle.borderColor = kMatchedBorderColor;
            matchedNodeStyle.fillColor   = kMatchedColor;
            matchedNodeStyle.lineWidth  *= 2;

            GraphEditor::EdgeStyle matchedEdgeStyle;
            matchedEdgeStyle.lineColor      = kMatchedBorderColor;
            matchedEdgeStyle.lineWidth *= 2;

            for (Pair p: *currMatching) {
                auto src = graph->nodeLabeled(p.first());
                auto dst = graph->nodeLabeled(p.second());

                nodeStyles[src] = matchedNodeStyle;
                nodeStyles[dst] = matchedNodeStyle;

                auto edge = graph->edgeBetween(src, dst);
                edgeStyles[edge] = matchedEdgeStyle;
            }

        }

        editor->draw(window().getCanvas(), nodeStyles, edgeStyles);
    }
}

GRAPHICS_HANDLER("Matchmaker", GWindow& window) {
    return make_shared<MatchmakerGUI>(window);
}
