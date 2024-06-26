R"(
#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vTexBounds;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in float vPal;
layout (location = 4) in float vFade;

uniform mat4 pv_mat;
//uniform mat4 projection;

out vec4 TexBounds;
out vec2 TexCoords;
out float palY;
out float fade;


void main() {
    // calculate position
    gl_Position = pv_mat * vec4(vPosition, 1);

    TexBounds = vTexBounds;
    TexCoords = vTexCoords;
    palY = vPal;
    fade = vFade;
}

)"