#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include<sstream>

#include"Renderer.h"

#include"VertexBuffer.h"
#include"IndexBuffer.h"

#include"VertexArray.h"
#include "Shader.h"

#include "VertexBufferLayout.h"

#include "Texture.h"

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
    -0.5f, -0.5f,       0.0f, 0.0f,         1.f, 0.f, 0.f,1.f,// bottom left
     0.5f, -0.5f,       1.0f, 0.0f,         0.f, 1.f, 0.f,1.f,// bottom right
     0.5f,  0.5f,       1.0f, 1.0f,         0.f, 0.f, 1.f,1.f,// top right
    -0.5f,  0.5f,       0.0f, 1.0f,         1.f, 1.f, 0.f,1.f,// top left 
    };
//    float positions[] = {
//-0.5f, -0.5f,       0.0f, 0.0f, // bottom left
// 0.5f, -0.5f,       1.0f, 0.0f, // bottom right
// 0.5f,  0.5f,       1.0f, 1.0f, // top right
//-0.5f,  0.5f,       0.0f, 1.0f, // top left 
//    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // second triangle
        2, 3, 0,
    };

    // 1. bind vertex array object
    VertexArray VA;
    VA.Bind();
    
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    VertexBuffer VB(positions, sizeof(positions));

    //3. copy our index array in a element buffer for OpenGL to use
    IndexBuffer IB(indices, 12);

    // 4. then set the vertex attributes pointers
    VertexBufferLayout layout;
    layout.Push<float>(2); //one for position
    layout.Push<float>(2); // one for texture
    layout.Push<float>(4); // one for color
    VA.AddBuffer(VB, layout);


    //5. Shader:
    Shader shader("res/shaders/Basic.Shader");
    shader.Bind();

    //every uniform got assigned to an id. find id using bytes name
    shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

    //6. texture
    Texture texture("res/textures/dog3.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);


    //---------unbind all the buffer
    VA.Unbind();
    VB.Unbind();
    IB.Unbind();
    shader.Unbind();

    float b = 0.0;
    float increment = 0.05f;

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        //----bind buffer for every frame
        shader.SetUniform4f("u_Color", 0.2, 0.3f, b, 1.0f);

        renderer.Draw(VA, IB, shader);

        //press 1 to draw line, press 2 to draw filled
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
    // 
    // --------no need to delete it, because by end of scope, it will deleted by destructor
    //glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
