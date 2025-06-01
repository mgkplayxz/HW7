#version 330 core
out vec4 FragColor;

in vec3 wPosition;
in vec3 wNormal;
in vec3 wColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // Normalize inputs
    vec3 norm = normalize(wNormal);
    vec3 lightDir = normalize(lightPos - wPosition);
    vec3 viewDir = normalize(viewPos - wPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Final color
    vec3 phong = (ambient + diffuse + specular) * wColor;
    FragColor = vec4(phong, 1.0);
}
