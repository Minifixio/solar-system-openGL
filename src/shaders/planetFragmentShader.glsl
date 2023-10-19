#version 330 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;

out vec4 color;

struct Material {
    sampler2D albedoTex;
};
uniform Material material;
uniform sampler2D ourTexture;
uniform vec3 camPos;

void main() {

    vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 0.7f);
    vec3 texColor = texture(material.albedoTex, fTexCoord).rgb;
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(lightPos - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(camPos - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * texColor;
    color = vec4(result, 1.0);
}
