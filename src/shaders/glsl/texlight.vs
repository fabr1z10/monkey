R"(
#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexture;
layout (location = 2) in vec3 vNormal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform mat3 nMat;

out vec2 tex;
out vec3 Normal;
//out vec4 eye;


void main()
{
    tex = vTexture;
    Normal = normalize(nMat * vNormal);
    //eye = - (model * view * vec4(vPosition, 1.0));
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}
)"