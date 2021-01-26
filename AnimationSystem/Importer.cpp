#include "Importer.h"
#include <cmath>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#define FBXSDK_SHARED

FbxManager* Importer::lSdkManager = nullptr;
FbxScene* Importer::lScene  = nullptr;
FbxNode* Importer::lRootNode = nullptr;

bool Importer::Init(const char * i_filepath)
{
	// Change the following filename to a suitable filename value.
	const char* lFilename = i_filepath;

	// Initialize the SDK manager. This object handles memory management.
	lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	Importer::lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(Importer::lScene);

	// The file is imported, so get rid of the importer.
	lImporter->Destroy();

	
	Importer::lRootNode = Importer::lScene->GetRootNode();

	FbxGeometryConverter geometryConverter(lSdkManager);
	geometryConverter.Triangulate(lScene, true);
	//geometryConverter.SplitMeshesPerMaterial(Importer::lScene, true);

	return true;
}

void Importer::PrintData()
{
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	if (Importer::lRootNode)
	{
		for (int i = 0; i < Importer::lRootNode->GetChildCount(); i++)
			PrintNode(Importer::lRootNode->GetChild(i));
	}
}

bool Importer::CleanUp()
{
	// Destroy the SDK manager and all the other objects it was handling.
	lSdkManager->Destroy();

	return true;
}

bool Importer::ImportMeshData(std::vector<MeshData>& mesh, std::vector<int>& index)
{
	//Get mesh in the scene
	int meshCount = Importer::lScene->GetSrcObjectCount<FbxMesh>();

	for (int i = 0; i < meshCount; ++i)
	{
		FbxMesh* pMesh = Importer::lScene->GetSrcObject<FbxMesh>(i);

		printf("The pmesh name is %s\n", pMesh->GetName());
		printf("The polygon count is %d\n", pMesh->GetPolygonCount());

		FbxNode* pNode = pMesh->GetNode();

		glm::mat4 model_matrix = glm::mat4(1.0);

		// Get translation rotation scale
		glm::vec3 translation((float)pNode->LclTranslation.Get().mData[0], (float)pNode->LclTranslation.Get().mData[1], (float)pNode->LclTranslation.Get().mData[2]);
		glm::vec3 rotation((float)pNode->LclRotation.Get().mData[0], (float)pNode->LclRotation.Get().mData[1], (float)pNode->LclRotation.Get().mData[2]);
		glm::vec3 scaling((float)pNode->LclScaling.Get().mData[0], (float)pNode->LclScaling.Get().mData[1], (float)pNode->LclScaling.Get().mData[2]);

		// Make a model matrix
		glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 rotation_mat = glm::eulerAngleZYX(rotation.x * 3.141592 / 180, rotation.y * 3.141592 / 180, rotation.z * 3.141592 / 180);
		glm::mat4 scaling_mat = glm::scale(glm::mat4(1.0f), scaling);
		model_matrix = translation_mat * rotation_mat * scaling_mat * model_matrix;
		glm::mat4 model_inverse_transpose_matrix = glm::transpose(glm::inverse(model_matrix));

		// Get index array
		int* index_array = pMesh->GetPolygonVertices();

		// Get vertex array
		FbxVector4* vertex_array = pMesh->GetControlPoints();

		// Get normal array
		FbxArray< FbxVector4 > normal_array;
		pMesh->GetPolygonVertexNormals(normal_array);

		// Get UV layer names
		FbxStringList uvsetName;
		pMesh->GetUVSetNames(uvsetName);

		// Current index count
		int n = 0 + (int)index.size();

		// The reason we can assume each polygon has 3 vertices is because we called triangulate function before
		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			index.push_back(n + 3 * j + 0);
			index.push_back(n + 3 * j + 1);
			index.push_back(n + 3 * j + 2);

			MeshData p1, p2, p3;

			p1.vertex.x = (float)vertex_array[index_array[3 * j + 0]].mData[0];
			p1.vertex.y = (float)vertex_array[index_array[3 * j + 0]].mData[1];
			p1.vertex.z = (float)vertex_array[index_array[3 * j + 0]].mData[2];

			p2.vertex.x = (float)vertex_array[index_array[3 * j + 1]].mData[0];
			p2.vertex.y = (float)vertex_array[index_array[3 * j + 1]].mData[1];
			p2.vertex.z = (float)vertex_array[index_array[3 * j + 1]].mData[2];

			p3.vertex.x = (float)vertex_array[index_array[3 * j + 2]].mData[0];
			p3.vertex.y = (float)vertex_array[index_array[3 * j + 2]].mData[1];
			p3.vertex.z = (float)vertex_array[index_array[3 * j + 2]].mData[2];

			p1.normal.x = (float)normal_array[3 * j + 0].mData[0];
			p1.normal.y = (float)normal_array[3 * j + 0].mData[1];
			p1.normal.z = (float)normal_array[3 * j + 0].mData[2];

			p2.normal.x = (float)normal_array[3 * j + 1].mData[0];
			p2.normal.y = (float)normal_array[3 * j + 1].mData[1];
			p2.normal.z = (float)normal_array[3 * j + 1].mData[2];

			p3.normal.x = (float)normal_array[3 * j + 2].mData[0];
			p3.normal.y = (float)normal_array[3 * j + 2].mData[1];
			p3.normal.z = (float)normal_array[3 * j + 2].mData[2];

			p1.vertex = model_matrix * glm::vec4(p1.vertex.x, p1.vertex.y, p1.vertex.z, 1.0);
			p2.vertex = model_matrix * glm::vec4(p2.vertex.x, p2.vertex.y, p2.vertex.z, 1.0);
			p3.vertex = model_matrix * glm::vec4(p3.vertex.x, p3.vertex.y, p3.vertex.z, 1.0);

			p1.normal = model_inverse_transpose_matrix * glm::vec4(p1.normal.x, p1.normal.y, p1.normal.z, 1.0);
			p2.normal = model_inverse_transpose_matrix * glm::vec4(p2.normal.x, p2.normal.y, p2.normal.z, 1.0);
			p3.normal = model_inverse_transpose_matrix * glm::vec4(p3.normal.x, p3.normal.y, p3.normal.z, 1.0);

			// Get the first UV sets
			if (uvsetName.GetCount() > 0)
			{
				FbxVector2 uv1, uv2, uv3;
				bool flag1, flag2, flag3;

				pMesh->GetPolygonVertexUV(j, 0, uvsetName.GetStringAt(0), uv1, flag1);
				pMesh->GetPolygonVertexUV(j, 1, uvsetName.GetStringAt(0), uv2, flag2);
				pMesh->GetPolygonVertexUV(j, 2, uvsetName.GetStringAt(0), uv3, flag3);

				p1.uv.x = (float)uv1.mData[0];
				p1.uv.y = (float)uv1.mData[1];

				p2.uv.x = (float)uv2.mData[0];
				p2.uv.y = (float)uv2.mData[1];

				p2.uv.x = (float)uv3.mData[0];
				p2.uv.y = (float)uv3.mData[1];
			}

			mesh.push_back(p1);
			mesh.push_back(p2);
			mesh.push_back(p3);
		}
		
	}

	return 0;
}

