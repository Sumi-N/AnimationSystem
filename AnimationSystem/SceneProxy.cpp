#pragma once
#include "SceneProxy.h"


void SceneProxy::Draw()
{
	glBindVertexArray(vertexarrayid);
	glDrawElements(static_cast<unsigned int>(drawtype), indexsize, GL_UNSIGNED_INT, (void*)0);

}

// This function will only call draw element without bind any shader.
// If shader is already binded, it will better call this.
void SceneProxy::DrawMeshOnly()
{
	glBindVertexArray(vertexarrayid);
	glDrawElements(static_cast<unsigned int>(drawtype), indexsize, GL_UNSIGNED_INT, (void*)0);
}

void SceneProxy::InitBuffer()
{
	// Create vertex array 
	glGenVertexArrays(1, &vertexarrayid);
	glBindVertexArray(vertexarrayid);

	// Create vertex buffer 
	glGenBuffers(1, &vertexbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferid);

	// Create index buffer
	glGenBuffers(1, &indexbufferid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbufferid);
}

void SceneProxy::InitMeshData(std::vector<MeshData> mesh, std::vector<int> index)
{
	// Set vertex data to vertex buffer, index data to index buffer
	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(mesh[0]), mesh.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), index.data(), GL_STATIC_DRAW);

	// Enable vertex attribute
	// From 0: vertex, 1: normal, 2: uv coordinate, 3: tangent, 4: bitangent
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(2 * sizeof(glm::vec3)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(2 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(3 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2)));

	// Memorize index size for Draw() fucntion
	indexsize = static_cast<unsigned int>(index.size()) * sizeof(index[0]);
}

void SceneProxy::CleanUpBuffer()
{
	// Unbind the vertex array
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &vertexarrayid);
	glDeleteBuffers(1, &vertexbufferid);
	glDeleteBuffers(1, &indexbufferid);
}
