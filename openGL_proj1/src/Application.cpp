#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>

static void ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) //return string pos
        {
            if (line.find("vertex") != std::string::npos)
            {
                //set mode to vertex
            }
            else if (line.find("fragment") != std::string::npos)
            {
                //set mode to fragment
            }
        }
    }
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
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) // or !result
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); //allocate on stack dynamically
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed compiling" << (type==GL_VERTEX_SHADER? "Vertex":"Fragment") << "shader!" << std::endl;
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //set up GLEW init it needs to run in a valid a valid OpenGL rendering context, this why it is here
    //file:///C:/Dev/CPP_Projects/openGL_proj1/Dependencies/GLEW/doc/basic.html
    if (glewInit() != GLEW_OK)
        std::cout << "glew init error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    //vertex, it is a point on the geometry
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); // create 1 buffer with adddress point to it, gives back an id
    //everything generate in open GL gets assigned an unique identifier: 0, 1,2 
    glBindBuffer(GL_ARRAY_BUFFER, buffer); //select the buffer, have to select it then can draw on it, if selected nothing, it will do nothing since nothing is selected
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); //size is in bytes

    glEnableVertexAttribArray(0);
    //after buffer is binded
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        //size 2 is 2 number per position attribute, in 2d
        //for pointer, if there is another attribute, use 8 byte instead of 0, 8 byte from beginning(position)
        //for better practice, prob have a strut set up, and use offset micro to figure it out, instead just write 8
        
    //stride: amount of bites between each vertex, ie. 2 floats for position, 3 floats for texture coordinate, 3 floats for normal  
    //2 floats * 4 bytes + 3 floats * 4 bytes + 3 floats * 4 bytes = 32 bytes is size of vertex
    
    //pointer is pointing into actual attribute, inside one vertex, for position, offset is 0
    //for texture coordinate, offset is 2 floats * 4 bytes = 8 bytes to offset from position 
    //for normal, offset is 2 floats * 4 bytes + 3 floats * 4 bytes = 20 bytes, to offset position and texture coordinates

    
    //=====================create shader:
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location =0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location =0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(0.0, 1.0, 1.0, 1.0);\n" //r g b a
        "}\n";
        

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //it draws triangle because it is binded. basically select it
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        //glDrawElements(GL_TRIANGLES, 3,)



        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //clean up shader
    glDeleteProgram(shader);

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}