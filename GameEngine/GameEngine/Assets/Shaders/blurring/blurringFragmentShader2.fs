#version 330 core

out vec4 fragColor;
in vec2 fragTexCoord;

in float intensity;
uniform sampler2D image;


float offsets[5] = float[](0.0, 1.3846153846, 3.2307692308, 5.91548518, 9.2354548);
uniform float weight[5] = float[] (0.2270270270, 0.3945945946, 0.1216216216, 0.94548, 0.456678);

void main()
{
	vec2 texelSize = 1.0 / textureSize(image, 0);
	vec3 result = texture(image, fragTexCoord).rgb * weight[0]; //contribution of current fragment
	
	for(int i = 1; i < 5; ++i)
	{
		float w = weight[i]; 		
		float offset = offsets[i] * (fragTexCoord.y < 0.7 ? 0.0 : intensity);
		result += texture(image, fragTexCoord + vec2(texelSize.x * offset, 0.0)).rgb * w;
		result += texture(image, fragTexCoord - vec2(texelSize.x * offset, 0.0)).rgb * w;
		result += texture(image, fragTexCoord + vec2(0.0, texelSize.y * offset * 2.0)).rgb * w;
		result += texture(image, fragTexCoord - vec2(0.0, texelSize.y * offset * 2.0)).rgb * w;
	}
	
	fragColor = vec4(result, 1.0);
}

