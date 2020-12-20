#version 330 core

layout(location = 0) in vec3 fragPos;

out float intensity;
uniform vec2 minAndMaxIntensity = vec2(0.0, 1.0);

out vec2 fragTexCoord;

void main()
{

	fragTexCoord = vec2((fragPos.x + 1.0) / 2, (fragPos.y + 1.0) / 2);
	gl_Position = vec4(fragPos, 1.0);
	
	intensity =  minAndMaxIntensity.x * (-fragTexCoord.y + 1.0) + minAndMaxIntensity.y * (fragTexCoord.y  - 1.0);
	intensity /= 0.3;
	intensity += minAndMaxIntensity.y;
}