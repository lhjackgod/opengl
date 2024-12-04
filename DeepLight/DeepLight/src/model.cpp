#include "model.h"
#include "Shader.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <stb_image.h>
static uint32_t GenTexID(const std::string& texPath, bool use_s = true);
Model::Model(const std::string& objPath)
{
	int index = objPath.find_last_of('/');
	dirc = objPath.substr(0, index);
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(objPath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
	);
	if (scene == NULL)
	{
		std::cout << "error to load obj" << std::endl;
		return;
	}
	ProcessNode(scene, scene->mRootNode);
}

void Model::Draw(Shader shader)
{
	for (uint32_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::ProcessNode(const aiScene* scene, const aiNode* node)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(scene, node->mChildren[i]);
	}
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		meshes.emplace_back(ProcessMesh(scene, scene->mMeshes[node->mMeshes[i]]));
	}
}

Mesh Model::ProcessMesh(const aiScene* scene, const aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<Texture> MeshToalTex;
	std::vector<uint32_t> indices;
	
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D vertex = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D uv = mesh->mTextureCoords[0][i];
		aiVector3D tangent = mesh->mTangents[i];
		glm::vec3 aPos = glm::vec3(vertex.x, vertex.y, vertex.z);
		glm::vec3 aNormal = glm::vec3(normal.x, normal.y, normal.z);
		glm::vec2 aTexCoord = glm::vec2(uv.x, uv.y);
		glm::vec3 aTangent = glm::vec3(tangent.x, tangent.y, tangent.z);
		Vertex v{ aPos, aNormal, aTangent, aTexCoord };
		vertices.push_back(v);
	}
	
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> diffuseTexs = ProcessMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType::aiTextureType_DIFFUSE, TEXTURE_TYPE::DIFFUSE);
	MeshToalTex.insert(MeshToalTex.end(), diffuseTexs.begin(), diffuseTexs.end());
	std::vector<Texture> specularTexs = ProcessMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType::aiTextureType_SPECULAR, TEXTURE_TYPE::SPECULAR);
	MeshToalTex.insert(MeshToalTex.end(), specularTexs.begin(), specularTexs.end());
	std::vector<Texture> normals = ProcessMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType::aiTextureType_HEIGHT, TEXTURE_TYPE::NORMAL);
	MeshToalTex.insert(MeshToalTex.end(), normals.begin(), normals.end());
	return Mesh(vertices, MeshToalTex, indices);
}

std::vector<Texture> Model::ProcessMaterial(const aiScene* scene, const aiMaterial* material, const aiTextureType& type, const TEXTURE_TYPE& m_type)
{
	std::vector<Texture> res;
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString aistring;
		material->GetTexture(type, i, &aistring);
		bool skip = false;
		std::string PicPath = dirc + "/" + aistring.C_Str();

		for (uint32_t j = 0; j < texs.size(); j++)
		{
			if (texs[j].texPath == PicPath)
			{
				res.push_back(texs[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture tex;
			tex.texPath = PicPath;
			tex.type = m_type;
			tex.m_TextureID = GenTexID(PicPath);
			aiColor3D ka, kd, ks;
			float ns;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
			material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
			material->Get(AI_MATKEY_COLOR_SPECULAR, ks);
			material->Get(AI_MATKEY_SHININESS, ns);
			tex.m_NormalDiffuse = glm::vec3(kd.r, kd.g, kd.b);
			tex.m_NormalAmbient = glm::vec3(ka.r, ka.g, ka.b);
			tex.m_NormalSpecular = glm::vec3(ks.r, ks.g, ks.b);
			tex.Ns = ns;
			texs.push_back(tex);
			res.push_back(tex);
		}
	}
	return res;
}
static uint32_t GenTexID(const std::string& texPath, bool use_s)
{
	static uint32_t m_RendererID;
	stbi_set_flip_vertically_on_load(true);
	int width, height, channel;
	GLenum dataFormat, internalFormat;
	stbi_uc *data = stbi_load(texPath.c_str(), &width, &height, &channel, 0);
	if (!data)
	{
		std::cout << "model error load pic: \t" + texPath << std::endl;
		return 0;
	}
	switch (channel)
	{
	case 1:
		dataFormat = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		dataFormat = GL_RGB;
		internalFormat = GL_RGB;
		if (use_s)
		{
			internalFormat = GL_SRGB;
		}
		break;
	case 4:
		dataFormat = internalFormat = GL_RGBA;
		if (use_s)
		{
			internalFormat = GL_SRGB_ALPHA;
		}
		break;
	}
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glGenerateMipmap(GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return m_RendererID;
}

void Mesh::Draw(Shader shader)
{
	shader.use();
	uint32_t diffuse_count = 0, specular_count = 0, normal_count = 0;

	for (uint32_t i = 0; i < textures.size(); i++)
	{
		std::string name = "";
		switch (textures[i].type)
		{
		case TEXTURE_TYPE::DIFFUSE:
			name = "uTextures.diffuse" + std::to_string(diffuse_count++);
			break;
		case TEXTURE_TYPE::NORMAL:
			name = "uTextures.normal" + std::to_string(normal_count++);
			break;
		case TEXTURE_TYPE::SPECULAR:
			name = "uTextures.specular" + std::to_string(specular_count++);
			shader.SetValue("uTextures.ns", textures[i].Ns);
			break;
			
		}
		shader.SetValue(name, i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].m_TextureID);
	}
	glBindVertexArray(m_MeshVao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Setup()
{
	uint32_t vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_VertexNormal)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Tangent)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_VertexTexCoord)));
	}

	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * (sizeof(uint32_t)), &indices[0], GL_STATIC_DRAW);
	}
	m_MeshVao = vao;
	glBindVertexArray(0);
}
