#ifndef LINES_H
#define LINES_H

#include <stdio.h>
#include <math.h>

#include "raster.h"

typedef struct {
	point start;
	point end;
} line;

typedef struct {
	point *data;
	uint num_elems;
} point_table;

line new_line(point start, point end);

double line_slope(line l);

/* Line drawing algorithms: assue line in 1st octant */
point_table naive(line l);
point_table dda(line l);
point_table bresenham(line l);

point_table line_raster(const line l, point_table (*algo) (line) );

/* Given a line, draw it to the raster using the algorithm provided */
void put_line(raster r, line l, color c, point_table (*algo) (line) );

/* Pre-check drawing using z-buffering */
// void put_line_z(raster r, line l, color c, point * (*algo) (line), double **zmat, double zs, double ze);

#endif // LINES_H