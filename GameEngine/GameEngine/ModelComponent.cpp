//#############################################################################################
/*
Copyright[2020][Gabriel G. Fernandes]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.

by Gabriel G. Fernandes 19/12/2020
gabrielgf6000@gmail.com
*/
//#############################################################################################

#include "ModelComponent.h"





inline glm::vec3 calcTangent(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
	glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) noexcept
{
	//compute the edges and delta uvs
	glm::vec3 edge1 = p2 - p1;
	glm::vec3 edge2 = p3 - p1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	FLOAT_TYPE f = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

	//compute tangent
	glm::vec3 tangent(0.0f);
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	
	return tangent;
}



//###################################################



glm::mat4 aiMatToglmMat(const aiMatrix4x4& m) noexcept
{
	glm::mat4 mat;
	
	mat[0][0] = m.a1; mat[1][0] = m.a2; mat[2][0] = m.a3; mat[3][0] = m.a4;
	mat[0][1] = m.b1; mat[1][1] = m.b2; mat[2][1] = m.b3; mat[3][1] = m.b4;
	mat[0][2] = m.c1; mat[1][2] = m.c2; mat[2][2] = m.c3; mat[3][2] = m.c4;
	mat[0][3] = m.d1; mat[1][3] = m.d2; mat[2][3] = m.d3; mat[3][3] = m.d4;
	
	
	return mat;
}

//###################################################


//###################################################################################################


void NodeData::initialize(const aiNode* node)
{
	name = node->mName.data;
	transformation = aiMatToglmMat(node->mTransformation);

	//fill the children 
	children.resize(node->mNumChildren);
	for (int i = 0; i < node->mNumChildren; ++i)
		children[i].initialize(node->mChildren[i]);

}


//###################################################################################################

