R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    //FragColor = vec4(vec3(pow(gl_FragCoord.z,50)), 1);
    //FragColor = vec4(TexCoords.x,0,0,1);
}
)"
