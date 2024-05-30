#include "GraphViewer.h"
#include "../GUI/MiniGUI.h"
#include "GVector.h"
#include "Utilities/Unicode.h"
#include <cmath>
#include <set>
#include <unordered_map>
#include <sstream>
#include <cctype>
using namespace MiniGUI;

namespace GraphEditor {
    namespace {
        /* Useful, not required. */
        const std::string kNonbreakingSpace = toUTF8(0xA0);

        /* Edge graphics parameters. */
        const double kLoopEdgeRadius = GraphEditor::kNodeRadius * 0.75;

        /* Length of the invisible line on which to draw the contents of a loop edge. */
        const double kLoopLabelLength = 150 / 1000.0;

        /* Font and height for edges. */
        const double kEdgeTextHeight = 48.0 / 1000; // 24pt in 1000px window

        /* Amount to offset the label by relative to the edge. */
        const double kEdgeLabelYOffset = 8.0 / 1000;
        const double kLoopEdgeYOffset  = 30.0 / 1000;

        /* How much, in radians, to rotate the origin points of the states when shifting
         * start positions of edges.
         */
        const double kAvoidanceRotation = -M_PI / 6;

        /* Arrowhead parameters. */
        const double kArrowheadRotation = M_PI / 8;
        const double kArrowheadSize     = 0.02;

        /* Parameters for avoiding collisions with self-loops. */
        const int kLowAngle = -5;
        const int kHighAngle = 355;
        const int kAngleStep = 10;
    }

    /* Edges can be either line edges or loop edges. */
    struct EdgeRender {
        EdgeRender(ViewerBase* editor, Edge* edge): editor(editor), edge(edge) {}
        virtual ~EdgeRender() = default;

        virtual void draw(GCanvas* canvas, double thickness, const std::string& lineColor, const std::string& textColor) const = 0;
        virtual bool contains(const GPoint& pt) const = 0;

        ViewerBase* editor;
        Edge* edge;
    };

    /* Linear edge. */
    struct LineEdge: EdgeRender {
        LineEdge(ViewerBase* editor, Edge* edge, GPoint from, GPoint to) : EdgeRender(editor, edge), lineStart(from), lineEnd(to) {}

        void draw(GCanvas* canvas, double thickness, const std::string& lineColor, const std::string& textColor) const override;
        bool contains(const GPoint& pt) const override;

        GPoint lineStart, lineEnd;
    };

    /* Self-loop. */
    struct LoopEdge: EdgeRender {
        LoopEdge(ViewerBase* editor, Edge* edge, const GPoint& center, const GPoint& arrowPt) : EdgeRender(editor, edge), center(center), arrowPt(arrowPt) {}

        void draw(GCanvas* canvas, double thickness, const std::string& lineColor, const std::string& textColor) const override;
        bool contains(const GPoint& pt) const override;

        /* Edge is represented by a circle. Where is the center of that
         * circle?
         */
        GPoint center;

        /* Point where the arrowhead is drawn. */
        GPoint arrowPt;
    };

    void ViewerBase::draw(GCanvas* canvas,
                      const std::unordered_map<Node*, NodeStyle>& nodeStyles,
                      const std::unordered_map<Edge*, EdgeStyle>& edgeStyles) {
        /* TODO: This is for testing purposes. Please remove this. */
        canvas->setColor("red");
        canvas->drawRect(baseX, baseY, width, height);

        /* Existing edges underdraw the nodes so we don't see the lines. */
        for (auto start: edges) {
            for (auto end: start.second) {
                /* We could have null entries; skip them. */
                /* TODO: Is this true? */
                if (end.second) {
                    auto style = edgeStyles.count(end.second.get()) ? edgeStyles.at(end.second.get()) : EdgeStyle();
                    end.second->style->draw(canvas, style.lineWidth, style.lineColor, style.labelColor);
                }
            }
        }

        /* Nodes. */
        for (auto node: nodes) {
            auto style = nodeStyles.count(node.get())? nodeStyles.at(node.get()) : NodeStyle();

            node->draw(this, canvas, style);
        }
    }

    namespace {
        bool isCloseTo(GPoint p0, GPoint p1, double distance) {
            double dx = p0.x - p1.x;
            double dy = p0.y - p1.y;

            return dx * dx + dy * dy <= distance * distance;
        }
    }