bool Importer::ImportSkeletonMeshData(Skeleton& skeleton)
{
	for (int childIndex = 0; childIndex < lRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = lRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1, skeleton);
	}

	return true;
}

bool Importer::ImportAnimationData()
{
	// Get animation information
	// Now only supports one take
	FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(0);
	FbxString animStackName = currAnimStack->GetName();
	auto mAnimationName = animStackName.Buffer();
	FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	auto mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

	Keyframe** currAnim = &mSkeleton.mJoints[currJointIndex].mAnimation;

	for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
	{
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eFrames24);
		*currAnim = new Keyframe();
		(*currAnim)->mFrameNum = i;
		FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
		(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
		currAnim = &((*currAnim)->mNext);
	}

	return true;
}

//// Process animations
//{
//	//Get mesh in the scene
//	int meshCount = lScene->GetSrcObjectCount<FbxMesh>();

//	for (int i = 0; i < meshCount; ++i)
//	{
//		auto* mesh = lScene->GetSrcObject<FbxMesh>(i);
//		//ProcessJointsAndAnimations(mesh, ThisSkeleton);
//		printf("hello");
//	}
//}

//{
//	// Get materials in the scene
//	auto materialCount = lScene->GetMaterialCount();

//	for (int i = 0; i < materialCount; ++i)
//	{
//		FbxSurfaceMaterial* material = lScene->GetMaterial(i);
//		std::string name = material->GetName();
//		//printf(name.c_str());
//		//printf("\n");
//	}
//}

