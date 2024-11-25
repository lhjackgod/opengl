#pragma once
#include <iostream>
class VertexArray
{
public:
	VertexArray(float* vertices, uint32_t size, int type = 0);
	VertexArray() = default;
	VertexArray(const VertexArray& copy) = default;
	void Bind();
	uint32_t GetVertexArray();
	void SetData(float* vertices, uint32_t size, int type = 0);
private:
	uint32_t m_rendererID;
	float* m_Vertices;
};

