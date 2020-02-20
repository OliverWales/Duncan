#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "mesh.h"
#include "marchingcubes.h"
#include "pointcloud.h"

void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

// settings
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;
const std::string vertexShader = parseShader("res/shaders/vertex.shader");
const std::string fragmentShader = parseShader("res/shaders/fragment.shader");
const std::string model_file = "res/models/teapot.txt";

// initial camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float fov = 45.0f;
const float cameraSpeed = 3.0f;
const float mouseSensitivity = 0.1f;

bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;


// global timing variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Duncan 0.0", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load all OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    unsigned int shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    // load model from file
    //Mesh mesh = MarchingCubes::poligonise();
    Mesh mesh = Mesh(model_file);
    //float* mesh_vertices = mesh.vertices.data();
    //unsigned int* mesh_indices = mesh.indices.data();
    //unsigned int mesh_vertices_count = mesh.vertices.size();
    //unsigned int mesh_indices_count = mesh.indices.size();

    unsigned int n = 25000;
    PointCloud pc = PointCloud(mesh, n);
    float* pc_vertices = pc.vertices.data();
    unsigned int pc_vertices_count = n;

    // write buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, mesh_vertices_count * sizeof(int), mesh_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, pc_vertices_count * sizeof(int), pc_vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_indices_count * sizeof(int), mesh_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // enable z buffer
    glEnable(GL_DEPTH_TEST);

    // create transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 mvp = glm::mat4(1.0f);
    unsigned int mvpLoc = glGetUniformLocation(shader, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // clear color and depth buffers
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        mvp = projection * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        // render the scene
        int vertexColorLocation = glGetUniformLocation(shader, "ourColor");
        glUniform3f(vertexColorLocation, 0.0f, 0.0f, 0.0f);
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, mesh_indices_count, GL_UNSIGNED_INT, (void*)0);
        glDrawArrays(GL_POINTS, 0, pc_vertices_count);

        // swap buffers
        glfwSwapBuffers(window);

        // poll I/O
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    // exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // draw modes
    ///*
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // filled
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // points
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    //*/

    // camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // forward
        cameraPos += cameraSpeed * deltaTime * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // backward
        cameraPos -= cameraSpeed * deltaTime * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // left
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // right
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // up
        cameraPos += cameraUp * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // down
        cameraPos -= cameraUp * cameraSpeed * deltaTime;

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;
    yaw += xOffset;
    pitch += yOffset;

    // limit pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}