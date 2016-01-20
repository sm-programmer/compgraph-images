#include "lines.h"

line new_line(point start, point end) {
	line l;
	l.start = start; l.end = end;
	return l;
}

double line_slope(line l) {
	return (double) (l.end.y - l.start.y) / (l.end.x - l.start.x);
}

/* Naive line drawing algorithm: assumes line in 1st octant */
point * naive(line l) {
	uint numelems = abs( l.end.x - l.start.x );
	double m = line_slope(l);
	point * tbl = calloc(numelems + 1, sizeof *tbl);
	if (tbl == NULL) return NULL;
	int x, nt;
	double b = (double) l.start.y - ((double) l.start.x * m);
	tbl[0] = l.start;
	tbl[numelems] = l.end;
	for (x = l.start.x + 1, nt = 1; nt < numelems; x++, nt++) {
		tbl[nt].x = x;
		tbl[nt].y = (int) ( m*x + b );
	}
	return tbl;
}

/* DDA line drawing algorithm: assumes line in 1st octant */
point * dda(line l) {
	uint numelems = abs( l.end.x - l.start.x );
	double m = line_slope(l);
	point * tbl = calloc(numelems + 1, sizeof *tbl);
	if (tbl == NULL) return NULL;
	int x, nt;
	double ny = (double) l.start.y;
	tbl[0] = l.start;
	tbl[numelems] = l.end;
	for (x = l.start.x + 1, nt = 1; nt < numelems; x++, nt++) {
		ny += m;
		tbl[nt].x = x;
		tbl[nt].y = (int) ny;
	}
	tbl[nt] = l.end;
	return tbl;
}

/* Bresenham line drawing algorithm: assumes line in 1st octant */
point * bresenham(line l) {
	/* parameters for Bresenham line drawing algorithm */
	int dx = l.end.x - l.start.x;
	int dy = l.end.y - l.start.y;
	uint numelems = abs( dx );
	/* initial values for: */
	int d = 2 * dy - dx;					/*<< the d value */
	int incE = 2 * dy;						/*<< the increment when in East direction */
	int incNE = 2 * (dy - dx);				/*<< the increment when in North East direction */
	int x, y, nt;
	point * tbl = calloc(numelems + 1, sizeof *tbl);
	if (tbl == NULL) return NULL;
	tbl[0] = l.start;
	tbl[numelems] = l.end;
	for (x = l.start.x + 1, y = l.start.y, nt = 1; nt < numelems; x++, nt++) {
		if (d <= 0) d += incE;		/* East direction adjustement to the parameter 'd' */
		else {
			d += incNE;				/* North East direction adjustment to the parameter 'd' */
			++y;					/* y increment  */
		}
		tbl[nt].x = x;
		tbl[nt].y = y;
	}
	return tbl;
}

/* Generate special line */
short int line_isspecial(line l, point **tbl) {
	// Parameter NULL?
	if (tbl == NULL) return 1;
	int dx = l.end.x - l.start.x;
	int dy = l.end.y - l.start.y;
	// Determine special type
	short int sp = -1;
	if ( points_equal(l.start, l.end) )		sp = 0;	// Point
	else if (l.start.y == l.end.y)			sp = 1;	// Horizontal
	else if (l.start.x == l.end.x)			sp = 2;	// Vertical
	else if (abs(dx) == abs(dy))			sp = 3;	// Diagonal
	// Isn't special?
	if (sp == -1) return -1;
	// Is special: adjust increments accordingly
	int incy = (sp & 2) >> 1, incx = sp & 1;
	if (incx != 0 && dx < 0) incx = -incx;
	if (incy != 0 && dy < 0) incy = -incy;
	uint nt = 0;
	// Allocate space for the table
	int noelems = (!incx) ? abs(dy) : abs(dx);
	*tbl = calloc(noelems + 1, sizeof **tbl);
	// Not allocated?
	if (*tbl == NULL) return 2;
	// Generate table to the end
	point tmp = l.start;
	do {
		(*tbl)[nt] = tmp;
		(tmp.x) += incx;
		(tmp.y) += incy;
	} while ( !points_equal((*tbl)[nt++], l.end) );
	return 0;
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
void table_reverse(point stop, point **tbl, short int transf) {
	if (tbl == NULL || (*tbl) == NULL) return;
	// Reverse transformation over the table of points
	point tmp;
	uint x = 0;
	do {
		tmp = (*tbl)[x];
		if ( (transf & 1) == 1 ) (*tbl)[x] = swap( (*tbl)[x] );
		if ( (transf & 2) == 2 ) (*tbl)[x] = flipY( (*tbl)[x] );
		if ( (transf & 4) == 4 ) (*tbl)[x] = flipX( (*tbl)[x] );
		x++;
	} while ( !points_equal( tmp, stop ) );
}

/* Given a line, raster it using the algorithm provided */
point * line_raster(const line l, point * (*algo) (line) ) {
	point *tbl = NULL;
	// Check for special case: if special, return point table
	short int sp = line_isspecial(l, &tbl);
	// Isn't special?
	if (sp < 0) {
		// 1) Transform line to first octant
		line lt = l;
		short int transf = line_firstoct(&lt);
		// 2) Apply the line drawing algorithm wanted
		tbl = algo(lt);
		// 3) Reverse trasnformation over the table of points
		table_reverse(lt.end, &tbl, transf);
	}
	// Return table of points or NULL if error.
	return tbl;
}

void put_line(raster r, line l, color c, point * (*algo) (line) ) {
	point *tbl = line_raster(l, algo);
	if (tbl != NULL) {
		uint n = 0;
		do {
			put_pixel(r, tbl[n], c);
		} while ( !points_equal( tbl[n++], l.end ) );
		free(tbl);
	}
}

void put_line_z(raster r, line l, color c, point * (*algo) (line), double **zmat, double zs, double ze) {
	point *tbl = line_raster(l, algo);
	if (tbl != NULL) {
		uint n = 0;
		uint elems = 0;
		while ( !points_equal( tbl[elems++], l.end ) );
		double inc = (ze - zs) / (double) elems;
		double z = zs;
		for (n = 0; n < elems; n++) { 
			fprintf(stderr, "(%d,%d) ", tbl[n].x, tbl[n].y);
			if (z < zmat[tbl[n].y][tbl[n].x]) {
				put_pixel(r, tbl[n], c);
				fprintf(stderr, "drawn!");
			} else {
				fprintf(stderr, "not drawn!");
			}
			z += inc;
			fprintf(stderr, "\n");
		}
		free(tbl);
		fprintf(stderr, "\n");
	}
}
