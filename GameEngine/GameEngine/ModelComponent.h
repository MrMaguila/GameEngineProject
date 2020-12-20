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


This code is part of a self made game engine. It contains the declarations of the 
of the Model, ModelHandler and ModelComponent classes, which are used to store models loaded from file. It
supports only .glb file format.
*/
//#############################################################################################



#ifndef MODEL_COMPONENT
#define MODEL_COMPONENT

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>
#include <exception>
#include <cassert>
#include <map>
#include <filesystem>

#include "Texture.h"
#include "Entity.h"

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GlobalDefines.h"


#define MAX_NUM_OF_BONES 4


glm::vec3 calcTangent(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
												glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) noexcept;


glm::mat4 aiMatToglmMat(const aiMatrix4x4& m) noexcept;


//############################################################################################



struct AnimNodeData
{
	glm::quat getInterpolatedRotation(FLOAT_TYPE animTime) const;
	glm::vec3 getInterpolatedTranslation(FLOAT_TYPE animTime) const;
	glm::vec3 getInterpolatedScaling(FLOAT_TYPE animTime) const;


	std::string name;
	std::vector<std::pair<FLOAT_TYPE, glm::vec3>> positionKeys;

	std::vector<std::pair<FLOAT_TYPE, glm::quat>> rotationKeys;

	std::vector<std::pair<FLOAT_TYPE, glm::vec3>> scalingKeys;
};


struct AnimationData
{
	FLOAT_TYPE ticksPerSecond;
	FLOAT_TYPE duration;
	std::map<std::string, AnimNodeData> channels;
};

struct NodeData
{
	void initialize(const aiNode* node);

	std::string name;
	glm::mat4 transformation;
	std::vector<NodeData> children;
};


struct SceneData
{
	void initialize(const aiScene*, bool glbFileType);

	std::vector<AnimationData> animations;
	NodeData rootNode;
};

//############################################################################################



struct Material
{
	FLOAT_TYPE Ns = 0.0f; //specular exponent
	glm::vec3 Ka = glm::vec3(0.0f); //ambient color
	glm::vec3 Kd = glm::vec3(0.0f); //diffuse color
	glm::vec3 Ks = glm::vec3(0.0f); //specular color
	glm::vec3 Ke = glm::vec3(0.0f); //emission color
	FLOAT_TYPE Ni = 0.0f; //refraction coeficient
	FLOAT_TYPE d = 0.0f; //opacity

	//animation data(the material textures can be sprite sheets):
	int animations = 1; //number of rows
	int framesPerAnimation = 1; //number of columns


	bool hasTexture = false;
	Texture albedoTexture;

	bool hasNormalMap = false;
	Texture normalMapTexture;

	bool hasEmissionMap = false;
	Texture emissionMapTexture;

	bool hasRoughnessMap = false;
	Texture roughnessMapTexture;

	bool hasMetallicMap = false;
	Texture metallicMapTexture;

	bool hasAlphaMap = false;
	Texture alphaMapTexture;
};


//#############################################################################################


class Model
{
public:

	Model() {};
	~Model() {};


	void loadFromFile(const std::string& filename, int nRows, int nColumns, bool glbFileType);
	void clearMemory();
	const Material* getMaterial() const noexcept;
	


private:	

	friend class ModelComponent;
	friend class InteractableObjectComponent;
	friend class CharacterComponent;
	friend class GraphicalSystem;

	struct BoneDataPerVertex
	{
		unsigned int boneIds[MAX_NUM_OF_BONES] = { 0, 0, 0, 0 };
		FLOAT_TYPE weights[MAX_NUM_OF_BONES] = { 0.0f, 0.0f, 0.0f, 0.0f };

		void pushBoneData(unsigned int id, FLOAT_TYPE w);
	};

	struct BoneData
	{
		glm::mat4 offset = glm::mat4(1.0);
		glm::mat4 fullTransform = glm::mat4(1.0);
	};

	struct Vec3D
	{
		FLOAT_TYPE x;
		FLOAT_TYPE y;
		FLOAT_TYPE z;
	};

	struct Vec2D
	{
		FLOAT_TYPE x;
		FLOAT_TYPE y;
	};

	struct Vertex
	{
		Vec3D position;
		Vec3D normal;
		Vec2D texCoord;
		Vec3D tangent;
		BoneDataPerVertex boneData;
	};

	



	void initFromScene(const aiScene* scene, const std::string& filename);
	void initMesh(unsigned int index, const aiMesh* mesh, std::vector<Vertex>& vertices, 
		std::vector<unsigned int>& indices);
	void initMaterials(const aiScene* scene, const std::string& filename);
	void initBones(unsigned int meshIndex, const aiMesh* mesh, std::vector<Vertex>& bones);
	
	

	struct MeshEntry
	{
		inline MeshEntry() {};
		inline ~MeshEntry() {};

		void init(const std::vector<Vertex>& vertices, 
				  const std::vector<unsigned int>& indices,
				 bool hasBones);

		void clearMemory();

		unsigned int numOfIndices = 0;
		unsigned int materialIndex = 0;
		unsigned int baseVertex = 0;
		unsigned int baseIndex = 0;
	};

	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int VAO = 0;

	std::vector<MeshEntry> mEntries;
	Material mMaterial;
	std::vector<BoneData> mBoneData;
	int numOfBones = 0;
	std::map<std::string, unsigned int> mBoneMapping;
	std::vector<BoneDataPerVertex> mBones;
	
	SceneData sceneData;
	glm::mat4 m_globalInverseTransform;
};



//##################################################################################################

class ModelComponent : public Component
{
public:

	ModelComponent();
	void update(); //this should be called each frame

	void setModel(const Model*);
	void setPosition(glm::vec3);
	glm::vec3 getPosition();
	void draw() const;
	void play(int dur, int row); //turn on playing, set duration of each frame to dur gameloop frames, and change the current row
	void stop(bool reset); //stop playing. if reset is true, the current column will be set to 0
	//void bindForDraw() const;

	bool cullFace = true;

private:

	friend class GraphicalSystem;

	//constructor:
	ModelComponent(Entity);

	//skinning data:
	void boneTransform(int animId, FLOAT_TYPE timeInSecs, std::vector<glm::mat4>& transforms);
	void readNodeHierachy(int animId, FLOAT_TYPE animTime, const NodeData& node, const glm::mat4& parentTransform);
	std::vector<glm::mat4> mBoneTransforms;

	//texture animation data:
	int currentRow = 0;
	int currentColumn = 0;
	int playing = 0;
	int frameCounter = 0;
	

	//main data:
	glm::vec3 pos = glm::vec3(0.0f); //this is used to keep the model from some distance of its Entity center
	const Model* model = nullptr;
	
};







#endif // !MODEL_COMPONENT