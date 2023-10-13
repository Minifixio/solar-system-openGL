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
        CelestialObject(float radius, CelestialObject *parent, float orbitRadius, float orbitPeriod, size_t m_resolution, CelestialType type);
        CelestialObject(float radius, size_t m_resolution, CelestialType type);
        void init(); // should properly set up the geometry buffer
        void render(GLuint program, Camera camera); // should be called in the main rendering loop
        CelestialType getType() { return this->type; }
        float getOrbitRadius() { return this->orbitRadius; }
    
    private:
        void initGPUgeometry();
        void genSphere();
        void updateOrbit(float deltaTime, float radius);

    private:
        CelestialType type;
        CelestialObject* parent;
        size_t m_resolution;
        float radius;
        float orbitRadius;
        float orbitPeriod;
        glm::vec3 center;
        std::vector<float> m_vertexPositions;
        std::vector<float> m_vertexNormals;
        std::vector<unsigned int> m_triangleIndices;
        GLuint m_vao;
        GLuint m_posVbo;
        GLuint m_normalVbo;
        GLuint m_ibo;
        glm::mat4 m_modelMatrix;
};

#endif