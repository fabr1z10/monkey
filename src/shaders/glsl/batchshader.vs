R"(
#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 1) in uint QuadId;

#define MAX_QUADS 200

uniform QuadInfo {
    vec3 Pos[MAX_QUADS];
    vec2 Size[MAX_QUADS];
    vec4 TexCoords[MAX_QUADS];
    //vec2 Anchor[MAX_QUADS];
};

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords0;

void main() {
    // calculate position

    //vec4 pos = projection * view * vec4(48, 48, 1);
    vec3 delta = vec3(Position * Size[QuadId], 0);
    vec4 pos = projection * view * vec4(Pos[QuadId] + delta, 1);
    // pos += vec3(Position * Size[QuadId], 0);
    //pos += vec4(Position * (0.5, 0.5), 0, 0);
    //gl_Position = projection * view * vec4(pos, 1.0);
    //gl_Position = vec4(Position * (0.5, 0.5), 0, 1);
    gl_Position = pos;
    // compute tex coords
    vec4 tc = TexCoords[QuadId];
    //tc=vec4(0,1,0,1);
    //TexCoords0 = vec2(tc[0] + Position.x * (tc[1] - tc[0]), tc[2] + Position.y * (tc[3] - tc[2]));
    TexCoords0 = vec2(tc[0], tc[3]) + Position * vec2(tc[1] - tc[0], tc[2] - tc[3]);

}

)"