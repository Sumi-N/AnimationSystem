#pragma once
#include "SceneProxy.h"


void SceneProxy::Draw()
{
	glBindVertexArray(vertexarrayid);
	glDrawElements(static_cast<unsigned int>(drawtype), indexsize, GL_UNSIGNED_INT, (void*)0);

}

void SceneProxy::DrawPoint()
{
	glBindVertexArray(vertexarrayid);
	glDrawElements(GL_POINTS, indexsize, GL_UNSIGNED_INT, (void*)0);
	
}

void SceneProxy::DrawLine()
{
	glBindVertexArray(vertexarrayid);
	glDrawElements(GL_LINES, indexsize, GL_UNSIGNED_INT, (void*)0);
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
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(2 * sizeof(glm::vec3)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(2 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(3 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2)));
	glVertexAttribPointer(5, 4, GL_INT,   GL_FALSE, sizeof(mesh[0]), (void*)(4 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mesh[0]), (void*)(4 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2) + sizeof(glm::ivec4)));

	// Memorize index size for Draw() fucntion
	indexsize = static_cast<unsigned int>(index.size()) * sizeof(index[0]);
}

void SceneProxy::InitSkeletonData(Skeleton skeleton, std::vector<int> index)
{
	std::vector<glm::vec3> skeleton_vector;

	int index_count = 0;
	for (int i = 0; i < skeleton.joints.size(); i++)
	{
		if (skeleton.joints[i].parent_index == -1)
		{
			continue;
		}
		else
		{
			skeleton_vector.push_back(skeleton.joints[i].coord);
			skeleton_vector.push_back(skeleton.joints[skeleton.joints[i].parent_index].coord);

			index.push_back(index_count);
			index.push_back(index_count + 1);
			index_count += 2;
		}
	}

	glBufferData(GL_ARRAY_BUFFER, skeleton_vector.size() * sizeof(skeleton_vector[0]), skeleton_vector.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), index.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));

	indexsize = static_cast<unsigned int>(index.size()) * sizeof(index[0]);
}

void SceneProxy::InitSkeletonAnimationData(Skeleton skeleton, AnimationClip clip, std::vector<int> index)
{
	std::vector<AnimationSkeleton> animation_skeleton_vector;

	int index_count = 0;
	for (int i = 0; i < skeleton.joints.size(); i++)
	{
		if (skeleton.joints[i].parent_index == -1)
		{
			continue;
		}
		else
		{
			AnimationSkeleton anim_ske;
			anim_ske.index = i;
			anim_ske.pos = glm::vec3(0, 0, 0);

			AnimationSkeleton anim_ske_parent;
			anim_ske_parent.index = skeleton.joints[i].parent_index;
			anim_ske_parent.pos = glm::vec3(0, 0, 0);

			animation_skeleton_vector.push_back(anim_ske);
			animation_skeleton_vector.push_back(anim_ske_parent);

			index.push_back(index_count);
			index.push_back(index_count + 1);
			index_count += 2;
		}
	}

	// Set vertex data to vertex buffer, index data to index buffer
	glBufferData(GL_ARRAY_BUFFER, animation_skeleton_vector.size() * sizeof(animation_skeleton_vector[0]), animation_skeleton_vector.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), index.data(), GL_STATIC_DRAW);

	// Enable vertex attribute
	// From 0: vertex, 1: normal, 2: uv coordinate, 3: tangent, 4: bitangent
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(animation_skeleton_vector[0]), (void*)(0));
	glVertexAttribIPointer(1, 1, GL_INT, sizeof(animation_skeleton_vector[0]), (void*)(sizeof(glm::vec3)));

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
