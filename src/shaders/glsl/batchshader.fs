R"(
#version 330 core

in vec2 TexCoords0;

out vec4 FragColor;

uniform usampler2D tex_main;
uniform sampler2D tex_pal;

in vec4 TexBounds;
in vec2 TexCoords;
in float palY;
in float fade;

void main()
{
    if (palY < 0) discard;

    vec2 tc = vec2(TexBounds[0] + fract(TexCoords.x) * (TexBounds[1] - TexBounds[0]),
                   TexBounds[2] + fract(TexCoords.y) * (TexBounds[3] - TexBounds[2]));
    float index = texture(tex_main, tc).r / 255.0;
    //vec4 texColor = texture(tex_pal, vec2(index, 0.1));
    vec4 texColor = texture(tex_pal, vec2(index, palY));
    if (texColor.a < 0.5) {
        discard;
    }
    FragColor=texColor;
    //gl_FragDepth = texColor.a;

}

)"