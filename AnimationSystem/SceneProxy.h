#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <gl/glew.h>
#include <vector>

__declspec(align(16)) struct MeshData
{
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec2 padding; //For alignment purpose
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct MaterialData
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 emissive;
	glm::vec3 specular;
	float shininess;
};

enum class TextureType : int8_t
{
	SkyBox = -1,
	Ohter = -1,

	//////////////////////

	Albedo = 0,
	Normal = 1,
	Roughness = 2,
	Metalic = 3,
	AmbientOcclusion = 4,

	//////////////////////

	PB_Diffuse = 0,
	PB_Specular = 1,
	PB_Normal = 2,
	PB_Displacement = 3,
};

struct Joint
{
	glm::mat4   inversed; // inversed bind pose translation matrix
	glm::vec3   coord;
	const char* name;
	int         parent_index;
};

struct BlendingWeight
{
	int index;
	float weight;
};

struct Skeleton
{
	std::vector<Joint>   joints;
};

struct JointPose
{
	glm::quat   rot;
	glm::vec4   trans;
	float       scale;
};

struct SkeletonPose
{
	Skeleton*  skeletons;
	JointPose* local_pose;
	glm::mat4      global_pose;
};

///////////////////////////////////////////////////////////////////

enum class DrawType : unsigned int
{
	LINE = GL_LINE,
	TRIANGLE = GL_TRIANGLES,
	PATCHES = GL_PATCHES,
};

class SceneProxy
{

public:

	//void AddRenderState(OwningPointer<RenderState>);
	//void ReplaceRenderState(OwningPointer<RenderState>, int);
	void Draw();
	void DrawPoint();
	void DrawLine();
	void DrawMeshOnly();
	//void CleanUp();

	//OwningPointer<MeshComponent> mesh;
	//std::vector<OwningPointer<RenderState>> states;

public:

	DrawType originaltype = DrawType::TRIANGLE;
	DrawType drawtype = DrawType::TRIANGLE;

	void InitBuffer();
	void InitMeshData(std::vector<MeshData> mesh, std::vector<int> index);
	void InitSkeletonData(Skeleton skeleton, std::vector<int> index);
	//void CheckDrawType(Shader i_shader);
	void SetDrawType(DrawType i_drawtype)
	{
		drawtype = i_drawtype;
	};
	void CleanUpBuffer();

	// Buffer data
	GLuint vertexarrayid = 0;
	GLuint vertexbufferid = 0;
	GLuint indexbufferid = 0;
	unsigned int indexsize = 0;

	// Texture data
	std::vector<GLuint> textureids;
	std::vector<GLuint> textureunits;
};