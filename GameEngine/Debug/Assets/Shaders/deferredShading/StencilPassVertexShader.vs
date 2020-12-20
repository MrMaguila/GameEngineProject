#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 viewAndProj;
uniform float sphereScale;

void main()
{
	gl_Position = viewAndProj * model * vec4(sphereScale * pos, 1.0);

}