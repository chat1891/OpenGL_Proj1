#include "Renderer.h"
#include<iostream>

void GLClearError()
{//clear errors first
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& VA, const IndexBuffer& IB, const Shader& shader) const
{
    shader.Bind();
    VA.Bind();
    IB.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, IB.GetCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::DrawArray(const VertexArray& VA, const Shader& shader, const int count) const
{
    shader.Bind();
    VA.Bind();

    //GLCall(glDrawElements(GL_TRIANGLES, IB.GetCount(), GL_UNSIGNED_INT, 0));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
}