glm::quat AnimNodeData::getInterpolatedRotation(FLOAT_TYPE animTime) const
{
	if (rotationKeys.size() == 0)
		throw std::logic_error("ERRROR::TRYING TO INTERPOLATE FROM AN ANIMNODE WITH NO KEYS;\n");

	if (rotationKeys.size() == 1) //it is need more than one keys to interpolate
	{
		return rotationKeys[0].second;
	}

	//get the first key:
	int index = -1;
	//std::cout << rotationKeys.size() << '\n';
	for (int i = 0; i < rotationKeys.size() - 1; ++i) //get the first key with a time higher than animTime
	{
		//std::cout << rotationKeys[i + 1].first << '\n';
		if (rotationKeys[i + 1].first > animTime)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		throw std::logic_error("ERROR::CANNOT FIND A VALID KEY TO INTERPOLATE;\n");

	//set the second key:
	int index2 = index + 1;

	FLOAT_TYPE deltaTime = rotationKeys[index2].first - rotationKeys[index].first;
	FLOAT_TYPE factor = (animTime - rotationKeys[index].first) / deltaTime;


	//std::cout << "->" << animTime << ", " << rotationKeys[index].first << ", " << deltaTime << '\n';
	myAssert(factor >= 0.0f && factor <= 1.0f);
	//return the interpolated rotation quaternion
	return glm::slerp(rotationKeys[index].second, rotationKeys[index2].second, factor);
}



//##################################################



glm::vec3 AnimNodeData::getInterpolatedTranslation(FLOAT_TYPE animTime) const
{
	if (positionKeys.size() == 0)
		throw std::logic_error("ERRROR::TRYING TO INTERPOLATE FROM AN ANIMNODE WITH NO KEYS;\n");

	if (positionKeys.size() == 1) //it is need more than one keys to interpolate
	{
		return positionKeys[0].second;
	}

	//get the first key:
	int index = -1;
	//std::cout << positionKeys.size() << '\n';
	for (int i = 0; i < positionKeys.size() - 1; ++i) //get the first key with a time higher than animTime
	{
		//std::cout << positionKeys[i + 1].first << '\n';
		if (positionKeys[i + 1].first > animTime)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		throw std::logic_error("ERROR::CANNOT FIND A VALID KEY TO INTERPOLATE;\n");

	//set the second key:
	int index2 = index + 1;

	FLOAT_TYPE deltaTime = positionKeys[index2].first - positionKeys[index].first;
	FLOAT_TYPE factor = (animTime - positionKeys[index].first) / deltaTime;

	//std::cout << "->" << animTime << ", " << positionKeys[index].first << ", " << deltaTime << '\n';
	myAssert(factor >= 0.0f && factor <= 1.0f);
	glm::vec3 delta = positionKeys[index2].second - positionKeys[index].second;
	//return the interpolated position vector
	return positionKeys[index].second + (delta * factor);
}



//##################################################



glm::vec3 AnimNodeData::getInterpolatedScaling(FLOAT_TYPE animTime) const
{
	if (scalingKeys.size() == 0)
		throw std::logic_error("ERRROR::TRYING TO INTERPOLATE FROM AN ANIMNODE WITH NO KEYS;\n");

	if (scalingKeys.size() == 1) //it is need more than one keys to interpolate
	{
		return scalingKeys[0].second;
	}

	//get the first key:
	int index = -1;
	//std::cout << scalingKeys.size() << '\n';
	for (int i = 0; i < scalingKeys.size() - 1; ++i) //get the first key with a time higher than animTime
	{
		if (scalingKeys[i + 1].first > animTime)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		throw std::logic_error("ERROR::CANNOT FIND A VALID KEY TO INTERPOLATE;\n");

	//set the second key:
	int index2 = index + 1;

	FLOAT_TYPE deltaTime = scalingKeys[index2].first - scalingKeys[index].first;
	FLOAT_TYPE factor = (animTime - scalingKeys[index].first) / deltaTime;

	//std::cout << "->" << animTime  << ", " << scalingKeys[index].first << ", " << scalingKeys[index2].first << '\n';
	myAssert(factor >= 0.0f && factor <= 1.0f);
	glm::vec3 delta = scalingKeys[index2].second - scalingKeys[index].second;

	//return the interpolated scaling vector
	return scalingKeys[index].second + (delta * factor);
}


//###################################################################################################


void SceneData::initialize(const aiScene* scene, bool glbFileType)
{
	myAssert(scene);

	//initialize the nodes:
	if (!scene->mRootNode)
		throw std::logic_error("ERROR::ASSIMP DOES NOT SUCCESSFULLY LOADED THE MODEL, IT DOES NOT HAVE A ROOT NODE\n");
	rootNode.initialize(scene->mRootNode);

	//initialize the animations:
	animations.resize(scene->mNumAnimations);
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		//set duration and ticks per second:
		animations[i].duration = scene->mAnimations[i]->mDuration / 
			(glbFileType ? (1000.0f * (1 / scene->mAnimations[i]->mTicksPerSecond)) : 1.0f); //convert 
												//the animation time to seconds glbFileType is true
		animations[i].ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;

		std::cout << "\nAnimation: \n\tDur: " << animations[i].duration << "\tTicksPerSec: " 
			<< animations[i].ticksPerSecond << '\n';

		std::cout << "\tChannels: " << scene->mAnimations[i]->mNumChannels << '\n';

		//now set the channels:
		for (int j = 0; j < scene->mAnimations[i]->mNumChannels; ++j)
		{
			AnimNodeData data;
			const aiNodeAnim* nAnim = scene->mAnimations[i]->mChannels[j];
			data.name = nAnim->mNodeName.data;
			
			std::cout << "\t\tChannel " << j << " Size: " << nAnim->mNumPositionKeys << ", " <<
				nAnim->mNumRotationKeys << ", " << nAnim->mNumScalingKeys << '\n';

			
			//set position keys
			data.positionKeys.resize(nAnim->mNumPositionKeys);
			for (int k = 0; k < nAnim->mNumPositionKeys; ++k)
			{
				data.positionKeys[k].first = (FLOAT_TYPE)nAnim->mPositionKeys[k].mTime / 
					(glbFileType ? (1000.0f * (1 / scene->mAnimations[i]->mTicksPerSecond) ) : 1.0f);

				data.positionKeys[k].second.x = nAnim->mPositionKeys[k].mValue.x;
				data.positionKeys[k].second.y = nAnim->mPositionKeys[k].mValue.y;
				data.positionKeys[k].second.z = nAnim->mPositionKeys[k].mValue.z;
			}

			//set rotation keys
			data.rotationKeys.resize(nAnim->mNumRotationKeys);
			for (int k = 0; k < nAnim->mNumRotationKeys; ++k)
			{
				data.rotationKeys[k].first = (FLOAT_TYPE)nAnim->mRotationKeys[k].mTime 
					/ (glbFileType ? (1000.0f * (1 / scene->mAnimations[i]->mTicksPerSecond)) : 1.0f);

				data.rotationKeys[k].second.w = nAnim->mRotationKeys[k].mValue.w;
				data.rotationKeys[k].second.x = nAnim->mRotationKeys[k].mValue.x;
				data.rotationKeys[k].second.y = nAnim->mRotationKeys[k].mValue.y;
				data.rotationKeys[k].second.z = nAnim->mRotationKeys[k].mValue.z;
			}

			//set scaling keys

			std::cout << "\nScalings for Chanel " << j << ": ";
			data.scalingKeys.resize(nAnim->mNumScalingKeys);
			for (int k = 0; k < nAnim->mNumScalingKeys; ++k)
			{
				data.scalingKeys[k].first = (FLOAT_TYPE)nAnim->mScalingKeys[k].mTime / 
					(glbFileType ? (1000.0f * (1 / scene->mAnimations[i]->mTicksPerSecond)) : 1.0f);
				//std::cout << (FLOAT_TYPE)nAnim->mScalingKeys[k].mTime << " ";

				data.scalingKeys[k].second.x = nAnim->mScalingKeys[k].mValue.x;
				data.scalingKeys[k].second.y = nAnim->mScalingKeys[k].mValue.y;
				data.scalingKeys[k].second.z = nAnim->mScalingKeys[k].mValue.z;
			}
			//std::cout << '\n';
			animations[i].channels.insert(std::pair<std::string, AnimNodeData>(data.name , data));
		}
	}
	
}



//####################################################################################################
//Model definitions:


void Model::loadFromFile(const std::string& filename, int nRows, int nColumns, bool glbFileType)
{
	Assimp::Importer m_importer;

	int importFlags = aiProcess_Triangulate |
		aiProcess_ValidateDataStructure |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_SortByPType |
		aiProcess_FlipUVs;
		//aiProcess_PreTransformVertices |

		
	const aiScene* m_scene = m_importer.ReadFile(filename.c_str(), importFlags);
	//m_scene = 
		

	//test for errors:
	if (!m_scene)
		throw std::logic_error("ERROR::CANNOT LOAD MODEL FROM FILE IN Mesh::loadFromFile(); File: " +
			filename + "; Error String: " + m_importer.GetErrorString() + "\n");

	if(m_scene->mNumMeshes > 0)
	{ 
		for (int i = 0; i < m_scene->mNumMeshes; ++i)
		{
			std::cout << "Bones: " << m_scene->mMeshes[i]->mNumBones << '\t';
			std::cout << "Vertices: " << m_scene->mMeshes[i]->mNumVertices << '\n';
			//myAssert(m_scene->mMeshes[i]->mNumBones == 0);
		}
		std::cout << "Number of Animations: " << m_scene->mNumAnimations << '\n';

		//if (scene->mMeshes[0]->mNumBones > 0) myAssert(false);
	}

	sceneData.initialize(m_scene, glbFileType); //save the needed data of the scene, since the aiScene will be destroied
								//after exiting this function. This data is needed for animations and skinning

	m_globalInverseTransform = aiMatToglmMat(m_scene->mRootNode->mTransformation);
	m_globalInverseTransform = glm::inverse(m_globalInverseTransform);
	
	//----------------------
	initFromScene(m_scene, filename);
	

	mMaterial.animations = nRows;
	
	//sceneData.animations[0].duration /= sceneData.animations[0].ticksPerSecond;
	mMaterial.framesPerAnimation = nColumns;

	std::cout << "->" << !glbFileType << '\n';

	//if (glbFileType == false)
	//	return;

	//.glb files use the animation duration as milliseconds, so it is needed to convert 
	//it to seconds(by dividing by 1000)
	//for (int i = 0; i < sceneData.animations.size(); ++i)
	//	sceneData.animations[i].duration /= 1000.0f;

	//std::cout << "->" << sceneData.animations[0].duration << '\n';
}


//#############################################################



void Model::initFromScene(const aiScene* scene, const std::string& filename)
{
	mEntries.resize(scene->mNumMeshes);
	std::cout << "Number of Materials: " << scene->mNumMaterials << '\n';
	myAssert(scene->mNumMaterials >= 1);
	//mBones.resize(scene->);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	int numOfVertices = 0;
	int numOfIndices = 0;

	//initialize meshes:
	for (int i = 0; i < mEntries.size(); ++i)
	{
		mEntries[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		mEntries[i].numOfIndices = scene->mMeshes[i]->mNumFaces * 3;
		mEntries[i].baseVertex = numOfVertices;
		mEntries[i].baseIndex = numOfIndices;

		numOfVertices += scene->mMeshes[i]->mNumVertices;
		numOfIndices += mEntries[i].numOfIndices;

		
	}

	vertices.reserve(numOfVertices);
	indices.reserve(numOfIndices);


	for (int i = 0; i < mEntries.size(); ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		initMesh(i, mesh, vertices, indices);
	}
	myAssert(vertices.size() == numOfVertices && indices.size() == numOfIndices);

	numOfIndices = indices.size(); //!


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//create and configure the vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(float) * 0)); //position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(float) * 3)); //normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(float) * 6)); //uv coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(float) * 8)); //tangent
	glEnableVertexAttribArray(3);


	myAssert(sizeof(Vertex) == 19 * 4);
	if (numOfBones > 0)
	{

		//configure bones ids
		glVertexAttribIPointer(4, MAX_NUM_OF_BONES, GL_INT, sizeof(Vertex), (void*)(sizeof(float) * 11));
		glEnableVertexAttribArray(4);

		//configure bones weights
		glVertexAttribPointer(5, MAX_NUM_OF_BONES, GL_FLOAT, false, sizeof(Vertex),
			(void*)((sizeof(float) * 11) + (sizeof(unsigned int) * MAX_NUM_OF_BONES)));
		glEnableVertexAttribArray(5);
	}

	//create the index buffer
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	initMaterials(scene, filename);
}


//#########################################################


void Model::clearMemory()
{
	for (int i = 0; i < mEntries.size(); ++i)
		mEntries[i].clearMemory();
	
	if (mMaterial.hasTexture) 
	{
		mMaterial.albedoTexture.clearMemory(); //clear albedo texture
		mMaterial.hasTexture = false;
	}
	if (mMaterial.hasNormalMap) //clear normal map texture
	{
		mMaterial.normalMapTexture.clearMemory();
		mMaterial.hasNormalMap = false;
	}
	if (mMaterial.hasEmissionMap)
	{
		mMaterial.emissionMapTexture.clearMemory(); //clear emission map texture
		mMaterial.hasEmissionMap = false;
	}
	if (mMaterial.hasMetallicMap)
	{
		mMaterial.albedoTexture.clearMemory(); //clear metallic map texture
		mMaterial.hasMetallicMap = false;
	}
	if (mMaterial.hasRoughnessMap)
	{
		mMaterial.roughnessMapTexture.clearMemory(); //clear roughness map texture
		mMaterial.hasRoughnessMap = false;
	}
	if (mMaterial.hasAlphaMap)
	{
		mMaterial.alphaMapTexture.clearMemory(); //clear alpha map texture
		mMaterial.hasAlphaMap = false;
	}
		


	if (VAO != 0)
		glDeleteBuffers(1, &VAO);
	if (VBO != 0)
		glDeleteBuffers(1, &VBO);
	if (EBO != 0)
		glDeleteBuffers(1, &EBO);

}



//#########################################################


void Model::MeshEntry::clearMemory()
{

}



//#########################################################


const Material* Model::getMaterial() const noexcept
{
	return &mMaterial;
}


//#########################################################


void Model::MeshEntry::init(const std::vector<Vertex>& vertices, 
							const std::vector<unsigned int>& indices,
							bool hasBones)
{
	
}



//#########################################################

void Model::initMesh(unsigned int id, const aiMesh* mesh, std::vector<Vertex>& vertices,
	std::vector<unsigned int>& indices)
{
	aiVector3D zeroVec(0.0f, 0.0f, 0.0f);

	bool result = true;

	

	//fill the vertices vector
	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		const aiVector3D* position = &(mesh->mVertices[i]);

		const aiVector3D* normal = &(mesh->mNormals[i]);

		const aiVector3D* tangent = &zeroVec;
		if (mesh->HasTangentsAndBitangents()) 
			tangent = &mesh->mTangents[i];  //set the tangent.
		else 
			result = false; //if it does not have a tangent, throw, because assimp failed to generate them

		const aiVector3D* texCoord = &zeroVec;
		if (mesh->HasTextureCoords(0))
			texCoord = &(mesh->mTextureCoords[0][i]);
		else
			myAssert(false);

		BoneDataPerVertex bone{};

		//push a vertex in the vertices vector
		Vertex v = { Vec3D{FLOAT_TYPE(position->x), FLOAT_TYPE(position->y), FLOAT_TYPE(position->z)},
			Vec3D{FLOAT_TYPE(normal->x), FLOAT_TYPE(normal->y), FLOAT_TYPE(normal->z)},
			Vec2D{FLOAT_TYPE(texCoord->x), FLOAT_TYPE(texCoord->y)},
			Vec3D{FLOAT_TYPE(tangent->x), FLOAT_TYPE(tangent->y), FLOAT_TYPE(tangent->z)},
			bone };

		vertices.push_back(v);
	}

	if (!result)
		throw std::logic_error("ERROR::MODEL DOES NOT HAVE TEXTURE COORDINATES OR TANGENT VECTORS;\n");


	//fill the indices vector
	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace* face = &mesh->mFaces[i];
		if (face->mNumIndices != 3)
			throw std::logic_error("ERROR::CANNOT LOAD MODEL; IT IS NOT TRIANGULATED;\n");
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	if (mesh->HasBones())
	{
		initBones(id, mesh, vertices);

		//copy the bones data to the vertices vector
		//for (int i = 0; i < vertices.size(); ++i)
			//vertices[i].boneData = bones[i];
	}

	//mEntries[id].init(vertices, indices, mesh->HasBones());
}

//########################################################


void Model::initMaterials(const aiScene* scene, const std::string& filename)
{
	std::string path = "";
	int slashIndex = filename.find_last_of("/");
	path = filename.substr(0, slashIndex + 1);

	//initialize the material
	const aiMaterial* pMat = scene->mMaterials[0];
	Material material;

	int count = scene->mNumTextures;
		
	std::cout << "Num of Textures: " << count << "\n{";
		
	int count2 = 0;
	for (int j = 0; j < 18; ++j)
	{
		count2 += pMat->GetTextureCount(aiTextureType(j));
		if (pMat->GetTextureCount(aiTextureType(j)) > 0) std::cout << j << " ";
	}
	std::cout << "}\nNum of Textures2: " << count2 << '\n';
		
	aiString diffMapPath;
	aiString normalMapPath;
	aiString emissionMapPath;
	aiString metallicMapPath;
	aiString roughnessMapPath;
	aiString alphaMapPath;

	/*
	//------------------------------------
	//load the diffuse texture
	if (pMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		if (pMat->GetTexture(aiTextureType_DIFFUSE, 0, &diffMapPath, nullptr, nullptr, nullptr,
			nullptr, nullptr) != AI_SUCCESS)
			throw std::logic_error("ERROR::FAILED TO ACCESS THE MODEL DIFFUSE TEXTURE;\n");

		material.albedoTexture = Texture(path + diffMapPath.data, false);
		material.hasTexture = true;
	}
	//else throw std::logic_error("ERROR::LOADED TEXTURE DOES NOT CONTAIN DIFFUSE TEXTURE;\n");

	std::cout << diffMapPath.data << "\n";

		
	//------------------------------------
	//load the normal map texture
	if (pMat->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		if (pMat->GetTexture(aiTextureType_NORMALS, 0, &normalMapPath, nullptr, nullptr, nullptr,
			nullptr, nullptr) != AI_SUCCESS)
			throw std::logic_error("ERROR::FAILED TO ACCESS THE MODEL NORMAL MAP TEXTURE;\n");

		material.normalMapTexture = Texture(path + normalMapPath.data, false);
		material.hasNormalMap = true;

		std::cout << "Normal Map: " << normalMapPath.data << "\n";
	}
		
		
	//------------------------------------
	//load the emission map texture
	if (pMat->GetTextureCount(aiTextureType_EMISSIVE) > 0) 
	{
		if (pMat->GetTexture(aiTextureType_EMISSIVE, 0, &emissionMapPath, nullptr, nullptr, nullptr,
			nullptr, nullptr) != AI_SUCCESS)
			throw std::logic_error("ERROR::FAILED TO ACCESS THE MODEL EMISSION MAP TEXTURE;\n");

		material.emissionMapTexture = Texture(path + emissionMapPath.data, false);
		material.hasEmissionMap = true;

		std::cout << "Emission Map: " << emissionMapPath.data << "\n";
	}
	//else throw std::logic_error("ERROR::LOADED TEXTURE DOES NOT CONTAIN EMISSION MAP TEXTURE;\n");

		
		
	//------------------------------------
	//load the roughness map texture

	if (pMat->GetTextureCount(aiTextureType_SHININESS) > 0)
	{
		if (pMat->GetTexture(aiTextureType_SHININESS, 0, &roughnessMapPath, nullptr, nullptr, nullptr,
			nullptr, nullptr) != AI_SUCCESS)
			throw std::logic_error("ERROR::FAILED TO ACCESS THE MODEL ROUGHNESS MAP TEXTURE;\n");

		material.roughnessMapTexture = Texture(path + roughnessMapPath.data, false);
		material.hasRoughnessMap = true;


		std::cout << "Roughness Map: " << roughnessMapPath.data << "\n";
	}
		

	//-----------------------------------
	//load the alpha map texture
	if (pMat->GetTextureCount(aiTextureType_OPACITY) > 0)
	{
		if (pMat->GetTexture(aiTextureType_OPACITY, 0, &alphaMapPath, nullptr, nullptr, nullptr,
			nullptr, nullptr) != AI_SUCCESS)
			throw std::logic_error("ERROR::FAILED TO ACCESS THE MODEL ALPHA MAP TEXTURE;\n");

		material.alphaMapTexture = Texture(path + alphaMapPath.data, false);
		material.hasAlphaMap = true;

		std::cout << "Alpha Map: " << alphaMapPath.data << '\n';
	}


	*/
	//-----------------------------------
	//load the textures(collada does not support materials, so these textures will be
	//in the same file as the .dae file. It is then needed to load each of these textures. Each
	//texture type should have it's type in its name, for example, a normal map should have
	//"nmap" in its name. Note that due to this, each model can contain just one material)

	for(const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string texturePath = entry.path().string();
			
		//std::cout << "Texture: " << texturePath << '\n';
		if (texturePath.find("dmap") != std::string::npos) //if it is a normal map
		{
			std::cout << "Diffuse Map: " << texturePath << '\n';
			material.albedoTexture = Texture(texturePath, false);
			material.hasTexture = true;
		}
		if (texturePath.find("nmap") != std::string::npos) //if it is a normal map
		{
			std::cout << "Normal Map: " << texturePath << '\n';
			material.normalMapTexture = Texture(texturePath, false);
			material.hasNormalMap = true;
		}
		else if (texturePath.find("mmap") != std::string::npos) //if it is a metallic map
		{
			std::cout << "Metallic Map: " << texturePath << '\n';
			material.metallicMapTexture = Texture(texturePath, false);
			material.hasMetallicMap = true;
		}
		else if (texturePath.find("rmap") != std::string::npos) //if it is a roughness map
		{
			std::cout << "Roughness Map: " << texturePath << '\n';
			material.roughnessMapTexture = Texture(texturePath, false);
			material.hasRoughnessMap = true;
		}
		else if (texturePath.find("emap") != std::string::npos) //if it is a emission map
		{
			std::cout << "Emission Map: " << texturePath << '\n';
			material.emissionMapTexture = Texture(texturePath, false);
			material.hasEmissionMap = true;
		}
		else if (texturePath.find("amap") != std::string::npos) //if it is a alpha map
		{
			std::cout << "Alpha Map: " << texturePath << '\n';
			material.alphaMapTexture = Texture(texturePath, false);
			material.hasAlphaMap = true;
		}
	}
		
	
	mMaterial = material;
	
}


//####################################################


void Model::BoneDataPerVertex::pushBoneData(unsigned int id, FLOAT_TYPE w)
{
	for (int i = 0; i < MAX_NUM_OF_BONES; ++i)
	{
		if (weights[i] == 0.0f) //try to find an empty slot for the bone data
		{
			boneIds[i] = id;
			weights[i] = w;
			return;
		}
	}

	throw std::logic_error("ERROR::MORE BONES BEING ASSIGNED TO A VERTEX THAN THE ALLOWED AMOUNT;\n");
}




//####################################################

void Model::initBones(unsigned int meshIndex, const aiMesh* mesh, std::vector<Vertex>& bones)
{

	for (int i = 0; i < mesh->mNumBones; ++i)
	{
		unsigned int bIndex = 0;
		const std::string bName = mesh->mBones[i]->mName.data;

		if (mBoneMapping.find(bName) == mBoneMapping.end()) //add new bone data
		{
			bIndex = numOfBones;
			++numOfBones;
			BoneData bData;
			mBoneData.push_back(bData);
			mBoneData[bIndex].offset = aiMatToglmMat(mesh->mBones[i]->mOffsetMatrix);
			mBoneMapping[bName] = bIndex;
		}
		else //there already have a mapping for this bone name
		{
			bIndex = mBoneMapping[bName];
		}

		for (int j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			unsigned int vertexIndex = mEntries[meshIndex].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			FLOAT_TYPE weight = mesh->mBones[i]->mWeights[j].mWeight;
			
			bones[vertexIndex].boneData.pushBoneData(bIndex, weight);
		}


	}

}


//####################################################################################################
//ModelComponent definitions


void ModelComponent::boneTransform(int animId, FLOAT_TYPE timeInSecs, std::vector<glm::mat4>& transforms)
{
	myAssert(model);

	if (animId < 0 || animId >= model->sceneData.animations.size())
		throw std::logic_error("ERRROR::INVALID ANIMATION ID PASSED TO boneTransform();\n");

	FLOAT_TYPE ticksPerSec = model->sceneData.animations[animId].ticksPerSecond;
	if (ticksPerSec == 0.0f)
		ticksPerSec = 25.0f;

	FLOAT_TYPE timeInTicks = timeInSecs * ticksPerSec;
	FLOAT_TYPE animTime = std::fmod(timeInTicks, model->sceneData.animations[animId].duration);

	readNodeHierachy(animId, animTime, model->sceneData.rootNode, glm::mat4(1.0f));

	//*transforms = &mBoneData; //transforms now points to a pointer that points to a vector containing all bone transformations

	//set the transformation matrix for each bone
	//transforms.resize(mBoneTransforms.size());
	//for (int i = 0; i < mBoneTransforms.size(); ++i)
	//{
	//	transforms[i] = mBoneTransforms[i];
	//}
}


//#########################################################


void ModelComponent::readNodeHierachy(int animId, FLOAT_TYPE animTime, const NodeData& node, const glm::mat4& parentTransform)
{

	auto animNode = model->sceneData.animations[animId].channels.find(node.name);

	glm::mat4 nodeTransform = node.transformation;
	if (animNode != model->sceneData.animations[animId].channels.end())
	{
		//get the interpolated transformation matrix
		glm::mat4 scaling = glm::mat3(1.0f);
		glm::vec3 scalingVec = animNode->second.getInterpolatedScaling(animTime);
		scaling[0][0] = scalingVec.x;
		scaling[1][1] = scalingVec.y;
		scaling[2][2] = scalingVec.z;
		//scaling = glm::scale(glm::identity<glm::mat4>(), scalingVec);
	

		glm::vec3 translationVec = animNode->second.getInterpolatedTranslation(animTime);
		//glm::mat4 translation = glm::identity<glm::mat4>();
		//translation[3] = glm::vec4(translationVec, 1.0f);

		//glm::mat4 rotAndScale = glm::mat4(scaling * rotation);
		//rotAndScale[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		nodeTransform =
			glm::translate(glm::mat4(1.0f), translationVec) *
			glm::scale(glm::mat4(1.0f), scalingVec) *
			glm::toMat4(glm::normalize(animNode->second.getInterpolatedRotation(animTime)));
			;
			
			 //rotAndScale * translation;
				
		//nodeTransform = glm::identity<glm::mat4>();
		//nodeTransform = toMat4(animNode->second.getInterpolatedRotation(animTime))
		//* scaling;
		//nodeTransform[3] = glm::vec4(translationVec, 1.0f);
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	//configure bones
	if (model->mBoneMapping.find(node.name) != model->mBoneMapping.end())
	{
		unsigned int boneIndex = model->mBoneMapping.at(node.name);
		//std::cout << boneIndex << '\n';
		//set the bone full transform:
		mBoneTransforms[boneIndex] =
			model->m_globalInverseTransform *
			globalTransform * 
			model->mBoneData[boneIndex].offset;

	
	}

	for (int i = 0; i < node.children.size(); ++i)
		readNodeHierachy(animId, animTime, node.children[i], globalTransform);

}



ModelComponent::ModelComponent()
	:Component{-1}
{

}


//########################################################


ModelComponent::ModelComponent(Entity id)
	:Component{ id }
{

}



//#########################################################


void ModelComponent::update() //should be called each frame
{
	if (playing > 0 && model)
	{
		//std::cout << "HAAAA " << playing << "\n";
		++frameCounter;
		if (frameCounter >= playing)
		{
			currentColumn = (currentColumn + 1) % model->mMaterial.framesPerAnimation;
			frameCounter = 0;
		}
	}
}

//#########################################################


void ModelComponent::play(int dur, int row)
{
	myAssert(model);

	if ((model->mMaterial.animations < 2 && model->mMaterial.framesPerAnimation < 2) || 
		(model->mMaterial.animations < row) || row < 0) myAssert(false); //the sprite isn't a spritesheet
	playing = dur;
	if (currentRow != row) { playing = 0; frameCounter = 0; currentColumn = 0; } //reset current frame if the animation was changed
	currentRow = row;
}

//#########################################################


void ModelComponent::stop(bool reset)
{
	playing = 0;
}


//#########################################################


void ModelComponent::setModel(const Model* m)
{
	if (m)
	{
		model = m;
		for(int i = 0; i < model->mBoneData.size(); ++i)
			mBoneTransforms.push_back(model->mBoneData[i].fullTransform);
	}
	else
	{
		model = nullptr;
		std::cout << "WARNING::NULL PTR PASSED AS ARGUMENT TO setModel() FUNCTION;\n";
	}
}


//#######################################################


void ModelComponent::setPosition(glm::vec3 p)
{
	pos = p;
}



//########################################################


glm::vec3 ModelComponent::getPosition()
{
	return pos;
}


//######################################################


void ModelComponent::draw() const
{
	
}

