#include "matrix.h"

matrix_double md_new(uint rows, uint cols) {
	matrix_double empty = {NULL, 0, 0};
	matrix_double ans = {NULL, rows, cols};
	if (rows != 0 && cols != 0) {
		ans.data = calloc(rows, sizeof *(ans.data));
		if (ans.data != NULL) {
			uint r;
			for (r = 0; r < ans.rows; r++) {
				(ans.data)[r] = calloc(cols, sizeof **(ans.data));
				if (ans.data[r] == NULL) {
					while (r--) free((ans.data)[r]);
					free(ans.data);
					ans = empty;
				}
			}
		}
	}
	return ans;
}

void md_dispose(matrix_double *m) {
	if (!md_is_empty(*m)) {
		uint r;
		for (r = 0; r < m->rows; r++) free((m->data)[r]);
		free(m->data);
		m->rows = 0;
		m->cols = 0;
	}
}

int md_is_empty(const matrix_double m) {
	int ans = ( m.data == NULL && m.rows == 0 && m.cols == 0 ) ? TRUE : FALSE;
	return ans;
}

matrix_double md_product(const matrix_double m1, const matrix_double m2) {
	matrix_double ans = {NULL, 0, 0};
	if (!md_is_empty(m1) && !md_is_empty(m2) && m1.cols == m2.rows) {
		ans = md_new(m1.rows, m2.cols);
		if (!md_is_empty(ans)) {
			uint x, y, e;
			for (y = 0; y < ans.rows; y++)
				for (x = 0; x < ans.cols; x++)
					for (e = 0; e < m1.cols; e++)
						(ans.data)[y][x] += (m1.data)[y][e] * (m2.data)[e][x];
		}
	}
	return ans;
}

void md_show(const matrix_double m, FILE *fd) {
	if (!md_is_empty(m) && fd != NULL) {
		uint x, y;
		for (y = 0; y < m.rows; y++) {
			for (x = 0; x < m.cols; x++) {
				fprintf(fd, "%f", (m.data)[y][x]);
				if (x < m.cols - 1) fprintf(fd, "\t");
			}
			fprintf(fd, "\n");
		}
	}
}