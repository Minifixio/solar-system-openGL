// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020-2023 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "stb_image.h"

#include "CelestialObject.h"
#include "Skybox.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

// constants
const static float kSizeSun = 1;
const static float kSizeEarth = 0.5;
const static float kSizeMoon = 0.25;
const static float kSizeMars = 0.75;
const static float kSizeJupiter = 1.5;

const static float kRadOrbitEarth = 10;
const static float kRadOrbitMoon = 2;
const static float kRadOrbitMars = 15;
const static float kRadOrbitJupiter = 20;

const static float kOrbitPeriodEarth = 365;
const static float kOrbitPeriodMoon = 28;
const static float kOrbitPeriodMars = 687;
const static float kOrbitPeriodJupiter = 4332;

const static float kRotationPeriodEarth = 24;
const static float kRotationPeriodMoon = 27;
const static float kRotationPeriodSun = 25;
const static float kRotationPeriodMars = 24;
const static float kRotationPeriodJupiter = 10;

const static float kInclinationAngleEarth = glm::radians(23.5f);
const static float kInclinationAngleMoon = glm::radians(5.14f);
const static float kInclinationAngleMars = glm::radians(25.19f);
const static float kInclinationAngleJupiter = glm::radians(3.13f);

// Model transformation matrices
glm::mat4 g_sun, g_earth, g_moon;

// Meshes array
std::vector<CelestialObject*> g_celestialObjects;

Skybox* g_skybox;

bool arrowUpPressed = false;
bool arrowDownPressed = false;
bool arrowRightPressed = false;
bool arrowLeftPressed = false;

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader
GLuint l_program = 0; // A GPU program for the light objects
GLuint s_program = 0; // A GPU program for the skybox

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_ibo = 0;
GLuint g_colorVbo = 0;
GLuint g_skyboxVbo = 0;
GLuint g_skyboxVao = 0;

// Skybox faces
std::vector<std::string> skyboxFaces;

// Basic camera model
Camera g_camera;

unsigned int cubemapTexture;

GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {

        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void initSkyBox() {
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &g_skyboxVao);
    glGenBuffers(1, &g_skyboxVbo);
    glBindVertexArray(g_skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    skyboxFaces =
            {
                    "media/skybox/skybox_right.png",
                    "media/skybox/skybox_left.png",
                    "media/skybox/skybox_top.png",
                    "media/skybox/skybox_bottom.png",
                    "media/skybox/skybox_front.png",
                    "media/skybox/skybox_back.png",
            };

    cubemapTexture = loadCubemap(skyboxFaces);
}


void renderSkybox() {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(s_program);
    const glm::mat4 viewMatrix = glm::mat4(glm::mat3(g_camera.computeViewMatrix()));
    const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(s_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(s_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    glBindVertexArray(g_skyboxVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
      std::cout << "W pressed" << std::endl;
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
      std::cout << "F pressed" << std::endl;
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
      glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_UP) {
            arrowUpPressed = true;
        } else if (key == GLFW_KEY_DOWN) {
            arrowDownPressed = true;
        } else if (key == GLFW_KEY_RIGHT) {
            arrowRightPressed = true;
        } else if (key == GLFW_KEY_LEFT) {
            arrowLeftPressed = true;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_UP) {
            arrowUpPressed = false;
        } else if (key == GLFW_KEY_DOWN) {
            arrowDownPressed = false;
        } else if (key == GLFW_KEY_RIGHT) {
            arrowRightPressed = false;
        } else if (key == GLFW_KEY_LEFT) {
            arrowLeftPressed = false;
        }
    }
}

void updateCameraRotation() {
    if (arrowUpPressed) {
        // Rotation vers le haut autour de l'axe X
        g_camera.rotateUp(1.0f);
    }
    if (arrowDownPressed) {
        // Rotation vers le bas autour de l'axe X
        g_camera.rotateDown(1.0f);
    }
    if (arrowLeftPressed) {
        // Rotation vers le bas autour de l'axe X
        g_camera.rotateLeft(1.0f);
    }
    if (arrowRightPressed) {
        // Rotation vers le bas autour de l'axe X
        g_camera.rotateRight(1.0f);
    }
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    g_camera.processMouseScroll(static_cast<float>(yoffset));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);

  glfwSetCursorPosCallback(g_window, mouseCallback); // For the mouse movement
  glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
  glfwSetScrollCallback(g_window, scrollCallback);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // specify the background color, used any time the framebuffer is cleared
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprograms() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "shaders/planetVertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "shaders/planetFragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons

  l_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(l_program, GL_VERTEX_SHADER, "shaders/starVertexShader.glsl");
  loadShader(l_program, GL_FRAGMENT_SHADER, "shaders/starFragmentShader.glsl");
  glLinkProgram(l_program); // The main GPU program is ready to be handle streams of polygons

  s_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(s_program, GL_VERTEX_SHADER, "shaders/skyboxVertexShader.glsl");
  loadShader(s_program, GL_FRAGMENT_SHADER, "shaders/skyboxFragmentShader.glsl");
  glLinkProgram(s_program); // The main GPU program is ready to be handle streams of polygons

}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 50.0, 70.0));
  g_camera.setNear(0.1);
  g_camera.setFar(200.1);
}

