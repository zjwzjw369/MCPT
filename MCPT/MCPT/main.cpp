#include "head.hpp"
Camera camera(Vector3(0.0, 4.5, 14.5), Vector3(0.0, 0, -1.0), Vector3(0.0, 1.0, 0), 50);
Model model("scene01.obj");
const int width = 768;
const int height = 768;//768
Vector3 *pixels = new Vector3[width*height];
clock_t pre_time = clock();
clock_t now_time = clock();

Result SceneIntersect(Ray ray) {
	double minDis = INF;
	double t=0.0, u=0.0, v=0.0;
	Result minResult; 
	for (auto it = model.meshes.begin(), end = model.meshes.end(); it != end; ++it) {
		for (int i = 0; i < it->indices.size(); i+=3) {
			if (!IntersectTriangle(ray.origin, ray.direction,
				it->vertices[it->indices[i]].Position, it->vertices[it->indices[i + 1]].Position, it->vertices[it->indices[i + 2]].Position, &t, &u, &v))	continue;
			
			if (t<minDis) {
				minDis = t;
				minResult.isEmpty = false;
				minResult.distance = minDis;
				minResult.position = ray.origin + minDis*ray.direction;
				//minResult.normal = ((it->vertices[it->indices[i + 0]].Position - it->vertices[it->indices[i + 1]].Position).Cross(it->vertices[it->indices[i + 0]].Position - it->vertices[it->indices[i + 2]].Position)).Normalize();
				//minResult.normal = (it->vertices[it->indices[i + 0]].Normal+ it->vertices[it->indices[i + 1]].Normal=it->vertices[it->indices[i + 2]].Normal)/3;
				minResult.normal = (it->vertices[it->indices[i + 0]].Normal *(1 - u - v) + it->vertices[it->indices[i + 1]].Normal*u + it->vertices[it->indices[i + 2]].Normal*v).Normalize();
				minResult.mesh = &(*it);
			}
		}
	}
	
	return minResult;
}

void renderDepth(Vector3 *pixels, double maxDepth) {
	int index = 0;
	for (int y = 0; y < height; ++y) {
		double sy = 1 - y / static_cast<double>(height);
			//pre_time = clock();
		for (int x = 0; x < width; ++x) {
			double sx = x / static_cast<double>(width);
			Ray ray = camera.generateRay(sx, sy);
			Result result = SceneIntersect(ray);
			if (!result.isEmpty) {
				//cout << result.distance << endl;
				double depth = 1.0 - fmin(result.distance / maxDepth * 1.0, 1.0);
			//	pixels[index].x = depth;
			//	pixels[index].y = depth;
			//	pixels[index].z = depth;
				
				pixels[index] = result.normal;
			//	pixels[index].x = (result.normal.x);
			//	pixels[index].y = (result.normal.y);
			//	pixels[index].z = (result.normal.z);
				//pixels[index] = result.mesh->diffuseColor;
			}
			++index;
		}
			//now_time = clock();
		//	cout << (now_time - pre_time) / (double)CLOCKS_PER_SEC << endl;
	}
}

