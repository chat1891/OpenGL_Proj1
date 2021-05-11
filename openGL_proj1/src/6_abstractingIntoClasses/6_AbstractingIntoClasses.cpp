#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include<sstream>

#include"Renderer.h"

#include"VertexBuffer.h"
#include"IndexBuffer.h"

/// <summary>
/// return multiple variables, can also use tuple
/// </summary>
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
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

static unsigned int CompileShader(unsigned int type, const std::string& source)
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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "glew init error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
    -0.7f,  0.5f, 0.0f,  // top right
    -0.6f, 0.5f, 0.0f,  // bottom right
    -0.7f, -0.5f, 0.0f,  // bottom left
    -0.6f, -0.5f, 0.0f,   // top left 
    -0.6f, -0.4f, 0.0f,  // bottom right
    -0.3f, -0.4f, 0.0f,  // bottom left
    -0.3f, -0.5f, 0.0f   // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // second triangle
        1, 2, 3,
        3, 4, 6,
        4, 5, 6
    };

    float colors[] = {
     1.0f, 0.0f, 0.0f, // red
     0.0f, 0.0f, 1.0f,
     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, 0.0f, // red
     0.0f, 0.0f, 1.0f,
     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 1.0f, 0.0f
    };

    // 1. bind vertex array object
    unsigned int VAO;
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glBindVertexArray(VAO));


    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    VertexBuffer vb(positions, sizeof(positions));

    //3. copy our index array in a element buffer for OpenGL to use
    IndexBuffer ib(indices, 12);

    // 4. then set the vertex attributes pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.Shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    //every uniform got assigned to an id. find id using bytes name
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    //---------unbind all the buffer
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    float b = 0.0;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //----bind buffer for every frame
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, 0.2, 0.3f, b, 1.0f));

        GLCall(glBindVertexArray(VAO));
        ib.Bind();

        //press 1 to draw line, press 2 to draw filled
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        GLCall(glBindVertexArray(VAO));
        GLCall(glUniform4f(location, 0.2, 0.3f, b, 1.0f));
        GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0));
        GLCall(glBindVertexArray(0));

        if (b > 1.0f) increment = -0.05;
        else if (b < 0.0f) increment = 0.05;
        b += increment;

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //clean up shader
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