    double ViewerBase::graphicsToWorld(double width) {
        return width / this->width;
    }
    GPoint ViewerBase::graphicsToWorld(GPoint in) {
        return { (in.x - baseX) / width, (in.y - baseY) / width };
    }
    GRectangle ViewerBase::graphicsToWorld(GRectangle in) {
        auto top = graphicsToWorld(GPoint{ in.x, in.y });
        auto bot = graphicsToWorld(GPoint{ in.x + in.width, in.y + in.height });
        return { top.x, top.y, bot.x - top.x, bot.y - top.y };
    }

    double ViewerBase::worldToGraphics(double width) {
        return width * this->width;
    }
    GPoint ViewerBase::worldToGraphics(GPoint in) {
        return { in.x * width + baseX, in.y * width + baseY };
    }
    GRectangle ViewerBase::worldToGraphics(GRectangle in) {
        auto top = worldToGraphics(GPoint{ in.x, in.y });
        auto bot = worldToGraphics(GPoint{ in.x + in.width, in.y + in.height });
        return { top.x, top.y, bot.x - top.x, bot.y - top.y };
    }

    /* All parameters are in world coordinates. */
    void ViewerBase::drawArrow(GCanvas* canvas, const GPoint& from, const GPoint& to,
                               double thickness, const std::string& color) {
        GLine line(worldToGraphics(from), worldToGraphics(to));
        line.setLineWidth(ceil(thickness * width));
        line.setColor(color);

        canvas->draw(&line);

        drawArrowhead(canvas, from, to, thickness, color);
    }

    void ViewerBase::drawArrowhead(GCanvas* canvas, const GPoint& from, const GPoint& to,
                                   double thickness, const std::string& color) {
        /* Skip arrowheads if graph is undirected. */
        if (type() == Type::UNDIRECTED) return;

        /* Draw the arrowheads. First, get a vector pointing from end to start so that
         * we can shift it around to compute the endpoints.
         */
        GVector v = normalizationOf(from - to);

        /* Compute the two endpoints. */
        GPoint left  = to + rotate(v, kArrowheadRotation)  * kArrowheadSize;
        GPoint right = to + rotate(v, -kArrowheadRotation) * kArrowheadSize;

        GLine line(worldToGraphics(left), worldToGraphics(to));
        line.setLineWidth(ceil(thickness * width));
        line.setColor(color);
        canvas->draw(&line);

        line.setStartPoint(worldToGraphics(right));
        canvas->draw(&line);
    }

    namespace {
        /* Given a quadratic equation, returns whether there are any solutions that
         * correspond to a line/circle intersection. This happens if solutions exist
         * UNLESS both intersections are less than zero or both intersections are
         * greater than one.
         */
        size_t quadraticSolnsInRange(double a, double b, double c) {
            double discriminant = b * b - 4 * a * c;
            if (discriminant < 0) return 0;

            double x1 = (-b + sqrt(discriminant)) / (2 * a);
            double x2 = (-b - sqrt(discriminant)) / (2 * a);

            return !((x1 < 0 && x2 < 0) || (x1 > 1 && x2 > 1));
        }

        /* Counts collisions a circle and a collection of lines. */
        size_t collisionsBetween(const GPoint& center, double radius,
                                 const std::vector<std::pair<GPoint, GPoint>>& lines) {
            /* Any point (x, y) on a circle satisfies
             *
             *    (x - x_c)^2 + (y - y_c)^2 = r^2.
             *
             * Any point on the line from p0 to p1 has parametric form
             *
             *    (x(t), y(t)) = p0 + t(p1 - p0).
             *
             * Substituting, we get
             *
             *    (p0x + t(p1x - p0x) - x_c)^2 + (p0y + t(p1y - p0y) - y_c)^2 = r^2
             *
             * Everything here except for t is a constant. Isolating t and simplifying, we get
             *
             *    (t(p1x - p0x) + p0x - x_c)^2 + (t(p1y - p0y) + p0y - y_c)^2 = r^2
             *       --- dx --                     --- dy ---
             *
             *    (t * dx + p0x - x_c)^2 + (t * dy + p0y - y_c)^2 = r^2
             *              --- sx --                -- sy ---
             *
             *    (t * dx + sx)^2 + (t * dy + sy)^2 = r^2
             *
             *    ((dx)^2 t^2 + 2*dx*sx*t + (sx)^2) + ((dy)^2 t^2 + 2*dy*dy*t + (sy)^2) = r^2
             *
             *    ((dx)^2 + (dy)^2) t^2 + 2(dx*sx + dy*sy) t + ((sx)^2 + (sy)^2 - r^2) = 0
             *        dot(d, d) t^2 + 2 dot(d, s) t + (dot(s, s) - r^2) = 0
             *
             * At this point this is a quadratic, and we just need to count solutions.
             */
            size_t solns = 0;
            for (const auto& line: lines) {
                GVector d = line.second - line.first;
                GVector s = line.first - center;

                solns += quadraticSolnsInRange(dot(d, d), 2 * dot(d, s), dot(s, s) - radius * radius);
            }
            return solns;
        }

