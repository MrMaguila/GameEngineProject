#version 330 core

out vec4 outColor;
in vec2 texCoordinates;

uniform sampler2D tex;


uniform sampler2D bloomTex;
uniform bool useBloom;

uniform sampler2D blurTex;
uniform bool useBlur;

void main()
{
	outColor = vec4(texture(tex, texCoordinates).rgb, 1.0);

	if(useBlur)
	{
		vec3 blurColor = texture(blurTex, texCoordinates).rgb;
		outColor = vec4(blurColor, 1.0);
	}
	if(useBloom)
	{
		vec3 bloomColor = texture(bloomTex, texCoordinates).rgb;
		outColor += vec4(bloomColor, 0.0);
		outColor /= 2.0;
	}
	float gamma = 1.0;
	
	outColor.xyz = vec3(1.0) - exp(-outColor.xyz * 0.5);
	outColor.xyz = pow(outColor.xyz, vec3(1.0 / gamma));  
	
	outColor = vec4(outColor.xyz, 1.0);
}