// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/Water.hpp>
#include <common/texture.hpp>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// rotation
float angle = 0.;
float zoom = 1.;

float theta;

glm::mat4 Model, View, Projection;
/*******************************************************************************/

GLuint texture;
std::string texturePathFaceAquarium("carreaux.jpg");
int mode;

int main(void)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "TP1 - GLFW", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    // glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");

    //------------------------------------------------AQUARIUM BASE STRUCTURE--------------------------------------//
    //Load texture for aquarium base
    GLuint textureFaceAquarium = loadTexture2DFromFilePath(texturePathFaceAquarium);

    std::vector<glm::vec3> faceTextureeVertices = {glm::vec3(-0.5, -0.5, -0.5),
                                            glm::vec3(-0.5, 0.5, -0.5),
                                            glm::vec3(0.5, 0.5, -0.5),
                                            glm::vec3(0.5, -0.5, -0.5),
                                            glm::vec3(0.5, 0.5, 0.5),
                                            glm::vec3(0.5, -0.5, 0.5),
                                            glm::vec3(-0.5, -0.5, 0.5)};

    std::vector<unsigned short> faceTextureeIndices = {0, 1, 2,
                                                0, 2, 3,
                                                3, 2, 4,
                                                3, 4, 5,
                                                6, 0, 3,
                                                6, 3, 5};

    std::vector<glm::vec2> faceTextureeUVs = compute_uv(faceTextureeVertices);

    std::vector<glm::vec3> faceTransparenteVertices = {glm::vec3(-0.5, -0.5, 0.5),
                                            glm::vec3(-0.5, 0.5, 0.5),
                                            glm::vec3(-0.5, 0.5, -0.5),
                                            glm::vec3(-0.5, -0.5, -0.5),
                                            glm::vec3(0.5, 0.5, 0.5),
                                            glm::vec3(0.5, -0.5, 0.5),
                                           };

    std::vector<unsigned short> faceTransparenteIndices = {0, 1, 2,
                                                0, 2, 3,
                                                0, 1, 4,
                                                0, 4, 5,
                                                };
    
    GLuint vertexbuffertexture;
    glGenBuffers(1, &vertexbuffertexture);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffertexture);
    glBufferData(GL_ARRAY_BUFFER, faceTextureeVertices.size() * sizeof(glm::vec3), &faceTextureeVertices[0], GL_STATIC_DRAW);

    GLuint elementbuffertexture;
    glGenBuffers(1, &elementbuffertexture);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffertexture);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceTextureeIndices.size() * sizeof(unsigned short), &faceTextureeIndices[0], GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, faceTextureeUVs.size() * sizeof(vec2), &faceTextureeUVs[0], GL_STATIC_DRAW);

    GLuint vertexbuffertransparant;
    glGenBuffers(1, &vertexbuffertransparant);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffertransparant);
    glBufferData(GL_ARRAY_BUFFER, faceTransparenteVertices.size() * sizeof(glm::vec3), &faceTransparenteVertices[0], GL_STATIC_DRAW);

    GLuint elementbuffertransparent;
    glGenBuffers(1, &elementbuffertransparent);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffertransparent);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceTransparenteIndices.size() * sizeof(unsigned short), &faceTransparenteIndices[0], GL_STATIC_DRAW);
    //--------------------------------------------------AQUARIUM BASE STRUCTURE--------------------------------------------//

    //--------------------------------------------------AQUARIUM WATER--------------------------------------//
    Water w = Water();
    w.createAquarium();

    std::vector<unsigned short> indicesWater; // Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short>> trianglesWater;
    std::vector<glm::vec3> indexed_verticesWater;

    indicesWater = w.getIndices();
    std::cout << indicesWater.size() << std::endl;
    indexed_verticesWater = w.getSommets();

    GLuint vertexbufferWater;
    glGenBuffers(1, &vertexbufferWater);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferWater);
    glBufferData(GL_ARRAY_BUFFER, indexed_verticesWater.size() * sizeof(glm::vec3), &indexed_verticesWater[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbufferWater;
    glGenBuffers(1, &elementbufferWater);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferWater);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesWater.size() * sizeof(unsigned short), &indicesWater[0], GL_STATIC_DRAW);
    //-----------------------------------------------------------------------AQUARIUM WATER-----------------------------------------------------//

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    do
    {

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        // std::cout<<currentFrame<<std::endl ;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        Model = glm::rotate(Model, glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
        View = glm::lookAt(
            camera_position,
            camera_target,
            camera_up); // matrice de vue
        View = glm::rotate(View, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f); // matrice de projection

        // Envoi des matrices de transformation au shader
        glUniformMatrix4fv(glGetUniformLocation(programID, "ModelMatrix"), 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "ProjectionMatrix"), 1, GL_FALSE, &Projection[0][0]);

        mode = 1;
        glUniform1i(glGetUniformLocation(programID, "mode"), mode);

        if(mode==1){
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffertexture);
            glVertexAttribPointer(
                0,        // attribute
                3,        // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0,        // stride
                (void *)0 // array buffer offset
            );

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glVertexAttribPointer(
                1, 
                2, 
                GL_FLOAT, 
                GL_FALSE, 
                0, 
                (void *)0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffertexture);

            glDrawElements(
                GL_TRIANGLES,      // mode
                faceTextureeIndices.size(),    // count
                GL_UNSIGNED_SHORT, // type
                (void *)0          // element array buffer offset
            );

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureFaceAquarium);
            glUniform1i(glGetUniformLocation(programID, "texture_aquarium"), 0);
        }

        mode = 2;
        glUniform1i(glGetUniformLocation(programID, "mode"), mode);

        if(mode==2){
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffertransparant);
            glVertexAttribPointer(
                0,        // attribute
                3,        // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0,        // stride
                (void *)0 // array buffer offset
            );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffertransparent);

            glDrawElements(
                GL_TRIANGLES,      // mode
                faceTransparenteIndices.size(),    // count
                GL_UNSIGNED_SHORT, // type
                (void *)0          // element array buffer offset
            );
        }

        mode=3;
        glUniform1i(glGetUniformLocation(programID, "mode"), mode);

        if(mode==3){
            // if (currentFrame < 10)
            // {
            //     w.simulateWater(currentFrame, w);
            //     indexed_verticesWater = w.getSommets();
            //     std::cout<<indexed_verticesWater[0][1]<<std::endl ;
            //     GLuint vertexbufferWater;
            //     glGenBuffers(1, &vertexbufferWater);
            //     glBindBuffer(GL_ARRAY_BUFFER, vertexbufferWater);
            //     glBufferData(GL_ARRAY_BUFFER, indexed_verticesWater.size() * sizeof(glm::vec3), &indexed_verticesWater[0], GL_STATIC_DRAW);
            // }

            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbufferWater);
            glVertexAttribPointer(
                0,        // attribute
                3,        // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0,        // stride
                (void *)0 // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferWater);

            // Draw the triangles !
            glDrawElements(
                GL_TRIANGLES,        // mode
                indicesWater.size(), // count
                GL_UNSIGNED_SHORT,   // type
                (void *)0            // element array buffer offset
            );

        }
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &vertexbuffertexture);
    glDeleteBuffers(1, &vertexbuffertransparant);
    glDeleteBuffers(1, &elementbuffertexture);
    glDeleteBuffers(1, &elementbuffertransparent);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &elementbufferWater);
    glDeleteBuffers(1, &vertexbufferWater);




    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;

    // TODO add translations
    // if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
    // {
    //     nbPointH++;
    //     nbPointL++;
    // }
    // if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
    // {
    //     nbPointH--;
    //     nbPointL--;
    // }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        theta = 0.5;
        Model = glm::rotate(Model, glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
        View = glm::lookAt(
            camera_position,
            camera_target,
            camera_up); // matrice de vue
        View = glm::rotate(View, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f); // matrice de projection
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        theta = 0;
        // ModelMatrix = glm::mat4(1.0f);
        View = glm::lookAt(
            camera_position,
            camera_target,
            camera_up); // matrice de vue
        Projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f);
    }

    // theta = 0.5 ;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        theta += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        theta -= 0.01;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
