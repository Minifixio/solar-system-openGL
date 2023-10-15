#version 330 core	     // Minimal GL version support expected from the GPU

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;

out vec4 color;	  // Shader output: the color response attached to this fragment

struct Material {
    sampler2D albedoTex; // texture unit, relate to glActivateTexture(GL_TEXTURE0 + i)
};
uniform Material material;
uniform sampler2D ourTexture;
uniform vec3 camPos;

void main() {
    vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 0.7f);
    vec3 texColor = texture(ourTexture, fTexCoord).rgb;
    // vec3 objectColor = vec3(1.0f, 0.5f, 0.31f);
    vec3 norm = normalize(fNormal);

    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    float specularStrength = 0.3;
    vec3 lightDir = normalize(lightPos - fPosition);
    vec3 viewDir = normalize(camPos - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = (ambient + diffuse + specular) * texColor;

    color = vec4(result, 1.0);
}
