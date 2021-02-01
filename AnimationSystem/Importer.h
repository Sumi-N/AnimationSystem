#pragma once
#include <fbxsdk.h>
#include <glm/matrix.hpp>
#include "SceneProxy.h"

#include <map>

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

	bool ImportMeshData(std::vector<MeshData>&, std::vector<int>&, Skeleton);
	bool ImportSkeletonMeshData(Skeleton&);
	bool ImportMaterialData(MaterialData&);
	bool ImportAnimationData(AnimationClip&);
	bool ImportAnimationSample(AnimationSample&, FbxTime);
	bool ImportSkinData(Skeleton);

public:
	static FbxManager* lSdkManager;
	static FbxScene* lScene;
	static FbxNode* lRootNode;
	static std::multimap<int, BlendingWeight> WeightMap;

private:

	// For debug purpose
	void PrintNode(FbxNode*);
	void ImportNode(FbxNode*);
	void PrintAttribute(FbxNodeAttribute*);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType);
	void PrintTabs();

	// Recursive function
	void ProcessSkeletonHierarchyRecursively(FbxNode*, int, int, int, Skeleton&);
	void ProcessAnimationSampleRecursively(FbxNode*, int, int, int, AnimationSample&, FbxTime);

	// Find joint 
	int FindJointIndexUsingName(std::string, Skeleton);
};

