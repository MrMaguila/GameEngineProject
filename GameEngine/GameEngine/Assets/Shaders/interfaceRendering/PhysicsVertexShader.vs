#version 330 core
layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform vec3 size;
uniform mat4 viewAndProj;

out vec3 fragPos;

//Render the texture on a squad
void main()
{
	vec3 pos2 = pos;
	vec3 size2 = size;
	pos2.x *= size.x;
	pos2.y *= size.y;
	pos2.z *= size.z;

	fragPos = pos2;
	gl_Position = viewAndProj * model * vec4(fragPos, 1.0);
}
