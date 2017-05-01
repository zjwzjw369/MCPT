#pragma once
struct AABB{
	void computeAABBFromOriginalPointSet(std::vector<Vertex> vertices);
	void AABB::extrameDistanceAlongDir(Vector3 dir, std::vector<Vertex> vertices, unsigned int* min, unsigned int*max);
	bool AABB::intersectWithAABB(Ray ray, Vector3* vcHit);
	void AABB::expand(AABB& exBox);
	int AABB::longest_axis() const;
	Vector3 max=0.0;
	Vector3 min=0.0;
	AABB() = default;
	AABB(Vector3 _max, Vector3 _min) :max(_max), min(_min) {};
}; 

void AABB::expand(AABB& exBox) {
	this->max.x = this->max.x > exBox.max.x ? this->max.x : exBox.max.x;
	this->max.y = this->max.y > exBox.max.y ? this->max.y : exBox.max.y;
	this->max.z = this->max.z > exBox.max.z ? this->max.z : exBox.max.z;
	this->min.x = this->min.x < exBox.min.x ? this->min.x : exBox.min.x;
	this->min.y = this->min.y < exBox.min.y ? this->min.y : exBox.min.y;
	this->min.z = this->min.z < exBox.min.z ? this->min.z : exBox.min.z;
}

int AABB::longest_axis() const{
	return (max - min).MaxDimension();
}


void AABB::computeAABBFromOriginalPointSet(std::vector<Vertex> vertices)
{
	unsigned int minX_i = 0, maxX_i = 0;
	extrameDistanceAlongDir(Vector3(1, 0, 0), vertices, &minX_i, &maxX_i);
	min.x = vertices[minX_i].Position.x;
	max.x = vertices[maxX_i].Position.x;

	unsigned int minY_i = 0, maxY_i = 0;
	extrameDistanceAlongDir(Vector3(0, 1, 0), vertices, &minY_i, &maxY_i);
	min.y = vertices[minY_i].Position.y;
	max.y = vertices[maxY_i].Position.y;

	unsigned int minZ_i = 0, maxZ_i = 0;
	extrameDistanceAlongDir(Vector3(0, 0, 1), vertices, &minZ_i, &maxZ_i);
	min.z = vertices[minZ_i].Position.z;
	max.z = vertices[maxZ_i].Position.z;
}// end for computeAABBFromOriginalPointSet  

void AABB::extrameDistanceAlongDir(Vector3 dir, std::vector<Vertex> vertices, unsigned int* min, unsigned int*max)
{
	double maxProj = -INF, minProj = INF;
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		double proj = 0;

		proj = vertices[i].Position.Dot(dir);

		if (proj > maxProj)
		{
			maxProj = proj;
			*max = i;
		}

		if (proj < minProj)
		{
			minProj = proj;
			*min = i;
		}
	}// end for  
}// end for extrameDistanceAlongDir  

bool AABB::intersectWithAABB(Ray ray, Vector3* vcHit)
{
	double tmin = 0.0f;
	double tmax = INF;

	//The plane perpendicular to x-axie  
	if (abs(ray.direction.x) < 0.0000001) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (ray.origin.x < min.x || ray.origin.x > max.x)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		double ood = 1.0f / ray.direction.x;
		double t1 = (min.x - ray.origin.x) * ood;
		double t2 = (max.x - ray.origin.x) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}// end for perpendicular to x-axie  

	 //The plane perpendicular to y-axie  
	if (abs(ray.direction.y) < 0.0000001) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (ray.origin.y < min.y || ray.origin.y > max.y)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		double ood = 1.0f / ray.direction.y;
		double t1 = (min.y - ray.origin.y) * ood;
		double t2 = (max.y - ray.origin.y) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}// end for perpendicular to y-axie  

	 //The plane perpendicular to z-axie  
	if (abs(ray.direction.z) < 0.0000001) //If the ray parallel to the plane  
	{
		//If the ray is not within AABB box, then not intersecting  
		if (ray.origin.z < min.z || ray.origin.z > max.z)
			return false;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane  
		double ood = 1.0f / ray.direction.z;
		double t1 = (min.z - ray.origin.z) * ood;
		double t2 = (max.z - ray.origin.z) * ood;

		//Make t1 be intersecting with the near plane, t2 with the far plane  
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Compute the intersection of slab intersection intervals  
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;

		//Exit with no collision as soon as slab intersection becomes empty  
		if (tmin > tmax) return false;
	}// end for perpendicular to z-axie  

	vcHit->x = ray.origin.x + tmin * ray.direction.x;
	vcHit->y = ray.origin.y + tmin * ray.direction.y;
	vcHit->z = ray.origin.z + tmin * ray.direction.z;
	return true;
}// end for intersectWithAABB</span>  