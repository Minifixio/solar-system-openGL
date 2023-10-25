//
// Created by Émile Le Gallic on 13/10/2023.
//

#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "glm/gtx/string_cast.hpp"
#include <iostream>


enum CameraMovement {FORWARD, BACKWARD, LEFT, RIGHT};

class Camera {
public:
    inline float getFov() const { return m_fov; }

    inline void setFoV(const float f) { m_fov = f; }

    inline float getAspectRatio() const { return m_aspectRatio; }

    inline void setAspectRatio(const float a) { m_aspectRatio = a; }

    inline float getNear() const { return m_near; }

    inline void setNear(const float n) { m_near = n; }

    inline float getFar() const { return m_far; }

    inline void setFar(const float n) { m_far = n; }

    inline void setPosition(const glm::vec3 &p) { m_pos = p; }

    inline glm::vec3 getPosition() { return m_pos; }

    inline glm::mat4 computeViewMatrix() const {
        return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

    // Returns the projection matrix stemming from the camera intrinsic parameter.
    inline glm::mat4 computeProjectionMatrix() const {
        return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
    }

    // In order to adjust the zoom (i.e fov)
    void processMouseScroll(float yoffset) {
        m_fov -= (float)yoffset;
        if (m_fov < 1.0f)
            m_fov = 1.0f;
        if (m_fov > 80.0f)
            m_fov = 80.0f;
    }

    // Fonction pour faire pivoter la caméra vers le haut autour de l'axe X
    void rotateRight(float angleDegrees) {
        std::cout << "rotateUp" << std::endl;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        m_pos = glm::vec3(rotation * glm::vec4(m_pos, 1.0f));
        std::cout << "m_pos: " << glm::to_string(m_pos) << std::endl;
    }

    void rotateLeft(float angleDegrees) {
        std::cout << "rotateUp" << std::endl;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        m_pos = glm::vec3(rotation * glm::vec4(m_pos, 1.0f));
        std::cout << "m_pos: " << glm::to_string(m_pos) << std::endl;
    }

    // Fonction pour faire pivoter la caméra vers le haut autour de l'axe perpendiculaire à la direction de vue
    void rotateUp(float angleDegrees) {
        glm::vec3 viewDirection = -glm::normalize(m_pos); // Direction vers laquelle la caméra regarde
        glm::vec3 right = glm::normalize(glm::cross(viewDirection, glm::vec3(0.0f, 1.0f, 0.0f))); // Vecteur "vers la droite"

        // Limite la rotation à +80 degrés
        if (m_pos.y < 80.0f) {
            std::cout << "rotateUp" << std::endl;
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-angleDegrees), right);
            m_pos = glm::vec3(rotation * glm::vec4(m_pos, 1.0f));
            std::cout << "m_pos: " << glm::to_string(m_pos) << std::endl;
        }
    }

// Fonction pour faire pivoter la caméra vers le bas autour de l'axe perpendiculaire à la direction de vue
    void rotateDown(float angleDegrees) {
        glm::vec3 viewDirection = -glm::normalize(m_pos); // Direction vers laquelle la caméra regarde
        glm::vec3 right = glm::normalize(glm::cross(viewDirection, glm::vec3(0.0f, 1.0f, 0.0f))); // Vecteur "vers la droite"

        // Limite la rotation à -80 degrés
        if (m_pos.y > -80.0f) {
            std::cout << "rotateDown" << std::endl;
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), right);
            m_pos = glm::vec3(rotation * glm::vec4(m_pos, 1.0f));
            std::cout << "m_pos: " << glm::to_string(m_pos) << std::endl;
        }
    }





private:
    glm::vec3 m_pos = glm::vec3(0, 0, 0);
    float m_fov = 45.f; // Field of view, in degrees
    float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
    float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
    float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process
    float m_zoom = 45.0f;
};


#endif //TPOPENGL_CAMERA_H
