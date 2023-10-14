#version 330 core
layout (location = 0) in vec3 vPosition;

uniform mat4 viewMat, projMat;

void main()
{
    gl_Position = projMat * viewMat * vec4(vPosition, 1.0);
}