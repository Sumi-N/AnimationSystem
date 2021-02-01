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

	if(index.y == -1){

		gl_Position = model_view_perspective_matrix * global_inversed_matrix[index.x] *  vec4(model_position, 1);

	}else if(index.z == -1){

		gl_Position = model_view_perspective_matrix * (weight.x * global_inversed_matrix[index.x] *  vec4(model_position, 1)
		                                             + weight.y * global_inversed_matrix[index.y] *  vec4(model_position, 1));

	}else if(index.w == -1){

		gl_Position = model_view_perspective_matrix * (weight.x * global_inversed_matrix[index.x] *  vec4(model_position, 1)
		                                             + weight.y * global_inversed_matrix[index.y] *  vec4(model_position, 1)
													 + weight.z * global_inversed_matrix[index.z] *  vec4(model_position, 1));		

	}else{

		gl_Position = model_view_perspective_matrix * (weight.x * global_inversed_matrix[index.x] *  vec4(model_position, 1)
		                                             + weight.y * global_inversed_matrix[index.y] *  vec4(model_position, 1)
													 + weight.z * global_inversed_matrix[index.z] *  vec4(model_position, 1)
													 + weight.w * global_inversed_matrix[index.w] *  vec4(model_position, 1));

	}

	//gl_Position = model_view_perspective_matrix * vec4(model_position, 1);
}