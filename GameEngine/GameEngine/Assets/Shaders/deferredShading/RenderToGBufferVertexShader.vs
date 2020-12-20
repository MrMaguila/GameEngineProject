#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;


uniform mat4 model;
uniform mat4 viewAndProj;
uniform mat3 modelInverse;


const int NUM_OF_BONES = 50;

uniform mat4 boneTransforms[NUM_OF_BONES];

uniform bool useBones = false;


out vec3 fragPos; //fragment position in world space
out vec3 fragNormal;
out vec2 texCoordinates;

out mat3 TBNmatrix;

void main()
{
	mat4 boneTransform = mat4(1.0);
	if(useBones == true && boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w != 0.0)
	{
		boneTransform = boneTransforms[boneIds[0]] * boneWeights[0];
		//if(boneWeights[1] != 0.0) 
			boneTransform += boneTransforms[boneIds[1]] * boneWeights[1];
		//if(boneWeights.z != 0.0) 
			boneTransform += boneTransforms[boneIds[2]] * boneWeights[2];
		//if(boneWeights.w != 0.0) 
			boneTransform += boneTransforms[boneIds[3]] * boneWeights[3];
	}

	//fragPos = vec3(model * vec4(pos, 1.0));
	//gl_Position = viewAndProj * vec4(fragPos, 1.0);
	vec4 fragPos2 = boneTransform * vec4(pos, 1.0);
	gl_Position =  (viewAndProj * model) * vec4(fragPos2);
	fragPos = (model * fragPos2).xyz;

	//mat3 boneTransformMat3 = mat3(transpose(inverse(boneTransform)));

	vec4 fragNormal2 = boneTransform * vec4(normal, 0.0);
	fragNormal = normalize(modelInverse * fragNormal2.xyz);
	//fragNormal = normalize(model * vec4(normal, 0.0));

	texCoordinates = texCoord;
	vec4 T2 = boneTransform * vec4(tangent, 0.0);
	vec3 T = normalize(modelInverse * T2.xyz);
	//vec3 T = normalize((modelInverse) * tangent);
	vec3 N = fragNormal;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	TBNmatrix = mat3(T, B, N);
}