#include "GVector.h"
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

/* Underlying angle. */
double angleOf(const GVector& v) {
    return atan2(v.y, v.x);
}

/* A unit vector can be computed straight from sines and cosines. */
GVector unitToward(double theta) {
    return { cos(theta), sin(theta) };
}

/* Dot product. */
double dot(const GVector& a, const GVector& b) {
    return a.x * b.x + a.y * b.y;
}

/* Vector projection. */
GVector project(const GVector& vector, const GVector& onto) {
    return onto * dot(vector, onto) / dot(onto, onto);
}

/* Stream printing. */
ostream& operator<< (ostream& out, const GVector& v) {
    ostringstream builder;
    builder << "(" << v.x << ", " << v.y << ")";
    return out << builder.str();
}



GVector operator* (const GMatrix& m, const GVector& v) {
    return { m.a * v.x + m.b * v.y, m.c * v.x + m.d * v.y };
}
GPoint operator* (const GMatrix& m, const GPoint& v) {
    return { m.a * v.x + m.b * v.y, m.c * v.x + m.d * v.y };
}

GMatrix operator* (const GMatrix& m, const GMatrix& n) {
    return { m.a * n.a + m.b * n.c, m.a * n.b + m.a * n.d ,
             m.c * n.a + m.d * n.c, m.c * n.b + m.d * n.d };
}
GMatrix operator* (const GMatrix& m, double scalar) {
    return { m.a * scalar, m.b * scalar, m.c * scalar, m.d * scalar };
}
GMatrix operator/ (const GMatrix& m, double scalar) {
    return { m.a / scalar, m.b / scalar, m.c / scalar, m.d / scalar };
}

GMatrix& operator*= (GMatrix& m, const GMatrix& n) {
    m = (m * n);
    return m;
}
GMatrix& operator*= (GMatrix& m, double scalar) {
    m = (m * scalar);
    return m;
}
GMatrix& operator/= (GMatrix& m, double scalar) {
    m = (m / scalar);
    return m;
}

double det(const GMatrix& m) {
    return m.a * m.d - m.b * m.c;
}

GMatrix inverse(const GMatrix& m) {
    return GMatrix{ m.d, -m.b, -m.c, m.a } / det(m);
}

GMatrix rotation(double theta) {
    return { cos(theta), -sin(theta), sin(theta), cos(theta) };
}

ostream& operator<< (ostream& out, const GMatrix& m) {
    ostringstream builder;
    builder << "[[" << m.a << ", " << m.b << "][" << m.c << ", " << m.d << "]]";
    return out << builder.str();
}