Color MCPTRecurisve(Ray ray,int depth,int seed) {
	depth++;
	Result result = SceneIntersect(ray);
	if (result.isEmpty) return Color(0.0);
	//cout << result.distance << endl;
	Color f = result.mesh->diffuseColor;
	double p = f.Max();
	//cout << p<< endl; 
	Vector3 normal_real = result.normal.Dot(ray.direction) < 0 ? result.normal : result.normal*-1.0;
	if (depth > 5) {
		if ((rand()%100)/100.0 < p)	f = f*(1 / p);
		else return result.mesh->ambientColor;
	}
	if (depth > 100) return result.mesh->ambientColor;
	switch (result.mesh->material){
	case Mesh::diffuse: {
		double r1 = 2.0 * M_PI*((rand() % 100) / 100.0);
		double r2 = ((rand() % 100) / 100.0);
		double r2_sqrt = sqrt(r2);
		Vector3 w = normal_real;
		Vector3 u = (fabs(w.x) > 0.1 ? Vector3(0.0, 1.0, 0.0) : Vector3(1.0).Cross(w)).Normalize();
		Vector3 v = w.Cross(u);
		Vector3 dir = ((u*cos(r1)*r2_sqrt + v*sin(r1)*r2_sqrt + w*sqrt(1 - r2))).Normalize();
		return result.mesh->ambientColor + f*(MCPTRecurisve(Ray(result.position, dir), depth,seed));
		break;
	}
	case Mesh::mirror: {
		Vector3 dir_ref = ray.direction - result.normal * 2 * result.normal.Dot(ray.direction);
		//cout << "a" << endl;
		return result.mesh->ambientColor + MCPTRecurisve(Ray(result.position, dir_ref), depth, seed);//
		break;
	}
	case Mesh::reflective: {
		Ray ray_refl(result.position, (ray.direction - result.normal * 2 * result.normal.Dot(ray.direction)));//

		bool into = result.normal.Dot(normal_real)>0;
		double nc = 1.0;//Õæ¿Õ
		double nt = result.mesh->REFRACTI;
		double nnt = into ? nc / nt : nt / nc;
		double ddn = ray.direction.Dot(normal_real)*-1;//ÈëÉä½ÇÓàÏÒ
		double sin_2_t = (1 - ddn*ddn)*nnt*nnt;
		if (sin_2_t > 1.0)//È«ÄÚ·´Éä
			return result.mesh->ambientColor + f*MCPTRecurisve(ray_refl, depth, seed);
		double sin_t = sqrt(sin_2_t);
		double cos_t = sqrt(1 - sin_2_t);
		Vector3 refr_dir = normal_real*(-1 * cos_t) + (ray.direction + normal_real*ddn).Normalize()*sin_t;
		Ray ray_refr(result.position, refr_dir);
		double F0 = (nc - nt)*(nc - nt) / ((nc + nt)*(nc + nt));
		double c = 1 - (into ? ddn : refr_dir.Dot(normal_real) * -1);
		double Fe = F0 + (1 - F0) * c * c * c * c * c;//·ÆÄù¶û·´Éä
		double Fr = 1 - Fe;//·ÆÄù¶ûÕÛÉä
		//cout << Fr << " " << Fe << endl;
		if (depth > 2) {
			double P = 0.25 + 0.5 * Fe;
			if (((rand() % 100) / 100.0) < P)
				return MCPTRecurisve(ray_refl, depth,seed) * (Fe / P);
			else
				return MCPTRecurisve(ray_refr, depth, seed) * (Fr / (1 - P));
		}
		else {
			return MCPTRecurisve(ray_refl, depth, seed) * Fe
				+ MCPTRecurisve(ray_refr, depth, seed) * Fr;
		}
		break;
	}
	default:
		break;
	}
	return Color(0.0);
}
void renderMCPT(Vector3 *pixels, int depth) {
	int index = 0;
	int nsample = 20;
	for (int y = 0; y < height; ++y) {
	//	now_time = clock();
		//system("cls");
		//cout << (now_time - pre_time) / (double)CLOCKS_PER_SEC << endl;
		fprintf(stderr, "\rRendering  %5.2f%%",  100.0 * y / (height - 1));
	//	pre_time = clock();
		double sy = 1 - y / static_cast<double>(height);
		for (int x = 0; x < width; ++x) {
			double sx = x / static_cast<double>(width);
			Ray ray = camera.generateRay(sx, sy);
			Color color(0.0);
			for (int s = 0; s < nsample;s++) {
				color=color+MCPTRecurisve(ray, depth, 0)/(double)nsample;
			}
			pixels[index].x = color.x;
			pixels[index].y = color.y;
			pixels[index].z = color.z;
			++index;
		}

	}
}
std::vector<Triangle> Scene;

void initTriangle() {
	for (auto it = model.meshes.begin(), end = model.meshes.end(); it != end; ++it) {
		for (int i = 0; i < it->indices.size(); i += 3) {
			Triangle triangle;
			triangle.mesh = &(*it);
			triangle.vertics.push_back(it->vertices[it->indices[i]]);
			triangle.vertics.push_back(it->vertices[it->indices[i+1]]);
			triangle.vertics.push_back(it->vertices[it->indices[i+2]]);
			Scene.push_back(triangle);
		}
	}
}

std::vector<Triangle*> KDrootNodeTriangle() {
	initTriangle();
	std::vector<Triangle*> rootNodeT;
	for (int i = 0; i < Scene.size(); ++i) {
		rootNodeT.push_back(&Scene[i]);
	}
	return rootNodeT;
}


