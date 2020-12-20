#version 330 core


in vec2 fragTexCoord;
in vec4 fragPos;

uniform sampler2D tex;

uniform sampler2D alphaMap;
uniform bool useAlphaMap;

uniform vec3 lightPos;
uniform int farPlane;

uniform bool modelSpriteSheet = false;
uniform int numOfRows = 1;
uniform int numOfColumns = 1;
uniform int textureRow = 0;
uniform int textureColumn = 0;

void main()
{
	float x = (float(textureColumn) / numOfColumns) +  (fragTexCoord.x / numOfColumns);
	float y = (float(textureRow) / numOfRows) +  (fragTexCoord.y / numOfRows);
	
	if(modelSpriteSheet)
	{
		x = fragTexCoord.x + (float(textureColumn) *  (1.0 / numOfColumns));
		y = fragTexCoord.y - (float(textureRow) * (1.0 / float(numOfRows)));
	}
	else 
	{
		x = (float(textureColumn) / numOfColumns) +  (fragTexCoord.x / numOfColumns);
		y = (float(textureRow) / numOfRows) +  (fragTexCoord.y / numOfRows);
	}

	if(texture(tex, vec2(x, y)).a <= 0.1 || (useAlphaMap && texture(alphaMap, vec2(x,y)).r == 0)) {gl_FragDepth = 1.1; return; };
	//else
	{
		float distance = length(fragPos.xyz - lightPos);
		distance /= farPlane;
		gl_FragDepth = distance;
	}
	

	
}

