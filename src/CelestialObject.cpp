#include "CelestialObject.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"

CelestialObject::CelestialObject(float radius, size_t m_resolution, std::string texPath, CelestialType type) {
    this->type = type;
    this->radius = radius;
    this->m_resolution = m_resolution;
    this->parent = nullptr;
    this->texPath = texPath;
    this->center = glm::vec3(0.0);
}

CelestialObject::CelestialObject(float radius, CelestialObject *parent, float orbitRadius, float orbitPeriod, size_t m_resolution, std::string texPath, CelestialType type) {
    this->type = type;
    this->radius = radius;
    this->parent = parent;
    this->orbitPeriod = orbitPeriod;
    this->orbitRadius = orbitRadius;
    this->m_resolution = m_resolution;
    this->texPath = texPath;
    this->center = glm::vec3(parent->center.x  + orbitRadius, parent->center.y , parent->center.z);
}

void CelestialObject::init() {
  m_vao = 0;
  m_posVbo = 0;
  m_normalVbo = 0;
  m_ibo = 0;
  this->genSphere();
  this->initGPUgeometry();
  m_texVbo = loadTextureFromFileToGPU(this->texPath);
}

void CelestialObject::genSphere() {
    // Effacement des vecteurs existants s'ils contiennent des données
    this->m_vertexPositions.clear();
    this->m_vertexNormals.clear();
    this->m_triangleIndices.clear();

    // Calcul des sommets de la sphère
    for (size_t i = 0; i <= this->m_resolution; ++i) {
        float phi = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(this->m_resolution); // Angle vertical (0 à pi)
        for (size_t j = 0; j <= this->m_resolution; ++j) {
            float theta = 2 * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(this->m_resolution); // Angle horizontal (0 à 2*pi)

            // Coordonnées sphériques
            float x = this->radius * sin(phi) * cos(theta);
            float z = this->radius * sin(phi) * sin(theta);
            float y = this->radius * cos(phi);


            // Ajout des coordonnées des sommets
            m_vertexPositions.push_back(x);
            m_vertexPositions.push_back(y);
            m_vertexPositions.push_back(z);

            // Ajout les normales (les mêmes que les positions pour une sphère)
            m_vertexNormals.push_back(x);
            m_vertexNormals.push_back(y);
            m_vertexNormals.push_back(z);


            float t1 = static_cast<float>(j)/static_cast<float>(m_resolution);
            float t2 = static_cast<float>(i)/static_cast<float>(m_resolution);
            m_vertexTexCoords.push_back(t1);
            m_vertexTexCoords.push_back(t2);
        }
    }

    // Calcul des indices pour les triangles
    for (size_t i = 0; i < m_resolution; ++i) {
        for (size_t j = 0; j < m_resolution; ++j) {
            size_t p1 = i * (m_resolution + 1) + j;
            size_t p2 = p1 + 1;
            size_t p3 = (i + 1) * (m_resolution + 1) + j;
            size_t p4 = p3 + 1;

            // Triangle 1
            m_triangleIndices.push_back(p1);
            m_triangleIndices.push_back(p2);
            m_triangleIndices.push_back(p3);

            // Triangle 2
            m_triangleIndices.push_back(p2);
            m_triangleIndices.push_back(p4);
            m_triangleIndices.push_back(p3);
        }
    }
}

void CelestialObject::initGPUgeometry() {
 // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)
#ifdef _MY_OPENGL_IS_33_
  glGenVertexArrays(1, &m_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.
#else
  glCreateVertexArrays(1, &m_vao);
#endif
  glBindVertexArray(m_vao);

  // Generate a GPU buffer to store the positions of the vertices and the normals
  size_t vertexBufferSize = sizeof(float)*m_vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector
  size_t vertexNormalsBufferSize = sizeof(float)*m_vertexNormals.size();
  size_t vertexTexCoordsBufferSize = sizeof(float)*m_vertexTexCoords.size();
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexPositions.data(), GL_DYNAMIC_READ);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

  glGenBuffers(1, &m_normalVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexNormalsBufferSize, m_vertexNormals.data(), GL_DYNAMIC_READ);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

  glGenBuffers(1, &m_texCoordVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexTexCoordsBufferSize, m_vertexTexCoords.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
#else
  glCreateBuffers(1, &m_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
  glNamedBufferStorage(m_posVbo, vertexBufferSize, m_vertexPositions.data(), GL_DYNAMIC_STORAGE_BIT); // Create a data storage on the GPU and fill it from a CPU array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
#endif

  // Same for an index buffer object that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*m_triangleIndices.size();
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices.data(), GL_DYNAMIC_READ);
#else
  glCreateBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glNamedBufferStorage(g_ibo, indexBufferSize, m_triangleIndices.data(), GL_DYNAMIC_STORAGE_BIT);
#endif

  glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}

GLuint CelestialObject::loadTextureFromFileToGPU(const std::string &filename) {
    int width, height, numComponents;
    // Loading the image in CPU memory using stb_image
    unsigned char *data = stbi_load(
            filename.c_str(),
            &width, &height,
            &numComponents, // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
            0);

    GLuint texID;
    glGenTextures(1, &texID); // generate an OpenGL texture container
    glBindTexture(GL_TEXTURE_2D, texID); // activate the texture
    // Setup the texture filtering option and repeat mode; check www.opengl.org for details.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Fill the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Free useless CPU memory
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture

    return texID;
}

void CelestialObject::updateOrbit(float deltaTime, float r) {
    // Mise à jour l'angle orbital en fonction du temps et de la vitesse orbitale
    float orbitAngle = 2 * M_PI * (1 / (this->orbitPeriod * 0.1)) * deltaTime; // * 0.1 for a smaller period

    // Calcul des nouvelles coordonnées X et Z en utilisant trigonométrie
    float newX = parent->center.x + r * glm::cos(orbitAngle);
    float newZ = parent->center.z + r * glm::sin(orbitAngle);

    // Mise à jour la position de la planète
    center = glm::vec3(newX, parent->center.y, newZ);
}


void CelestialObject::render(GLuint program, Camera camera) {

    glm::mat4 model = glm::mat4(1.0f);
    const glm::mat4 viewMatrix = camera.computeViewMatrix();
    const glm::mat4 projMatrix = camera.computeProjectionMatrix();
    const glm::vec3 camPosition = camera.getPosition();

    glUseProgram(program); // Activate the program to be used for upcoming primitive

    glUniformMatrix4fv(glGetUniformLocation(program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform3f(glGetUniformLocation(program, "camPos"), camPosition[0], camPosition[1], camPosition[2]);

    glActiveTexture(GL_TEXTURE0); // activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, this->m_texVbo);
    // glUniform1i(this->m_texVbo, 0);
    // glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
    // glBindTexture(GL_TEXTURE_2D, this->m_texVbo);
    glUniform1i(glGetUniformLocation(program, "material.albedoTex"), 0);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    if (this->type != CelestialType::Star) {
        float distance = this->orbitRadius;
        this->updateOrbit((float) glfwGetTime(), distance);

        model = glm::translate(model, this->center);

        glUniformMatrix4fv(glGetUniformLocation(program, "modelMat"), 1, GL_FALSE, glm::value_ptr(model));
    }

    glBindVertexArray(m_vao);     // activate the VAO storing geometry data
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0); // Call for rendering: stream the current GPU geometry through the current GPU program
}