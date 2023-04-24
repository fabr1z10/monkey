R"(
#version 330 core

in vec2 TexCoord0;

out vec4 FragColor;

uniform usampler2D tex_main;
uniform sampler2D tex_pal;

void main()
{
    //FragColor = texture2D(tex_main, TexCoord0);
    float index = texture(tex_main, TexCoord0).r / 255.0;
    vec4 texColor = texture(tex_pal, vec2(index, 0));
    FragColor=vec4(1,1,1,1);//texColor;

}

)"