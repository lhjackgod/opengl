#pragma once
#include <stb_image.h>
#include <iostream>
class Texture
{
public:
	Texture(const std::string& path);
	void Bind(uint32_t slot = 0);
	inline uint32_t GetTexture() { return m_RendererID; }
private:
	uint32_t m_RendererID;
};

