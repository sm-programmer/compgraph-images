#include "lines.h"

line new_line(point start, point end) {
	line l = {start, end};
	return l;
}

double line_slope(line l) {
	return (double) (l.end.y - l.start.y) / (l.end.x - l.start.x);
}

/* Naive line drawing algorithm: assumes line in 1st octant */
point_table naive(line l) {
	double m = line_slope(l);
	point_table tbl = {NULL, 0};
	tbl.num_elems = abs( l.end.x - l.start.x ) + 1;
	tbl.data = calloc(tbl.num_elems, sizeof *(tbl.data));
	if (tbl.data != NULL) {
		int x, nt;
		double b = (double) l.start.y - ((double) l.start.x * m);
		(tbl.data)[0] = l.start;
		(tbl.data)[tbl.num_elems - 1] = l.end;
		for (x = l.start.x + 1, nt = 1; nt < tbl.num_elems - 1; x++, nt++)
			(tbl.data)[nt] = new_point( x, (int) ( m*x + b ) );
	}
	return tbl;
}

/* DDA line drawing algorithm: assumes line in 1st octant */
point_table dda(line l) {
	double m = line_slope(l);
	point_table tbl = {NULL, 0};
	tbl.num_elems = abs( l.end.x - l.start.x ) + 1;
	tbl.data = calloc(tbl.num_elems, sizeof *(tbl.data));
	if (tbl.data != NULL) {
		int x, nt;
		double ny = (double) l.start.y;
		(tbl.data)[0] = l.start;
		(tbl.data)[tbl.num_elems - 1] = l.end;
		for (x = l.start.x + 1, nt = 1; nt < tbl.num_elems - 1; x++, nt++) {
			ny += m;
			(tbl.data)[nt] = new_point( x, (int) ny );
		}
	}
	return tbl;
}

/* Bresenham line drawing algorithm: assumes line in 1st octant */
point_table bresenham(line l) {
	point_table tbl = {NULL, 0};
	/* parameters for Bresenham line drawing algorithm */
	int dx = l.end.x - l.start.x;
	int dy = l.end.y - l.start.y;
	tbl.num_elems = abs( dx ) + 1;
	/* initial values for: */
	int d = 2 * dy - dx;					/*<< the d value */
	int incE = 2 * dy;						/*<< the increment when in East direction */
	int incNE = 2 * (dy - dx);				/*<< the increment when in North East direction */
	int x, y, nt;
	tbl.data = calloc(tbl.num_elems, sizeof *(tbl.data));
	if (tbl.data != NULL) {
		(tbl.data)[0] = l.start;
		(tbl.data)[tbl.num_elems - 1] = l.end;
		for (x = l.start.x + 1, y = l.start.y, nt = 1; nt < tbl.num_elems - 1; x++, nt++) {
			if (d <= 0) d += incE;		/* East direction adjustement to the parameter 'd' */
			else {
				d += incNE;				/* North East direction adjustment to the parameter 'd' */
				++y;					/* y increment  */
			}
			(tbl.data)[nt] = new_point( x, y );
		}
	}
	return tbl;
}

/* Generate special line */
point_table line_isspecial(line l, short int *ind) {
	point_table tbl = {NULL, 0};
 	int dx = l.end.x - l.start.x;
 	int dy = l.end.y - l.start.y;
	// Determine special type
	short int sp = -1;
	if ( points_equal(l.start, l.end) )		sp = 0;	// Point
	else if (l.start.y == l.end.y)			sp = 1;	// Horizontal
	else if (l.start.x == l.end.x)			sp = 2;	// Vertical
	else if (abs(dx) == abs(dy))			sp = 3;	// Diagonal
	// Is special?
	if (sp != -1) {
		// Adjust increments accordingly
		int incy = (sp & 2) >> 1, incx = sp & 1;
		if (incx != 0 && dx < 0) incx = -incx;
		if (incy != 0 && dy < 0) incy = -incy;

		// Allocate space for the table
		tbl.num_elems = (!incx) ? abs(dy) : abs(dx);
		++(tbl.num_elems);
		tbl.data = calloc(tbl.num_elems, sizeof *(tbl.data));

		// Allocated?
		if (tbl.data != NULL) {
			// Generate tabe of points
			uint ne;
			(tbl.data)[0] = l.start;
			(tbl.data)[tbl.num_elems - 1] = l.end;
			for (ne = 1; ne < tbl.num_elems - 1; ne++) {
				(tbl.data)[ne].x = (tbl.data)[ne-1].x + incx;
				(tbl.data)[ne].y = (tbl.data)[ne-1].y + incy;
			}
		} else sp = -2;
	}
	// Set final indicator
	if (ind != NULL) *ind = sp;
	return tbl;
}

