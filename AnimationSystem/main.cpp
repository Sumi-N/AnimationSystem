#include <fbxsdk.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Macro.h"
#include "Shader.h"
#include "ConstantBuffer.h" 
#include "SceneProxy.h"
#include "Importer.h"

#define PI 3.14159265

int FrameRate = 120;	

GLFWwindow * glfwwindow;


void ConvertJointPoseBySkeleton(AnimationClip& clip, Skeleton skeleton)
{
	int diff;
	bool finished = false;

	for (int i = 0; i < clip.samples.size(); i++)
	{
		diff = 0;
		for (int j = 0; j < skeleton.joints.size(); j++)
		{
			// For now, it's hard coded. 
			if (j > 20)
			{
				clip.samples[i].jointposes[j].global_inverse_matrix = clip.samples[i].jointposes[j].global_inverse_matrix * skeleton.joints[j].inversed;
				continue;
			}
			if (finished)
			{
				clip.samples[i].jointposes[j].parent_index += diff;
			}

			if (skeleton.joints[j].parent_index != clip.samples[i].jointposes[j].parent_index)
			{
				JointPose empty;
				//empty.global_inverse_matrix = skeleton.joints[j].inversed;
				empty.global_inverse_matrix = glm::mat4(1.0);
				empty.parent_index = skeleton.joints[j].parent_index;
				clip.samples[i].jointposes.insert(clip.samples[i].jointposes.begin() + j, empty);
				diff++;
				finished = false;
			}
			else
			{
				clip.samples[i].jointposes[j].global_inverse_matrix = clip.samples[i].jointposes[j].global_inverse_matrix * skeleton.joints[j].inversed;
				finished = true;
			}
		}
	}
}

void InterpolateMatrixInAFrame(AnimationClip clip, int frame, glm::mat4* matrixs)
{
	int clip_frame_count = clip.frame_count;
	float frame_per_count = (float)(FrameRate) / clip_frame_count;

	int current_frame = 0;
	while (frame_per_count * current_frame + frame_per_count < frame)
	{
		current_frame++;
	}

	if (current_frame >= clip_frame_count)
	{
		current_frame = 0;
		frame = 0;
	}

	if (current_frame >= clip_frame_count - 1)
	{
		float t = (current_frame + 1) * frame_per_count - (float)frame;
		t /= frame_per_count;

		for (int i = 0; i < clip.samples[0].jointposes.size(); i++)
		{
			glm::vec4 pointA = clip.samples[current_frame].jointposes[i].trans;
			glm::vec4 pointB = clip.samples[0].jointposes[i].trans;

			glm::vec4 result_translation = t * pointA + (1 - t) * pointB;

			//glm::quat result_rotation = glm::normalize(glm::slerp(clip.samples[current_frame].jointposes[i].rot, clip.samples[0].jointposes[i].rot, t));
			glm::quat result_rotation = glm::normalize(t* clip.samples[current_frame].jointposes[i].rot + (1 - t) * clip.samples[0].jointposes[i].rot);

			glm::mat4 RotationMatrix = glm::toMat4(result_rotation);
			glm::mat4 answer = glm::translate(glm::mat4(1.0), glm::vec3(result_translation)) * RotationMatrix;
			matrixs[i] = answer;
		}
	}
	else
	{
		float t = (current_frame + 1) * frame_per_count - (float)frame;
		t /= frame_per_count;

		for (int i = 0; i < clip.samples[0].jointposes.size(); i++)
		{
			glm::vec4 pointA = clip.samples[current_frame].jointposes[i].trans;
			glm::vec4 pointB = clip.samples[current_frame + 1].jointposes[i].trans;

			glm::vec4 result_translation = t * pointA + (1 - t) * pointB;

			//glm::quat result_rotation = glm::slerp(clip.samples[current_frame].jointposes[i].rot, clip.samples[current_frame + 1].jointposes[i].rot, t);
			glm::quat result_rotation =  glm::normalize(t* clip.samples[current_frame].jointposes[i].rot + (1 - t) * clip.samples[current_frame + 1].jointposes[i].rot);

			glm::mat4 RotationMatrix = glm::toMat4(result_rotation);
			glm::mat4 answer = glm::translate(glm::mat4(1.0), glm::vec3(result_translation)) * RotationMatrix;
			matrixs[i] = answer;
		}
	}
}

glm::vec3 GetCameraRotation(float angle, glm::vec3 camera_pos, glm::vec3 model_pos)
{
	float r = glm::length(camera_pos - model_pos);
	glm::vec3 change =  glm::vec3(r * cosf(angle * PI / 180), 0, r * sinf(angle * PI / 180));

	return change + model_pos;
}