        size_t collisionsBetween(const GPoint&, double,
                                 const std::vector<std::pair<GPoint, double>>&) {
            /* TODO: Implement this function to count circle/circle collisions. */
            return 0;
        }

        /* Given a collection of circles and lines and a new circle, determines how many
         * collisions there are. This counts the number of colliding ENTITIES, not the
         * number of collision points overall.
         */
        size_t collisionsBetween(const GPoint& center, double radius,
                                 const std::vector<std::pair<GPoint, GPoint>>& lines,
                                 const std::vector<std::pair<GPoint, double>>& circles) {
            return collisionsBetween(center, radius, lines) +
                   collisionsBetween(center, radius, circles);
        }

        /* Determines the best angle at which to orient a self-loop, which is one that
         * hits the fewest other objects.
         */
        double bestThetaFor(const GPoint& nodeCenter, const std::vector<std::pair<GPoint, GPoint>>& lines,
                            const std::vector<std::pair<GPoint, double>>& circles) {
            /* Our algorithm for placing the circle goes as follows. We iterate over a fixed
             * number of potential angles that we can use. For each one, we count the number
             * of collisions that would result if we put the circle there, forming an array
             * that you can think of as a "height map" of the collisions.
             *
             * We then find the minimum number of collisions and look for the longest range
             * in the array (remembering to loop back around when we're done!), which
             * corresponds to the widest margin of error we can find. From there, we then
             * pick the midpoint of that range.
             *
             * TODO: It would be a LOT more elegant to do this by using some sort of nice
             * and pretty math instead of trial and error. Can you improve upon this?
             */
            std::vector<std::size_t> collisions;
            for (int degAngle = kLowAngle; degAngle < kHighAngle; degAngle += kAngleStep) {
                double theta = degAngle * M_PI / 180;
                GPoint center = nodeCenter + unitToward(theta) * kNodeRadius;

                collisions.push_back(collisionsBetween(center, kLoopEdgeRadius, lines, circles));
            }

            /* Find the minimum number of collisions. */
            size_t min = *min_element(collisions.begin(), collisions.end());

            /* Find the longest range of minimum values. */
            size_t bestStart  = 0;
            size_t bestLength = 0;

            size_t currStart  = 0;
            size_t currLength = 0;

            /* To handle wraparound, scan backwards over the array and look for the
             * first spot that isn't the min.
             */
            for (size_t i = collisions.size(); i > 0; --i) {
                if (collisions[i - 1] != min) break;

                /* Back up a step. */
                currStart = (currStart + collisions.size() - 1) % collisions.size();
                currLength++;
            }

            for (size_t i = 0; i < collisions.size(); i++) {
                /* Doesn't match? Gotta stop. */
                if (collisions[i] != min) {
                    if (bestLength < currLength) {
                        bestLength = currLength;
                        bestStart  = currStart;
                    }
                    currLength = 0;
                    currStart  = i+1;
                }
                /* Otherwise, extend. */
                else {
                    currLength++;
                }
            }

            /* Handle edge case of finding the best at the end. */
            if (bestLength < currLength) {
                bestLength = currLength;
                bestStart  = currStart;
            }

            /* Casts to double necessary here to avoid bizarre integer overflows as negative
             * angles turn into massively positive unsigned angles!
             */
            double lowTheta   = (kLowAngle + double(bestStart * kAngleStep)) * M_PI / 180;
            double highTheta  = (kLowAngle + double(bestStart + bestLength - 1) * kAngleStep) * M_PI / 180;
            return (lowTheta + highTheta) / 2;
        }

