#pragma once
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
	void DrawMeshOnly();
	//void CleanUp();

	//OwningPointer<MeshComponent> mesh;
	//std::vector<OwningPointer<RenderState>> states;

public:

	DrawType originaltype = DrawType::TRIANGLE;
	DrawType drawtype = DrawType::TRIANGLE;

	void InitBuffer();
	void InitMeshData(std::vector<MeshData> mesh, std::vector<int> index);
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