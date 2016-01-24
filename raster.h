#ifndef RASTER_H
#define RASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"

#define WIDTH 1920
#define HEIGHT 1080
#define MAXCOLOR 255

#define R 0
#define G 1
#define B 2

typedef struct {
	raster_data data;
	uint width;
	uint height;
	uint layers;
} raster;

typedef struct {
	int x;
	int y;
} point;

typedef struct {
	byte r;
	byte g;
	byte b;
} color;

/* Raster shape: raster [layer][height][width] */
raster new_raster(uint w, uint h, uint l);
void dispose_raster(raster r);

point new_point(uint x, uint y);
color new_color(byte r, byte g, byte b);

void put_pixel(raster r, point p, color c);
color get_pixel(raster r, point p);

int raster_write(raster r, const char *file);
int raster_out(raster r);

byte points_equal(point p1, point p2);

/* Given a point, flip its Y value (flip in X-axis) */
point flipX(point p);
/* Given a point, flip its X value (flip in Y-axis) */
point flipY(point p);
/* Given a point, flip in both values (symmetry in origin as pivot) */
point flipXY(point p);
/* Given a point, swap its values (flip in x = y line) */
point swap(point p);

/* Translate a point, inverting the axes if desired */
point raster_translate( point p, point c, int invertX, int invertY );

/* Add two colors together */
color color_add(color c1, color c2);
/* Multiply a color by a scalar */
color color_amplify(color c, double factor);

#endif // RASTER_H