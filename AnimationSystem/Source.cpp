#include <fbxsdk.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Macro.h"
#include "Shader.h"
#include "ConstantBuffer.h" 
#include "SceneProxy.h"

#define PI 3.14159265

GLFWwindow * glfwwindow;

float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

glm::vec3 GetCameraRotation(float angle, glm::vec3 camera_pos, glm::vec3 model_pos)
{
	glm::vec3 change =  glm::vec3((camera_pos - model_pos).length() * sinf(angle * PI / 180), 0, (camera_pos - model_pos).length() * cosf(angle * PI / 180));

	return camera_pos + change;
}

int main()
{
	std::vector<int> index{ 0, 1, 2, 3, 4, 5};
	MeshData d1;
	MeshData d2;
	MeshData d3;
	MeshData d4;
	MeshData d5;
	MeshData d6;

	d1.vertex = glm::vec3(0.5f, 0.5f, 0.0f);
	d2.vertex = glm::vec3(0.5f, -0.5f, 0.0f);
	d3.vertex = glm::vec3(-0.5f, 0.5f, 0.0f);
	d4.vertex = glm::vec3(0.5f, -0.5f, 0.0f);
	d5.vertex = glm::vec3(-0.5f, -0.5f, 0.0f);
	d6.vertex = glm::vec3(-0.5f, 0.5f, 0.0f);

	std::vector<MeshData> mesh;

	mesh.push_back(d1);
	mesh.push_back(d2);
	mesh.push_back(d3);
	mesh.push_back(d4);
	mesh.push_back(d5);
	mesh.push_back(d6);

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
	//Shader* shader = new Shader();
	//shader->SetShader("../Shaders/empty.vert.glsl", "../Shaders/empty.frag.glsl");
	//shader->LoadShader();

	Shader* shader = new Shader();
	shader->SetShader("../Shaders/debug_polygon.vert.glsl", "../Shaders/debug_polygon.geo.glsl","../Shaders/debug_polygon.frag.glsl");
	shader->LoadShader();

	//////////////////////////////////////////////////////////////
	
	// Create Buffer

	SceneProxy proxy;
	proxy.InitBuffer();
	proxy.InitMeshData(mesh, index);


	//////////////////////////////////////////////////////////////

	// Constant Buffer
	ConstantBuffer buffer;
	buffer.Init(ConstantData::Index::Model, ConstantData::Size::Model);

	// create transformations
	glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	ConstantData::Model constant_model;

	float angle = 0;
	

	//////////////////////////////////////////////////////////////

	while (glfwWindowShouldClose(glfwwindow) == GL_FALSE)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (angle <= 360)
		{
			angle += 0.5f;
		}
		else
		{
			angle = 0;
		}

		// Submit constant data
		// Calculate camera matrix
		glm::vec3 current_camera_pos = GetCameraRotation(angle, glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(0, 0, -10));
		view = glm::lookAt(current_camera_pos, glm::vec3(0, 0, -10), glm::vec3(0, 1.0, 0));

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, -10));
		float angle = 20.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		// Calculate MVP matrix and submit to constant buffer
		constant_model.model_view_perspective_matrix = projection * view * model;
		buffer.Update(&constant_model);

		// draw our first triangle
		shader->BindShader();
		proxy.Draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(glfwwindow);
		glfwPollEvents();
	}

	return 0;
}