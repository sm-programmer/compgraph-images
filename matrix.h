#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

typedef struct {
	double **data;
	uint rows;
	uint cols;
} matrix_double;

matrix_double md_new(uint rows, uint cols);
void md_dispose(matrix_double *m);
int md_is_empty(const matrix_double m);
matrix_double md_product(const matrix_double m1, const matrix_double m2);
void md_show(const matrix_double m, FILE *fd);

#endif // MATRIX_H