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
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

#include "tests/TestClearColor.h"

int main_camera(void)
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
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 2,
    3, 4, 5,
    6,7,8,
    9,10,11,
    12,13,14,
    15, 16,17,
    18,19,20,
    21,22,23,
    24,25,26,
    27,28,29,
    30,31,32,
    33,34,35
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
    IndexBuffer IB(indices, 36);

    // 4. then set the vertex attributes pointers
    VertexBufferLayout layout;
    layout.Push<float>(3); //one for position
    layout.Push<float>(2); // one for texture
    //layout.Push<float>(4); // one for color
    VA.AddBuffer(VB, layout);

    //projection matrix
    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

    // view matrix
    //move camera to the right means move object to the left
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    //5. Shader:
    Shader shader("res/shaders/Basic.Shader");
    shader.Bind();

    //every uniform got assigned to an id. find id using bytes name
    shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

    //6. texture
    Texture texture("res/textures/seal.png");
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

    test::TestClearColor testClearColor;

    // init cube pos
    glm::mat4 model3 = glm::mat4(1.0);
    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 model10[] = {
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0),
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // load different position of each cube
    for (unsigned int i = 0; i < 10; i++)
    {
        if (i == 0)
        {
            model10[i] = glm::translate(glm::mat4(1.0f), cubePositions[i]);
        } 
        else
        {
            model10[i] = glm::translate(model10[i], cubePositions[i]);
        }
        model10[i] = glm::scale(model10[i], glm::vec3(1.5, 1.5, 1.5));
    }

    glEnable(GL_DEPTH_TEST);


    //---camera
    //camera direction
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); //pointing in the reverse direction of what it is targeting

    //right axis, x axis, cross product of up vector and camera direction (z)
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    //up axis
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection,cameraRight));
    //With the help of the cross product and a few tricks we were able to create all the vectors that form the view/camera space.

    //look at
    /*
    if you define a coordinate space using 3 perpendicular (or non-linear) axes 
    you can create a matrix with those 3 axes plus a translation vector 
    and you can transform any vector to that coordinate space by multiplying it with this matrix. 
    */
    //a camera position, a target position, a vector that represents the up vector in world space 
    glm::mat4 cameraView = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));


    float b = 0.0;
    float increment = 0.05f;
    float rotationDegree = 0.01;
    const float radius = 10.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testClearColor.OnUpdate(0.0f);
        testClearColor.OnRender();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        testClearColor.OnImGuiRender();
        testClearColor.ImGuiTimesColor();

        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;


        for (unsigned int i = 0; i < 10; i++)
        {
            float angle = 20.0f * i;
            model10[i] = glm::rotate(model10[i], rotationDegree, glm::vec3(1.0f, 0.3f, 0.5f));
            glm::mat4 proj3 = glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f);
            //glm::mat4 view3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.50f, 0.0f, -3.0f));
            glm::mat4 view3 = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            glm::mat4 mvp = proj3 * view3 * model10[i];
            shader.SetUniformMat4f("u_ModelViewProjectionMatrix", mvp);

            //renderer.DrawArray(VA, shader,36);
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
