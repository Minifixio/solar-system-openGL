#version 330 core

layout (location = 0) in vec3 vTexCoord;

out vec3 fTexCoord;

uniform mat4 projMat, viewMat;

void main()
{
    fTexCoord = vTexCoord;
    vec4 pos = projMat * viewMat * vec4(vTexCoord, 1.0);
    gl_Position = pos.xyww;
}