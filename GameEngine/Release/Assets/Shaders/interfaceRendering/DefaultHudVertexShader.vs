#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

uniform vec2 hudPos;
uniform vec2 hudSize;
uniform vec2 screenSize;

out vec2 texCoordinates;


//Render the texture on a squad
void main()
{
	gl_Position = vec4(vec3(pos.x * (hudSize.x / 480), pos.y * (hudSize.y / 270), pos.z) + vec3(hudPos, 0.0), 1.0);
	texCoordinates = texCoord;
}
