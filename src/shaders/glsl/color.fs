R"(
#version 330 core

out vec4 fragColor;

uniform vec4 mult_color;
uniform vec4 add_color;

in vec4 col;

void main()
{
    vec4 color = col;
    //color *= mult_color;
    //color += add_color;
	fragColor = color;
}
)"