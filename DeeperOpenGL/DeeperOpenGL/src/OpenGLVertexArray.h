#pragma once
#include <iostream>



class OpenGLVertexArray
{
public:
	OpenGLVertexArray(float* vertices, uint32_t size);
	void Bind();
	uint32_t GetVertexArray();
private:
	uint32_t m_RendererID;
};

