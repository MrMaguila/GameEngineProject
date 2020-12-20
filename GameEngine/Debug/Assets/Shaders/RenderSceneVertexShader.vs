#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 viewAndProj;

out vec3 fragPos; //fragment position in world space
out vec3 fragNormal;
out vec2 texCoordinates;

void main()
{
	fragPos = vec3(model * vec4(pos, 1.0));
	gl_Position = viewAndProj * vec4(fragPos, 1.0);
	fragNormal = normalize(transpose(inverse(mat3(model))) * normal);
	texCoordinates = texCoord;
}