#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Texture.h"
struct Point {
	glm::vec3 aPos;
	glm::vec3 aNormal;
	glm::vec2 aTexCoords;
};
struct m_Texture{
	unsigned int id;
	std::string type;
	std::string path;
};
class Mesh{
public:
	Mesh(std::vector<Point> p, std::vector<unsigned int> indice, std::vector<m_Texture> ts)
		:points(p), indices(indice), textures(ts){setupMesh();}
	
	void Draw(Shader shader, CubeTexture sky = CubeTexture());
private:
	uint32_t VAO, VBO, EBO;
	std::vector<Point> points;
	std::vector<unsigned int> indices;
	std::vector<m_Texture> textures;
private:
	void setupMesh();
};
class MyModel
{
public:
	MyModel(const std::string& path);
	void Draw(Shader shader, CubeTexture sky = CubeTexture());
private:
	Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
	void ProcessNode(const aiNode* node, const aiScene* scene);
	std::vector<m_Texture> processTexture(const aiMaterial* material, const aiTextureType type, const std::string& typeName);
private:

	std::vector<Mesh> meshes;
	std::string directory;
};