        /* Given the center of a node and the point at which the loop is centered, returns
         * a point where they intersect - a place where the arrow can be drawn.
         */
        GPoint loopArrowPointFor(const GPoint& nodeCenter, const GPoint& loopCenter) {
            /* Trig time! We have two circles where one is centered on the border of
             * another. We want to then find one of the intersection points. How do we
             * do it?
             *
             * For now, imagine that everything is colinear, like this:
             *
             *                *
             *               / \
             *           r  /   \ r'
             *             /  r  \
             *            * ----- *
             *          node      loop
             *         center     center
             *
             * We want to know the angle theta that is made between the node center,
             * the loop center, and the third triangle point (their intersection). The
             * Law of Cosines tells us that
             *
             *       r'^2 = r^2 + r^2 - 2r^2 cos theta
             *
             * Which, after some rearranging, gives us
             *
             *       theta = arccos(1 - r'^2 / 2r^2)
             *
             * Use this to get that angle measure.
             */
            double theta = acos(1 - kLoopEdgeRadius * kLoopEdgeRadius / (2 * kNodeRadius * kNodeRadius));

            /* Rotate the vector from the node to loop center by this amount. */
            return nodeCenter + rotate(loopCenter - nodeCenter, theta);
        }

        /* Boundaries of the world, represented as lines. */
        std::vector<std::pair<GPoint, GPoint>> worldBoundaries(double aspectRatio) {
            const double lft = 0;
            const double rgt = 1;
            const double top = 0;
            const double bot = 1 / aspectRatio;

            return {
                { { lft, top }, { rgt, top } },
                { { lft, bot }, { rgt, bot } },
                { { lft, top }, { lft, bot } },
                { { rgt, top }, { rgt, bot } },
            };
        }
    }

    /* Determines where each edge should start and end. There are dependencies
     * across these edges, so we need to do this all at once.
     */
    void ViewerBase::calculateEdgeEndpoints() {
        /* List of all line segments used. */
        std::vector<std::pair<GPoint, GPoint>> lines = worldBoundaries(mAspectRatio);

        /* First, handle linear edges. */
        forEachEdge([&](Edge* edge) {
            if (edge->from() != edge->to()) {
                /* Center coordinates. */
                GPoint p0 = edge->from()->position();
                GPoint p1 = edge->to()->position();

                /* If there is a edge running in the reverse direction, we need to shift
                 * this edge over so that we don't overlap it.
                 *
                 * Skip this if the graph is undirected.
                 */
                if (type() == Type::DIRECTED && hasEdge(edge->to(), edge->from())) {
                    /* Unit vector pointing in the p0 -> p1 direction saying how much we need to rotate. */
                    auto p0Delta = rotate(normalizationOf(p1 - p0), kAvoidanceRotation);

                    /* Unit vector pointing in the p1 -> p0 direction saying how much we need to rotate. */
                    auto p1Delta = rotate(normalizationOf(p0 - p1), -kAvoidanceRotation);

                    /* At the borders. */
                    p0 += p0Delta * kNodeRadius;
                    p1 += p1Delta * kNodeRadius;
                }
                /* Otherwise, translate the center points to the borders. */
                else {
                    p0 += normalizationOf(p1 - p0) * kNodeRadius;
                    p1 += normalizationOf(p0 - p1) * kNodeRadius;
                }

                edge->style = std::make_shared<LineEdge>(this, edge, p0, p1);
                lines.push_back(std::make_pair(p0, p1));
            }
        });

        /* All placed circles. Initially, that's all the nodes. */
        std::vector<std::pair<GPoint, double>> circles;
        for (auto node: nodes) {
            circles.push_back(std::make_pair(node->position(), kNodeRadius));
        }

        /* Now, place all self-loops. */
        forEachEdge([&](Edge* edge) {
            if (edge->from() == edge->to()) {
                double theta = bestThetaFor(edge->from()->position(), lines, circles);

                GPoint center  = edge->from()->position() + unitToward(theta) * kNodeRadius;
                GPoint arrowPt = loopArrowPointFor(edge->from()->position(), center);

                edge->style = std::make_shared<LoopEdge>(this, edge, center, arrowPt);
                circles.push_back(std::make_pair(center, kNodeRadius));
            }
        });
    }

