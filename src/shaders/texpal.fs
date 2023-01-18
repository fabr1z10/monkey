R"(
#version 330 core

out vec4 fragColor;

in vec2 tex;
in vec4 col;

uniform usampler2D texture_diffuse1;
uniform sampler1D texture_palette;
uniform vec4 add_color;
uniform vec4 mult_color;

void main()
{

	float index = texture(texture_diffuse1, tex).r / 255.0;
	vec4 texColor = texture(texture_palette, index);

	//texColor = vec4(1.0, 1.0, 1.0, 1.0);
	if (texColor.a < 0.5) {
		discard;
	}
	//texColor *= col;
	//texColor *= mult_color;
	//texColor += add_color;
	fragColor = texColor;
}
)"


