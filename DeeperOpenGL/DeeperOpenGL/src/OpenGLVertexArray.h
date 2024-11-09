#pragma once
#include <iostream>



class OpenGLVertexArray
{
public:
	OpenGLVertexArray(float* vertices, uint32_t size, int type = 0);
	void Bind();
	uint32_t GetVertexArray();
private:
	uint32_t m_RendererID;
};

