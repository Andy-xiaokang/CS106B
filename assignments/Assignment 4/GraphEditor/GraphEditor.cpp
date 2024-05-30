#include "GraphEditor.h"
#include "GVector.h"

namespace GraphEditor {
    namespace {
        /* Active state is displayed with a highlight color. */
        const std::string kActiveStateColor = "#ffd320"; // Slide highlight color

        /* Hovered state is displayed with a thicker, special border. */
        const std::string kHoverBorderColor = "blue";
        const double kHoverBorderWidth = 16.0 / 1000; // 8px on a 1000px window

        /* How far, in radians, you need to travel before it counts as a self-loop. */
        const double kSelfTransitionThreshold = M_PI / 3;

        const double kNewTransitionWidth = 3.0 / 1000; // 3px on 1000px window
        const std::string kNewTransitionColor = "red";

        const std::string kActiveTransitionColor = "#ff950e";
        const double kActiveTransitionWidth = GraphEditor::kEdgeTolerance;
        const std::string kHoverTransitionColor = "blue"; // Slide highlight dark color
    }

    EditorBase::EditorBase(std::shared_ptr<ViewerBase> viewer) : mViewer(viewer) {
        // Handled above
    }

    void EditorBase::setActive(Entity* active) {
        if (auto* node = dynamic_cast<Node*>(active)) {
            setActiveNode(node);
        } else if (auto* edge = dynamic_cast<Edge*>(active)) {
            setActiveEdge(edge);
        } else {
            setActiveNode(nullptr);
            setActiveEdge(nullptr);
        }

        /* Let folks know about this one. */
        for (auto listener: mListeners) {
            listener->entitySelected(active);
        }
    }

    void EditorBase::setHover(Entity* hover) {
        if (auto* node = dynamic_cast<GraphEditor::Node*>(hover)) {
            setHoverNode(node);
        } else if (auto* edge = dynamic_cast<GraphEditor::Edge*>(hover)) {
            setHoverEdge(edge);
        } else {
            setHoverNode(nullptr);
            setHoverEdge(nullptr);
        }

        /* Let folks know about this one. */
        for (auto listener: mListeners) {
            listener->entityHovered(hover);
        }
    }

    void EditorBase::setActiveNode(Node* state) {
        if (activeNode != state) requestRepaint();
        activeNode = state;

        if (activeNode) {
            activeEdge = nullptr;
        }
    }

    void EditorBase::setActiveEdge(Edge* transition) {
        if (activeEdge != transition) requestRepaint();
        activeEdge = transition;

        if (activeEdge != nullptr) {
            activeNode = nullptr;
        }
    }

    void EditorBase::setHoverNode(Node* state) {
        if (hoverNode != state) requestRepaint();
        hoverNode = state;

        if (hoverNode) {
            hoverEdge = nullptr;
        }
    }

    void EditorBase::setHoverEdge(Edge* transition) {
        if (hoverEdge != transition) requestRepaint();
        hoverEdge = transition;

        if (hoverEdge) {
            hoverNode = nullptr;
        }
    }

    void EditorBase::mouseDoubleClicked(double x, double y) {
        GPoint pos = mViewer->graphicsToWorld(GPoint{x, y});

        /* Anything there? If so, don't do anything. */
        if (mViewer->nodeAt(pos)) return;
        if (mViewer->edgeAt(pos)) return;

        auto node = newNode(pos);
        for (auto listener: mListeners) {
            listener->entityCreated(node);
        }

        setHover(node);
        setActive(node);
        requestRepaint();
        dirty();
    }

    void EditorBase::mouseMoved(double x, double y) {
        /* Skip this if we're dragging the mouse. */
        if (dragType != DragType::NONE) return;

        GPoint pt = mViewer->graphicsToWorld(GPoint{x, y});

        /* See if we hit a state. */
        if (auto over = mViewer->nodeAt(pt)) {
            setHover(over);
        } else if (auto over = mViewer->edgeAt(pt)) {
            setHover(over);
        } else {
            setHover(nullptr);
        }
    }

    namespace {
        bool isCloseTo(GPoint p0, GPoint p1, double distance) {
            double dx = p0.x - p1.x;
            double dy = p0.y - p1.y;

            return dx * dx + dy * dy <= distance * distance;
        }
    }

    void EditorBase::mousePressed(double x, double y) {
        GPoint pt = mViewer->graphicsToWorld(GPoint{x, y});

        /* Did we hit a state? */
        if (auto over = mViewer->nodeAt(pt)) {
            /* Forcibly set this to be the overed item as well. This prevents
             * us from crashing or getting into an inconsistent state in the
             * event that the mousePressed event happened without an intervening
             * mouseMoved event.
             */
            setHover(over);
            setActive(over);

            /* Compute the distance to the center of the state. */
            if (isCloseTo(pt, hoverNode->position(), GraphEditor::kNodeRadius - GraphEditor::kEdgeTolerance)) {
                /* Initiate a state drag. */
                lastState = pt;
                dragType = DragType::NODE;
            } else {
                dragEdge0 = dragEdge1 = pt;
                edgeStart = hoverNode;
                dragType = DragType::EDGE;
            }
        }
        /* Did we hit a transition? */
        else if (auto over = mViewer->edgeAt(pt)) {
            setActive(over);
        }
        /* Didn't hit anything. */
        else {
            setActive(nullptr);
        }
    }

    void EditorBase::dragState(GPoint pt) {
        /* TODO: Collisions with other states? */
        if (hoverNode) {
            hoverNode->position(hoverNode->position() + (lastState - hoverNode->position()));
            lastState = pt;
            requestRepaint();
            dirty();
        }
    }

