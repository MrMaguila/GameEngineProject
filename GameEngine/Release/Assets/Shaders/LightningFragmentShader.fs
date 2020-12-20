#version 330 core

const int maxDirLights = 5;

in vec3 fragPos; //fragment position in world space
in vec3 fragNormal;
in vec2 texCoordinates;
in vec4 lightSpaceFragPos[maxDirLights];
flat in int numOfDirectionalLights;

out vec4 outColor;

struct DirLight
{
	sampler2D depthMap;
	vec3 direction;
	vec3 ambient;
	vec3 color;
};

uniform DirLight directionalLights[maxDirLights];
uniform sampler2D tex;
uniform vec3 viewPos;

uniform int numOfRows = 4;
uniform int numOfCollums = 4;
uniform int textureRow = 0;
uniform int textureCollum = 0;



float computeDirLightShadows(vec4, int);


void main()
{ 
	float x = (float(textureCollum) / numOfCollums) +  (texCoordinates.x / numOfCollums);
	float y = (float(textureRow) / numOfRows) +  (texCoordinates.y / numOfRows);
	if(texture(tex, vec2(x, y)).a <= 0.01) discard;	
	vec3 color = texture(tex, vec2(x, y)).rgb;

	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);


	vec3 lightDir;
	vec3 viewDir;

	float shadow;
	for(int i = 0; i < numOfDirectionalLights; ++i)
	{
		shadow = computeDirLightShadows(lightSpaceFragPos[i], i);

		//set the ambient component:
		ambient += directionalLights[i].ambient * color;

		//the diffuse		
		diffuse += max(dot(fragNormal, directionalLights[i].direction), 0.0) * directionalLights[i].color * (1.0 - shadow);
		
		//and the specular
		viewDir = normalize(viewPos - fragPos);
		specular += pow(max(dot(fragNormal, normalize(viewDir + directionalLights[i].direction)), 0.0), 64) * directionalLights[i].color * (1.0 - shadow);
	}

	vec3 lightning = (ambient + (diffuse) + (specular)) * color;
	outColor = vec4(lightning, 1.0);

}

//#######################################################################

float computeDirLightShadows(vec4 fragPosInLightSpace, int index)
{
	vec3 projCoord = fragPosInLightSpace.xyz / fragPosInLightSpace.w;	
	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth = texture(directionalLights[index].depthMap, projCoord.xy).r;
	float currentDepth = projCoord.z;
	float bias = max(0.05 * (1.0 - dot(fragNormal, directionalLights[index].direction)), 0.005);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if(projCoord.z >= 1.0) shadow = 0.0;
	return shadow;
}






