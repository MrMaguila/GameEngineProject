#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 texCoord;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;

uniform mat4 pvm;

const int NUM_OF_BONES = 50;

uniform mat4 boneTransforms[NUM_OF_BONES];

uniform bool useBones = false;

out VS_OUT
{
	vec2 texCoordinates;
} vs_out;


//Render the scene from light point of view in the Depth buffer
void main()
{
	mat4 pvm2 = pvm;

	if(useBones == true && boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w != 0.0)
	{
		mat4 boneTransform = boneTransforms[boneIds.x] * boneWeights.x;
		//if(boneWeights.y != 0.0) 
			boneTransform += boneTransforms[boneIds.y] * boneWeights.y;
		//if(boneWeights.z != 0.0) 
			boneTransform += boneTransforms[boneIds.z] * boneWeights.z;
		//if(boneWeights.w != 0.0) 
			boneTransform += boneTransforms[boneIds.w] * boneWeights.w;
	
		pvm2 = pvm * boneTransform;
	}
	
	vs_out.texCoordinates = texCoord;
	gl_Position = pvm2 * vec4(pos, 1.0);
}