#version 420 core

layout (location = 0) in vec3 model_position;
layout (location = 1) in vec3 model_normal;
 layout (location = 3) in vec3 model_tangent;
layout (location = 5) in ivec4 index;
layout (location = 6) in vec4  weight;

// Consta data
const int   MAX_BONE_NUM = 256;

layout (std140, binding = 1) uniform const_drawcall
{
	mat4 model_position_matrix;
	mat4 model_view_perspective_matrix;
	mat4 model_inverse_transpose_matrix;
};

layout (std140, binding = 6) uniform const_animation_skeleton
{
	mat4 global_inversed_matrix[MAX_BONE_NUM];
};

void main()
{
	gl_Position = model_view_perspective_matrix * vec4(model_position, 1);
}