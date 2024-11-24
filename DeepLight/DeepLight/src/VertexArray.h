#pragma once
#include <iostream>
class VertexArray
{
public:
	VertexArray(float* vertices, uint32_t size, int type = 0);
	void Bind();
	uint32_t GetVertexArray();
private:
	uint32_t m_rendererID;
};

