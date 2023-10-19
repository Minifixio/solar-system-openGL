#version 330 core

in vec2 fTexCoord;
out vec4 color;

struct Material {
    sampler2D albedoTex;
};
uniform Material material;
uniform sampler2D ourTexture;

void main()
{
    vec3 texColor = texture(ourTexture, fTexCoord).rgb;
    color = vec4(texColor, 1.0f);
}