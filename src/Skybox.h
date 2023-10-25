//
// Created by Émile Le Gallic on 25/10/2023.
//

#ifndef TPOPENGL_SKYBOX_H
#define TPOPENGL_SKYBOX_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "stb_image.h"

#include "Camera.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

class Skybox {
    public:
        Skybox();
        void init();
        void render(GLuint program, Camera camera) const;

    private:
        GLuint loadCubemap(std::vector<std::string> faces);

    private:
        std::vector<std::string> skyboxFaces;
        unsigned int cubemapTexture;
        GLuint g_skyboxVbo;
        GLuint g_skyboxVao;
};


#endif //TPOPENGL_SKYBOX_H
