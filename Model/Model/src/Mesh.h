#pragma once
#include "Shader.h"
#include <vector>
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
};
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};
class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures):
		vertexs(vertices), textures(textures), indices(indices) {
		setupMesh();
	}
	void Draw(Shader &shader);
private:
	void setupMesh();
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> vertexs;
	std::vector<Texture> textures;
	std::vector<unsigned int > indices;
};

