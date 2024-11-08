#pragma once
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Model
{
public:
	Model(const char* path) { loadModel(path); }
	void Draw(Shader &shader);
private:
	std::string directory;
	void loadModel(const std::string& path);
	void processNode(const aiNode* node, const aiScene* scene);
	Mesh  processMesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& direc, bool gamma);
	std::vector<Mesh> meshs;
	std::vector<Texture> texture_loaded;

};

