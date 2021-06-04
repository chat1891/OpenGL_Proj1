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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
    -50.0f, -50.0f,       0.0f, 0.0f,         1.f, 0.f, 0.f,1.f,// bottom left
     50.0f, -50.0f,       1.0f, 0.0f,         0.f, 1.f, 0.f,1.f,// bottom right
     50.0f,  50.0f,       1.0f, 1.0f,         0.f, 0.f, 1.f,1.f,// top right
    -50.0f,  50.0f,       0.0f, 1.0f,         1.f, 1.f, 0.f,1.f,// top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // second triangle
        2, 3, 0,
    };

    //blending
    GLCall(glEnable(GL_BLEND));
    //glBlendFunc(source, destination)
    //source = how the source RGBA factor is computer (default is GL_ONE)
    //destination = how the destination RGBA factor is computer (default is GL_ZERO)
    //everything with source multiply by 1, which doesn't change
    //everything with destination multiply by 0, which is 0
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

	//projection matrix
    //multiply by 2 get 4 by 3
    //                       leftEdge  right  bottom top
	//glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	//ortho graphic matrix: map all the coordinates on 2d plane where objects further away do not actually get smaller

    //this orhographic projection will convert position coord to be between -1 and 1 space. 
    // center is 0 for -2.0f, 2.0f
    //position -0.5 is 1/4 of 0 to -2.0.
    //so in -1 to 1 space, -0.5 position will be 1/4 of 1, which is -0.25 on our actual screen


    // view matrix
    //move camera to the right means move object to the left
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

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

    Renderer renderer;

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init((char*)glGetString(330));

    glm::vec3 translationA(200, 200, 0);
    glm::vec3 translationB(400, 200, 0);

    float b = 0.0;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            glm::mat4 mvp = proj * view * model;
            //----bind buffer for every frame
            //shader.SetUniform4f("u_Color", 0.2, 0.3f, 0.0f, 1.0f);
            shader.SetUniformMat4f("u_ModelViewProjectionMatrix", mvp);
            renderer.Draw(VA, IB, shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
            glm::mat4 mvp = proj * view * model;
            //----bind buffer for every frame
            //shader.SetUniform4f("u_Color", 0.2, 0.3f, b, 1.0f);
            shader.SetUniformMat4f("u_ModelViewProjectionMatrix", mvp);
            renderer.Draw(VA, IB, shader);
        }

        //press 1 to draw line, press 2 to draw filled
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (b > 1.0f) increment = -0.05f;
        else if (b < 0.0f) increment = 0.05f;
        b += increment;

        //show simple window of imgui
        {
            //pass in first memory address of translation. y and z will follow
            ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