void init() {
  initGLFW();
  initOpenGL();
  initCamera();


  for (CelestialObject* o : g_celestialObjects) {
    o->init();
  }
  g_skybox->init();
  // initSkyBox();

  initGPUprograms();
}

void clear() {
  glDeleteProgram(g_program);
  glDeleteProgram(l_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// The main rendering call
void render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

  // renderSkybox();
  g_skybox->render(s_program, g_camera);

  for(CelestialObject* o : g_celestialObjects) {
      if (o->getType() == CelestialType::Star) {
          o->render(l_program, g_camera);
      } else if (o->getType() == CelestialType::Planet) {
          o->render(g_program, g_camera);
          // glUniform3f(glGetUniformLocation(g_program, "objectColor"), 1.0f, 0.5f, 0.31f);
      }
  }
}

int main(int argc, char ** argv) {

    g_skybox = new Skybox();

    CelestialObject* sun = new CelestialObject(kSizeSun, kRotationPeriodSun, (size_t) 100, "media/sun-2.jpg", CelestialType::Star);
    g_celestialObjects.push_back(sun);

    CelestialObject* mars = new CelestialObject(kSizeMars, sun, kRadOrbitMars, kOrbitPeriodMars, kRotationPeriodMars,  kInclinationAngleMars, (size_t) 100, "media/mars.jpeg", CelestialType::Planet);
    g_celestialObjects.push_back(mars);

    CelestialObject* jupiter = new CelestialObject(kSizeJupiter, sun, kRadOrbitJupiter, kOrbitPeriodJupiter, kRotationPeriodJupiter,  kInclinationAngleJupiter, (size_t) 100, "media/jupiter.jpg", CelestialType::Planet);
    g_celestialObjects.push_back(jupiter);

    CelestialObject* earth = new CelestialObject(kSizeEarth, sun, kRadOrbitEarth, kOrbitPeriodEarth, kRotationPeriodEarth,  kInclinationAngleEarth, (size_t) 100, "media/earth.jpg", CelestialType::Planet);
    g_celestialObjects.push_back(earth);

    CelestialObject* moon = new CelestialObject(kSizeMoon, earth, kRadOrbitMoon, kOrbitPeriodMoon, kRotationPeriodMoon, kInclinationAngleMoon, (size_t) 100, "media/moon.jpg", CelestialType::Planet);
    g_celestialObjects.push_back(moon);

  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)

  while(!glfwWindowShouldClose(g_window)) {
    render();
    glfwSwapBuffers(g_window);
    updateCameraRotation();
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