Color KDTreeMCPTRecurisve(Ray ray, int depth, int seed,KDNode& KDTree) {
	depth++;
	Result result;
	double inf = INF;
	if (!KDTree.hit(&KDTree,ray,&result,&inf)) return Color(0.0);
	//cout << result.distance << endl;
	Color f = result.mesh->diffuseColor;
	double p = f.Max();
	//return result.normal;
	//cout << p<< endl; 
	Vector3 normal_real = result.normal.Dot(ray.direction) < 0 ? result.normal : result.normal*-1.0;
	if (depth > 5) {
		if ((rand() % 100) / 100.0 < p)	f = f*(1 / p);
		else return result.mesh->ambientColor;
	}
	if (depth > 100) return result.mesh->ambientColor;
	switch (result.mesh->material) {
	case Mesh::diffuse: {
		double r1 = 2.0 * M_PI*((rand() % 100) / 100.0);
		double r2 = ((rand() % 100) / 100.0);
		double r2_sqrt = sqrt(r2);
		Vector3 w = normal_real;
		Vector3 u = (fabs(w.x) > 0.1 ? Vector3(0.0, 1.0, 0.0) : Vector3(1.0,0.0,0.0).Cross(w)).Normalize();
		Vector3 v = w.Cross(u);
		Vector3 dir = ((u*cos(r1)*r2_sqrt + v*sin(r1)*r2_sqrt + w*sqrt(1 - r2))).Normalize();
		return result.mesh->ambientColor + f*(MCPTRecurisve(Ray(result.position, dir), depth, seed));
		break;
	}
	case Mesh::mirror: {
		Vector3 dir_ref = ray.direction - result.normal * 2 * result.normal.Dot(ray.direction);
		return result.mesh->ambientColor + MCPTRecurisve(Ray(result.position, dir_ref), depth, seed);//
		break;
	}
	case Mesh::reflective: {
		Ray ray_refl(result.position, (ray.direction - result.normal * 2 * result.normal.Dot(ray.direction)));//
		bool into = result.normal.Dot(normal_real)>0;
		double nc = 1.0;//Õæ¿Õ
		double nt = result.mesh->REFRACTI;
		double nnt = into ? nc / nt : nt / nc;
		double ddn = ray.direction.Dot(normal_real)*-1;//ÈëÉä½ÇÓàÏÒ
		double sin_2_t = (1 - ddn*ddn)*nnt*nnt;
		if (sin_2_t > 1.0)//È«ÄÚ·´Éä
			return result.mesh->ambientColor + f*MCPTRecurisve(ray_refl, depth, seed);
		double sin_t = sqrt(sin_2_t);
		double cos_t = sqrt(1 - sin_2_t);
		Vector3 refr_dir = normal_real*(-1 * cos_t) + (ray.direction + normal_real*ddn).Normalize()*sin_t;
		Ray ray_refr(result.position, refr_dir);
		double F0 = (nc - nt)*(nc - nt) / ((nc + nt)*(nc + nt));
		double c = 1 - (into ? ddn : refr_dir.Dot(normal_real) * -1);
		double Fe = F0 + (1 - F0) * c * c * c * c * c;//·ÆÄù¶û·´Éä
		double Fr = 1 - Fe;//·ÆÄù¶ûÕÛÉä
						   //cout << Fr << " " << Fe << endl;
		if (depth > 2) {
			double P = 0.25 + 0.5 * Fe;
			if (((rand() % 100) / 100.0) < P)
				return MCPTRecurisve(ray_refl, depth, seed) * (Fe / P);
			else
				return MCPTRecurisve(ray_refr, depth, seed) * (Fr / (1 - P));
		}
		else {
			return MCPTRecurisve(ray_refl, depth, seed) * Fe
				+ MCPTRecurisve(ray_refr, depth, seed) * Fr;
		}
		break;
	}
	default:
		break;
	}
	return Color(0.0);
}

void KDTreeRenderMCPT(Vector3 *pixels, int depth, KDNode& KDTree) {
	int index = 0;
	int nsample = 500;
	for (int y = 0; y < height; ++y) {
		//	now_time = clock();
		//system("cls");
		//cout << (now_time - pre_time) / (double)CLOCKS_PER_SEC << endl;
		fprintf(stderr, "\rRendering  %5.2f%%", 100.0 * y / (height - 1));
		//	pre_time = clock();
		double sy = 1 - y / static_cast<double>(height);
		for (int x = 0; x < width; ++x) {
			double sx = x / static_cast<double>(width);
			Ray ray = camera.generateRay(sx, sy);
			Color color(0.0);
			for (int s = 0; s < nsample; s++) {
				color = color + KDTreeMCPTRecurisve(ray, depth, 0, KDTree) / (double)nsample;
			}
			pixels[index].x = color.x;
			pixels[index].y = color.y;
			pixels[index].z = color.z;
			++index;
		}

	}
}

int main() {
	clock_t c1 = clock();
	std::vector<Triangle*> rootNodeT = KDrootNodeTriangle();
	KDNode* KDTree = new KDNode();
	KDTree=KDTree->build(rootNodeT, 0);
	KDTreeRenderMCPT(pixels, 0, *KDTree);
	//renderMCPT(pixels,0);
	clock_t c2 = clock();
	cout << (c2 - c1) / (double)CLOCKS_PER_SEC << endl;
	//renderDepth(pixels, 30);
	//Ray r(Vector3(1.0,1.0,1.0),Vector3(1.0,1.0,1.0));
	//for (int i = 0; i < 10000000000;i++) {
	//	SceneIntersect(r);
		//test();
	//}
	WritePPM(width, height, pixels, "MCPT.ppm");
	system("pause");
	return 0;
}