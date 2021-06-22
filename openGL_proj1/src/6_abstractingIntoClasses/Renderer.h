#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"


#define ASSERT(x) if(!(x)) __debugbreak();

#ifdef DEBUG
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__));
#else
#define GLCall(x) x;
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public: 
    void Clear() const;
    void Draw(const VertexArray& VA, const IndexBuffer& IB, const Shader& shader) const;
    void DrawArray(const VertexArray& VA, const Shader& shader, const int count) const;
};

