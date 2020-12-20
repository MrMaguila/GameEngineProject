#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

uniform vec2 mousePos;
uniform vec2 screenSize;

out vec2 texCoordinates;


//Render the texture on a squad
void main()
{
	vec2 mouseSize = vec2(16.0, 16.0);
	vec2 mpos = mousePos; //mousePos is in screenCordinates, so it must be converted to normalized device coordinates
	mpos.x = -(2 * mousePos.x / -screenSize.x) -1;
	mpos.y = (2 * mousePos.y / -screenSize.y) + 1;
	gl_Position = vec4(vec3(pos.x * (mouseSize.x / 480), pos.y * (mouseSize.y / 270), pos.z) + vec3(mpos, 0.0), 1.0);
	texCoordinates = texCoord;
}
