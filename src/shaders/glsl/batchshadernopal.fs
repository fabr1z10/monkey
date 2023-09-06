R"(
#version 330 core

in vec2 TexCoords0;

out vec4 FragColor;

uniform sampler2D tex_main;

in vec4 TexBounds;
in vec2 TexCoords;

void main()
{
    vec2 tc = vec2(TexBounds[0] + fract(TexCoords.x) * (TexBounds[1] - TexBounds[0]),
                   TexBounds[2] + fract(TexCoords.y) * (TexBounds[3] - TexBounds[2]));
    vec4 texColor = texture(tex_main, tc);
    //if (texColor.a < 0.5) {
    //    discard;
    //}
    FragColor=texColor;
}

)"