    /* Linear edge implementation. */
    bool LineEdge::contains(const GPoint& pt) const {
        /* Our goal is to see both (1) how far from the line we are and (2) how far
         * down the line we are.
         *
         * To do this, we're going to change coordinates. Let b1 be a vector pointing
         * 90 degrees to the right of the line, and let b2 be a vector pointing down
         * the line. That gives us this coordinate system:
         *
         *              b2
         *              ^
         *              |
         *              |
         *          ----+----> b1
         *
         * Now, we rewrite our cursor's position in this coordinate system. We do this
         * by multiplying the cursor vector by a rotation matrix [b1, b2].
         *
         * From here we can see where we are. To be "close enough," we'll say that the
         * cursor needs to have x coordinate that places it within the hover width,
         * and it needs to have a y coordinate between zero and the length of the line.
         */

        /* Switch coordinate systems to place the origin of the edge at (0, 0). */
        GVector cursor = pt - lineStart;

        /* Get the vectors b1 and b2. */
        GVector lineVec = lineEnd - lineStart;

        GVector b2 = lineVec / magnitudeOf(lineVec);
        GVector b1 = rotate(b2, M_PI / 2);

        /* Compute the cursor's representation in this system. That's
         *
         * | b1x b2x | |cx|
         * | b1y b2y | |cy|
         */
        GVector result = GMatrix(b1, b2) * cursor;

        /* Make sure we're in range. */
        return fabs(result.x) <= kEdgeTolerance / 2.0 &&
               result.y >= 0 && result.y <= magnitudeOf(lineVec);
    }

    namespace {
        /* Is this a space character? */
        bool isSpace(char32_t ch) {
            return ch >= 0 && ch <= 127 && isspace(ch);
        }

        /* Given a string, replaces all the spaces in the string with nonbreaking spaces.
         *
         * TODO: Remove this and replace with LineBreak::NO_BREAK_SPACES.
         */
        std::string toNonbreakingSpaces(const std::string& input) {
            std::string result;
            for (char32_t ch: utf8Reader(input)) {
                if (isSpace(ch)) {
                    result += kNonbreakingSpace;
                } else {
                    result += toUTF8(ch);
                }
            }

            return result;
        }
    }

    void ViewerBase::drawEdgeLabel(GCanvas* canvas,
                                   const GPoint& p0, const GPoint& p1,
                                   const std::string& labelText,
                                   const std::string& color,
                                   bool hugLine) {
        GPoint from = worldToGraphics(p0);
        GPoint to   = worldToGraphics(p1);

        std::string label = toNonbreakingSpaces(labelText);

        /* Determine the length of this line. */
        double length = magnitudeOf(to - from);

        /* Determine what font we should use for the label by computing a text render
         * and extracting the font it uses.
         */
        Font font = TextRender::construct(label, {0, 0, length, width * kEdgeTextHeight }, kEdgeFont.color(color))->computedFont();

        /* Create a graphics object for the label. */
        GText text(label);
        text.setFont(font.stanfordCPPLibFontString());
        text.setColor(font.color());

        /* Figure out where the label needs to go. */
        double theta = angleOf(to - from);

        /* Never draw text upside-down. See whether we go above or below the line. */
        if (theta < -M_PI / 2 || theta > M_PI / 2) {
            /* Below the line. Rotate our angle by 180 degrees and exchange the roles
             * of the endpoints.
             */
            theta += M_PI;
            std::swap(to, from);

            /* If we are supposed to hug the line, we need to do an extra step and shift the
             * line position over so that when we draw on top of it, we appear to have just
             * flipped rather than flipped and translated.
             */
            if (hugLine) {
                /* Get a perpendicular to the line. */
                GVector normal = rotate(normalizationOf(to - from), M_PI / 2) * text.getHeight();
                from += normal;
                to   += normal;
            }
        }

        /* We'll aim to draw on the edge as the baseline,
         * so we need to compute the (x, y) coordinate of the edge center.
         *
         * ... except that it's not the exact center. Rather, it's the center of the
         * edge, offset by half the width of the label. In other words, we want
         * to walk to the center, then advance a bit further
         */
        GPoint target = from + normalizationOf(to - from) * (length - text.getWidth()) / 2.0;

        /* Now, shift up off the line. */
        target += rotate(normalizationOf(to - from), -M_PI / 2) * ceil(width * kEdgeLabelYOffset);

        /* GText behaves strangely when rotated. The rotation is done around
         * the graphics origin point (0, 0) rather than the center of the object.
         * This means that we need to reposition the label so that it rotates the
         * text into the exact position we want.
         *
         * Goal: Given a target position of (x, y), find a position
         * (x', y') such that (x', y') rotates onto (x, y).
         *
         * Given a rotation angle theta, the ACTUAL position where the object
         * will be located is ROT(theta) (x', y').
         *
         * So we want ROT(theta) (x', y') = (x, y), meaning that
         * (x', y') = ROT(-theta)(x, y).
         */
        GPoint textPos = rotation(-theta) * target;

        /* GText rotations are in degrees. */
        text.rotate(theta * 180 / M_PI);
        text.setLocation(textPos);
        canvas->draw(&text);
    }

