#include "Demos/GVector.h"
#include <cmath>
#include <sstream>
using namespace std;

/* Subtracting two points gives a vector. */
GVector operator- (const GPoint& p1, const GPoint& p2) {
    return { p1.x - p2.x, p1.y - p2.y };
}

/* Subtracting two vectors gives a vector. */
GVector operator- (const GVector& v1, const GVector& v2) {
    return { v1.x - v2.x, v1.y - v2. y };
}
GVector& operator-= (GVector& lhs, const GVector& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

/* Adding a point and a vector gives a point. */
GPoint  operator+ (const GVector& v, const GPoint& p) {
    return { v.x + p.x, v.y + p.y };
}
GPoint  operator+ (const GPoint& p, const GVector& v) {
    return v + p;
}
GPoint& operator+= (GPoint& lhs, const GVector& rhs) {
    lhs = (lhs + rhs);
    return lhs;
}

GPoint  operator- (const GPoint& p, const GVector& v) {
    return { p.x - v.x, p.y - v.y };
}
GPoint& operator-= (GPoint& lhs, const GVector& rhs) {
    lhs = (lhs - rhs);
    return lhs;
}

/* Adding two vectors gives a vector. */
GVector  operator+ (const GVector& v1, const GVector& v2) {
    return { v1.x + v2.x, v1.y + v2.y };
}
GVector& operator+= (GVector& lhs, const GVector& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

/* Scaling a vector scales the components. */
GVector  operator* (const GVector& v, double scalar) {
    return { v.x * scalar, v.y * scalar };
}
GVector  operator* (double scalar, const GVector& v) {
    return v * scalar;
}
GVector& operator*= (GVector& v, double scalar) {
    v.x *= scalar;
    v.y *= scalar;
    return v;
}

GVector  operator/ (const GVector& v, double scalar) {
    return v * (1.0 / scalar);
}
GVector& operator/= (GVector& lhs, double scalar) {
    return lhs *= (1.0 / scalar);
}

/* Magnitude and normalization. */
double magnitudeOf(const GVector& v) {
    return sqrt(v.x * v.x + v.y * v.y);
}
GVector normalizationOf(const GVector& v) {
    return v / magnitudeOf(v);
}

/* Rotation by an angle in radians. */
GVector rotate(const GVector& v, double theta) {
    /* Rotation matrix is
     *
     *   | cos T  -sin T |
     *   | sin T   cos T |
     *
     * So rotating a vector v about an angle theta gives
     *
     *   | cos T  -sin T | |x|   | x cos T - y sin T |
     *   | sin T   cos T | |y| = | x sin T + y cos T |
     */
    return {
        v.x * cos(theta) - v.y * sin(theta),
        v.x * sin(theta) + v.y * cos(theta)
    };
}

/* A unit vector can be computed straight from sines and cosines. */
GVector unitToward(double theta) {
    return { cos(theta), sin(theta) };
}

ostream& operator<< (ostream& out, const GVector& v) {
    ostringstream builder;
    builder << "(" << v.x << ", " << v.y << ")";
    return out << builder.str();
}
