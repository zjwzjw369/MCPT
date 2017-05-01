#pragma once
struct Camera {
	Camera(Vector3 eye, Vector3 front, Vector3 up, double fov) :
		eye(eye), front(front), refup(up), fov(fov) {
		this->right = this->front.Cross(this->refup);
		this->up = this->right.Cross(this->front);
		this->fovScale = tan(this->fov*0.5*M_PI / 180.0)*2.0;
	};
	Ray generateRay(double x, double y);
	Vector3 eye, front, refup, up, right;
	double fov, fovScale;
};

Ray Camera::generateRay(double x, double y) {
	Vector3 r = this->right*((x - 0.5)*this->fovScale);
	Vector3 u = this->up*((y - 0.5)*this->fovScale);
	Ray ray(this->eye, (this->front + r + u).Normalize());
	return ray;
}