    Node* ViewerBase::nodeAt(const GPoint& pt) {
        /* TODO: Do we need to do this in reverse so that we pick the
         * topmost node?
         */
        for (auto node: nodes) {
            if (isCloseTo(pt, node->position(), kNodeRadius)) {
                return node.get();
            }
        }

        return nullptr;
    }

    Edge* ViewerBase::edgeAt(const GPoint& pt) {
        for (const auto& one: edges) {
            for (const auto& two: one.second) {
                /* Get the edge itself. */
                auto edge = two.second;

                if (edge->style->contains(pt)) {
                    return edge.get();
                }
            }
        }

        return nullptr;
    }

    void LineEdge::draw(GCanvas* canvas, double thickness, const std::string& lineColor, const std::string& labelColor) const {
        editor->drawArrow(canvas, lineStart, lineEnd, thickness, lineColor);
        editor->drawEdgeLabel(canvas, lineStart, lineEnd, edge->label(), labelColor, false);
    }

    bool LoopEdge::contains(const GPoint& pt) const {
        /* We hit the circle if our distance to the center is within kHover of the
         * actual radius.
         */
        return fabs(magnitudeOf(pt - center) - kLoopEdgeRadius) < kEdgeTolerance;
    }

    void LoopEdge::draw(GCanvas* canvas, double width, const std::string& lineColor, const std::string& labelColor) const {
        double size = 2 * editor->width * kLoopEdgeRadius;
        GPoint pt = editor->worldToGraphics(center);

        GOval toDraw(pt.x - size / 2, pt.y - size / 2, size, size);
        toDraw.setColor(lineColor);
        toDraw.setLineWidth(ceil(editor->width * width));
        canvas->draw(&toDraw);

        /* Draw the arrowhead. You might think that we'd want the arrowhead
         * to appear as though it was entering the node normal to the circle
         * at the intersection point, but, surprisingly, that doesn't look good.
         * Instead, it's better to look like you're hitting the the circle
         * tangent to the line drawn between the node center and the loop
         * center.
         */
        GPoint exterior = arrowPt + (center - edge->from()->position());
        editor->drawArrowhead(canvas, exterior, arrowPt, width, lineColor);


        /* We will draw the edge contents by imagining there's an invisible tangent
         * line to the circle that we'll draw on top of.
         */

        /* Get a vector pointing away from the circle center. */
        GVector out = normalizationOf(center - edge->from()->position());

        /* Move outward to the end of the loop. */
        GPoint tangentPoint = center + out * (kLoopEdgeRadius + kLoopEdgeYOffset);

        /* Construct a perpendicular vector and use it to form a line. */
        GVector tangent = rotate(out, M_PI / 2);
        GPoint p0 = tangentPoint + tangent * kLoopLabelLength / 2;
        GPoint p1 = tangentPoint - tangent * kLoopLabelLength / 2;
        editor->drawEdgeLabel(canvas, p0, p1, edge->label(), labelColor, true);
    }

    void ViewerBase::forEachNode(std::function<void (Node *)> callback) {
        for (const auto& node: nodes) {
            callback(node.get());
        }
    }

