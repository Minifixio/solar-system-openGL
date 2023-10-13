#version 330 core	     // Minimal GL version support expected from the GPU

in vec3 fPosition;
out vec4 color;	  // Shader output: the color response attached to this fragment
uniform vec3 camPos;
in vec3 fNormal;

void main() {
    vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 0.7f);
    vec3 objectColor = vec3(1.0f, 0.5f, 0.31f);
    vec3 norm = normalize(fNormal);

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    float specularStrength = 0.1;
    vec3 lightDir = normalize(lightPos - fPosition);
    vec3 viewDir = normalize(camPos - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    color = vec4(result, 1.0);

	// color = vec4(1.0); // build an RGBA from an RGB
}
