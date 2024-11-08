#include "Model.h"
#include<stb_image.h>
void Model::Draw(Shader &shader)
{
	for (int i = 0; i < meshs.size(); i++) {
		meshs[i].Draw(shader);
		
	}
}

void Model::loadModel(const std::string &path)
{
	std::cout << path << std::endl;
	directory = path.substr(0, path.find_last_of('/'));
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR: ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	
	processNode(scene->mRootNode, scene);
	
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++) {
		meshs.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int>indices;
	std::vector<Texture> texs;
	for (int i = 0; i < mesh->mNumVertices; i++) {
		glm::vec3 v;
		Vertex ver;
		v.x = mesh->mVertices[i].x;
		v.y = mesh->mVertices[i].y;
		v.z = mesh->mVertices[i].z;
		ver.Position = v;
		if (mesh->HasNormals()) {
			glm::vec3 n;
			n.x = mesh->mNormals[i].x;
			n.y = mesh->mNormals[i].y;
			n.z = mesh->mNormals[i].z;
			ver.Normal = n;
		}
		if (mesh->mTextureCoords[0]) {
			glm::vec2 uv;
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
			ver.UV = uv;
		}
		vertices.push_back(ver);
	}
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
	texs.insert(texs.end(),diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
	texs.insert(texs.end(), specularMaps.begin(), specularMaps.end());
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_NORMALS, "texture_normal");
	texs.insert(texs.end(), normalMaps.begin(), normalMaps.end());
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, "texture_height");
	texs.insert(texs.end(), heightMaps.begin(), heightMaps.end());
	
	return Mesh(vertices, indices, texs);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> texture;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString aistr;
		mat->GetTexture(type, i, &aistr);
		bool skip = false;
		for (unsigned int j = 0; j < texture_loaded.size(); j++) {
			if (std::strcmp(texture_loaded[j].path.data(), aistr.C_Str()) == 0) {
				texture.push_back(texture_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture tex;
			tex.id = TextureFromFile(aistr.C_Str(), this->directory, false);
			tex.path = aistr.C_Str();
			tex.type = typeName;
			texture.push_back(tex);
			texture_loaded.push_back(tex);
		}
	}
	return texture;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& direc, bool gamma)
{
	unsigned int texid;
	std::string name = std::string(path);
	std::string fileName = direc + "/" + name;
	std::cout << fileName << std::endl;
	glGenTextures(1, &texid);
	int width, height, channle;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channle, 0);
	
	GLenum format;
	if (data) {
		if (channle == 1) {
			format = GL_RED;
		}
		else if (channle == 3) format == GL_RGB;
		else if (channle == 4) format == GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "Error binding texture: " << error << std::endl;
		}
		GLint width2, height2, internalFormat;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width2);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height2);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
		std::cout << width2 << " " << height2 << std::endl;
		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	
	return texid;
}

