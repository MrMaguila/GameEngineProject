#version 330 core

layout(location = 0) out vec4 outColor; //output color for the first color buffer
layout(location = 1) out vec4 outColor2; //for the second color buffer


in vec3 fragPos; //fragment position in world space
in vec3 fragNormal;
in vec2 texCoordinates;


uniform sampler2D tex;
uniform sampler2D dirLightMap;
uniform sampler2D emissionTex;
uniform bool useEmissionMap = false;
uniform vec3 viewPos;

uniform int numOfRows = 4;
uniform int numOfCollums = 4;
uniform int textureRow = 0;
uniform int textureCollum = 0;

uniform int screenWidth;
uniform int screenHeight;

void main()
{ 
	float x = (float(textureCollum) / numOfCollums) +  (texCoordinates.x / numOfCollums);
	float y = (float(textureRow) / numOfRows) +  (texCoordinates.y / numOfRows);
	if(texture(tex, vec2(x, y)).a <= 0.01) discard;	
	vec3 color = texture(tex, vec2(x, y)).rgb;

	vec2 dirLightMapCoord = gl_FragCoord.xy;
	dirLightMapCoord.x /= screenWidth;
	dirLightMapCoord.y /= screenHeight;

	vec3 lightning;
	lightning = texture(dirLightMap, dirLightMapCoord).rgb;

	
	//apply tone mapping and gamma correction:
	float gamma = 1.0;
	
	lightning = vec3(1.0) - exp(-lightning * 0.5);
	lightning = pow(lightning, vec3(1.0 / gamma));  

	//see if emission map is avaible:
	//if(useEmissionMap) 
	//{	
	//	vec3 lightning2 = texture(emissionTex, vec2(x,y)).rgb;
	//	lightning += lightning2;
	//}
	//multiply the light by the texture color
	lightning.r *= color.r;
	lightning.g *= color.g;
	lightning.b *= color.b; 	
	
	if(useEmissionMap) 
	{	
		vec3 lightning2 = texture(emissionTex, vec2(x,y)).rgb;
		if(dot(lightning2, vec3(0.4126, 0.5152, 0.0722)) < 0.7)
			lightning += lightning2;
		else lightning = lightning2 * 2;
	}

	outColor = vec4(lightning, 1.0);

	if(dot(outColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 0.82)
		outColor2 = vec4(outColor.rgb, 1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
	
}






