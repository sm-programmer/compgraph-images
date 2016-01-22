#include "raster.h"

// Creates a new raster in the form [layer][height][width]
raster new_raster(uint w, uint h, uint l) {
	uint ln, y;
	raster tmp = {NULL, w, h, l};

	tmp.data = calloc(l, sizeof *(tmp.data));
	for (ln = 0; ln < l; ln++) {
		(tmp.data)[ln] = calloc(h, sizeof **(tmp.data));
		for(y = 0; y < h; y++)
			(tmp.data)[ln][y] = calloc(w, sizeof ***(tmp.data));
	}

	return tmp;
}

void dispose_raster(raster r) {
	uint ln, y;
	for (ln = 0; ln < r.layers; ln++) {
		for (y = 0; y < r.height; y++)
			free((r.data)[ln][y]);
		free((r.data)[ln]);
	}
	free(r.data);
}

point new_point(uint x, uint y) {
	point p = {x, y};
	return p;
}

color new_color(byte r, byte g, byte b) {
	color c = {r, g, b};
	return c;
}

void put_pixel(raster r, point p, color c) {
	if (p.x >= 0 && p.x < r.width && p.y >= 0 && p.y < r.height) {
		(r.data)[R][p.y][p.x] = c.r;
		(r.data)[G][p.y][p.x] = c.g;
		(r.data)[B][p.y][p.x] = c.b;
	}
}

color get_pixel(raster r, point p) {
	color res;
	res.r = (r.data)[R][p.x][p.y];
	res.g = (r.data)[G][p.x][p.y];
	res.b = (r.data)[B][p.x][p.y];
	return res;
}

/* PRIVATE */
int raster_tostream(raster r, FILE *f) {
	uint x, y;
	if (f == NULL) return -1;
	fprintf(f, "P6\n%u %u %u\n", r.width, r.height, MAXCOLOR);
	for (y = 0; y < r.height; y++) {
		for (x = 0; x < r.width; x++) {
			fprintf(f, "%c%c%c", (r.data)[R][y][x], (r.data)[G][y][x], (r.data)[B][y][x]);
		}
	}
	return 0;
}

int raster_write(raster r, const char *file) {
	FILE *f = fopen(file, "wt");
	int ans = raster_tostream(r, f);
	fclose(f);
	return ans;
}

int raster_out(raster r) {
	return raster_tostream(r, stdout);
}

/* Given two points, determine whether they are equal */
byte points_equal(point p1, point p2) {
	byte equal = (p1.x == p2.x && p1.y == p2.y) ? 1 : 0;
	return equal;
}

/* Given a point, flip its Y value (flip in X-axis) */
point flipX(point p) {
	return new_point(p.x, -p.y);
}

/* Given a point, flip its X value (flip in Y-axis) */
point flipY(point p) {
	return new_point(-p.x, p.y);
}

/* Given a point, flip in both values (symmetry in origin as pivot) */
point flipXY(point p) {
	return new_point(-p.x, -p.y);
}

/* Given a point, swap its values (flip in x = y line) */
point swap(point p) {
	return new_point(p.y, p.x);
}

/* Translate a point, inverting the axes if desired */
point raster_translate( point p, point c, int invertX, int invertY ) {
	point ans = p;
	ans.x = (invertX == 0) ? c.x + p.x : c.x - p.x;
	ans.y = (invertY == 0) ? c.y + p.y : c.y - p.y;
	return ans;
}

/* PRIVATE */
byte bound(int i) {
	if (i < 0) return 0;
	else if (i > 255) return 255;
	else return i;
}

/* Add two colors together */
color color_add(color c1, color c2) {
	color ans = {bound(c1.r + c2.r), bound(c1.g + c2.g), bound(c1.b + c2.b)};
	return ans;
}

/* Multiply a color by a scalar */
color color_amplify(color c, double factor) {
	color ans;
	ans.r = (int) round(c.r * factor);
	ans.g = (int) round(c.g * factor);
	ans.b = (int) round(c.b * factor);
	return ans;
}