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

#include "CelestialObject.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"

// constants
const static float kSizeSun = 1;
const static float kSizeEarth = 0.5;
const static float kSizeMoon = 0.25;
const static float kRadOrbitEarth = 10;
const static float kRadOrbitMoon = 2;
const static float kOrbitPeriodEarth = 365;
const static float kOrbitPeriodMoon = 28;

// Model transformation matrices
glm::mat4 g_sun, g_earth, g_moon;

// Meshes array
std::vector<CelestialObject*> g_celestialObjects;

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader
GLuint l_program = 0; // A GPU program for the light objects

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_ibo = 0;
GLuint g_colorVbo = 0;

// All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
std::vector<float> g_vertexPositions;
std::vector<float> g_vertexColors;
// All triangle indices packed in one array [v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle
std::vector<unsigned int> g_triangleIndices;

// Basic camera model
Camera g_camera;

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  int width, height, numComponents;
  // Loading the image in CPU memory using stb_image
  unsigned char *data = stbi_load(
    filename.c_str(),
    &width, &height,
    &numComponents, // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
    0);

  GLuint texID;
  // TODO: create a texture and upload the image data in GPU memory using
  // glGenTextures, glBindTexture, glTexParameteri, and glTexImage2D

  // Free useless CPU memory
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture

  return texID;
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
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
    // TODO: set shader variables, textures, etc.
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 10.0, 30.0));
  g_camera.setNear(0.1);
  g_camera.setFar(200.1);
}

void init() {
  initGLFW();
  initOpenGL();
  initGPUprograms();
  initCamera();

  for (CelestialObject* o : g_celestialObjects) {
    o->init();
  }
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

  for(CelestialObject* o : g_celestialObjects) {
      if (o->getType() == CelestialType::Star) {
          o->render(l_program, g_camera);
      } else if (o->getType() == CelestialType::Planet) {
          o->render(g_program, g_camera);
          // glUniform3f(glGetUniformLocation(g_program, "objectColor"), 1.0f, 0.5f, 0.31f);
      }
  }

  // glBindVertexArray(g_vao);     // activate the VAO storing geometry data
  // glDrawElements(GL_TRIANGLES, g_triangleIndices.size(), GL_UNSIGNED_INT, 0); // Call for rendering: stream the current GPU geometry through the current GPU program
}

// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
  // std::cout << currentTimeInSec << std::endl;

}

int main(int argc, char ** argv) {

    CelestialObject* sun = new CelestialObject(kSizeSun, (size_t) 100, CelestialType::Star);
  g_celestialObjects.push_back(sun);

    CelestialObject* planet1 = new CelestialObject(kSizeEarth, sun, kRadOrbitEarth, kOrbitPeriodEarth, (size_t) 100, CelestialType::Planet);
    g_celestialObjects.push_back(planet1);

    CelestialObject* planet2 = new CelestialObject(kSizeMoon, planet1, kRadOrbitMoon, kOrbitPeriodMoon, (size_t) 100, CelestialType::Planet);
    g_celestialObjects.push_back(planet2);

  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)

  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
