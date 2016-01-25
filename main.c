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

	matrix_double scale = scale_by(sx, sy, sz);
	matrix_double tras = traslate(tx, ty, tz);
	matrix_double rot = rotate(rx, ry, rz);
	matrix_double proj = projection(fd);
	
	file_data vnf = readobj(argv[1]);

	// int p;
	// for (p = 0; p < vnf.vertices.num_elems; p++) {
	// 	point3d t = (vnf.vertices.data)[p];
	// 	fprintf(stderr, "(%f,%f,%f)\n", t.x, t.y, t.z);
	// }

	// Join all the transformations in a single matrix
	// Operation order: Scaling, rotation, traslation and projection
	matrix_double t1 = product(proj, tras);
	matrix_double t2 = product(t1, rot);
	matrix_double t3 = product(t2, scale);

	apply_matrix(t3, vnf.vertices);
		
	dispose_matrix(&t3);
	dispose_matrix(&t2);
	dispose_matrix(&t1);
	dispose_matrix(&scale);
	dispose_matrix(&tras);
	dispose_matrix(&rot);
	dispose_matrix(&proj);

	// Projection sets w = z, so divide everything by w.
	for (i = 0; i < vnf.vertices.num_elems; i++) {
		point3d tmp = (vnf.vertices.data)[i];
		tmp.x /= tmp.w;
		tmp.y /= tmp.w;
		tmp.z /= tmp.w;
		(vnf.vertices.data)[i] = tmp;
	}

	// The camera normal (taking advantage of perspective projection)
	vector camera_normal = new_vector( new_point3d(0.0, 0.0, 0.0, 1.0), new_point3d(0.0, 0.0, 1.0, 1.0) );

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
	for (i = 0; i < vnf.faces.num_elems; i++) {
		point3d v1 = (vnf.vertices.data)[((vnf.faces.data)[i].v1) - 1];
		point3d v2 = (vnf.vertices.data)[((vnf.faces.data)[i].v2) - 1];
		point3d v3 = (vnf.vertices.data)[((vnf.faces.data)[i].v3) - 1];

		point pt[3] = { new_point(v1.x, v1.y), new_point(v2.x, v2.y), new_point(v3.x, v3.y) };

		for (j = 0; j < 3; j++)
			pt[j] = raster_translate(pt[j], center, invertX, invertY);

		// Hiding faces using face normals
		if (hiding == 1) {			
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
			// if (hiding == 2)
			// 	put_line_z(tmp, new_line( ps, pe ), c, bresenham, zbuf, vertices[3][curr], vertices[3][next]);
			// else
			put_line(tmp, new_line( ps, pe ), c, bresenham);
			
		}

		// Filling the face using a random, eye-pleasing colour
		color cr = { ((rand() % 255) + 255) >> 1, ((rand() % 255) + 255) >> 1, ((rand() % 255) + 255) >> 1 };
		// if (hiding == 2)
		// 	fill_face_z(tmp, pt[0], pt[1], pt[2], cr, zbuf, vertices[3][j], vertices[3][(j+1) % 3]);
		// elseelse
			fill_face(tmp, pt[0], pt[1], pt[2], cr);

	}
	raster_out(tmp);
	dispose_raster(tmp);

	for (i = 0; i < HEIGHT; i++) free(zbuf[i]);
	free(zbuf);

	free(vnf.vertices.data);
	free(vnf.faces.data);

	return 0;
}