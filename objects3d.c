#include "objects3d.h"

void readobj(const char *file, double v[4][NVERT], int f[4][NFACE], int *numv, int *numf) {
	FILE *fi = fopen(file, "r");
	if (fi == NULL) return;
	char c;
	while (!feof(fi)) {
		c = fgetc(fi);
		switch (c) {
			case '#':
				while ( fgetc(fi) != '\n' );
				break;
			case 'v':
				fscanf(fi, " %lf %lf %lf\n", &(v[0][*numv]), &(v[1][*numv]), &(v[2][*numv]));
				v[3][(*numv)++] = 1.0;
				break;
			case 'f':
				fscanf(fi, " %d %d %d\n", &(f[0][*numf]), &(f[1][*numf]), &(f[2][*numf]));
				++(*numf);
				break;
			default: continue;
		}
	}
	fclose(fi);
}

//void apply_matrix(double matrix[4][4], double v[4][NVERT]) {
void apply_matrix(double **matrix, double v[4][NVERT]) {
	if (matrix == NULL) return;
	unsigned int x, y, e;
	double tmp, oldval[4];
	for (e = 0; e < NVERT; e++) {
		for (y = 0; y < 4; y++) oldval[y] = v[y][e];
		for (y = 0; y < 4; y++) {
			tmp = 0.0;
			for (x = 0; x < 4; x++) tmp += matrix[y][x] * oldval[x];
			v[y][e] = tmp;
		}
	}
}

double ** new_matrix() {
	double **res = calloc(MATH, sizeof *res);
	if (res == NULL) return NULL;
	unsigned int x;
	for (x = 0; x < MATH; x++) {
		res[x] = calloc(MATW, sizeof **res);
		if (res[x] == NULL) {
			while (x--) free(res[x]);
			free(res);
			return NULL;
		}
	}
	return res;
}

void dispose_matrix(double **mat) {
	unsigned int x;
	for (x = 0; x < MATH; x++) free(mat[x]);
	free(mat);
}

double ** traslate(double tx, double ty, double tz) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[0][0] = res[1][1] = res[2][2] = res[3][3] = 1.0;
	res[0][3] = tx;
	res[1][3] = ty;
	res[2][3] = tz;
	return res;
}

double ** scale_by(double sx, double sy, double sz) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[0][0] = sx;
	res[1][1] = sy;
	res[2][2] = sz;
	res[3][3] = 1.0;
	return res;
}

double ** rotate_x(double ax) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[0][0] = res[3][3] = 1;
	res[1][1] = res[2][2] = cos(ax * RADIANS);
	res[2][1] = sin(ax * RADIANS);
	res[1][2] = -res[2][1];
	return res;
}

double ** rotate_y(double ay) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[1][1] = res[3][3] = 1;
	res[0][0] = res[2][2] = cos(ay * RADIANS);
	res[2][0] = sin(ay * RADIANS);
	res[0][2] = -res[2][0];
	return res;
}

double ** rotate_z(double az) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[2][2] = res[3][3] = 1;
	res[0][0] = res[1][1] = cos(az * RADIANS);
	res[1][0] = sin(az * RADIANS);
	res[0][1] = -res[1][0];
	return res;
}

double ** rotate(double ax, double ay, double az) {
	double **rx = rotate_x(ax);
	double **ry = rotate_y(ay);
	double **rz = rotate_z(az);

	double **zy = product(rz, ry);
	double **res = product(zy, rx);

	dispose_matrix(zy);
	dispose_matrix(rz);
	dispose_matrix(ry);
	dispose_matrix(rx);

	return res;
}

double ** product(double **m1, double **m2) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	unsigned int x, y, e;
	for (y = 0; y < MATH; y++) {
		for (x = 0; x < MATW; x++) {
			double acum = 0.0;
			for (e = 0; e < MATW; e++) {
				acum += m1[y][e] * m2[e][x];
			}
			res[y][x] = acum;
		}
	}
	return res;
}

double ** projection(double f) {
	double **res = new_matrix();
	if (res == NULL) return NULL;
	res[0][0] = res[1][1] = f;
	res[2][2] = res[3][2] = 1.0;
	return res;
}

void show_matrix(double **mat) {
	unsigned int i,j;
	for (j = 0; j < 4; j++) {
		for (i = 0; i < 4; i++) {
			printf("%f", mat[j][i]);
			if (i < 3) printf("\t"); else printf("\n");
		}
	}
}

/* PRIVATE */
int compare_points_y(const void *p1, const void *p2) {
	point cp1 = *((point *) p1);
	point cp2 = *((point *) p2);
	
	if (cp1.y == cp2.y) {
		if (cp1.x == cp2.x) return 0;
		else if (cp1.x < cp2.x) return 1;
		else return -1;
	} else if (cp1.y < cp2.y) return 1;
	else return -1;
}

/* Fill a face using the scanline algorithm */
int fill_face(raster r, point p1, point p2, point p3, color c) {
	point p[3];
	point *bigtbl = NULL, *tmp = NULL, *ffp = NULL;
	int i, j, s, btbls = 0, ffps = 0, last = 0, lowest, highest;
	
	// Get the point values into the array
	p[0] = p1; p[1] = p2; p[2] = p3;
	
	// Obtain the highest and lowest "y" values
	lowest = p[0].y;
	highest = p[0].y;
	for (i = 0; i < 3; i++) {
		if ( p[i].y < lowest ) lowest = p[i].y;
		if ( p[i].y > highest ) highest = p[i].y;
	}
	
	// First generate the lines (just the tables) and store them in a big table
	for (i = 0; i < 3; i++) {
		int start = i, end = (i+1) % 3;
		unsigned int tmps = 0, y = 0;
		tmp = line_raster( new_line( p[start], p[end] ), bresenham );

		do {
			++tmps;
		} while ( !points_equal( tmp[y++], p[end] ) );

		bigtbl = realloc(bigtbl, (btbls + tmps) * sizeof *bigtbl);
		if ( bigtbl == NULL ) return -1;
		memcpy(bigtbl+btbls, tmp, tmps * sizeof *tmp);
		free(tmp);
		btbls += tmps;
	}
	
	// Sort the table using QuickSort
	qsort(bigtbl, btbls, sizeof *bigtbl, compare_points_y);
	
	// Remove duplicates
	for (i = 1; i < btbls; i++) {
		if ( !points_equal( bigtbl[i], bigtbl[last] ) )
			bigtbl[++last] = bigtbl[i];
	}
	++last;
	
	// Using a scanline, move through the ordered table (from highest to lowest)
	j = 0;
	for (s = highest; s >= lowest; s--) {
	
		// Get both minimum and maximum Xs
		int xmin = bigtbl[j].x, xmax = bigtbl[j].y;
		while ( bigtbl[j].y == s && j < btbls ) {
			if ( bigtbl[j].x < xmin ) xmin = bigtbl[j].x;
			if ( bigtbl[j].x > xmax ) xmax = bigtbl[j].x;
			++j;
		}
		
		for (i = xmin + 1; i < xmax; i++) {
			ffp = realloc( ffp, ++ffps * sizeof(*ffp) );
			if (ffp == NULL) return -1;
			ffp[ffps-1] = new_point( i, s );
		}
	}

	// All the internal points are in "ffp"
	// Use a color to fill them
	for (i = 0; i < ffps; i++) put_pixel(r, ffp[i], c);

	return 0;
}
