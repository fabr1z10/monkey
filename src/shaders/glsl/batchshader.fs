R"(
#version 330 core

in vec2 TexCoords0;

out vec4 FragColor;

uniform usampler2D tex_main;
uniform sampler2D tex_pal;
in vec4 TexBounds;
in float palY;

void main()
{
    vec2 tc = vec2(TexBounds[0] + fract(TexCoords0.x) * (TexBounds[1] - TexBounds[0]),
        TexBounds[3] + fract(TexCoords0.y) * (TexBounds[2] - TexBounds[3]));
    //FragColor = texture2D(tex_main, TexCoords0);
    float index = texture(tex_main, tc).r / 255.0;
    vec4 texColor = texture(tex_pal, vec2(index, 0.1));
    texColor = texture(tex_pal, vec2(index, palY));
    FragColor=texColor; //vec4(index * 255 * 16, 0,0, 1);//texColor;//vec4(index, 0,0,1);//texColor;//vec4(1,1,1,1);//texColor;

}

)"