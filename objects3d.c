#include "objects3d.h"

point3d new_point3d(double x, double y, double z, double w) {
	point3d p = {x, y, z, w};
	return p;
}

face new_face(int v1, int v2, int v3) {
	face f = {v1, v2, v3};
	return f;
}

file_data readobj(const char *file) {
	file_data empty = {{NULL, 0}, {NULL, 0}};
	file_data ans = empty;
	FILE *fi = fopen(file, "r");
	if (fi != NULL) {
		char c;
		int no_err = 1;
		while (!feof(fi) && no_err) {
			c = fgetc(fi);
			switch (c) {
				case '#':
					while( fgetc(fi) != '\n' );
					break;
				case 'v': {
					ans.vertices.data = realloc( ans.vertices.data, ++(ans.vertices.num_elems) * sizeof *(ans.vertices.data) );
					if (ans.vertices.data == NULL) {
						if (ans.faces.data != NULL) free(ans.faces.data);
						ans = empty;
						no_err = 0;
					}
					point3d tmp = new_point3d(0.0, 0.0, 0.0, 1.0);
					fscanf(fi, " %lf %lf %lf\n", &(tmp.x), &(tmp.y), &(tmp.z));
					(ans.vertices.data)[ans.vertices.num_elems - 1] = tmp;
				} break;
				case 'f': {
					ans.faces.data = realloc( ans.faces.data, ++(ans.faces.num_elems) * sizeof *(ans.faces.data) );
					if (ans.faces.data == NULL) {
						if (ans.vertices.data != NULL) free(ans.vertices.data);
						ans = empty;
						no_err = 0;
					}
					face tmp = new_face(0, 0, 0);
					fscanf(fi, " %d %d %d\n", &(tmp.v1), &(tmp.v2), &(tmp.v3));
					(ans.faces.data)[ans.faces.num_elems - 1] = tmp;
				} break;
				default: continue;
			}
		}
		fclose(fi);
	}
	return ans;
}

// void readobj(const char *file, double v[4][NVERT], int f[4][NFACE], int *numv, int *numf) {
// 	FILE *fi = fopen(file, "r");
// 	if (fi != NULL) {
// 		char c;
// 		while (!feof(fi)) {
// 			c = fgetc(fi);
// 			switch (c) {
// 				case '#':
// 					while ( fgetc(fi) != '\n' );
// 					break;
// 				case 'v':
// 					fscanf(fi, " %lf %lf %lf\n", &(v[0][*numv]), &(v[1][*numv]), &(v[2][*numv]));
// 					v[3][(*numv)++] = 1.0;
// 					break;
// 				case 'f':
// 					fscanf(fi, " %d %d %d\n", &(f[0][*numf]), &(f[1][*numf]), &(f[2][*numf]));
// 					++(*numf);
// 					break;
// 				default: continue;
// 			}
// 		}
// 		fclose(fi);
// 	}
// }

//void apply_matrix(double matrix[4][4], double v[4][NVERT]) {
// void apply_matrix(double **matrix, double v[4][NVERT]) {
void apply_matrix(double **matrix, point3d_table v) {
	if (matrix == NULL || v.data == NULL) return;
	uint x, y, e;
	for (e = 0; e < v.num_elems; e++) {
		double tmp[4] = {0};
		double oldval[4] = { (v.data)[e].x, (v.data)[e].y, (v.data)[e].z, (v.data)[e].w };
		for (y = 0; y < 4; y++) {
			double res = 0.0;
			for (x = 0; x < 4; x++) res += matrix[y][x] * oldval[x];
			tmp[y] = res;
		}
		(v.data)[e] = new_point3d(tmp[0], tmp[1], tmp[2], tmp[3]);
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
	point_table bigtbl = {NULL, 0}, tmp = {NULL, 0}, ffp = {NULL, 0};
	int i, j, s;
	
	// Get the point values into the array
	p[0] = p1; p[1] = p2; p[2] = p3;
	
	// Obtain the highest and lowest "y" values
	int lowest = p[0].y;
	int highest = p[0].y;
	for (i = 0; i < 3; i++) {
		if ( p[i].y < lowest ) lowest = p[i].y;
		if ( p[i].y > highest ) highest = p[i].y;
	}
	
	// First generate the lines (just the tables) and store them in a big table
	for (i = 0; i < 3; i++) {
		int start = i, end = (i+1) % 3;
		tmp = line_raster( new_line( p[start], p[end] ), bresenham );

		bigtbl.data = realloc(bigtbl.data, (bigtbl.num_elems + tmp.num_elems) * sizeof *(bigtbl.data));

		if ( bigtbl.data == NULL ) {
			free(tmp.data);
			free(bigtbl.data);
			return -1;
		}

		memcpy(bigtbl.data + bigtbl.num_elems, tmp.data, tmp.num_elems * sizeof *(tmp.data));
		free(tmp.data);

		bigtbl.num_elems += tmp.num_elems;
	}
	
	// Sort the table using QuickSort
	qsort(bigtbl.data, bigtbl.num_elems, sizeof *(bigtbl.data), compare_points_y);
	
	// Remove duplicates
	int last = 0;
	for (i = 1; i < bigtbl.num_elems; i++) {
		if ( !points_equal( (bigtbl.data)[i], (bigtbl.data)[last] ) )
			(bigtbl.data)[++last] = (bigtbl.data)[i];
	}
	++last;
	
	// Using a scanline, move through the ordered table (from highest to lowest)
	j = 0;
	for (s = highest; s >= lowest; s--) {
	
		// Get both minimum and maximum Xs
		int xmin = (bigtbl.data)[j].x, xmax = (bigtbl.data)[j].x;
		while ( (bigtbl.data)[j].y == s && j < bigtbl.num_elems ) {
			if ( (bigtbl.data)[j].x < xmin ) xmin = (bigtbl.data)[j].x;
			if ( (bigtbl.data)[j].x > xmax ) xmax = (bigtbl.data)[j].x;
			++j;
		}
		
		for (i = xmin + 1; i < xmax; i++) {
			ffp.data = realloc( ffp.data, ++(ffp.num_elems) * sizeof *(ffp.data) );
			if (ffp.data == NULL) {
				free(bigtbl.data);
				free(ffp.data);
				return -1;
			}
			(ffp.data)[ffp.num_elems - 1] = new_point( i, s );
		}
	}

	// All the internal points are in "ffp"
	// Use a color to fill them
	for (i = 0; i < ffp.num_elems; i++) put_pixel(r, (ffp.data)[i], c);

	free(bigtbl.data);
	free(ffp.data);

	return 0;
}