bool Importer::ImportMaterial(FbxSurfaceMaterial* material, MaterialData& mat)
{
	//glm::vec3 ambient(0, 0, 0);
	//glm::vec3 diffuse(1, 1, 1);
	//glm::vec3 emissive(0, 0, 0);
	//glm::vec3 specular(0, 0, 0);
	//float shininess = 80.0;

	//// Get material information
	//{
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	//	if (prop.IsValid())
	//	{
	//		const auto& color = prop.Get<FbxDouble3>();
	//		ambient = glm::vec3(color[0], color[1], color[2]);
	//	}
	//}
	//{
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	//	if (prop.IsValid())
	//	{
	//		const auto& color = prop.Get<FbxDouble3>();
	//		diffuse = glm::vec3(color[0], color[1], color[2]);
	//	}
	//}
	//{
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sEmissive);
	//	if (prop.IsValid())
	//	{
	//		const auto& color = prop.Get<FbxDouble3>();
	//		emissive = glm::vec3(color[0], color[1], color[2]);
	//	}
	//}
	//{
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
	//	if (prop.IsValid())
	//	{
	//		const auto& color = prop.Get<FbxDouble3>();
	//		specular = glm::vec3(color[0], color[1], color[2]);
	//	}
	//}
	//{
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sShininess);
	//	if (prop.IsValid())
	//	{
	//		shininess = prop.Get<FbxDouble>();
	//	}
	//}

	//mat.ambient   = ambient;
	//mat.diffuse   = diffuse;
	//mat.emissive = emissive;
	//mat.specular  = specular;
	//mat.shininess = shininess;

	//{
	//	// Get the texture that is attached to diffuse
	//	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	//	if (prop.GetSrcObjectCount<FbxFileTexture>() > 0)
	//	{
	//		// Get the first texture only
	//		FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(0);
	//		if (texture)
	//		{
	//			// Get the file path
	//			const char * name = (const char*)(FbxPathUtils::GetFileName(texture->GetFileName()));

	//			//// Read the texture and flip it
	//			// Had to implement a loading function here
	//		}
	//	}
	//}

	return true;
}

void Importer::ProcessJointsAndAnimations(FbxMesh* currMesh, Skeleton skeleton)
{
	//FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = GetGeometryTransformation(currMesh->GetNode());

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			int currJointIndex = FindJointIndexUsingName(currJointName.c_str(), skeleton);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			// Update the information in mSkeleton 
			skeleton.joints[currJointIndex].inversed = convertFBXMatrix(globalBindposeInverseMatrix);
			//skeleton.joints[currJointIndex].mNode = currCluster->GetLink();

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				BlendingWeight currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.index = currJointIndex;
				currBlendingIndexWeightPair.weight = currCluster->GetControlPointWeights()[i];
				//mControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			std::cout << animStackName << "\n";
			auto mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			auto mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			//Keyframe** currAnim = &skeleton.joints[currJointIndex].mAnimation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				//*currAnim = new Keyframe();
				//(*currAnim)->mFrameNum = i;
				FbxAMatrix currentTransformOffset = currMesh->GetNode()->EvaluateGlobalTransform(currTime) * geometryTransform;
				//(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				//currAnim = &((*currAnim)->mNext);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	//BlendingIndexWeightPair currBlendingIndexWeightPair;
	//currBlendingIndexWeightPair.mBlendingIndex = 0;
	//currBlendingIndexWeightPair.mBlendingWeight = 0;
	//for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	//{
	//	for (unsigned int i = itr->second->mBlendingInfo.size(); i <= 4; ++i)
	//	{
	//		itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
	//	}
	//}
}

FbxAMatrix Importer::GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

int Importer::FindJointIndexUsingName(const char* name, Skeleton skeleton)
{
	for (int i = 0; i < skeleton.joints.size(); i++)
	{
		if (strcmp(skeleton.joints[i].name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

glm::mat4 Importer::convertFBXMatrix(FbxAMatrix)
{
	return glm::mat4();
}

//////////////////////////////////////////////////////////////////////////////////////
/**
 * Print a node, its attributes, and all its children recursively.
 */

 /* Tab character ("\t") counter */
int numTabs = 0;

/**
 * Print the required number of tabs.
 */
void Importer::PrintTabs()
{
	for (int i = 0; i < numTabs; i++)
		printf("\t");
}

void Importer::ImportNode(FbxNode* pNode)
{

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString Importer::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type)
	{
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
 * Print an attribute.
 */
void Importer::PrintAttribute(FbxNodeAttribute* pAttribute)
{
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}



void Importer::PrintNode(FbxNode* pNode)
{
	PrintTabs();
	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	// Print the contents of the node.
	printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
	);
	numTabs++;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
	{
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));
	}

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("</node>\n");
}

void Importer::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex, Skeleton& skeleton)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.parent_index = inParentIndex;
		currJoint.name = inNode->GetName();

		FbxAMatrix global_mat = inNode->EvaluateGlobalTransform(0);
		currJoint.coord = glm::vec3(global_mat.GetT().mData[0], global_mat.GetT().mData[1], global_mat.GetT().mData[2]);
		
		skeleton.joints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, skeleton.joints.size(), myIndex, skeleton);
	}
}