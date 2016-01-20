#ifndef OBJECTS3D_H
#define OBJECTS3D_H

#include <stdio.h>
#include <math.h>
#include <string.h>

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

void readobj(const char *file, double v[4][NVERT], int f[4][NFACE], int *numv, int *numf);
void apply_matrix(double **matrix, double v[4][NVERT]);
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