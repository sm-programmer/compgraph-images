#ifndef VECTORS_H
#define VECTORS_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEGREES
#define DEGREES 180.0 / M_PI
#endif

#ifndef RADIANS
#define RADIANS M_PI / 180.0
#endif

typedef struct {
	double x;
	double y;
	double z;
} point3d;

typedef struct {
	double x;
	double y;
	double z;
} vector;

point3d new_point3d(double x, double y, double z);
vector new_vector(const point3d ps, const point3d pe);
double vector_magnitude(const vector v);
vector vector_crossproduct(const vector a, const vector b);
double vector_dotproduct(const vector a, const vector b);
double vector_angle(const vector a, const vector b);
vector vector_normalization(const vector v);

#endif // VECTORS_H