#version 330 core

layout(location = 0) out vec4 outColor; //output color for the first color buffer
layout(location = 1) out vec4 outColor2; //for the second color buffer

uniform sampler2D gAlbedoSpec; //rgb components are the albedo and the a is the specular

in vec2 texCoord;

void main()
{
	vec4 albedoAndSpec = texture(gAlbedoSpec, texCoord).rgba;
	vec3 albedo = albedoAndSpec.rgb;
	float spec = albedoAndSpec.a;	

	vec3 lightning = albedo * 0.1; //hard coded ambient component
	outColor = vec4(lightning, 1.0);

	//if(dot(outColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 5.89)
	//	outColor2 = vec4(outColor.rgb, 1.0);
	//else
	//	outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
	
}






