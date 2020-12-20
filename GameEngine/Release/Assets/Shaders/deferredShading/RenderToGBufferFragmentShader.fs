#version 330 core

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 albedoSpec;
layout(location = 3) out vec3 emission;

in mat3 TBNmatrix;
in vec3 fragPos; //fragment position in world space
in vec3 fragNormal;
in vec2 texCoordinates;

uniform sampler2D tex;
uniform sampler2D normalsTex;
uniform sampler2D emissionTex;
uniform sampler2D roughnessTex;
uniform sampler2D metallicTex;


uniform bool useNormalMap = false;
uniform bool useEmissionMap = false;
uniform bool useRoughnessMap = false;
uniform bool useMetallicMap = false;

uniform vec3 viewPos;

uniform bool modelSpriteSheet = false;
uniform int numOfRows = 1;
uniform int numOfColumns = 1;
uniform int textureRow = 0;
uniform int textureColumn = 0;

uniform float materialSpec = 32.0;

void main()
{ 
	if(dot(viewPos - fragPos, fragNormal) < 0.0) discard;

	//store position:
	position = fragPos;
	
	//store normal:
	normal.rgb = fragNormal;

	//get the texture coordinates:
	float x = 0;
	float y = 0;
	if(modelSpriteSheet)
	{
		x = texCoordinates.x + (float(textureColumn) *  (1.0 / float(numOfColumns)));
		y = texCoordinates.y - (float(textureRow) * (1.0 / float(numOfRows)));
	}
	else
	{
		x = (float(textureColumn) / numOfColumns) +  (texCoordinates.x / numOfColumns);
		y = (float(textureRow) / numOfRows) +  (texCoordinates.y / numOfRows);
 	}

	if(useNormalMap)
	{
		normal.rgb = texture(normalsTex, vec2(x, y)).xyz;
		normal.rgb = 2.0 * normal.rgb  - vec3(1.0, 1.0, 1.0); //clamp from the [0, 1] range to [-1, 1]
		normal.rgb = normalize(TBNmatrix * normal.rgb);
	}
	

	if(useEmissionMap) //set the emission component
	{
		emission = texture(emissionTex, vec2(x, y)).rgb;
	}
	else emission = vec3(0.0);
	
	vec4 albedoAndAlpha = texture(tex, vec2(x, y)).rgba;
	if(albedoAndAlpha.a <= 0.01 && emission == vec3(0.0)) discard; 

	//store the albedo value:
	albedoSpec.rgb = albedoAndAlpha.rgb;
	albedoSpec.a = 1.0;
	
	

	
	if(useRoughnessMap)
	{
		albedoSpec.a = texture(roughnessTex, vec2(x, y)).r;
	}

	if(useMetallicMap)
	{
		normal.a = texture(metallicTex, vec2(x, y)).r;
	}
	else normal.a = 0.0;
}






