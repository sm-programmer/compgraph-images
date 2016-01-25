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

void apply_matrix(const matrix_double m, point3d_table v) {
	if (!md_is_empty(m) && v.data != NULL) {
		uint x, y, e;
		for (e = 0; e < v.num_elems; e++) {
			double tmp[4] = {0};
			double oldval[4] = { (v.data)[e].x, (v.data)[e].y, (v.data)[e].z, (v.data)[e].w };
			for (y = 0; y < m.rows; y++) {
				double res = 0.0;
				for (x = 0; x < m.cols; x++) res += (m.data)[y][x] * oldval[x];
				tmp[y] = res;
			}
			(v.data)[e] = new_point3d(tmp[0], tmp[1], tmp[2], tmp[3]);
		}
	}
}

matrix_double new_matrix() {
	return md_new(4, 4);
}

void dispose_matrix(matrix_double *m) {
	md_dispose(m);
}

matrix_double traslate(double tx, double ty, double tz) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[0][0] = (res.data)[1][1] = (res.data)[2][2] = (res.data)[3][3] = 1.0;
		(res.data)[0][3] = tx;
		(res.data)[1][3] = ty;
		(res.data)[2][3] = tz;
	}
	return res;
}

matrix_double scale_by(double sx, double sy, double sz) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[0][0] = sx;
		(res.data)[1][1] = sy;
		(res.data)[2][2] = sz;
		(res.data)[3][3] = 1.0;
	}
	return res;
}

matrix_double rotate_x(double ax) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[0][0] = (res.data)[3][3] = 1.0;
		(res.data)[1][1] = (res.data)[2][2] = cos(ax * RADIANS);
		(res.data)[2][1] = sin(ax * RADIANS);
		(res.data)[1][2] = -(res.data)[2][1];
	}
	return res;
}

matrix_double rotate_y(double ay) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[1][1] = (res.data)[3][3] = 1.0;
		(res.data)[0][0] = (res.data)[2][2] = cos(ay * RADIANS);
		(res.data)[2][0] = sin(ay * RADIANS);
		(res.data)[0][2] = -(res.data)[2][0];
	}
	return res;
}

matrix_double rotate_z(double az) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[2][2] = (res.data)[3][3] = 1.0;
		(res.data)[0][0] = (res.data)[1][1] = cos(az * RADIANS);
		(res.data)[1][0] = sin(az * RADIANS);
		(res.data)[0][1] = -(res.data)[1][0];
	}
	return res;
}

matrix_double rotate(double ax, double ay, double az) {
	matrix_double rx, ry, rz, zy, res;

	res = md_new(0, 0);
	
	rx = rotate_x(ax);
	ry = rotate_y(ay);
	rz = rotate_z(az);

	if (!md_is_empty(rx) && !md_is_empty(ry) && !md_is_empty(rz)) {
		zy = product(rz, ry);
		res = (!md_is_empty(zy)) ? product(zy, rx) : md_new(0, 0);
	}

	dispose_matrix(&zy);
	dispose_matrix(&rz);
	dispose_matrix(&ry);
	dispose_matrix(&rx);

	return res;
}

matrix_double product(const matrix_double m1, const matrix_double m2) {
	return md_product(m1, m2);
}

matrix_double projection(double f) {
	matrix_double res = new_matrix();
	if (!md_is_empty(res)) {
		(res.data)[0][0] = (res.data)[1][1] = f;
		(res.data)[2][2] = (res.data)[3][2] = 1.0;
	}
	return res;
}

void show_matrix(const matrix_double mat) {
	md_show(mat, stdout);
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
