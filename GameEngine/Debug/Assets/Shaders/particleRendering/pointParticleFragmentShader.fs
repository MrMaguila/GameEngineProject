#version 330 core

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 albedoSpec;
layout(location = 3) out vec3 emission;


in vec4 fragColor;
in vec3 fragPos;

void main()
{
	position = fragPos;
	albedoSpec = fragColor;

	//if(dot(fragColor.rgb, vec3(0.4526, 0.4552, 0.0722)) > 20.89)
	//	emission = vec3(fragColor.rgb);
	//else
	emission = vec3(0.0, 0.0, 0.0);

}

