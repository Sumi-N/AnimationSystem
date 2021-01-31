#version 420 core
layout (location = 0) in vec3 model_position1;
layout (location = 1) in int model_index;

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
     gl_Position = model_view_perspective_matrix * global_inversed_matrix[model_index] * vec4(model_position1, 1);
	 //gl_Position = model_view_perspective_matrix * model_index *vec4(model_position1, 1);
}
