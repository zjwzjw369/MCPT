#pragma once
struct KDNode{
	AABB bbox;
	KDNode *left;
	KDNode *right;
	std::vector<Triangle*> triangles;
	KDNode() = default;
	KDNode* build(std::vector<Triangle*>&tris, int depth) const;
	bool hit(KDNode* node, const Ray& ray, Result *result, double *minDis)const;
};

bool KDNode::hit(KDNode* node, const Ray& ray, Result *result,double *minDis) const {
	Vector3 vchit;
	//检查光线是否与当前节点相交
	result->isEmpty = true;
	Result minResult;
	if (node->bbox.intersectWithAABB(ray,&vchit)) {
		bool hit_tri = false;
		//如果子节点还有三角形，递归两边并检查是否有交
		if (node->left->triangles.size()>0||node->right->triangles.size()>0) {
			bool hitleft = hit(node->left, ray, result, minDis);
			bool hitright = hit(node->right, ray, result, minDis);
			return hitleft || hitright;
		}else {
			//到达子节点
			//double minDis = INF;
			double t = 0.0, u = 0.0, v = 0.0;
			for (int i = 0; i < node->triangles.size(); i++) {
				//如果有交，返回交点以及法向
				if (IntersectTriangle(ray.origin,ray.direction, node->triangles[i]->vertics[0].Position, node->triangles[i]->vertics[1].Position, node->triangles[i]->vertics[2].Position,&t,&u,&v)) {
					if (t<*minDis) {
						hit_tri = true;
						*minDis = t;
						minResult.isEmpty = false;
						minResult.distance = *minDis;
						minResult.position = ray.origin + (*minDis)*ray.direction;
						minResult.normal = (node->triangles[i]->vertics[0].Normal *(1 - u - v) + node->triangles[i]->vertics[1].Normal*u + node->triangles[i]->vertics[2].Normal*v).Normalize();
						minResult.mesh = node->triangles[i]->mesh;
					}
				}
			}
			if (hit_tri) {
				*result = minResult;
				return true;
			}
		}

	}
	return false;
}


KDNode* KDNode::build(std::vector<Triangle*>& tris, int depth) const {
	KDNode* node = new KDNode();
	node->triangles = tris;
	node->left = NULL;
	node->right = NULL;
	node->bbox = AABB();
	if (tris.size()==0) {
		return node;
	}
	if (tris.size()==1) {
		node->bbox = tris[0]->get_bounding_box();
		node->left = new KDNode();
		node->right = new KDNode();
		node->left->triangles = std::vector<Triangle*>();
		node->right->triangles = std::vector<Triangle*>();
		return node;
	}
	node->bbox = tris[0]->get_bounding_box();
	//获得包围全部三角形的bounding box
	for (int i = 1; i < tris.size();i++) {
		node->bbox.expand(tris[i]->get_bounding_box());
	}

	//cout << node->bbox.max<<""<<node->bbox.min << endl;
	Vector3 midPoint(0.0);
	for (int i = 0; i < tris.size();i++) {
		//找到所有三角形的中点
		midPoint = midPoint + tris[i]->get_midpoint(); //(tris[i]->get_midpoint()*(1.0 / tris.size()));
	}
	midPoint /= tris.size();//将除法放在循环外面。
	std::vector<Triangle*> left_tris;
	std::vector<Triangle*> right_tris;
	int axis = node->bbox.longest_axis();
	for (int i = 0; i < tris.size();i++) {
		//在最长的轴上对三角形进行分割
		switch (axis){
		case 0:
			midPoint.x >= tris[i]->get_midpoint().x ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 1:
			midPoint.y >= tris[i]->get_midpoint().y ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 2:
			midPoint.z >= tris[i]->get_midpoint().z ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		}
	}
	if (left_tris.size() == 0 && right_tris.size()>0) left_tris = right_tris;
	if (right_tris.size() == 0 && left_tris.size()>0) right_tris = left_tris;
	int matches = 0;
	//如果50%三角形匹配，不再需要平分
	for (int i = 0; i < left_tris.size(); i++) {
		for (int j = 0; j < right_tris.size(); j++) {
			if (left_tris[i] == right_tris[j])
				matches++;
		}
	}
	if ((double)matches / left_tris.size() < 0.5 && (double)matches / right_tris.size() < 0.5) {
		//递归建树
		node->left = build(left_tris, depth + 1);
		node->right = build(right_tris, depth + 1);
	}
	else{
		node->left = new KDNode();
		node->right = new KDNode();
		node->left->triangles = std::vector<Triangle*>();
		node->right->triangles = std::vector<Triangle*>();
	}

	return node;
}
