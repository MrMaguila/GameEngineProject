#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 fragPos;
out vec4 fragColor;

uniform vec3 positions[200];
uniform vec4 colors[200];

uniform mat4 projAndView;

void main()
{

	gl_Position = projAndView * vec4(positions[gl_InstanceID], 1.0);
	gl_PointSize = 2000.0 / gl_Position.z;

	fragColor = colors[gl_InstanceID];
	fragPos = positions[gl_InstanceID];
}