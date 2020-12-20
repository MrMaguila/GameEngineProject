#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 faceMatrices[6]; //a matrix for each face of the cubemap

out vec4 fragPos;  //position in world space, output per vertex

out vec2 fragTexCoord; //output per vertex

in VS_OUT {
	vec2 texCoordinates;
} gs_in[];

void main()
{
	
	for(int face = 0; face < 6; face++) //for each cubemap face
	{
		gl_Layer = face;
		for(int i = 0; i < 3; i++) //for each triangle vertex
		{
			fragPos = gl_in[i].gl_Position;
			fragTexCoord = gs_in[i].texCoordinates;
			gl_Position = faceMatrices[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}