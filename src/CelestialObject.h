#ifndef _CELESTIALOBJECT
#define _CELESTIALOBJECT

#include <iostream>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"

enum class CelestialType { Planet, Star };

class CelestialObject {
    public:
        CelestialObject(float radius, CelestialObject *parent, float orbitRadius, float orbitPeriod, float rotationPeriod, float inclinationAngle, size_t m_resolution, std::string texPath, CelestialType type);
        CelestialObject(float radius, float rotationPeriod, size_t m_resolution, std::string texPath, CelestialType type);
        void init(); // should properly set up the geometry buffer
        void render(GLuint program, Camera camera); // should be called in the main rendering loop
        CelestialType getType() { return this->type; }
        float getOrbitRadius() { return this->orbitRadius; }
    
    private:
        void initGPUgeometry();
        void genSphere();
        GLuint loadTextureFromFileToGPU(const std::string &filename);
        void updateOrbit(float deltaTime, float radius);
        float getRotationAngle(float deltaTime);

    private:
        CelestialType type;
        CelestialObject* parent;
        size_t m_resolution;
        std::string texPath;
        float radius;
        float orbitRadius;
        float orbitPeriod;
        float rotationPeriod;
        float inclinationAngle;
        glm::vec3 center;
        std::vector<float> m_vertexPositions;
        std::vector<float> m_vertexNormals;
        std::vector<unsigned int> m_triangleIndices;
        std::vector<float> m_vertexTexCoords;
        GLuint m_vao;
        GLuint m_texVbo;
        GLuint m_posVbo;
        GLuint m_normalVbo;
        GLuint m_ibo;
        GLuint m_texCoordVbo;
        glm::mat4 m_modelMatrix;
};

#endif