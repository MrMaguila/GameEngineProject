#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 viewAndProj;

out vec3 fragPos; //position in world coordinates
out vec3 fragNormal; //normal in world coordinates
out vec2 texCoordinates; 

void main()
{
	gl_Position = viewAndProj * model * vec4(pos, 1.0);
	fragPos = vec3(model * vec4(pos, 1.0));
	fragNormal = mat3(transpose(inverse(model))) * normal;
	texCoordinates = texCoord; //just pass it to the next stage
}
