#include "Texture.h"
#include <glad/glad.h>
Texture::Texture(const std::string& path)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	int width, height, channel;
	const stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
	GLenum dataFormat, internalFormat;
	if (channel == 1) {
		dataFormat = internalFormat = GL_RED;
	}
	else if (channel == 3) {
		dataFormat = internalFormat = GL_RGB;
	}
	else if (channel == 4) {
		dataFormat = internalFormat = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Bind(uint32_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}
