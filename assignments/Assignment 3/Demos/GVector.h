#ifndef GVector_Included
#define GVector_Included

#include "gobjects.h"
#include <ostream>

/* Type representing a 2D vector. */
struct GVector {
    double x, y;
};

/* Subtracting two points gives a vector. */
GVector operator- (const GPoint& p1, const GPoint& p2);

/* Subtracting two vectors gives a vector. */
GVector operator- (const GVector& v1, const GVector& v2);
GVector& operator-= (GVector& lhs, const GVector& rhs);

/* Adding a point and a vector gives a point. */
GPoint  operator+  (const GVector& v, const GPoint& p);
GPoint  operator+  (const GPoint& p, const GVector& v);
GPoint& operator+= (GPoint& lhs, const GVector& rhs);

GPoint  operator-  (const GPoint& p, const GVector& v);
GPoint& operator-= (GPoint& lhs, const GVector& rhs);

/* Adding two vectors gives a vector. */
GVector  operator+ (const GVector& v1, const GVector& v2);
GVector& operator+= (GVector& lhs, const GVector& rhs);

/* Scaling a vector scales the components. */
GVector  operator* (const GVector& v, double scalar);
GVector  operator* (double scalar, const GVector& v);
GVector& operator*= (GVector& v, double scalar);

GVector  operator/ (const GVector& v, double scalar);
GVector& operator/= (GVector& lhs, double scalar);

/* Magnitude and normalization. */
double magnitudeOf(const GVector& v);
GVector normalizationOf(const GVector& v);

/* Rotation by an angle in radians. */
GVector rotate(const GVector& v, double theta);

/* Gets a unit vector pointing in the given direction.
 *
 * Be careful - this returns a vector at that angle in Cartesian space, not
 * in graphics space! You may need to adjust the angle appropriately.
 */
GVector unitToward(double theta);

std::ostream& operator<< (std::ostream& out, const GVector& v);

#endif
