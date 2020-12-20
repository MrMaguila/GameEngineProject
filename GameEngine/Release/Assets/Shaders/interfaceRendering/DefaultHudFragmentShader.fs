#version 330 core

out vec4 outColor;
in vec2 texCoordinates;

uniform sampler2D tex;
uniform float horizontalPercent = 1.0;

void main()
{
	if(texCoordinates.x > horizontalPercent) discard;
	outColor = vec4(texture(tex, texCoordinates).rgba);
	if(outColor.a == 0.0) discard;
}