/* Move given line to first octant */
short int line_firstoct(line *l) {
	// Get line slope
	int dx = l->end.x - l->start.x;
	int dy = l->end.y - l->start.y;
	double m = (double) dy / dx;
	// Determine transform actions to move line to 1st octant
	short int transf = 0;
	if (dy < 0) transf |= 4;
	if ( ((dy < 0) && !(m < 0)) || (!(dy < 0) && (m < 0)) ) transf |= 2;
	if ( (m < -1) || (m > 1) ) transf |= 1;
	if ( (transf & 4) == 4 ) {
		l->start = flipX( l->start );
		l->end = flipX( l->end );
	}
	if ( (transf & 2) == 2 ) {
		l->start = flipY( l->start );
		l->end = flipY( l->end );
	}
	if ( (transf & 1) == 1 ) {
		l->start = swap( l->start );
		l->end = swap( l->end );
	}
	return transf;
}

/* Reverse transformations done over point data table */
void table_reverse(point_table *tbl, short int transf) {
	if (tbl == NULL) return;
	// Reverse transformation over the table of points
	uint ne;
	for (ne = 0; ne < tbl->num_elems; ne++) {
		if ( (transf & 1) == 1 ) (tbl->data)[ne] = swap( (tbl->data)[ne] );
		if ( (transf & 2) == 2 ) (tbl->data)[ne] = flipY( (tbl->data)[ne] );
		if ( (transf & 4) == 4 ) (tbl->data)[ne] = flipX( (tbl->data)[ne] );
	}
}

/* Given a line, raster it using the algorithm provided */
point_table line_raster(const line l, point_table (*algo) (line) ) {
	point_table tbl = {NULL, 0};
	// Check for special case: if special, return point table
	short int sp = -1;
	tbl = line_isspecial(l, &sp);
	// Isn't special?
	if (sp < 0) {
		// 1) Transform line to first octant
		line lt = l;
		short int transf = line_firstoct(&lt);
		// 2) Apply the line drawing algorithm wanted
		tbl = algo(lt);
		// 3) Reverse trasnformation over the table of points
		table_reverse(&tbl, transf);
	}
	// Return table of points or NULL if error.
	return tbl;
}

void put_line(raster r, line l, color c, point_table (*algo) (line) ) {
	point_table tbl = line_raster(l, algo);
	if (tbl.data != NULL) {
		uint ne;
		for (ne = 0; ne < tbl.num_elems; ne++)
			put_pixel(r, (tbl.data)[ne], c);
		free(tbl.data);
	}
}

void put_line_z(raster r, line l, color c, point_table (*algo) (line), double **zmat, double zs, double ze) {
	point_table tbl = line_raster(l, algo);
	if (tbl.data != NULL) {
		uint n = 0;
		double inc = (ze - zs) / (double) tbl.num_elems;
		double z = zs;
		for (n = 0; n < tbl.num_elems; n++) { 
			fprintf(stderr, "(%d,%d) ", (tbl.data)[n].x, (tbl.data)[n].y);
			if (z < zmat[(tbl.data)[n].y][(tbl.data)[n].x]) {
				put_pixel(r, (tbl.data)[n], c);
				fprintf(stderr, "drawn!");
			} else {
				fprintf(stderr, "not drawn!");
			}
			z += inc;
			fprintf(stderr, "\n");
		}
		free(tbl.data);
		fprintf(stderr, "\n");
	}
}
