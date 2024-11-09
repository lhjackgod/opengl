#pragma once
#include <stb_image.h>
#include <iostream>
#include <vector>
class Texture
{
public:
	Texture(const std::string& path);
	void Bind(uint32_t slot = 0);
	inline uint32_t GetTexture() { return m_RendererID; }
private:
	uint32_t m_RendererID;
};
class CubeTexture {
public:
	CubeTexture(const std::vector<std::string>& tp);
	void Bind(uint32_t slot = 0);
	inline uint32_t GetCubeTexture() { return m_RendererID; }
private:
	unsigned int m_RendererID;
	std::vector<std::string> texturePath;
};
