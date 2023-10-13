#version 330 core            // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
out vec3 fNormal;
out vec3 fPosition;

uniform mat4 viewMat, projMat, modelMat;

void main() {
        fNormal = vNormal;
        fPosition = vPosition;
        gl_Position = projMat * viewMat * modelMat * vec4(vPosition, 1.0); // mandatory to rasterize properly
}
