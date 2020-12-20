#version 330 core

in vec3 fragPos; //fragment position in world space
in vec3 outFragNormal;
in vec2 texCoordinates;
in mat3 TBNmatrix;

out vec4 outColor;

struct PointLight
{
	samplerCube depthMap;
	vec3 position;
	vec3 ambient;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

uniform PointLight pointLight;
uniform int numOfLights;
uniform sampler2D tex;
uniform sampler2D normalsTex;
uniform bool useNormalMap = false;
uniform vec3 viewPos;
uniform int farPlane;

uniform int numOfRows = 4;
uniform int numOfCollums = 4;
uniform int textureRow = 0;
uniform int textureCollum = 0;

vec3 fragNormal;

float computePointLightShadows(vec3);


void main()
{
	float x = (float(textureCollum) / numOfCollums) +  (texCoordinates.x / numOfCollums);
	float y = (float(textureRow) / numOfRows) +  (texCoordinates.y / numOfRows);
	if(texture(tex, vec2(x, y)).a <= 0.01) discard;
	
	float dist = length(pointLight.position - fragPos);
	float attenuation = pointLight.constant + (pointLight.linear * dist) + (pointLight.quadratic * dist * dist);		

	if(dist > 100) { outColor = vec4(pointLight.ambient / (attenuation), 1.0 / (numOfLights + 10)); return; }

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

	
	vec3 lightDir = normalize(pointLight.position - fragPos);
	vec3 viewDir = normalize(viewPos - fragPos);

	float shadow = computePointLightShadows(fragPos);

	//set the ambient component:
	ambient = pointLight.ambient;

	//the diffuse		
	diffuse = max(dot(fragNormal, lightDir), 0.0) * pointLight.color * (1.0 - shadow);
	
	//and the specular
	specular = pow(max(dot(fragNormal, normalize(viewDir + lightDir)), 0.0), 64) * pointLight.color * (1.0 - shadow);


	

	vec3 lightning = (ambient + (diffuse) + (specular))  * (1.0 /attenuation);
	//if(dist >= farPlane / 3) lightning = ambient  * (1.0 / attenuation);

		
	
	outColor = vec4(lightning, 1.0 / (numOfLights + 4));
}

//#######################################################################

float computePointLightShadows(vec3 fragPosInWorldSpace)
{
	vec3 fragToLight = fragPosInWorldSpace - pointLight.position;
	float closestDepth = texture(pointLight.depthMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);

	float bias = max(2.0 * (1.0 - dot(outFragNormal, -normalize(fragToLight))), 0.5);

	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}






