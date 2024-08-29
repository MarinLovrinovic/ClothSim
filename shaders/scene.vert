#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 position;

uniform mat4 model;
uniform mat4 viewProjection;

uniform vec3 eye;

uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

void main() {
    mat3 normalTransformation = mat3(transpose(inverse(model)));
    normal = normalize(normalTransformation * aNormal);
    position = vec3(model * vec4(aPos, 1.0));
    gl_Position = viewProjection * vec4(position, 1);
}