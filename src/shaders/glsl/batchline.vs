R"(
#version 330 core

layout (location = 0) in float loc;
layout (location = 1) in uint QuadId;

#define MAX_LINES 200

uniform QuadInfo {
    vec3 Pos0[MAX_LINES];
    vec3 Pos1[MAX_LINES];
    vec4 Color[MAX_LINES];
};

uniform mat4 view;
uniform mat4 projection;

out vec4 fs_color;

void main() {
    vec3 P = Pos0[QuadId] + loc * (Pos1[QuadId] - Pos0[QuadId]);
    //vec3 P = vec3(0,QuadId*16.0,0) + vec3(0,0,0) + loc * (vec3(32,16,0)-vec3(0,0,0));
    //P = vec3(16.0 * loc, 32+ 16.0 * QuadId, 0.0);
    gl_Position = projection * view * vec4(P, 1);
    fs_color = Color[QuadId];
}

)"