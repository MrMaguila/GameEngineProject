#version 330 core

layout(location = 0) out vec4 outColor; //output color for the first color buffer
layout(location = 1) out vec4 outColor2; //for the second color buffer(this one is used for bloom)


uniform sampler2D gPos; //fragment position from the g buffer, in world space
uniform sampler2D gNormal; //fragment normal and metallic from the g buffer
uniform sampler2D gAlbedoSpec; //rgb components are the albedo and the a is the specular
uniform sampler2D shadowMap;

uniform bool useShadowMap = true;

struct Light
{
	vec3 direction;
	vec3 color;
	mat4 pvm;
};

uniform Light light;

uniform vec3 viewPos;

uniform vec2 screenSize;

float computeShadow(vec4 fragPosInLightSpace, vec3 normal)
{
	vec3 projCoord = fragPosInLightSpace.xyz / fragPosInLightSpace.w;	
	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoord.xy).r;
	float currentDepth = projCoord.z;
	float bias = 0.01;//max(0.008 * (1.0 - dot(normal, light.direction)), 0.0005);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if(projCoord.z >= 1.0) shadow = 0.0;
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
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 lightDir = normalize(light.direction);
	vec4 normalAndMetallic = texture(gNormal, texCoordinates).rgba;
	vec3 fragNormal = normalize(normalAndMetallic.rgb);
	float metallic = normalAndMetallic.a;
	
	vec4 albedoAndSpec = texture(gAlbedoSpec, texCoordinates).rgba;
	vec3 albedo = albedoAndSpec.rgb;
	float roughness = albedoAndSpec.a;	
	
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
	vec3 radiance  = light.color;
	float shadow = 1.0 - (useShadowMap ? computeShadow(light.pvm * vec4(fragPos, 1.0), fragNormal) : 0.0);
		
	//compute the specular component:
	vec3 specular = (NDF * G * F) / max((4.0 * max(dot(fragNormal, viewDir), 0.0) * max(NdotL, 0.0)), 0.001); 
	specular *= shadow;
	
	lightning = (kD * albedo / PI + specular) * radiance * NdotL * shadow;

	lightning += (shadow == 0.0 ? 0.08 : 0.06) * light.color.rgb * albedo;

	//apply tone mapping and gamma correction:
	//float gamma = 1.0;
	
	//lightning = vec3(1.0) - exp(-lightning * 0.5);
	//lightning = pow(lightning, vec3(1.0 / gamma));  
	
	outColor = vec4(lightning, 1.0);
	
	if(dot(outColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 20.89)
		outColor2 = vec4(outColor.rgb, 1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
	
}






