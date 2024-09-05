#version 330 core

in vec3 normal;
in vec3 position;

uniform vec3 eye;

uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

out vec4 FragColor;

void main() {
    vec3 normalizedNormal = normalize(normal);

    vec3 toEye = normalize(eye - position);
    vec3 toLight = lightPosition - position;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;

    vec3 ambient = materialAmbient * lightAmbient;
    vec3 diffuse = materialDiffuse * clamp(dot(normalizedNormal, toLight), 0, 1) * lightDiffuse;

    float specularMultiplier = pow(max(0, dot(toEye, reflect(-toLight, normalizedNormal))), materialShininess);
    vec3 specular = materialSpecular * specularMultiplier * lightSpecular;

    vec3 light = ambient + (diffuse + specular) / (distanceToLight + 0.00001);

    FragColor = vec4(light, 1.0);
}
