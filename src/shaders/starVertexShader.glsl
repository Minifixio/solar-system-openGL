#version 330 core
layout(location=0) in vec3 vPosition;
layout(location=2) in vec2 vTexCoord;

out vec2 fTexCoord;
uniform mat4 viewMat, projMat;

void main()
{
    gl_Position = projMat * viewMat * vec4(vPosition, 1.0);
    fTexCoord = vTexCoord;
}