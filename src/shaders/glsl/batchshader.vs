R"(
#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vTexBounds;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in float vPal;
layout (location = 4) in uint vCamera;

uniform mat4 pv_mat[2];
//uniform mat4 projection;

out vec4 TexBounds;
out vec2 TexCoords;
out float palY;

void main() {
    // calculate position
    gl_Position = pv_mat[vCamera] * vec4(vPosition, 1);

    TexBounds = vTexBounds;
    TexCoords = vTexCoords;
    palY = vPal;
}

)"