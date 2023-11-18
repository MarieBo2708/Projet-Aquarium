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
// #include <common/stb_image.h>

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

struct BoiteAquarium
{
    glm::vec3 center;
    float tailleX, tailleY, tailleZ;
    glm::vec3 BBmin, BBmax;
    std::vector<glm::vec3> sommetsBox;
    std::vector<unsigned short> trianglesBox;
    std::vector<std::vector<unsigned short>> faces;
    int mode = 0;

    glm::vec3 getCenter() const { return center; }
    void setCenter(glm::vec3 newCenter) { center = newCenter; }
    float getHeight() const { return tailleY; }
    void setHeight(float newTailleY) { tailleY = newTailleY; }
    float getWidth() const { return tailleX; }
    void setWidth(float newTailleX) { tailleX = newTailleX; }
    float getDepth() const { return tailleZ; }
    void setDepth(float newTailleZ) { tailleZ = newTailleZ; }

    void createBox(glm::vec3 c, float x, float y, float z)
    {
        center = c;
        tailleX = x;
        tailleY = y;
        tailleZ = z;
        BBmin = glm::vec3(c[0] - tailleX / 2, c[1] - tailleY / 2, c[2] - tailleZ / 2);
        BBmax = glm::vec3(c[0] + tailleX / 2, c[1] + tailleY / 2, c[2] + tailleZ / 2);
        sommetsBox.resize(8);
    }

    void createVerticesBox()
    {
        /*
          5___7
         /    /|
        1____3 |
        |  4 | |6
        |____|/
        0    2
        */
        sommetsBox[0] = BBmin;
        sommetsBox[1] = glm::vec3(BBmin[0], BBmin[1] + tailleY, BBmin[2]);
        sommetsBox[2] = glm::vec3(BBmin[0] + tailleX, BBmin[1], BBmin[2]);
        sommetsBox[3] = glm::vec3(BBmin[0] + tailleX, BBmin[1] + tailleY, BBmin[2]);
        sommetsBox[4] = glm::vec3(BBmin[0], BBmin[1], BBmin[2] + tailleZ);
        sommetsBox[5] = glm::vec3(BBmin[0], BBmin[1] + tailleY, BBmin[2] + tailleZ);
        sommetsBox[6] = glm::vec3(BBmin[0] + tailleX, BBmin[1], BBmin[2] + tailleZ);
        sommetsBox[7] = glm::vec3(BBmin[0] + tailleX, BBmin[1] + tailleY, BBmin[2] + tailleZ);
        trianglesBox = {0, 1, 2,
                        1, 3, 2,
                        4, 5, 0,
                        5, 1, 0,
                        6, 7, 4,
                        7, 5, 4,
                        2, 3, 6,
                        3, 7, 6,
                        1, 5, 3,
                        5, 7, 3,
                        6, 2, 4,
                        2, 0, 4};
        faces = {{0, 1, 2, 3}, {4, 5, 0, 1}, {6, 7, 4, 5}, {2, 3, 6, 7}, {1, 5, 3, 7}, {6, 2, 4, 0}};
    }
};

void drawEnsemble(uint programID, int mode)
{
    glUniform1i(glGetUniformLocation(programID, "mode"), mode);

    // initbuffer !!!!!
    if (mode == 0)
    {
        // glUniform1i(glGetUniformLocation(programID, "texture_terrain"), 0);
    }
}

void simulateWater(float time, Water water)
{
    float amplitude = 2.0f;
    float frequency = 0.01f;

    float wave = amplitude * std::sin(frequency * time);
    int count = water.getSommets().size() / 2;
    for (int i = 0; i < water.getResolution(); i++)
    {
        for (int j = 0; j < water.getResolution(); j++)
        {
            water.getSommets()[count + i * water.getResolution() + j][1] = water.getSommets()[count + i * water.getResolution() + j][1] + wave;
            //std::cout << water.getSommets()[count + i * water.getResolution() + j][1] + wave << std::endl;
        }
    }
}

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

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/
    std::vector<unsigned short> indices; // Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short>> triangles;
    std::vector<glm::vec3> indexed_vertices;

    // Chargement du fichier de maillage
    // std::string filename("chair.off");
    BoiteAquarium aquarium;
    aquarium.createBox(glm::vec3(0, 0, 0), 1, 1, 1);
    aquarium.createVerticesBox();
    // loadOFF(filename, indexed_vertices, indices, triangles);
    Water w = Water();
    w.createAquarium();
    indices = aquarium.trianglesBox;

    // indices.insert(indices.end(), w.getSommets().begin(), w.getSommets().end());
    indexed_vertices = aquarium.sommetsBox;
    // indexed_vertices.insert(indexed_vertices.end(), w.getIndices().begin(), w.getIndices().end());
    std::vector<unsigned short> indicesWater; // Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short>> trianglesWater;
    std::vector<glm::vec3> indexed_verticesWater;

    indicesWater = w.getIndices();
    std::cout << indicesWater.size() << std::endl;
    indexed_verticesWater = w.getSommets();
    // Load it into a VBO

    // GLuint vertexbuffer;
    // glGenBuffers(1, &vertexbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    // // Generate a buffer for the indices as well
    // GLuint elementbuffer;
    // glGenBuffers(1, &elementbuffer);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    GLuint vertexbufferWater;
    glGenBuffers(1, &vertexbufferWater);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferWater);
    glBufferData(GL_ARRAY_BUFFER, indexed_verticesWater.size() * sizeof(glm::vec3), &indexed_verticesWater[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbufferWater;
    glGenBuffers(1, &elementbufferWater);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferWater);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesWater.size() * sizeof(unsigned short), &indicesWater[0], GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    for (int i = 0; i < 8; i++)
    {
        std::cout << aquarium.sommetsBox[i][0] << " ; " << aquarium.sommetsBox[i][1] << " ; " << aquarium.sommetsBox[i][2] << std::endl;
    }

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

        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        // Initialisation des matrices de transformation
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

        /****************************************/

        // 1rst attribute buffer : vertices
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // glVertexAttribPointer(
        //     0,        // attribute
        //     3,        // size
        //     GL_FLOAT, // type
        //     GL_FALSE, // normalized?
        //     0,        // stride
        //     (void *)0 // array buffer offset
        // );

        // // Index buffer
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // // Draw the triangles !
        // glDrawElements(
        //     GL_TRIANGLES,      // mode
        //     indices.size(),    // count
        //     GL_UNSIGNED_SHORT, // type
        //     (void *)0          // element array buffer offset
        // );

        // glDisableVertexAttribArray(0);

        // 1rst attribute buffer : vertices

        if (currentFrame < 10)
        {
            simulateWater(currentFrame, w);
            indexed_verticesWater = w.getSommets();
            std::cout<<indexed_verticesWater[0][1]<<std::endl ;
            GLuint vertexbufferWater;
            glGenBuffers(1, &vertexbufferWater);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbufferWater);
            glBufferData(GL_ARRAY_BUFFER, indexed_verticesWater.size() * sizeof(glm::vec3), &indexed_verticesWater[0], GL_STATIC_DRAW);
        }

        glEnableVertexAttribArray(0);
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

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    // glDeleteBuffers(1, &vertexbuffer);
    // glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

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
