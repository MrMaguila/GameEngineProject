#version 330 core

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 texCoord;

out float intensity;
uniform vec2 minAndMaxIntensity = vec2(0.0, 1.0);

out vec2 fragTexCoord;

void main()
{
	fragTexCoord = texCoord;
	gl_Position = vec4(fragPos, 1.0);

	intensity =  minAndMaxIntensity.x * (texCoord.y - 1.0) + minAndMaxIntensity.y * (-texCoord.y + 2);
}