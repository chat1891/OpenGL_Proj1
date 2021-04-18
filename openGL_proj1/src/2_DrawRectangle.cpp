#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>

/*
GLEW is just used to retrieve all the OpenGL function pointers, GLEW has nothing to do with the OpenGL backend and its operations.
*/
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "glew init error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;


    //-rectangle consists of 2 triangles, but we dont want to define same points twice
    //so we use indices
    //float positions[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};
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
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    //3. copy our index array in a element buffer for OpenGL to use
    unsigned int EBO;
    glGenBuffers(1, &EBO); 
    //GL_ELEMENT_ARRAY_BUFFER as the buffer type.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    // 4. then set the vertex attributes pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    //=====================create shader:
    //-layout (location = 0). This sets the location of the vertex attribute to 0
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
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //only draw lines
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //-it draws triangle because it is binded. basically select it
        //glBindVertexArray(VAO);

        //press 1 to draw line, press 2 to draw filled
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
        /*
        The indices are stored in a buffer (just like the vertices) 
        and whenever we use glDrawElements OpenGL (and your GPU) reads the corresponding buffers to draw what it's supposed to draw. 
        So your GPU reads from the index buffer, to get the vertices it needs to draw from the vertex data buffer.

        It reads its indices from the GL_ELEMENT_ARRAY_BUFFER buffer so we have to bind the correct buffer before drawing.
        The VAO stores the bind calls to element buffer objects
        so as long as we've bound an EBO while a VAO was bound, binding the VAO before drawing will bind the correct index buffer.

        The VBO binding calls are necessary for when we specify the vertex attribute layout using glVertexAttribPointer. 
        When we call glVertexAttribPointer OpenGL automatically sets the currently bound VBO as the vertex attribute's memory buffer 
        so all subsequent drawing calls will use the vertex data from the VBO's memory used as specified with glVertexAttribPointer.

        The glVertexAttribPointer calls are luckily also stored inside a VAO, so a VAO basically now stores the following:
            1. The VBO when we called glVertexAttribPointer for each individual vertex attribute.
            2. The last bound EBO object while the VAO was bound.

        Binding a VAO thus sets the vertex attribute data, and the index data for all subsequent drawing commands.
        VBO binds aren't directly stored and EBO binds are within VAOs.
        */

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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
