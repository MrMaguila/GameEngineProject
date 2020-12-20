#version 330 core
out vec4 outColor;

in vec3 fragPos; //position in world coordinates
in vec3 fragNormal; //normal in world coordinates
in vec2 texCoordinates;


uniform sampler2D tex;
uniform int numOfRows = 4;
uniform int numOfCollums = 4;
uniform int textureRow = 0;
uniform int textureCollum = 0;

void main()
{
	float x = (textureCollum / numOfCollums) +  (texCoordinates.x / numOfCollums);
	float y = (textureRow / numOfRows) +  (texCoordinates.y / numOfRows);
	if(texture(tex, vec2(x, y)).a <= 0.01) discard;
	outColor = vec4(texture(tex, vec2(x, y)).rgb, 1.0);
}