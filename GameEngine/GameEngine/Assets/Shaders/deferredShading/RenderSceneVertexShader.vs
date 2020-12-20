#version 330 core

layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 viewAndProj;

//out vec3 normal;
uniform float sphereRadius = 20;

void main()
{
	gl_Position = viewAndProj * model * vec4(pos * sphereRadius, 1.0);
	//normal = normalize(transpose(inverse(mat3(model))) * aNormal);
}