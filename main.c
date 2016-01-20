#include <stdio.h>
#include <float.h>
#include <time.h>

#include "raster.h"
#include "lines.h"
#include "objects3d.h"
#include "vectors.h"

int main(int argc, char *argv[]) {
	// Initialize randomness
	srand(time(NULL));
	// 7 parameters are required.
	if (argc != 13) {
		fprintf(stderr, "Usage: %s obj-file sx sy sz tx ty tz rx ry rz focal-dist hiding\n", argv[0]);
		return 0;
	}

	double vertices[4][NVERT];
	int faces[3][NFACE];
	int numv = 0, numf = 0;
	
	unsigned int i, j;

	double sx = atof( argv[2] );
	double sy = atof( argv[3] );
	double sz = atof( argv[4] );
	double tx = atof( argv[5] );
	double ty = atof( argv[6] );
	double tz = atof( argv[7] );
	double rx = atof( argv[8] );
	double ry = atof( argv[9] );
	double rz = atof( argv[10] );
	double fd = atof( argv[11] );

	int hiding = atoi( argv[12] );

	double **scale = scale_by(sx, sy, sz);
	double **tras = traslate(tx, ty, tz);
	double **rot = rotate(rx, ry, rz);
	double **proj = projection(fd);
	
	readobj(argv[1], vertices, faces, &numv, &numf);

	// Join all the transformations in a single matrix
	// Operation order: Scaling, rotation, traslation and projection
	double **t1 = product(proj, tras);
	double **t2 = product(t1, rot);
	double **t3 = product(t2, scale);

	apply_matrix(t3, vertices);
	
	dispose_matrix(t3);
	dispose_matrix(t2);
	dispose_matrix(t1);
	dispose_matrix(scale);
	dispose_matrix(tras);
	dispose_matrix(rot);
	dispose_matrix(proj);

	// Projection sets w = z, so divide everything by w.
	for (i = 0; i < numv; i++)
		for (j = 0; j < 3; j++)
			vertices[j][i] /= vertices[3][i];

	// fprintf(stderr, "numv = %d, numf = %d\n", numv, numf);
	// for (i = 0; i < numv; i++) {
	// 	fprintf(stderr, "(");
	// 	for (j = 0; j < 4; j++) {
	// 		fprintf(stderr, "%f", vertices[j][i]);
	// 		if (j < 3) fprintf(stderr, ",");
	// 	}
	// 	fprintf(stderr, ")\n");
	// }

	// The camera normal (taking advantage of perspective projection)
	vector camera_normal = new_vector( new_point3d(0.0, 0.0, 0.0), new_point3d(0.0, 0.0, 1.0) );

	// The z-buffer
	double **zbuf = calloc(HEIGHT, sizeof *zbuf);
	for (i = 0; i < HEIGHT; i++) {
		zbuf[i] = calloc(WIDTH, sizeof **zbuf);
		for (j = 0; j < WIDTH; j++)
			zbuf[i][j] = DBL_MAX;
	}
	
	color c = new_color( 255, 255, 255 );

	point center = { WIDTH >> 1, HEIGHT >> 1 };
	int invertX = 0, invertY = 1;
	
	raster tmp = new_raster(WIDTH, HEIGHT, 3);
	for (i = 0; i < numf; i++) {
		point pt[3];

		for (j = 0; j < 3; j++) {
			pt[j] = new_point( vertices[0][faces[j][i] - 1], vertices[1][faces[j][i] - 1] );
			pt[j] = raster_translate(pt[j], center, invertX, invertY);
		}

		// Hiding faces using face normals
		if (hiding == 1) {
			point3d v1 = new_point3d( vertices[0][faces[0][i] - 1], vertices[1][faces[0][i] - 1], vertices[3][faces[0][i] - 1] );
			point3d v2 = new_point3d( vertices[0][faces[1][i] - 1], vertices[1][faces[1][i] - 1], vertices[3][faces[1][i] - 1] );
			point3d v3 = new_point3d( vertices[0][faces[2][i] - 1], vertices[1][faces[2][i] - 1], vertices[3][faces[2][i] - 1] );

			vector va = new_vector( v1, v2 );
			vector vb = new_vector( v1, v3 );

			vector vn = vector_crossproduct( va, vb );

			if ( abs(vector_angle(vn, camera_normal)) < 90.0 ) continue;
		}

		// Drawing the face
		for (j = 0; j < 3; j++) {

			int curr = j;
			int next = (j+1) % 3;

			point ps = pt[curr];
			point pe = pt[next];

			// Use z-buffering if allowed
			if (hiding == 2)
				put_line_z(tmp, new_line( ps, pe ), c, bresenham, zbuf, vertices[3][curr], vertices[3][next]);
			else
				put_line(tmp, new_line( ps, pe ), c, bresenham);
			
		}

		// Filling the face using a random, eye-pleasing colour
		color cr = { ((rand() % 255) + 255) >> 1, ((rand() % 255) + 255) >> 1, ((rand() % 255) + 255) >> 1 };
		// if (hiding == 2)
		// 	fill_face_z(tmp, pt[0], pt[1], pt[2], cr, zbuf, vertices[3][j], vertices[3][(j+1) % 3]);
		// else
			fill_face(tmp, pt[0], pt[1], pt[2], cr);

	}
	raster_out(tmp);
	dispose_raster(tmp);

	for (i = 0; i < HEIGHT; i++) free(zbuf[i]);
	free(zbuf);

	return 0;
}