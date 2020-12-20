#version 330 core

out vec4 outColor;
in vec2 texCoordinates;

uniform sampler2D tex;

void main()
{
	outColor = vec4(texture(tex, texCoordinates).rgba);
	if(outColor.a == 0.0) discard;
}