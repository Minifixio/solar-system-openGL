#version 330 core
layout(location=0) in vec3 vPosition;
layout(location=2) in vec2 vTexCoord;

out vec2 fTexCoord;
uniform mat4 viewMat, projMat, modelMat;

void main()
{
    fTexCoord = vTexCoord;
    gl_Position = projMat * viewMat * modelMat * vec4(vPosition, 1.0);
}