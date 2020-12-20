#version 330 core

in vec2 texCoordinates;

uniform sampler2D tex;

uniform sampler2D alphaMap;
uniform bool useAlphaMap;

uniform bool modelSpriteSheet = false;
uniform int numOfRows = 1;
uniform int numOfColumns = 1;
uniform int textureRow = 0;
uniform int textureColumn = 0;

void main()
{
	float x = 0;
	float y = 0;
	if(modelSpriteSheet)
	{
		x = texCoordinates.x + (float(textureColumn) *  (1.0 / numOfColumns));
		y = texCoordinates.y - (float(textureRow) * (1.0 / numOfRows));
	}
	else
	{
		x = float(textureColumn + texCoordinates.x) / numOfColumns;
		y = float(textureRow + texCoordinates.y) / numOfRows;
	}
	if(texture(tex, vec2(x, y)).a <= 0.01 || (useAlphaMap && texture(alphaMap, vec2(x,y)).r == 0)) 
		gl_FragDepth = 1.1;
	else gl_FragDepth = gl_FragCoord.z;
}