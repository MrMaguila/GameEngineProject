#version 330 core

layout(location = 0) out vec4 outColor; //output color for the first color buffer
layout(location = 1) out vec4 outColor2; //for the second color buffer


uniform sampler2D gPos; //fragment position from the g buffer, in world space
uniform sampler2D gNormal; //fragment normal from the g buffer
uniform sampler2D gAlbedoSpec; //rgb components are the albedo and the a is the specular

struct Light
{
	samplerCube depthCubeMap;
	int farPlane;
	vec3 pos;
	vec3 color;
	float kConstant;
	float kLinear;
	float kQuadratic;
	float radius;
};

uniform Light light;

uniform vec3 viewPos;

uniform vec2 screenSize;
uniform float offset;

float computeShadow(vec3 fragInWorldSpace, vec3 fragNormal)
{
	vec3 cubeMapCoord = fragInWorldSpace - light.pos; //a vector going from light to the fragment
	float closestDepth = texture(light.depthCubeMap, cubeMapCoord).r * light.farPlane;
	float currentDepth = length(cubeMapCoord);
	float bias = max(0.5 * (1.0 + dot(fragNormal, normalize(-cubeMapCoord))), 1.0);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}



const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a2 = pow(roughness, 4);
	float num = a2;
	float denom = (pow(max(dot(N, H), 0.0), 2) * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
	float k = ((roughness + 1.0) * (roughness + 1.0)) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float ggx2 = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
	float ggx1 = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
	return ggx1 * ggx2;
}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}



void main()
{
	vec2 texCoordinates = (gl_FragCoord.xy) / (screenSize);

	vec3 fragPos = texture(gPos, texCoordinates).rgb;
	
	if(length(fragPos - light.pos) > light.radius) discard; //fragment outside the light sphere
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 lightDir = normalize(light.pos - fragPos);
	vec4 normalAndMetallic = texture(gNormal, texCoordinates).rgba;
	vec3 fragNormal	= normalize(normalAndMetallic.rgb);
	float metallic = normalAndMetallic.a;

	vec4 albedoAndSpec = texture(gAlbedoSpec, texCoordinates).rgba;
	vec3 albedo = albedoAndSpec.rgb;	
	float roughness = albedoAndSpec.a;	

	float shadow = 1.0 - computeShadow(fragPos, fragNormal);
	float dist = length(fragPos - light.pos);
	float attenuation = 1.0 / ((light.kQuadratic * dist * dist) + (light.kLinear * dist) + light.kConstant);
	
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	float NdotV = max(dot(fragNormal, viewDir), 0.0);
	float NdotL = max(dot(fragNormal, lightDir), 0.0);	

	float NDF = DistributionGGX(fragNormal, normalize(fragNormal + lightDir), roughness);
	float G = GeometrySmith(fragNormal, viewDir, lightDir, roughness);
	vec3 F = fresnelSchlick(max(dot(normalize(fragNormal + lightDir), viewDir), 0.0), F0);

	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

	vec3 lightning = vec3(0.0);
	vec3 radiance  = light.color * attenuation;
		
	//compute the specular component:
	vec3 specular = (NDF * G * F) / max(4.0 * NdotV * NdotL, 0.04); 
	specular *= shadow;	
	
	lightning = (kD * albedo / PI + specular) * radiance * NdotL * shadow;

	outColor = vec4(lightning, 1.0);

	//add the ambient component:
	outColor += vec4(albedo * light.color * attenuation * 0.03, 1.0);
			
	if(dot(outColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 40.89)
		outColor2 = vec4(outColor.rgb, 1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
}






