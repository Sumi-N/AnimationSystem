#pragma once
#include <fbxsdk.h>
#include <glm/matrix.hpp>
#include "SceneProxy.h"

class Importer
{
public:
	bool Init(const char*);
	void PrintData();
	bool CleanUp();

public:

	bool ImportMaterial(FbxSurfaceMaterial*, MaterialData&);

	void ProcessJointsAndAnimations(FbxMesh*, Skeleton);
	FbxAMatrix GetGeometryTransformation(FbxNode*);

	int FindJointIndexUsingName(const char*, Skeleton);
	glm::mat4 convertFBXMatrix(FbxAMatrix);

public:

	bool ImportMeshData(std::vector<MeshData>&, std::vector<int>&);
	bool ImportSkeletonMeshData(Skeleton&);
	bool ImportMaterialData(MaterialData&);
	bool ImportAnimationData();
	bool ImportJointData();

public:
	static FbxManager* lSdkManager;
	static FbxScene* lScene;
	static FbxNode* lRootNode;

private:

	// For debug purpose
	void PrintNode(FbxNode* pNode);
	void ImportNode(FbxNode* pNode);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintTabs();

	// For Skeleton
	void ProcessSkeletonHierarchyRecursively(FbxNode*, int, int, int, Skeleton&);
};

