#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include "glm\glm.hpp" 
#include "MenticsCommon.h" 
#include "Shader.h"
#include "glm\gtx\transform.hpp"
#include "World.h" 
#include "CameraController.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "Quip.h"
#include "SelectionManager.h"

namespace MenticsGame {

struct WorldObject {
    glm::vec4 pos;
    glm::vec4 vel;
    glm::vec4 acc;
    float radius;
    float ignore1;
    float ignore2;
    float ignore3;
};

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  -1.0f, 0.0f,
};

static int viewportWidth;
static int viewportHeight;

static Quip<RealTime, WorldModel<RealTime>> q(1, 0, nn::nn_make_unique<BasicTrajectory>(BasicTrajectory(0, 5000, vect3(0, 0, 0), vect3(0, 0, 0), vect3(0, 0, 0))), 0, 0, 0, 0, "player");

static CameraController<RealTime> cam(nn::nn_addr(q));
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.cam.ProcessKeyboard(FORWARD, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.cam.ProcessKeyboard(BACKWARD, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.cam.ProcessKeyboard(LEFT, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.cam.ProcessKeyboard(RIGHT, 0.1f);
}

static double lastX;
static double lastY;
static bool firstMouse = true;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    cam.cam.ProcessMouseMovement(xoffset, yoffset);
}

struct Sphere {
    glm::vec4 center;
    float radius2;
    float center2;
    glm::vec2 ignore;
};

const int dim = 10;
const int numWorldObjects = dim * dim*dim;
const int windowSize = 20;

class Renderer {
public:
    Renderer();
    void run();
    GLuint loadComputeShader();
    GLFWwindow* init();

    ~Renderer();
private:
    ONLY_MOVE(Renderer);
    Boss<RealTime, WorldModel<RealTime>> b;
    SelectionManager<RealTime> selectionManager;
    GLFWwindow* window;
    Shader shaders;
    GLuint compute_handle;
    double aspectRatio;
    GLint cameraId;
    GLint gameTimeId;
    GLint dtId;
    World w;
    WorldObject world[numWorldObjects];
    AgentPosVelAcc a_data[numWorldObjects];
    GLuint worldId;
    GLuint counterBuffer;
    Sphere indexData[numWorldObjects]; // TODO: we shouldn't have to allocate it CPU side
    GLuint indexId = 0;
    GLint Resolution;
    GLint ray00Id;
    GLint ray01Id;
    GLint ray10Id;
    GLint ray11Id;
    GLuint vao;
    GLuint vertexbuffer;
    uint64_t frameTimes[windowSize];
    uint8_t index;
    uint64_t startNanos;
};
}
