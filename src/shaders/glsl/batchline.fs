R"(
#version 330 core

in vec4 Color;
out vec4 FragColor;

void main()
{
    if (Color.a < 0.5) {
        discard;
    }
    FragColor = Color;
}

)"