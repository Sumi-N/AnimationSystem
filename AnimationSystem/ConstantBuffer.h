#pragma once
#include <gl/glew.h>
#include <glm.hpp>
#include <matrix.hpp>

namespace ConstantData
{
	struct Model
	{
		glm::mat4 model_position_matrix;
		glm::mat4 model_view_perspective_matrix;
		glm::mat4 model_inverse_transpose_matrix;
	};

	enum class Index : uint8_t
	{
		Camera = 0,
		Model = 1,
		Material = 2,
		Light = 3,
		SkyBox = 4,
		CubeMap = 5,
	};

	enum class Size : uint16_t
	{
		Model = sizeof(ConstantData::Model),
		//Camera = sizeof(ConstantData::Camera),
		//Material = sizeof(ConstantData::Material),
		//Light = sizeof(ConstantData::Light),
		//SkyBox = sizeof(ConstantData::SkyBox),
		//CubeMap = sizeof(ConstantData::CubeMap),
	};
}

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	GLuint bufferid = 0;
	GLuint index = 0;
	GLsizeiptr size = 0;

	void Init(ConstantData::Index, ConstantData::Size);
	void Update(const void* const);
};