    void ViewerBase::forEachEdge(std::function<void (Edge *)> callback) {
        for (const auto& e1: edges) {
            for (const auto& e2: e1.second) {
                callback(e2.second.get());
            }
        }
    }

    bool ViewerBase::hasEdge(Node* from, Node* to) {
        return edgeBetween(from, to) != nullptr;
    }

    Edge* ViewerBase::edgeBetween(Node* from, Node* to) {
        /* If the graph is undirected, get from and to into the right order. */
        if (type() == Type::UNDIRECTED && from->index() >= to->index()) {
            std::swap(from, to);
        }

        if (!edges.count(from) || !edges.at(from).count(to)) return nullptr;
        return edges.at(from).at(to).get();
    }

    Type ViewerBase::type() const {
        return mType;
    }

    void ViewerBase::type(Type type) {
        /* This could be a no-op if the type already matches our underlying type. */
        if (type == mType) return;

        /* Converting undirected to directed is basically a no-op internally. */
        if (type == Type::DIRECTED) {
            // Nothing to do
        }
        /* Converting directed to undirected requires us to delete all self-loops
         * and to break ties between pairs of edges.
         */
        else if (type == Type::UNDIRECTED) {
            std::vector<Edge*> toNix;
            std::vector<Edge*> toFlip;
            forEachEdge([&](Edge* edge) {
                /* Ensure the invariant that from() < to() for all edges. */
                if (edge->from()->index() >= edge->to()->index()) {
                    /* If the reverse edge exists, then delete it. This also handles
                     * self-loops.
                     */
                    if (edgeBetween(edge->to(), edge->from())) {
                        toNix.push_back(edge);
                    } else {
                        toFlip.push_back(edge);
                    }
                }
            });

            /* Delete all edges that need to be dealt with. */
            for (Edge* edge: toNix) {
                removeEdge(edge);
            }

            /* Flip all other edges. */
            for (Edge* flip: toFlip) {
                /* Invert the underlying map. */
                edges[flip->to()][flip->from()] = edges[flip->from()][flip->to()];
                edges[flip->from()][flip->to()].reset();

                /* Change the underlying edge details. */
                std::swap(flip->mFrom, flip->mTo);
            }
        }

        /* Set the type for future reference. */
        mType = type;
    }

    void ViewerBase::removeEdge(Edge* edge) {
        edges[edge->from()].erase(edge->to());
        calculateEdgeEndpoints();
    }

    Node* ViewerBase::nodeLabeled(const std::string& label) {
        for (auto node: nodes) {
            if (node->label() == label) return node.get();
        }

        return nullptr;
    }

    GRectangle ViewerBase::bounds() const {
        return rawBounds;
    }

    GRectangle ViewerBase::computedBounds() const {
        return { baseX, baseY, width, height };
    }

    size_t ViewerBase::numNodes() {
        return nodes.size();
    }

    void ViewerBase::setBounds(const GRectangle& bounds) {
        rawBounds = bounds;

        /* Too narrow? */
        if (bounds.width / bounds.height <= mAspectRatio) {
            width = bounds.width;
            height = width / mAspectRatio;
        } else {
            height = bounds.height;
            width = height * mAspectRatio;
        }

        baseX = bounds.x + (bounds.width  - width)  / 2.0;
        baseY = bounds.y + (bounds.height - height) / 2.0;
    }

    double ViewerBase::aspectRatio() {
        return mAspectRatio;
    }

    void ViewerBase::aspectRatio(double ratio) {
        mAspectRatio = ratio;
        setBounds(rawBounds); // Recalculate to the last provided rectangle.
    }

    Node::Node(ViewerBase* editor, const NodeArgs& args, JSON)
        : Node(editor, args) {
        // Forwarded!
    }

    Node::Node(ViewerBase* editor, const NodeArgs& args)
        : owner(editor), mPos(args.pt), mIndex(args.index), mLabel(args.label) {
        owner->calculateEdgeEndpoints();
    }

    const std::string& Node::label() {
        return mLabel;
    }
    void Node::label(const std::string& label) {
        mLabel = label;
    }

    const GPoint& Node::position() {
        return mPos;
    }

    size_t Node::index() {
        return mIndex;
    }

