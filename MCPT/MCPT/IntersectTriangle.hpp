#pragma once
// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection
struct  Result{
	Vector3 position;
	Vector3 normal;
	double distance;
	Mesh *mesh;
	bool isEmpty = true;
};

bool IntersectTriangle(const Vector3& orig, const Vector3& dir,
	Vector3& v0, Vector3& v1, Vector3& v2,
	double* t, double* u, double* v)
{
	// E1
	Vector3 E1 = v1 - v0;

	// E2
	Vector3 E2 = v2 - v0;

	// P
	Vector3 P = dir.Cross(E2);

	// determinant
	double det = E1.Dot(P);

	// keep det > 0, modify T accordingly
	Vector3 T;
	if (det >0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0000001)
		return false;

	// Calculate u and make sure u <= 1
	*u = T.Dot(P);
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	Vector3 Q = T.Cross(E1);

	// Calculate v and make sure u + v <= 1
	*v = dir.Dot(Q);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = E2.Dot(Q);

	if (*t<0.0000001) {
		return false;
	}
	double fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;
	if ((*t)<0.0000001) {
		return false;
	}

	return true;
}