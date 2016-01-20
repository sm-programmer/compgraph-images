#include "vectors.h"

point3d new_point3d(double x, double y, double z) {
	point3d p = {x, y, z};
	return p;
}

vector new_vector(const point3d ps, const point3d pe) {
	vector v;
	v.x = pe.x - ps.x;
	v.y = pe.y - ps.y;
	v.z = pe.z - ps.z;
	return v;
}

double vector_magnitude(const vector v) {
	return sqrt(pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0));
}

vector vector_crossproduct(const vector a, const vector b) {
	vector ans;
	ans.x = a.y * b.z - b.y * a.z;
	ans.y = b.x * a.z - a.x * b.z;
	ans.z = a.x * b.y - b.x * a.y;
	return ans;
}

double vector_dotproduct(const vector a, const vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vector_angle(const vector a, const vector b) {
	double ma = vector_magnitude(a);
	double mb = vector_magnitude(b);
	double adb = vector_dotproduct(a, b);

	double angle = acos( adb / (ma * mb) ) * DEGREES;

	return angle;
}

vector vector_normalization(const vector v) {
	double mv = vector_magnitude(v);
	vector ans;
	ans.x = v.x / mv;
	ans.y = v.y / mv;
	ans.z = v.z / mv;
	return ans;
}
