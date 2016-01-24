#ifndef OBJECTS3D_H
#define OBJECTS3D_H

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "types.h"
#include "raster.h"
#include "lines.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEGREES
#define DEGREES 180.0 / M_PI
#endif

#ifndef RADIANS
#define RADIANS M_PI / 180.0
#endif

#define NVERT 100000
#define NFACE 100000

#define MATW 4
#define MATH 4

typedef struct {
	double x;
	double y;
	double z;
	double w;
} point3d;

typedef struct {
	point3d *data;
	uint num_elems;
} point3d_table;

typedef struct {
	int v1;
	int v2;
	int v3;
} face;

typedef struct {
	face *data;
	uint num_elems;
} face_table;

typedef struct {
	point3d_table vertices;
	face_table faces;
} file_data;

point3d new_point3d(double x, double y, double z, double w);
face new_face(int v1, int v2, int v3);
file_data readobj(const char *file);
void apply_matrix(double **matrix, point3d_table v);
double ** new_matrix();
void dispose_matrix(double **mat);
double ** traslate(double tx, double ty, double tz);
double ** scale_by(double sx, double sy, double sz);
double ** rotate_x(double ax);
double ** rotate_y(double ay);
double ** rotate_z(double az);
double ** rotate(double ax, double ay, double az);
double ** product(double **m1, double **m2);
double ** projection(double f);
void show_matrix(double **mat);
int fill_face(raster r, point p1, point p2, point p3, color c);

#endif // OBJECTS3D_H