#version 330 core

layout(location = 0) in vec3 fragPos;
//layout(location = 1) in vec2 texCoord;


void main()
{
	gl_Position = vec4(fragPos, 1.0);
}