#pragma once
struct Ray {
	Ray(Vector3& org, Vector3& dir) :origin(org), direction(dir) {};
	inline Vector3 getPoint(const double t) const { return origin + t*direction; };
	Vector3 origin, direction;
};