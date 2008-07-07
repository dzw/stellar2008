//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2modelreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "models/attributes.h"
//#include "models/nodes/transformnode.h"
#include "models/nodes/shapenode.h"
#include "models/nodes/skinshapenode.h"
#include "system/byteorder.h"
//#include "wow/m2model/m2modelnode.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "resources/resourceloader.h"
#include "resources/resourcemanager.h"
#include "resources/sharedresourceserver.h"
#include "coregraphics/streammeshloader.h"
#include "wow/m2/animation/animated.h"
#include "wow/m2/m2skinshapenode.h"

int globalTime = 0;

namespace Models
{
ImplementClass(Models::M2ModelReader, 'WM2R', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Attr;
using namespace System;
using namespace Models;
using namespace CoreGraphics;
using namespace Resources;
using namespace WOW;

//------------------------------------------------------------------------------
/**
*/
M2ModelReader::M2ModelReader() :
    isSetToModel(false),
    isSetToModelNode(false),
	header(0),
	mapPtr(0),
	groupDataPtr(0),
	vertexDataPtr(0),
	indexDataPtr(0),
	groupDataSize(0),
	vertexDataSize(0),
	indexDataSize(0),
	numGroups(0),
	numVertices(0),
	vertexWidth(0),
	numIndices(0),
	numEdges(0),
	vertexComponentMask(0),
	viewPtr(0),
	opsPtr(0),
	texPtr(0),
	texDefPtr(0),
	texLookUpPtr(0),
	globalSequences(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
M2ModelReader::~M2ModelReader()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanRead());
    n_assert(this->stream->CanSeek());
    n_assert(!this->stream->IsMapped());
    n_assert(this->modelResId.Value().IsValid());

	n_assert(!this->vertexBuffer.isvalid());
	n_assert(!this->indexBuffer.isvalid());
	n_assert(0 == this->primGroups.Size());
	n_assert(this->stream->CanBeMapped());
	n_assert(0 == this->mapPtr);

	this->isSetToModel = false;
	this->isSetToModelNode = false;

    if (StreamReader::Open())
    {
		this->mapPtr = (unsigned char*)this->stream->Map();
		this->header = (ModelHeader*)this->mapPtr;

        if (header->id[0] != 'M' && header->id[1] != 'D' &&
            header->id[2] != '2' && header->id[3] != '0')
            n_error("M2ModelReader: '%s' is not a m2 file!", this->stream->GetURI().AsString().AsCharPtr());
        /*if (header->version[0] != '4' && header->version[1] != '1' &&		// modelview 0.48e导出来好像是 wow 1.0 models所以不能判断
            header->version[2] != '0' && header->version[3] != '0')
            n_error("M2ModelReader: '%s' Model version is incorrect!", this->stream->GetURI().AsString().AsCharPtr());*/

		// read data
		this->ReadHeaderData();
		this->ReadPrimitiveGroups();
		this->SetupVertexComponents();
		this->SetupVertexBuffer();
		this->SetupIndexBuffer();
		this->UpdateGroupBoundingBoxes();
		this->SetupManagedMesh();

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::Close()
{
    n_assert(this->IsOpen());
	n_assert(0 != this->mapPtr);

	this->mapPtr = 0;
	this->groupDataPtr = 0;
	this->vertexDataPtr = 0;
	this->indexDataPtr = 0;
	this->vertexBuffer = 0;
	this->indexBuffer = 0;
	this->viewPtr = 0;
	this->opsPtr = 0;
	this->texPtr = 0;
	this->texDefPtr = 0;
	this->texLookUpPtr = 0;
	this->primGroups.Clear();
	this->vertexComponents.Clear();

	this->stream->Unmap();
    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
	暂时只加载动画
*/
M2ModelReader::ModelType 
M2ModelReader::GatherModelType()
{
	return ModelType::Character;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::FillModel()
{
	n_assert(this->model.isvalid());

	// check if special case: if its character file without character3node
	bool success = true;
	ModelType type = GatherModelType();
	switch (type)
	{
	case Character:
		{
			this->SetToFirstModel();
			// jump to skinanimator and fill characternode with animators attributes
			/*Ptr<M2CharacterNode>*/ characterNode = this->CreateCharacterNode();

			// attach to our model
			this->model->AttachNode(characterNode.upcast<ModelNode>());

			this->SetToFirstModelNode();
			while(this->SetToNextModelNode()){}
		}
		break;        
	case Static:
		{
			this->SetToFirstModel();
			// go thru hierarchy and build modelnodes
			if (this->SetToFirstModelNode()) do
			{
			}
			while(this->SetToNextModelNode());
		}
		break;
	}

	return success;
}

Vec3D fixCoordSystem(Vec3D v)
{
	return Vec3D(v.x, v.z, -v.y);
}

Vec3D fixCoordSystem2(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

Quaternion fixCoordSystemQuat(Quaternion v)
{
	return Quaternion(-v.x, -v.z, v.y, v.w);
}

//------------------------------------------------------------------------------
/**
	创建动画节点(CharacterNode)
*/
Ptr<M2CharacterNode> 
M2ModelReader::CreateCharacterNode()
{
	Ptr<M2CharacterNode> characterNode;
	
	// 创建CharacterNode
	characterNode = M2CharacterNode::Create();
	characterNode->SetName(modelResId);
	
	// anim  动画数据，直接设置  在N2中是加载nax2文件数据，形成ManagedAnimation
	//characterNode->SetAnim(this->binaryReader->ReadString());

	// joints
	characterNode->BeginJoints(this->header->nBones);

	/*Animated<Vec3D> trans;
	Animated<Quaternion, PACK_QUATERNION, Quat16ToQuat32> rot;
	Animated<Vec3D> scale;*/

	if (this->header->nGlobalSequences) {
		globalSequences = n_new_array(int, header->nGlobalSequences);
		CopyMemory(globalSequences, (this->mapPtr + this->header->ofsGlobalSequences), this->header->nGlobalSequences * 4);
	}

	//vector pivot;
	//Bone *bones = new Bone[this->header->nBones];

	ModelBoneDef *mb = (ModelBoneDef*)(this->mapPtr + this->header->ofsBones);
	for (size_t i=0; i<this->header->nBones; i++) {
		//if (i==0) mb[i].rotation.ofsRanges = 1.0f;
		//bones[i].init(f, mb[i], globalSequences);
		characterNode->SetJoint(i, this->mapPtr, mb[i], globalSequences);
	}
	characterNode->EndJoints();

	if (this->header->nAnimations > 0) {
		characterNode->SetAniManager(this->mapPtr + this->header->ofsAnimations, this->header->nAnimations);
	}



	// clip
	/*characterNode->BeginClips(this->header->nAnimations);
	for (SizeT i = 0; i <this->header->nAnimations; i++)
	{
		String clipName = "clip";
		clipName.AppendInt(i);
		characterNode->SetClip(i, i, clipName);
	}
	characterNode->EndClips();*/


	// variations 变化的动画？？直接设置 
	//characterNode->SetVariationsUri(this->binaryReader->ReadString());
		
		
	/*Ptr<nMemoryAnimation> animation = nMemoryAnimation::Create();
	Array<Math::float4> keyArray;
	
	Ptr<Anim::ManagedAnimation> managedAnim = Anim::ManagedAnimation::Create();
	managedAnim->SetResource(animation.upcast<Resources::Resource>());
	
	for(IndexT i = 0 ; i < this->header->nAnimations; i++)
	{
		nAnimation::Group& group = animation->GetGroupAt(i);
		group->SetNumCurves(this->header->nAnimations * this->header->nBones * 3);
		group->SetStartKey(0);
		group.SetNumKeys(0);
		group.SetKeyStride(1);
		group.SetKeyTime(0.1f);
		group.SetFadeInFrames(0.2f);
		group.SetLoopType(1);
	}*/
	
	// 求最大关键帧
	// 循环所有range
	/*int numKeys, beginRange, endRange;
	for (IndexT i = 0; i < this->header->nAnimations; i++)
	{
		numKeys = 0;
		for (IndexT n = 0; n < this->header->nBones; n++)
		{
			if (bones[n].trans.ranges.size() > i)
			{
				beginRange = bones[n].trans.ranges[i].first;
				endRange = bones[n].trans.ranges[i].second;

				if (endRange - beginRange > numKeys)
					numKeys = endRange - beginRange;
			}
			if (bones[n].rot.ranges.size() > i)
			{
				beginRange = bones[n].rot.ranges[i].first;
				endRange = bones[n].rot.ranges[i].second;

				if (endRange - beginRange > numKeys)
					numKeys = endRange - beginRange;
			}
			if (bones[n].scale.ranges.size() > i)
			{
				beginRange = bones[n].scale.ranges[i].first;
				endRange = bones[n].scale.ranges[i].second;

				if (endRange - beginRange > numKeys)
					numKeys = endRange - beginRange;
			}
		}
		nAnimation::Group& group = animation->GetGroupAt(i);
		group.SetNumKeys(numKeys);
	}*/
	
	//int isanim, curveIndex = 0, keyIndex = 0, ntemp;
	//Vec3D v;
	//Quaternion q;
	//float4 constValue;
	//// 设置KEYS
	//for (IndexT i = 0; i < this->header->nAnimations; i++)
	//{
	//	nAnimation::Group& group = animation->GetGroupAt(i);
	//	numKeys = group.GetNumKeys();
	//	for(IndexT boneIndex = 0; boneIndex < this->header->nBones; boneIndex++)
	//	{
	//		// trans
	//		isanim = 0;
	//		type = bones[boneIndex].trans.type;
	//		if (type == 1 && bones[boneIndex].trans.ranges.size() > 0)
	//			isanim = 1;
	//		v = bones[boneIndex].trans.data[0];
	//		constValue = float4(v.x, v.y, v.z, 0.0f);
	//		nAnimation::Curve& curve = group->GetCurveAt(curveIndex++);
	//		curve.SetIpolType((Animation::Curve::IpolType)1);
	//		curve.SetIsAnimated(isanim);
	//		curve.SetConstValue(constValue);
	//		if (isanim)
	//		{
	//			curve.SetFirstKeyIndex(keyArray.Size());

	//			ntemp = 0;
	//			for (IndexT n = 0; n < numKeys; n++)
	//			{
	//				if (ntemp >= bones[boneIndex].trans.data.size())
	//					ntemp = 0;

	//				v = bones[boneIndex].trans.data[ntemp];
	//				keyArray.Append(float4(v.x, v.y, v.z, 0.0f));
	//			}
	//		}
	//		else
	//		{
	//			curve.SetFirstKeyIndex(-1);
	//		}


	//		// rot
	//		isanim = 0;
	//		type = bones[boneIndex].rot.type;
	//		if (type == 1 && bones[boneIndex].rot.ranges.size() > 0)
	//			isanim = 1;
	//		q = bones[boneIndex].rot.data[0];
	//		constValue = float4(q.x, q.y, q.z, q.w);
	//		nAnimation::Curve& curve = group->GetCurveAt(curveIndex++);
	//		curve.SetIpolType((Animation::Curve::IpolType)1);
	//		curve.SetIsAnimated(isanim);
	//		curve.SetConstValue(constValue);
	//		if (isanim)
	//		{
	//			curve.SetFirstKeyIndex(keyArray.Size());

	//			ntemp = 0;
	//			for (IndexT n = 0; n < numKeys; n++)
	//			{
	//				if (ntemp >= bones[boneIndex].rot.data.size())
	//					ntemp = 0;

	//				q = bones[boneIndex].rot.data[ntemp];
	//				keyArray.Append(float4(q.x, q.y, q.z, q.w));
	//			}
	//		}
	//		else
	//		{
	//			curve.SetFirstKeyIndex(-1);
	//		}


	//		// scale
	//		isanim = 0;
	//		type = bones[boneIndex].scale.type;
	//		if (type == 1 && bones[boneIndex].scale.ranges.size() > 0)
	//			isanim = 1;
	//		v = bones[boneIndex].scale.data[0];
	//		constValue = float4(v.x, v.y, v.z, 0.0f);
	//		nAnimation::Curve& curve = group->GetCurveAt(curveIndex++);
	//		curve.SetIpolType((Animation::Curve::IpolType)1);
	//		curve.SetIsAnimated(isanim);
	//		curve.SetConstValue(constValue);
	//		if (isanim)
	//		{
	//			curve.SetFirstKeyIndex(keyArray.Size());

	//			ntemp = 0;
	//			for (IndexT n = 0; n < numKeys; n++)
	//			{
	//				if (ntemp >= bones[boneIndex].scale.data.size())
	//					ntemp = 0;

	//				v = bones[boneIndex].scale.data[ntemp];
	//				keyArray.Append(float4(v.x, v.y, v.z, 0.0f));
	//			}
	//		}
	//		else
	//		{
	//			curve.SetFirstKeyIndex(-1);
	//		}
	//	}
	//}
	
	return characterNode;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::SetToFirstModel()
{
    n_assert(this->isOpen);
    n_assert(!this->isSetToModel);
    n_assert(!this->isSetToModelNode);

    // note: there is only one Model per n2 file, so this method is kind of simple
    this->isSetToModel = true;
    this->isSetToModelNode = false;
    this->ReadModelData();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::SetToNextModel()
{
    n_assert(this->isOpen);
    n_assert(this->isSetToModel);
    n_assert(!this->isSetToModelNode);

    // note: there is only one Model per n2 file, so this method is kind of simple
    this->isSetToModel = false;
    this->isSetToModelNode = false;
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::SetToFirstModelNode()
{
    n_assert(this->isOpen);
    n_assert(this->isSetToModel);
    n_assert(!this->isSetToModelNode);

	this->numTexture = 0;
    this->isSetToModelNode = true;
    this->ReadModelNodeData();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2ModelReader::SetToNextModelNode()
{
    n_assert(this->isOpen);
    n_assert(this->isSetToModel);
    n_assert(this->isSetToModelNode);
	n_assert(0 != this->header);
	n_assert(0 != this->mapPtr);

	numTexture++;
	if (numTexture >= (int)this->viewPtr->nTex)
		return false;

	this->ReadModelNodeData();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::ReadModelData()
{
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::ReadModelNodeData()
{
	String texName/*, shaName*/;

	//this->modelNodeRtti = &ShapeNode::RTTI;
	Ptr<ModelNode> newModelNode;
	if (GatherModelType() == ModelType::Character)
		newModelNode = M2SkinShapeNode::Create();
	else //if (GatherModelType() == ModelType::Static)
		newModelNode = ShapeNode::Create();

	String objName;
	objName.AppendInt(numTexture);
	/*this->modelNodeName = objName;
	if (this->modelNodeStack.IsEmpty())
	{
		this->modelNodeParentName.Clear();
	}
	else
	{
		this->modelNodeParentName = this->modelNodeStack.Peek();
	}
	this->modelNodeStack.Push(objName);*/
	newModelNode->SetName(objName);

	//Ptr<ManagedTexture> managedTexture = ManagedTexture::Create();
	//Ptr<ShaderInstance> shaderInstance = ShaderInstance::Create();
	//PrimitiveGroup primGroup;

	// Primitive Group
	SizeT geoset = this->texPtr[numTexture].op;
	//primGroup.SetBaseVertex(this->opsPtr[geoset].vstart);
	//primGroup.SetNumVertices(this->opsPtr[geoset].vcount);
	//primGroup.SetBaseIndex(this->opsPtr[geoset].istart);   // firstTriangle
	//primGroup.SetNumIndices(this->opsPtr[geoset].icount);   // numTriangles

	// texture
	int nTex = this->texLookUpPtr[this->texPtr[numTexture].textureid];	// 取纹理号
	if (this->header->nTextures > 0 && nTex <= (int)this->header->nTextures)
	{
		if (this->texDefPtr[nTex].type == 0)
		{
			texName.AppendRange((char*)this->mapPtr + this->texDefPtr[nTex].nameOfs, this->texDefPtr[nTex].nameLen);
			newModelNode->SetString(Attr::DiffMap0, texName);
		}
		else
		{
			// special texture - only on characters and such...
			newModelNode->SetString(Attr::DiffMap0, String("textures:system/white.dds"));
		}
	}

	// shader of alpha test
	//if (this->renderFlags[this->texPtr[numTexture].flagsIndex].blend == 1)
	//	newModelNode->SetType(Models::ModelNodeType::Alpha);
	/*	shaName = "shd:sampler_alphatest";
	else
		shaName = "shd:sampler";*/
	newModelNode->SetString(Attr::Shader, "shd:static");

	newModelNode->SetFloat4(Attr::BoxCenter, this->vcenter);
	newModelNode->SetFloat4(Attr::BoxExtents, this->vmax - this->vcenter);

	newModelNode->SetString(Attr::MeshResourceId, meshName);
	newModelNode->SetInt(Attr::MeshGroupIndex, geoset);

	if (newModelNode->IsA(M2SkinShapeNode::RTTI))
	{
		newModelNode->SetString(Attr::Shader, "shd:skinned");

		SkinShapeNode* skinShapeNode = newModelNode.downcast<SkinShapeNode>().get();
		// fragments  每个node只要一个！！一般一个geoset不会越过72个骨骼，所以不需要再按骨骼拆分mesh
		skinShapeNode->BeginFragments(1/*this->viewPtr->nTex*/);
		//for (IndexT i = 0; i < )
		{
			//Array<int> frgBoneList;
			//CreateFragment(geoset, frgBoneList);
			// fraggroupindex
			skinShapeNode->SetFragGroupIndex(0/*numTexture*/, geoset);
			// jointpalette
			skinShapeNode->BeginJointPalette(0/*numTexture*/, frgBoneList[numTexture].Size());
			// jointindices
			for (IndexT i = 0; i < frgBoneList[numTexture].Size(); i++)
				skinShapeNode->SetJointIndex(0/*numTexture*/, i, frgBoneList[numTexture][i]);
			//skinShapeNode->SetJointIndices(numTexture, numTexture, boneLookup[numTexture], 0, 0, 0, 0, 0, 0, 0);
			// jointpalette
			skinShapeNode->EndJointPalette(0/*numTexture*/);
		}
		// fragments
		skinShapeNode->EndFragments();
	}

	newModelNode->SetParent(characterNode.upcast<ModelNode>());
	characterNode->AddChild(newModelNode.upcast<ModelNode>());
	Attr::AttributeContainer attr = newModelNode->GetAttrs();
	newModelNode->LoadFromAttrs(attr);

	this->model->AttachNode(newModelNode);
}

//------------------------------------------------------------------------------
/**
	// 创建动画所需的骨骼列表，因为矩阵模板有使用限制，所以将所有的顶点骨骼按goeset分批处理。
	// M2原来骨骼是全部放在一个队列中，从顶点的取得骨骼编号，再到队列里面找到相应的骨骼做处理，
	// 现在需要将这个队列按goeset分批处理，在GPU中做骨骼运算，所以需要重新排列骨骼队列，就是把
	// 当前goeset的所有顶点用到的骨骼编号重新设置一下，以便按照新的排序方式：
	// 原来的：bones[vertex.bones]
	// 新的：bones = 所有当前goeset的骨骼，然后将vertex.bones编号改成重新设置的编号。
	// 1、重新设置 V->B-->new bone list
	// 2、新的V->B=new bone list[B]
	
	采用这种方式之后，骨骼数超过72个也没问题，只要再分fragment即可。
*/
void
M2ModelReader::CreateFragment(int geoset, Array<int>& frgBoneList)
{
	//Array<int> frgBoneList;
	//for (IndexT g = this->opsPtr[geoset].istart; g < this->opsPtr[geoset].icount; g++)
	//{
	//	uint16 ind = indices[g];
	//	for (IndexT n = 0; n < 4; n++)
	//	{
	//		if (origVertices[ind].weights[n] > 0)
	//		{
	//			uint32 nBone = origVertices[ind].bones[n];
	//			IndexT indexFind = frgBoneList.Find(nBone);
	//			if (indexFind == InvalidIndex)
	//			{
	//				frgBoneList.Append(nBone);
	//				indexFind = frgBoneList.Size();
	//			}
	//			// 设置成在新的列表中的位置
	//			origVertices[ind].bones[n] = indexFind;
	//		}
	//	}
	//}

}
//------------------------------------------------------------------------------
/**
	This reads the nvx2 header data and checks whether the file is 
	actually an nvx2 file through the magic number. All header data
	will be read into member variables, and pointers to the
	start of the group-, vertex- and index-data will be setup.
*/
void
M2ModelReader::ReadHeaderData()
{
	n_assert(0 != this->mapPtr);
	//uint* headerPtr = (uint*) this->mapPtr;

	this->header = (ModelHeader*)this->mapPtr;
	this->viewPtr = (ModelView*)(this->mapPtr + this->header->ofsViews);
	this->opsPtr = (ModelGeoset*)(this->mapPtr + this->viewPtr->ofsSub);
	this->texPtr = (ModelTexUnit*)(this->mapPtr + this->viewPtr->ofsTex);
	this->texLookUpPtr = (uint16*)(this->mapPtr + this->header->ofsTexLookup);
	this->texDefPtr = (ModelTextureDef*)(this->mapPtr + this->header->ofsTextures);
	this->renderFlags = (ModelRenderFlags*)(this->mapPtr + this->header->ofsTexFlags);

	//int16 *p = (int16*)(this->mapPtr + this->header->ofsF);
	//for (size_t i=0; i<header->nF; i++) {
	//	boneLookup[i] = p[i];
	//}
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::ReadPrimitiveGroups()
{
	n_assert(this->primGroups.IsEmpty());

	for (size_t i = 0; i < this->viewPtr->nTex; i++)
	{
		PrimitiveGroup primGroup;
		SizeT geoset = this->texPtr[i].op;
		primGroup.SetBaseVertex(this->opsPtr[geoset].vstart);
		primGroup.SetNumVertices(this->opsPtr[geoset].vcount);
		primGroup.SetBaseIndex(this->opsPtr[geoset].istart);   // firstTriangle
		primGroup.SetNumIndices(this->opsPtr[geoset].icount);   // numTriangles

		primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		this->primGroups.Append(primGroup);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::SetupVertexComponents()
{
	n_assert(this->vertexComponents.IsEmpty());

	this->vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	this->vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	//this->vertexComponents.Append(VertexComponent(VertexComponent::Tangent, 0, VertexComponent::Float3));
	//this->vertexComponents.Append(VertexComponent(VertexComponent::Binormal, 0, VertexComponent::Float3));
	this->vertexComponents.Append(VertexComponent(VertexComponent::SkinWeights, 0, VertexComponent::Float4));
	this->vertexComponents.Append(VertexComponent(VertexComponent::SkinJIndices, 0, VertexComponent::Float4));
	this->vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
}

//------------------------------------------------------------------------------
/**
	Since nvx2 files don't contain any bounding box information
	we need to compute per-primitive-group bounding boxes
	manually by walking the triangle indices. This may be inefficient
	with large meshes.	
*/
void
M2ModelReader::UpdateGroupBoundingBoxes()
{

}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::SetupVertexBuffer()
{
	n_assert(0 != this->mapPtr);
	n_assert(!this->vertexBuffer.isvalid());
	n_assert(this->vertexComponents.Size() > 0);

			
	struct vfvf 
	{
		float x,y,z;
		float nx,ny,nz;
		float weights[4];
		float bones[4];
		float2 tex;
	};

	vfvf *dataPtr = new vfvf[this->header->nVertices];
	ModelVertex *origVertices = (ModelVertex*)(this->mapPtr + this->header->ofsVertices);

	vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
	vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);


	uint16 *indexLookup = (uint16*)(this->mapPtr + this->viewPtr->ofsIndex);
	uint16 *triangles = (uint16*)(this->mapPtr + this->viewPtr->ofsTris);
	SizeT nIndices = this->viewPtr->nTris;
	uint16 *indices = n_new_array(uint16, nIndices);

	for (size_t i = 0; i < nIndices; i++)
		indices[i] = indexLookup[triangles[i]];

	frgBoneList.SetSize(this->viewPtr->nTex);
	for (IndexT j = 0; j < this->viewPtr->nTex; j++)
	{
		SizeT geoset = this->texPtr[j].op;
		for (IndexT g = this->opsPtr[geoset].istart; g < this->opsPtr[geoset].istart + this->opsPtr[geoset].icount; g++)
		{
			uint16 ind = indices[g];
			for (IndexT n = 0; n < 4; n++)
			{
				if (origVertices[ind].weights[n] > 0)
				{
					uint32 nBone = origVertices[ind].bones[n];
					IndexT indexFind = frgBoneList[geoset].FindIndex(nBone);
					if (indexFind == InvalidIndex)
					{
						frgBoneList[geoset].Append(nBone);
						indexFind = frgBoneList[geoset].Size()-1;
					}
					// 设置成在新的列表中的位置
					dataPtr[ind].bones[n] = indexFind;
					dataPtr[ind].weights[n] = (float)origVertices[ind].weights[n] / 255.0f;
				}
			}
		}
	}

	n_delete_array(indices);

	for (size_t i = 0; i < this->header->nVertices; i++)
	{
		//CopyMemory(dataPtr[i].bones, origVertices[i].bones, sizeof(uint8)*4);
		//CopyMemory(dataPtr[i].weights, origVertices[i].weights, sizeof(uint8)*4);
		/*for (int n = 0; n < 4; n++)
		{
			dataPtr[i].bones[n] = (float)origVertices[i].bones[n];
			if (origVertices[i].weights[n] > 0)
				dataPtr[i].weights[n] = (float)origVertices[i].weights[n] / 255.0f;
		}*/

		dataPtr[i].x = origVertices[i].pos.x;//= FixCoordSystem(vector(origVertices[i].pos.x, origVertices[i].pos.y, origVertices[i].pos.z));
		dataPtr[i].y = origVertices[i].pos.z;
		dataPtr[i].z = -origVertices[i].pos.y;
		//dataPtr[i].w = 1.0f;
		//dataPtr[i].w = 1.0;
		//dataPtr[i].nx = origVertices[i].normal.x;//FixCoordSystem(vector(origVertices[i].normal.x, origVertices[i].normal.y, origVertices[i].normal.z));
		//dataPtr[i].ny = origVertices[i].normal.z;
		//dataPtr[i].nz = origVertices[i].normal.y;
		dataPtr[i].tex = float2(origVertices[i].texcoords.x(), origVertices[i].texcoords.y());

		//dataPtr[i].normal = float4::normalize(dataPtr[i].normal);
		vector vec(origVertices[i].normal.x, origVertices[i].normal.z, -origVertices[i].normal.y);

		vec = float4::normalize(vec);
		dataPtr[i].nx = vec.x(); //1.0f;
		dataPtr[i].ny = vec.y();
		dataPtr[i].nz = vec.z();

		if ((vmin.x() - dataPtr[i].x) > TINY) vmin.x() = dataPtr[i].x;
		if ((vmin.y() - dataPtr[i].y) > TINY) vmin.y() = dataPtr[i].y;
		if ((vmin.z() - dataPtr[i].z) > TINY) vmin.z() = dataPtr[i].z;
		if ((dataPtr[i].x - vmax.x()) > TINY) vmax.x() = dataPtr[i].x;
		if ((dataPtr[i].y - vmax.y()) > TINY) vmax.y() = dataPtr[i].y;
		if ((dataPtr[i].z - vmax.z()) > TINY) vmax.z() = dataPtr[i].z;

		//float len = origVertices[i].pos.lensquared();
		//if (len > this->rad)
		//	this->rad = len;
	}
	//this->rad = n_sqrt(this->rad);

	this->vcenter = (vmax + vmin) * 0.5f;

	// setup new vertex buffer
	this->vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(this->vertexComponents, this->header->nVertices, dataPtr, this->header->nVertices * sizeof(vfvf));
	this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->vertexBuffer->Load();
	this->vertexBuffer->SetLoader(0);
	delete[] dataPtr;
	n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);
}

//------------------------------------------------------------------------------
/**
*/
void
M2ModelReader::SetupIndexBuffer()
{
	n_assert(!this->indexBuffer.isvalid());

	uint16 *indexLookup = (uint16*)(this->mapPtr + this->viewPtr->ofsIndex);
	uint16 *triangles = (uint16*)(this->mapPtr + this->viewPtr->ofsTris);
	SizeT nIndices = this->viewPtr->nTris;
	uint16 *indices = n_new_array(uint16, nIndices);

	for (size_t i = 0; i < nIndices; i++)
		indices[i] = indexLookup[triangles[i]];

	// setup a new index buffer
	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, nIndices, indices, sizeof(uint16)*nIndices);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	n_delete_array(indices);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);
}

//------------------------------------------------------------------------------
/**
	所有的ModelNode都是用相同的顶点，只是PrimativeGroupIndex不同。
*/
void 
M2ModelReader::SetupManagedMesh()
{
	// 相同的ShapeNode使用同一组VB & IB所以名字相同，实现共享
	static int totalM2Model = 0;
	meshName = "m2_";
	meshName.AppendInt(totalM2Model);
	totalM2Model++;

	// 手动建立资源，并填充数据
	/*Ptr<ManagedMesh> managedMesh = ResourceManager::Instance()->ManualCreateManagedResource(Mesh::RTTI, meshName).downcast<ManagedMesh>();
	Ptr<Mesh> mesh = managedMesh->GetResource().downcast<Mesh>();
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(this->indexBuffer);
	mesh->SetPrimitiveGroups(this->primGroups);*/

	Ptr<Mesh> mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(this->indexBuffer);
	mesh->SetPrimitiveGroups(this->primGroups);
}

} // namespace Models
