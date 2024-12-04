#pragma once
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
struct Vertex
{
public:
	Vertex(glm::vec3 apos, glm::vec3 anormal, glm::vec3 atangent, glm::vec2 atexcoord)
		:m_VertexPosition(apos), m_VertexNormal(anormal), m_Tangent(atangent), m_VertexTexCoord(atexcoord)
	{

	}
	Vertex() = default;
	glm::vec3 m_VertexPosition;
	glm::vec3 m_VertexNormal;
	glm::vec3 m_Tangent;
	glm::vec2 m_VertexTexCoord;
};

enum  class TEXTURE_TYPE
{
	DIFFUSE = 0,
	SPECULAR,
	NORMAL
};
struct Texture
{
	std::string texPath;
	TEXTURE_TYPE type;
	uint32_t m_TextureID;
	float Ns;
	glm::vec3 m_NormalDiffuse;
	glm::vec3 m_NormalAmbient;
	glm::vec3 m_NormalSpecular;
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<Texture>& textures,
		const std::vector<uint32_t> indices)
		:vertices(vertices), textures(textures), indices(indices) {
		Setup();
	}
	void Draw(Shader shader);

private:
	void Setup();
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<uint32_t> indices;
	uint32_t m_MeshVao;
};
class Model
{
public:
	Model(const std::string& objPath);
	void Draw(Shader shader);
private:
	void ProcessNode(const aiScene* scene, const aiNode* node);
	Mesh ProcessMesh(const aiScene* scene, const aiMesh* mesh);
	
	std::vector<Texture> ProcessMaterial(const aiScene* scene, const aiMaterial* material, const aiTextureType& type, const TEXTURE_TYPE& m_type);
	std::vector<Mesh> meshes;
	std::vector<Texture> texs;

	std::string dirc;
};

