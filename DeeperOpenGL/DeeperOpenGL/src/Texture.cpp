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


CubeTexture::CubeTexture(const std::vector<std::string>& tp)
	:texturePath(tp)
{
	if (tp.size() == 0)
	{
		return;
	}
	hasTexture = true;
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	for (int i = 0; i < texturePath.size(); i++) {
		int width, height, channels;
		stbi_uc* data = stbi_load(texturePath[i].c_str(), &width, &height, &channels, 0);
		GLenum dataFormat;
		if (channels == 1)
			dataFormat = GL_RED;
		else if (channels == 3)
			dataFormat = GL_RGB;
		else if (channels == 4)
			dataFormat = GL_RGBA;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data
		);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeTexture::Bind(uint32_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}