int main()
{
	std::vector<int> index;
	std::vector<MeshData> mesh;
	Skeleton this_skeleton;
	AnimationClip this_clip;
	this_clip.pSkeleton = &this_skeleton;

	Importer fbx;

	fbx.Init("../models/SK_PlayerCharacter.fbx");
	//fbx.Init("../models/SK_Enemy_Bird.fbx");
	//fbx.PrintData();
	fbx.ImportSkeletonMeshData(this_skeleton);
	fbx.ImportMeshData(mesh, index, this_skeleton);
	fbx.CleanUp();

	fbx.Init("../models/Anim_PlayerCharacter_run.fbx");
	//fbx.Init("../models/Anim_PlayerCharacter_falling.fbx");
	//fbx.Init("../models/Anim_PlayerCharacter_swim.fbx");
	//fbx.Init("../models/Anim_Enemy_Bird_attack_down.fbx");
	//fbx.PrintData();
	fbx.ImportAnimationData(this_clip);
	fbx.CleanUp();

	ConvertJointPoseBySkeleton(this_clip, this_skeleton);

	if (glfwInit() == GL_FALSE)
	{
		DEBUG_PRINT("Cannot initialize GLFW");
		return 0;
	}

	atexit(glfwTerminate);

	//Select OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creating a window
	glfwwindow = glfwCreateWindow(1920, 1080, "window", NULL, NULL);
	if (!glfwwindow)
	{
		// If the window is not created
		DEBUG_PRINT("Cannot create GLFW window");
		glfwTerminate();
		return 0;
	}

	// Attached the OpenGL to this window
	glfwMakeContextCurrent(glfwwindow);

	// Set V-Sync
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		DEBUG_PRINT("Cannot initialize GLEW");
		return 0;
	}

	//glfwSetCursorPosCallback(window, cursorPositionCallback);
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//////////////////////////////////////////////////////////////

	// CreateShader
	Shader* skeletonanimationshader = new Shader();
	skeletonanimationshader->SetShader("../Shaders/skeleton_animation.vert.glsl", "../Shaders/skeleton_animation.frag.glsl");
	skeletonanimationshader->LoadShader();

	Shader* skeletonshader = new Shader();
	skeletonshader->SetShader("../Shaders/empty.vert.glsl", "../Shaders/empty.frag.glsl");
	skeletonshader->LoadShader();

	Shader* shader = new Shader();
	shader->SetShader("../Shaders/debug_polygon.vert.glsl", "../Shaders/debug_polygon.geo.glsl","../Shaders/debug_polygon.frag.glsl");
	shader->LoadShader();

	Shader* animationshader = new Shader();
	animationshader->SetShader("../Shaders/debug_animation.vert.glsl", "../Shaders/debug_animation.geo.glsl", "../Shaders/debug_animation.frag.glsl");
	animationshader->LoadShader();

	//////////////////////////////////////////////////////////////
	
	// Create mesh

	SceneProxy proxy;
	proxy.InitBuffer();
	proxy.InitMeshData(mesh, index);

	// Create skeleton
	SceneProxy skeleton_proxy;
	skeleton_proxy.InitBuffer();
	std::vector<int> skeleton_index;
	skeleton_proxy.InitSkeletonData(this_skeleton, skeleton_index);

	// Create skeleton animation
	SceneProxy skeleton_animation_proxy;
	skeleton_animation_proxy.InitBuffer();
	std::vector<int> skeleton_index2;
	skeleton_animation_proxy.InitSkeletonAnimationData(this_skeleton, this_clip, skeleton_index2);


	//////////////////////////////////////////////////////////////

	// Constant Buffer
	ConstantBuffer buffer;
	buffer.Init(ConstantData::Index::Model, ConstantData::Size::Model);

	ConstantBuffer buffer2;
	buffer2.Init(ConstantData::Index::Skeleton, ConstantData::Size::Skeleton);

	ConstantData::Skeleton animation_inversed_matrix;

	// create transformations
	glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
	glm::vec3 camera_position = glm::vec3(0.0, 0.0f, 0.0f);
	view = glm::translate(view, camera_position);

	ConstantData::Model constant_model;

	float angle = 0;
	glm::vec3 obj_position = glm::vec3(0.0, -50.0f, -300.0f);

	int animation_sample_count = 0;

	glm::mat4 interpolated_matrix[256];
	

	//////////////////////////////////////////////////////////////

	while (glfwWindowShouldClose(glfwwindow) == GL_FALSE)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (angle <= 360)
			angle += 0.3f;
		else
			angle = 0;

		if (animation_sample_count < FrameRate)
		{
			animation_sample_count++;
		}
		else
		{
			animation_sample_count = 0;
		}

		//Submit constant data
		//Calculate camera matrix
		glm::vec3 current_camera_pos = GetCameraRotation(angle, camera_position, obj_position);
		view = glm::lookAt(current_camera_pos, glm::vec3(obj_position.x, 0, obj_position.z), glm::vec3(0, 1.0, 0));

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, obj_position);
		//model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(0, 1.0, 0));
		//model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));

		// Calculate MVP matrix and submit to constant buffer
		constant_model.model_view_perspective_matrix = projection * view * model;
		buffer.Update(&constant_model);

		// draw mesh 
		shader->BindShader();
		//proxy.Draw();

		//// draw skeleton
		skeletonshader->BindShader();
		//skeleton_proxy.DrawLine();



		// Calculate skeleton's matrix
		InterpolateMatrixInAFrame(this_clip, animation_sample_count, interpolated_matrix);
		if (!this_clip.samples.empty())
		{
			int fixed_frame = (int)(animation_sample_count / ((float)FrameRate / animation_sample_count));
			if (fixed_frame >= 14)
				fixed_frame = 0;

			for (int i = 0; i < this_clip.samples[0].jointposes.size(); i++)
			{
				//animation_inversed_matrix.global_inversed_matrix[i] = this_clip.samples[fixed_frame].jointposes[i].global_inverse_matrix * this_skeleton.joints[i].inversed;
				animation_inversed_matrix.global_inversed_matrix[i] = interpolated_matrix[i] * this_skeleton.joints[i].inversed;
			}
			buffer2.Update(&animation_inversed_matrix);
		}

		// draw animation
		animationshader->BindShader();
		proxy.Draw();

		// draw skeleton animation 
		//skeletonanimationshader->BindShader();
		//skeleton_animation_proxy.DrawLine();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(glfwwindow);
		glfwPollEvents();
	}

	return 0;
}