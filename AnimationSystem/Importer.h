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
	FbxAMatrix GetGeometryTransformation(FbxNode*);

public:

	bool ImportMeshData(std::vector<MeshData>&, std::vector<int>&);
	bool ImportSkeletonMeshData(Skeleton&);
	bool ImportMaterialData(MaterialData&);
	bool ImportAnimationData(AnimationClip&);
	bool ImportAnimationSample(AnimationSample&, FbxTime);
	bool ImportSkinData(std::vector<MeshData>&, Skeleton);

public:
	static FbxManager* lSdkManager;
	static FbxScene* lScene;
	static FbxNode* lRootNode;

private:

	// For debug purpose
	void PrintNode(FbxNode*);
	void ImportNode(FbxNode*);
	void PrintAttribute(FbxNodeAttribute*);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType);
	void PrintTabs();

	// Recursive function
	void ProcessSkeletonHierarchyRecursively(FbxNode*, int, int, int, Skeleton&);
	void ProcessAnimationSampleRecursively(FbxNode*, AnimationSample&, FbxTime);

	// Find joint 
	int FindJointIndexUsingName(const char*, Skeleton);

	// Convert between joinstpose and joint
	void ConvertJointPoseBySkeleton(AnimationSample&);
};

