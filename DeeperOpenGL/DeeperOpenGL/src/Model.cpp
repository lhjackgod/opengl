#include "Model.h"
#include <stb_image.h>
static unsigned int TextureFromFile(const std::string& name, const std::string directory, bool gamma);
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * points.size(), &points[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(offsetof(Point, aPos)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(offsetof(Point, aNormal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(offsetof(Point, aTexCoords)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Mesh::Draw(Shader shader, uint32_t amount, CubeTexture sky)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int ambientNr = 1;
    unsigned int heightNr = 1;
    shader.use();
    for(uint32_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        std::string name;
        if(textures[i].type == "texture_diffuse")
        {
            name = "material.diffuse" + std::to_string(diffuseNr++);
        }
        else if (textures[i].type == "texture_specular")
        {
            name = "material.specular" + std::to_string(specularNr++);
        }
        else if (textures[i].type == "texture_ambient")
        {
            name = "material.ambient" + std::to_string(ambientNr++);
        }
        else if (textures[i].type == "texture_height")
        {
            name = "material.height" + std::to_string(heightNr++);
        }
        shader.setInt(name, i);
    }
    if (sky.hasTexture == true)
    {
        shader.setInt("haveEnviromentLight", 1);
        sky.Bind(textures.size());
        shader.setInt("material.enviromentLight", textures.size());
    }
    else {
        
        shader.setInt("haveEnviromentLight", 0);
    }
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount);
    //glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
MyModel::MyModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (scene == nullptr) {
        std::cout << "aiScene init wrong" << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}
void MyModel::Draw(Shader shader, uint32_t amount, CubeTexture sky)
{
    for (uint32_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader, amount, sky);
    }
}
void MyModel::ProcessNode(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh MyModel::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Point> vertexs;
    std::vector<m_Texture> tex;
    std::vector<unsigned int> indices;
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D meshPos = mesh->mVertices[i];
        aiVector3D meshNormal = mesh->mNormals[i];
        aiVector3D meshUV = mesh->mTextureCoords[0][i];
        vertexs.push_back({
            glm::vec3(meshPos.x, meshPos.y, meshPos.z),
            glm::vec3(meshNormal.x, meshNormal.y, meshNormal.z),
            glm::vec2(meshUV.x, meshUV.y)
        });
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<m_Texture> diffuseMaps = processTexture(material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
    tex.insert(tex.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<m_Texture> specularMaps = processTexture(material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
    tex.insert(tex.end(), specularMaps.begin(), specularMaps.end());
    std::vector<m_Texture> ambientMaps = processTexture(material, aiTextureType::aiTextureType_AMBIENT, "texture_ambient");
    tex.insert(tex.end(), ambientMaps.begin(), ambientMaps.end());
    std::vector<m_Texture> hightMaps = processTexture(material, aiTextureType::aiTextureType_HEIGHT, "texture_height");
    tex.insert(tex.end(), hightMaps.begin(), hightMaps.end());
    return Mesh(vertexs, indices, tex);
}

std::vector<m_Texture> MyModel::processTexture(const aiMaterial* material, const aiTextureType type, const std::string& typeName)
{
    std::vector<m_Texture> textures;
    for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString path;
        material->GetTexture(type, i, &path);
        textures.push_back({
            TextureFromFile(path.C_Str(), directory, false),
            typeName,
            path.C_Str()
        });
    }
    return textures;
}
static unsigned int TextureFromFile(const std::string& name, const std::string directory, bool gamma)
{
    std::string fileName = directory + "/" + name;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    int width, height, channel;
    GLenum dataFormat;
    stbi_uc* data = stbi_load(fileName.c_str(), &width, &height, &channel, 0);
    if (!data) {
        std::cout << "texture wrong" << std::endl;
        return 0;
    }
    switch (channel)
    {
    case 1:
        dataFormat = GL_RED;
        break;
    case 3:
        dataFormat = GL_RGB;
        break;
    case 4:
        dataFormat = GL_RGBA;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(data);
    return textureID;
}