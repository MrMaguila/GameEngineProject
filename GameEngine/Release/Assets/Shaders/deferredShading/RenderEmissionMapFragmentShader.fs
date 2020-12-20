#version 330 core

layout(location = 0) out vec4 outColor; //output color for the first color buffer
layout(location = 1) out vec4 outColor2; //for the second color buffer


uniform sampler2D gAlbedoSpec; //rgb components are the albedo and the a is the specular
uniform sampler2D gEmission;
uniform vec2 screenSize;

void main()
{
	vec2 texCoordinates = (gl_FragCoord.xy) / (screenSize);
	vec4 albedoAndSpec = texture(gAlbedoSpec, texCoordinates).rgba;
	vec3 albedo = albedoAndSpec.rgb;

	vec3 lightning = texture(gEmission, texCoordinates).rgb * 4.0;
	outColor = vec4(lightning, 1.0);
	
	if(dot(outColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 1.00)
		outColor2 = vec4(outColor.rgb, 1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
	
}






