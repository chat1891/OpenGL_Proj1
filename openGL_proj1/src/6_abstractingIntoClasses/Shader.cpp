#include "Shader.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include<sstream>

#include"Renderer.h"

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filePath)
{
    //Read line by line to check if it is vertex shader or fragment shader
    std::ifstream stream(filePath);
    std::string line;

    std::stringstream ss[2];

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        // string search functions return npos if nothing is found
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {//set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {//set mode to fragment
                type = ShaderType::FRAGMENT;

            }
        }
        else
        {
            //use type as index of array
            ss[(int)type] << line << '\n';
        }
    }
    return{ ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); //pointer of beginning of our data, or write as &source[0]
    //to use c_str(), string source has to be alive, use GetString(), if string got cleared up, it points to garbage, wont work
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //todo: error handling
    int result;
    //check if compilation was successful
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) // or !result
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); //allocate on stack dynamically
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed compiling" << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;

}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //string is the shader sauce code
    unsigned int program = glCreateProgram();
    //you can use GLUint instead of unsigned int, but if you use multiple API, you will need to include openGL everywhere, use c++ build in is more convinient
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char* infoLog = (char*)alloca(success * sizeof(char));
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR: Attach shader failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLoaction(name), v0, v1, v2, v3));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLoaction(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLoaction(name), value));
}

void Shader::SetUniformMat4f(const std::string& name, glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLoaction(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLoaction(const std::string& name)
{
    //it is not fast to retrieve this location again, again, again.
    //can cache it to only retrieve it first time then done
    if (m_Uniform_LocationCache.find(name) != m_Uniform_LocationCache.end())
    {
        return m_Uniform_LocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist!";
    }
    
    m_Uniform_LocationCache[name] = location;
    return location;
}

