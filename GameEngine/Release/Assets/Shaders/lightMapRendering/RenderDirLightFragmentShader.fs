#version 330 core

in vec3 fragPos; //fragment position in world space
in vec3 outFragNormal;
in vec2 texCoordinates;
in vec4 lightSpaceFragPos;
in mat3 TBNmatrix;

out vec4 outColor;

struct DirLight
{
	sampler2D depthMap;
	vec3 direction;
	vec3 ambient;
	vec3 color;
};

uniform DirLight directionalLight;
uniform int numOfLights;
uniform sampler2D tex;
uniform vec3 viewPos;
uniform sampler2D normalsTex;
uniform bool useNormalMap = false;

uniform int numOfRows = 4;
uniform int numOfCollums = 4;
uniform int textureRow = 0;
uniform int textureCollum = 0;

vec3 fragNormal;

float computeDirLightShadows(vec4);


void main()
{ 
	float x = (float(textureCollum) / numOfCollums) +  (texCoordinates.x / numOfCollums);
	float y = (float(textureRow) / numOfRows) +  (texCoordinates.y / numOfRows);
	if(texture(tex, vec2(x, y)).a <= 0.01) discard;	

	if(useNormalMap)
	{
		fragNormal = texture(normalsTex, vec2(x, y)).rgb;
		fragNormal = normalize(fragNormal * 2.0 - 1.0);
		fragNormal = normalize(TBNmatrix * fragNormal);
	}
	else 
		fragNormal = outFragNormal;


	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 lightDir;
	vec3 viewDir;

	float shadow = computeDirLightShadows(lightSpaceFragPos);

	//set the ambient component:
	ambient = directionalLight.ambient;

	//the diffuse		
	diffuse = max(dot(fragNormal, -directionalLight.direction), 0.0) * directionalLight.color * (1.0 - shadow);
		
	//and the specular
	viewDir = normalize(viewPos - fragPos);
	specular = pow(max(dot(fragNormal, normalize(viewDir + (-directionalLight.direction))), 0.0), 64) * directionalLight.color * (1.0 - shadow);
	
	vec3 lightning = (ambient + (diffuse) + (specular));
	outColor = vec4(lightning, 0.5 / numOfLights);
}

//#######################################################################

float computeDirLightShadows(vec4 fragPosInLightSpace)
{
	vec3 projCoord = fragPosInLightSpace.xyz / fragPosInLightSpace.w;	
	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth = texture(directionalLight.depthMap, projCoord.xy).r;
	float currentDepth = projCoord.z;
	float bias = max(0.05 * (1.0 - dot(fragNormal, -directionalLight.direction)), 0.005);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if(projCoord.z >= 1.0) shadow = 0.0;
	return shadow;
}






