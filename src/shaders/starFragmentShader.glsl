#version 330 core

in vec2 fTexCoord;
out vec4 color;

struct Material {
    sampler2D albedoTex; // texture unit, relate to glActivateTexture(GL_TEXTURE0 + i)
};
uniform Material material;
uniform sampler2D ourTexture;

void main()
{
    vec3 texColor = texture(ourTexture, fTexCoord).rgb;
    color = vec4(texColor, 1.0f);
    // color = vec4(1.0f, 0.5f, 0.31f, 1.0);
}