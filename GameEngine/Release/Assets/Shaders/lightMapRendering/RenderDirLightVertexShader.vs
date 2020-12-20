#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
//layout(location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 viewAndProj;
uniform mat4 lightMatrix;

out vec3 fragPos; //fragment position in world space
out vec3 outFragNormal;
out vec2 texCoordinates;
out vec4 lightSpaceFragPos;
out mat3 TBNmatrix;

void main()
{
	fragPos = vec3(model * vec4(pos, 1.0));
	gl_Position = viewAndProj * vec4(fragPos, 1.0);
	outFragNormal = normalize(transpose(inverse(mat3(model))) * normal);
	texCoordinates = texCoord;
	lightSpaceFragPos = lightMatrix * vec4(fragPos, 1.0);	

	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	vec3 B = cross(N, T);
	TBNmatrix = mat3(T, B, N);

}