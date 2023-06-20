R"(
#version 330 core


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vColor;

uniform mat4 view;
uniform mat4 projection;

out vec4 Color;

void main() {

    // calculate position
    gl_Position = projection * view * vec4(vPosition, 1);
    Color = vColor;
}

)"