#pragma once
struct  Mesh{
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	enum Material{
		diffuse, mirror,reflective
	}material;
	Color diffuseColor, specularColor, ambientColor;
	double shininess=0.0, REFRACTI=0.0;
	Mesh(std::vector<Vertex> vertices, std::vector<int> indices, Material mtl, Color diffuseColor, Color specularColor, Color ambientColor);
	Mesh(std::vector<Vertex> vertices, std::vector<int> indices, Material mtl);
};


struct  Triangle{
	Mesh *mesh;
	std::vector<Vertex> vertics;
	AABB bbox;
	AABB get_bounding_box();
	Vector3 get_midpoint() const;
};

Vector3 Triangle::get_midpoint() const{
	return (bbox.max + bbox.min) / 2.0;
}

AABB Triangle::get_bounding_box() {
	bbox.computeAABBFromOriginalPointSet(vertics);
	return bbox;
}

class  Model{
public:
	Model(string path) { this->loadModel(path); };
	~Model() {};
	std::vector<Mesh> meshes;
private:
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	string directory;
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices, Material mtl) {
	this->vertices = vertices;
	this->indices = indices;
	this->material = mtl;
	this->diffuseColor = Color(0.0);
	this->specularColor = Color(0.0);
	this->ambientColor = Color(0.0);
}
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices, Material mtl, Color diffuseColor, Color specularColor, Color ambientColor) {
	this->vertices = vertices;
	this->indices = indices;
	this->material = mtl;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->ambientColor = ambientColor;
}

void Model::loadModel(string path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of('/'));

	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node,const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// Then do the same for each of its children
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Color diffuseC=Color(0.0), specularC = Color(0.0), ambientC = Color(0.0);
	ai_real shininess, REFRACTI;
	Mesh::Material mtl = Mesh::diffuse;
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		Vector3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector; 
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector.Normalize();
		vertices.push_back(vertex);
	}
	for (int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0){
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuse;
		aiColor4D specular;
		aiColor4D ambient;
		aiColor4D emission;
		//diffuse is kd,specular is ks,ambient is ka,shininess is Ns,REFRACTI is Ni
	//	cout << mesh->mMaterialIndex << endl;
		unsigned int  max=1; 
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
			diffuseC = Color(diffuse.r, diffuse.g, diffuse.b);
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular)) {
			specularC = Color(specular.r, specular.g, specular.b);
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
			ambientC = Color(ambient.r, ambient.g, ambient.b);
			ambientC *= 100.0;
			//cout << ambientC << endl;
		}

		//aiGetMaterialFloatArray(material, AI_MATKEY_OPACITY, &strength, &max);
	//	cout << strength << endl;
		aiGetMaterialFloatArray(material, AI_MATKEY_REFRACTI, &REFRACTI, &max);
		aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
		if (shininess>0) {
			mtl = Mesh::mirror;
			diffuseC = Vector3(1.0);
		}
		if (REFRACTI>1.00001) {
			mtl = Mesh::reflective;
		}
		//int ret1 = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);

	}
	//cout << mtl << endl;
	Mesh _mesh(vertices, indices, mtl, diffuseC, specularC, ambientC);
	_mesh.shininess = shininess;
	_mesh.REFRACTI = REFRACTI;
	return _mesh;
}