    void EditorBase::dragTransition(GPoint pt) {
        dragEdge1 = pt;

        setHover(mViewer->nodeAt(pt));
        requestRepaint();
    }

    void EditorBase::mouseDragged(double x, double y) {
        if (dragType == DragType::NODE) {
            dragState(mViewer->graphicsToWorld(GPoint{x, y}));
        } else if (dragType == DragType::EDGE) {
            dragTransition(mViewer->graphicsToWorld(GPoint{x, y}));
        }
    }

    void EditorBase::mouseReleased(double x, double y) {
        if (dragType == DragType::EDGE) {
            finishCreatingEdge(mViewer->graphicsToWorld(GPoint{ x, y }));
        }
        dragType = DragType::NONE;
        requestRepaint();
    }

    void EditorBase::finishCreatingEdge(GPoint pt) {
        /* For starters, see what we hit. */
        auto end = mViewer->nodeAt(pt);

        /* If this isn't a state, there's nothing to do. */
        if (end == nullptr) {
            edgeStart = nullptr;
            return;
        }

        /* If this is the same state, confirm that we dragged enough for this
         * to count as a self-loop.
         */
        if (end == edgeStart) {
            double theta0 = angleOf(dragEdge0 - edgeStart->position());
            double theta1 = angleOf(dragEdge1 - edgeStart->position());

            /* Each are in (-pi, pi], so the difference is in [-2pi, 2pi]. We want something in
             * (-pi, pi]. To do this, first get us in [0, 2pi].
             */
            double thetaDiff = theta0 - theta1;
            if (thetaDiff < 0) {
                thetaDiff = fmod(thetaDiff + 2 * M_PI, 2 * M_PI);
            }

            /* Now, shift us to [-pi, pi] from [0, 2pi]. */
            if (thetaDiff > M_PI) {
                thetaDiff -= 2 * M_PI;
            }

            if (fabs(thetaDiff) < kSelfTransitionThreshold) {
                edgeStart = nullptr;
                return;
            }
        }

        /* If the edge already exists, select it and do nothing. */
        auto* edge = edgeBetween(edgeStart, end);
        if (!edge) {
            edge = newEdge(edgeStart, end);
            for (auto listener: mListeners) {
                listener->entityCreated(edge);
            }
            dirty();
        }

        setActive(edge);
    }

    void EditorBase::dirty() {
        for (auto listener: mListeners) {
            listener->isDirty();
        }
    }

    void EditorBase::drawGraph(GCanvas* canvas,
                               const std::unordered_map<Node*, NodeStyle>& clientNodeStyles,
                               const std::unordered_map<Edge*, EdgeStyle>& clientEdgeStyles) {
        /* Configure styles. */
        std::unordered_map<Node*, NodeStyle> nodeStyles = clientNodeStyles;
        std::unordered_map<Edge*, EdgeStyle> edgeStyles = clientEdgeStyles;

        /* Active and hover states are NOT mutually exclusive! */
        if (activeNode && !clientNodeStyles.count(activeNode)) {
            nodeStyles[activeNode].fillColor = kActiveStateColor;
        }
        if (hoverNode && !clientNodeStyles.count(hoverNode)) {
            nodeStyles[hoverNode].borderColor = kHoverBorderColor;
            nodeStyles[hoverNode].lineWidth   = kHoverBorderWidth;
            nodeStyles[hoverNode].radius     -= kHoverBorderWidth / 2.0;
        }

        /* Active transition always takes precedence over hover transition. */
        if (hoverEdge && !clientEdgeStyles.count(hoverEdge)) {
            edgeStyles[hoverEdge].lineColor = kHoverTransitionColor;
            edgeStyles[hoverEdge].lineWidth = GraphEditor::kEdgeTolerance;
        }
        if (activeEdge && !clientEdgeStyles.count(activeEdge)) {
            edgeStyles[activeEdge].lineColor = kActiveTransitionColor;
            edgeStyles[activeEdge].lineWidth = kActiveTransitionWidth;
        }

        mViewer->draw(canvas, nodeStyles, edgeStyles);
    }

    void EditorBase::drawDraggedEdge(GCanvas* canvas) {
        if (dragType == DragType::EDGE) {
            mViewer->drawArrow(canvas, dragEdge0, dragEdge1, kNewTransitionWidth, kNewTransitionColor);
        }
    }

    void EditorBase::draw(GCanvas* canvas,
                          const std::unordered_map<Node*, NodeStyle>& nodeStyles,
                          const std::unordered_map<Edge*, EdgeStyle>& edgeStyles) {
        drawGraph(canvas, nodeStyles, edgeStyles);
        drawDraggedEdge(canvas);
    }

    void EditorBase::requestRepaint() {
        for (auto listener: mListeners) {
            listener->needsRepaint();
        }
    }

    void EditorBase::addListener(std::shared_ptr<Listener> listener) {
        mListeners.push_back(listener);
    }

    Node* EditorBase::selectedNode() {
        return activeNode;
    }

    Edge* EditorBase::selectedEdge() {
        return activeEdge;
    }

    /**** Default listener interface. ****/
    void Listener::isDirty() {
        // Do nothing
    }

    void Listener::needsRepaint() {
        // Do nothing
    }

    void Listener::entityHovered(Entity *) {
        // Do nothing
    }

    void Listener::entitySelected(Entity *) {
        // Do nothing
    }

    void Listener::entityCreated(Entity *) {
        // Do nothing
    }
}