    void Node::position(const GPoint& pt) {
        /* Clamp to appropriate bounds. */
        double x = pt.x;
        if (x < kNodeRadius) x = kNodeRadius;
        if (x > 1 - kNodeRadius) x = 1 - kNodeRadius;

        double y = pt.y;
        if (y < kNodeRadius) y = kNodeRadius;
        if (y > 1 / owner->aspectRatio() - kNodeRadius) y = 1 / owner->aspectRatio() - kNodeRadius;

        mPos = { x, y };
        owner->calculateEdgeEndpoints();
    }

    void Node::draw(ViewerBase* editor, GCanvas* canvas, const NodeStyle& style) {
        /* Calculate the size of the node. */
        double size = 2.0 * style.radius;
        auto bounds = editor->worldToGraphics({ position().x - size / 2.0, position().y - size / 2.0, size, size });

        GOval mainNode(bounds.x, bounds.y, bounds.width, bounds.height);

        mainNode.setFilled(true);
        mainNode.setFillColor(style.fillColor);
        mainNode.setLineWidth(ceil(editor->worldToGraphics(style.lineWidth)));
        mainNode.setColor(style.borderColor);
        canvas->draw(&mainNode);

        /* Draw the node name. */
        auto render = TextRender::construct(label(), bounds, kNodeFont.color(style.textColor));
        render->alignCenterVertically();
        render->alignCenterHorizontally();
        render->draw(canvas);
    }

    Edge::Edge(ViewerBase* owner, const EdgeArgs& args, JSON)
        : Edge(owner, args) {
        // Forwarded
    }

    Edge::Edge(ViewerBase* owner, const EdgeArgs& args)
        : mOwner(owner), mFrom(args.from), mTo(args.to), mLabel(args.label) {
        owner->calculateEdgeEndpoints();
    }

    Node* Edge::to() {
        return mTo;
    }

    Node* Edge::from() {
        return mFrom;
    }

    std::string Edge::label() {
        return mLabel;
    }

    void Edge::label(const std::string& label) {
        mLabel = label;
    }

    /*** Serialization / Deserialization ***/

    /* JSON format is
     *
     * {"nodes", [<node data>],
     *  "edges", [<edge data>],
     *  "aux",   <aux data>,
     *  "type",  <graph type>}
     *
     * Here, each node is encoded as
     *
     *   { "index": <index>, "label": <label>, "pos": [<x>, <y>], "aux": <aux> }
     *
     * Each edge is encoded as
     *
     *   { "from": <index>, "to": <index>, "label": <label> }
     */

    JSON ViewerBase::nodesToJSON() {
        std::vector<JSON> result;
        for (auto node: nodes) {
            result.push_back(toJSON(node.get()));
        }
        return result;
    }

    JSON ViewerBase::toJSON(Node* node) {
        return JSON::object({
            { "index", node->index() },
            { "label", node->label() },
            { "pos",   JSON::array(node->position().x, node->position().y) },
            { "aux",   node->toJSON() }
        });
    }

    JSON ViewerBase::edgesToJSON() {
        std::vector<JSON> result;
        forEachEdge([&](Edge* edge) {
            result.push_back(toJSON(edge));
        });
        return result;
    }

    JSON ViewerBase::typeToJSON() {
        if (mType == Type::DIRECTED) {
            return "directed";
        } else if (mType == Type::UNDIRECTED) {
            return "undirected";
        } else {
            error("Unknown graph type?");
        }
    }

    /* Default aux data is nothing at all. */
    JSON ViewerBase::auxData() {
        return nullptr;
    }

    JSON ViewerBase::toJSON(Edge* edge) {
        return JSON::object({
            { "from",  edge->from()->index() },
            { "to",    edge->to()->index()   },
            { "label", edge->label()         },
            { "aux",   edge->toJSON()        }
        });
    }

    /* Serializes to JSON. */
    JSON ViewerBase::toJSON() {
        /* Pair that with the serialized NFA. */
        return JSON::object({
            { "nodes", nodesToJSON() },
            { "edges", edgesToJSON() },
            { "aux",   auxData()     },
            { "type",  typeToJSON()  }
        });
    }

    /* Default serializers. */
    JSON Node::toJSON() {
        return nullptr;
    }
    JSON Edge::toJSON() {
        return nullptr;
    }
}
