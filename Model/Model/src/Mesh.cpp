#include "Mesh.h"

void Mesh::Draw(Shader &shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	shader.use();

	//std::cout << textures.size() << std::endl;
	for (unsigned int i = 0; i < textures.size(); i++) {
		
		std::string name = textures[i].type;
		std::string number;
		if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		else if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
			
		}
		else {
			continue;
		}
		
		shader.setInt((name + number).c_str(), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		
	}
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex) * vertexs.size(), &vertexs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
