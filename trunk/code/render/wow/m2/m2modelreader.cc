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
#include "io/mpqfs/mpqfilesystem.h"
#include "io/ioserver.h"

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
	globalSequences(0),
	dataPtr(0)
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

	if(dataPtr)
		n_delete_array(dataPtr);
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
		//this->ReadPrimitiveGroups();
		this->SetupVertexComponents();
		//this->SetupVertexBuffer();
		//this->SetupIndexBuffer();
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

	if (this->stream->IsMapped())
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

			//this->SetToFirstModelNode();
			//while(this->SetToNextModelNode()){}

			
		}
		break;        
	case Static:
		{
			//this->SetToFirstModel();
			//// go thru hierarchy and build modelnodes
			//if (this->SetToFirstModelNode()) do
			//{
			//}
			//while(this->SetToNextModelNode());
		}
		break;
	}

	LoadLodMesh();
	SetupVertexBuffer();

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

	String fullName = modelResId.Value();
	fullName.StripFileExtension();
	//fullName = fullName.ExtractToEnd(4);

	FixedArray<Ptr<Stream>> animfiles;
	if (header->nAnimations > 0) {
		characterNode->anims = new ModelAnimation[header->nAnimations];

		#ifndef WotLK
		memcpy(anims, f.getBuffer() + header.ofsAnimations, header.nAnimations * sizeof(ModelAnimation));
		#else
		ModelAnimationWotLK animsWotLK;
		String tempname;
		
		animfiles.SetSize(header->nAnimations);
		for(size_t i=0; i<header->nAnimations; i++) {
			memcpy(&animsWotLK, this->mapPtr + header->ofsAnimations + i*sizeof(ModelAnimationWotLK), sizeof(ModelAnimationWotLK));
			characterNode->anims[i].animID = animsWotLK.animID;
			characterNode->anims[i].timeStart = 0;
			characterNode->anims[i].timeEnd = animsWotLK.length;
			characterNode->anims[i].moveSpeed = animsWotLK.moveSpeed;
			characterNode->anims[i].flags = animsWotLK.flags;
			characterNode->anims[i].probability = animsWotLK.probability;
			characterNode->anims[i].d1 = animsWotLK.d1;
			characterNode->anims[i].d2 = animsWotLK.d2;
			characterNode->anims[i].playSpeed = animsWotLK.playSpeed;
			characterNode->anims[i].rad = animsWotLK.rad;
			characterNode->anims[i].NextAnimation = animsWotLK.NextAnimation;
			characterNode->anims[i].Index = animsWotLK.Index;

			tempname.Format("%s%04d-%02d.anim", fullName.AsCharPtr(), 
				characterNode->anims[i].animID, animsWotLK.subAnimID);
			//if (MPQFileSystem::Instance()->GetMPQFileSize(tempname) > 0) 
			{
				animfiles[i] = IoServer::Instance()->CreateStream(tempname);
			}
		}
		#endif

		characterNode->animManager = new AnimManager(characterNode->anims);
	}
	
	if (1) {
		// init bones...
		characterNode->bones.SetSize(header->nBones);
		ModelBoneDef *mb = (ModelBoneDef*)(this->mapPtr + header->ofsBones);
		for (size_t i=0; i<header->nBones; i++) {
			//if (i==0) mb[i].rotation.ofsRanges = 1.0f;
#ifdef WotLK
			//bones[i].model = this;
			characterNode->bones[i].Init(this->stream, mb[i], globalSequences, &animfiles[0]);
#else
			bones[i].init(f, mb[i], globalSequences);
#endif
		}

		// Block keyBoneLookup is a lookup table for Key Skeletal Bones, hands, arms, legs, etc.
		if (header->nKeyBoneLookup < BONE_MAX) {
			memcpy(characterNode->keyBoneLookup, this->mapPtr + header->ofsKeyBoneLookup, sizeof(int16)*header->nKeyBoneLookup);
		} else {
			memcpy(characterNode->keyBoneLookup, this->mapPtr + header->ofsKeyBoneLookup, sizeof(int16)*BONE_MAX);
			n_error("Error: keyBone number [%d] over [%d]", header->nKeyBoneLookup, BONE_MAX);
		}
	}

	for (SizeT i = 0; i < animfiles.Size(); i++)
	{
		animfiles[0] = 0;
	}
	animfiles.SetSize(0);

	// Index at ofsAnimations which represents the animation in AnimationData.dbc. -1 if none.
	if (header->nAnimationLookup > 0) 
	{
		characterNode->animLookups.SetSize(header->nAnimationLookup);
		memcpy(&characterNode->animLookups[0], this->mapPtr + header->ofsAnimationLookup, sizeof(int16)*header->nAnimationLookup);
	}
	characterNode->nAnimationLookup = header->nAnimationLookup;

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
		const Ptr<M2SkinShapeNode>& node = newModelNode.downcast<M2SkinShapeNode>();
		node->SetString(Attr::Shader, "shd:skinned");
		node->SetJointArray(frgBoneList[nTex]);

		newModelNode->SetParent(characterNode.upcast<ModelNode>());
		characterNode->AddChild(newModelNode.upcast<ModelNode>());
	}

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
//	this->viewPtr = (ModelView*)(this->mapPtr + this->header->ofsViews);
//	this->opsPtr = (ModelGeoset*)(this->mapPtr + this->viewPtr->ofsSub);
//	this->texPtr = (ModelTexUnit*)(this->mapPtr + this->viewPtr->ofsTex);
	this->texLookUpPtr = (uint16*)(this->mapPtr + this->header->ofsTexLookup);
	this->texDefPtr = (ModelTextureDef*)(this->mapPtr + this->header->ofsTextures);
	this->renderFlags = (ModelRenderFlags*)(this->mapPtr + this->header->ofsTexFlags);

	//int16 *p = (int16*)(this->mapPtr + this->header->ofsF);
	//for (size_t i=0; i<header->nF; i++) {
	//	boneLookup[i] = p[i];
	//}

	/*vfvf **/dataPtr = new vfvf[this->header->nVertices];

	globalSequences = 0;
	if (header->nGlobalSequences) {
		globalSequences = new uint32[header->nGlobalSequences];
		memcpy(globalSequences, (this->mapPtr + header->ofsGlobalSequences), header->nGlobalSequences * sizeof(uint32));
	}
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

			
	

	ModelVertex *origVertices = (ModelVertex*)(this->mapPtr + this->header->ofsVertices);

	vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
	vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);


	//uint16 *indexLookup = (uint16*)(this->mapPtr + this->viewPtr->ofsIndex);
	//uint16 *triangles = (uint16*)(this->mapPtr + this->viewPtr->ofsTris);
	//SizeT nIndices = this->viewPtr->nTris;
	//uint16 *indices = n_new_array(uint16, nIndices);

	//for (size_t i = 0; i < nIndices; i++)
	//	indices[i] = indexLookup[triangles[i]];

	//frgBoneList.SetSize(this->viewPtr->nTex);
	//for (IndexT j = 0; j < this->viewPtr->nTex; j++)
	//{
	//	SizeT geoset = this->texPtr[j].op;
	//	for (IndexT g = this->opsPtr[geoset].istart; g < this->opsPtr[geoset].istart + this->opsPtr[geoset].icount; g++)
	//	{
	//		uint16 ind = indices[g];
	//		for (IndexT n = 0; n < 4; n++)
	//		{
	//			if (origVertices[ind].weights[n] > 0)
	//			{
	//				uint32 nBone = origVertices[ind].bones[n];
	//				IndexT indexFind = frgBoneList[geoset].FindIndex(nBone);
	//				if (indexFind == InvalidIndex)
	//				{
	//					frgBoneList[geoset].Append(nBone);
	//					indexFind = frgBoneList[geoset].Size()-1;
	//				}
	//				// 设置成在新的列表中的位置
	//				dataPtr[ind].bones[n] = indexFind;
	//				dataPtr[ind].weights[n] = (float)origVertices[ind].weights[n] / 255.0f;
	//			}
	//		}
	//	}
	//}

	//n_delete_array(indices);

	for (size_t i = 0; i < this->header->nVertices; i++)
	{
		//CopyMemory(dataPtr[i].bones, origVertices[i].bones, sizeof(uint8)*4);
		//CopyMemory(dataPtr[i].weights, origVertices[i].weights, sizeof(uint8)*4);
		for (int n = 0; n < 4; n++)
		{
			//dataPtr[i].bones[n] = (float)origVertices[i].bones[n];
			if (origVertices[i].weights[n] > 0)
				dataPtr[i].weights[n] = (float)origVertices[i].weights[n] / 255.0f;
		}

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
	//delete[] dataPtr;
	n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);

	for (SizeT i = 0; i < meshs.Size(); i++)
	{
		meshs[i]->SetVertexBuffer(this->vertexBuffer);
	}
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

void
M2ModelReader::LoadLodMesh()
{
	if (header->nViews <= 0)
		return;

	// remove suffix .M2
	String fullName = modelResId.Value();
	fullName.StripFileExtension();
	String lodname;
	lodname = fullName + "00.skin";	// Lods: 00, 01, 02, 03
	Ptr<Stream> g = IO::IoServer::Instance()->CreateStream(lodname);
	if (!g->Open())
	{
		n_warning("load resource error: %s", lodname.AsCharPtr());
		return;
	}
	char* ptr = (char*)g->Map();
	ModelView *view = (ModelView*)(ptr);

	// Indices,  Triangles
	uint16 *indexLookup = (uint16*)(ptr + view->ofsIndex);
	uint16 *triangles = (uint16*)(ptr + view->ofsTris);
	uint32 nIndices = view->nTris;
	uint16* indices = n_new_array(uint16, nIndices);
	for (size_t i = 0; i<nIndices; i++) {
        indices[i] = indexLookup[triangles[i]];
	}

	// render ops
	ModelGeoset *ops = (ModelGeoset*)(ptr + view->ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(ptr + view->ofsTex);

	/////////// create resource
	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	for (size_t i = 0; i < view->nTex; i++)
	{
		PrimitiveGroup primGroup;
		SizeT geoset = tex[i].op;
		primGroup.SetBaseVertex(ops[geoset].vstart);
		primGroup.SetNumVertices(ops[geoset].vcount);
		primGroup.SetBaseIndex(ops[geoset].istart);   // firstTriangle
		primGroup.SetNumIndices(ops[geoset].icount);   // numTriangles

		primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		primGroups.Append(primGroup);
	}

	// setup a new index buffer
	Ptr<CoreGraphics::IndexBuffer> indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, nIndices, indices, sizeof(uint16)*nIndices);
	indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	indexBuffer->Load();
	indexBuffer->SetLoader(0);
	n_assert(indexBuffer->GetState() == IndexBuffer::Loaded);
	
	Ptr<Mesh> mesh = SharedResourceServer::Instance()->CreateSharedResource(lodname, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetPrimitiveGroups(primGroups);
	meshs.Append(mesh);

	ModelVertex *origVertices = (ModelVertex*)(this->mapPtr + this->header->ofsVertices);
	frgBoneList.SetSize(0);
	frgBoneList.SetSize(view->nTex);
	/*for (IndexT i = 0; i < view->nTex; i++)
	{
		for (IndexT j = 0; j < header->nBones; j++)
		{
			frgBoneList[i].Append(j);
		}
	}*/
	for (IndexT j = 0; j < view->nTex; j++)
	{
		SizeT geoset = tex[j].op;
		for (IndexT g = ops[geoset].istart; g < ops[geoset].istart + ops[geoset].icount; g++)
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
					//// 设置成在新的列表中的位置
					dataPtr[ind].bones[n] = indexFind;
					//dataPtr[ind].weights[n] = (float)origVertices[ind].weights[n] / 255.0f;
				}
			}
		}
	}
	n_delete_array(indices);
	///////////////////////////////



	ModelRenderFlags *renderFlags = (ModelRenderFlags*)(this->mapPtr + header->ofsTexFlags);
	uint16 *texlookup = (uint16*)(this->mapPtr + header->ofsTexLookup);
	uint16 *texanimlookup = (uint16*)(this->mapPtr + header->ofsTexAnimLookup);
	int16 *texunitlookup = (int16*)(this->mapPtr + header->ofsTexUnitLookup);

	
	bool *showGeosets = new bool[view->nSub];
	for (size_t i=0; i<view->nSub; i++) {
		//characterNode->geoset.Append(ops[i]);
		showGeosets[i] = true;
	}

	for (size_t j = 0; j<view->nTex; j++) {
		ModelRenderPass pass;

		pass.useTex2 = false;
		pass.useEnvMap = false;
		pass.cull = false;
		pass.trans = false;
		pass.unlit = false;
		pass.noZWrite = false;
		pass.billboard = false;

		//pass.texture2 = 0;
		size_t geoset = tex[j].op;
		
		pass.geoset = (int)geoset;

		pass.indexStart = ops[geoset].istart;
		pass.indexCount = ops[geoset].icount;
		pass.vertexStart = ops[geoset].vstart;
		pass.vertexEnd = pass.vertexStart + ops[geoset].vcount;
		
		pass.order = tex[j].shading; //pass.order = 0;
		
		//TextureID texid = textures[texlookup[tex[j].textureid]];
		//pass.texture = texid;
		pass.tex = texlookup[tex[j].textureid];
		
		/*
		// Render Flags
		flags:
		0x01 = Unlit
		0x02 = ? glow effects ? no zwrite?
		0x04 = Two-sided (no backface culling if set)
		0x08 = (probably billboarded)
		0x10 = Disable z-buffer?

		blend:
		Value	 Mapped to	 Meaning
		0	 	0	 		Combiners_Opaque
		1	 	1	 		Combiners_Mod
		2	 	1	 		Combiners_Decal
		3	 	1	 		Combiners_Add
		4	 	1	 		Combiners_Mod2x
		5	 	4	 		Combiners_Fade
		6	 	4	 		Used in the Deeprun Tram subway glass, supposedly (src=dest_color, dest=src_color) (?)
		*/
		// TODO: figure out these flags properly -_-
		ModelRenderFlags &rf = renderFlags[tex[j].flagsIndex];
		
		pass.blendmode = rf.blend;
		//if (rf.blend == 0) // Test to disable/hide different blend types
		//	continue;

		pass.color = tex[j].colorIndex;
		//pass.opacity = transLookup[tex[j].transid];

		pass.unlit = (rf.flags & RENDERFLAGS_UNLIT)!= 0;

		// This is wrong but meh.. best I could get it so far.
		//pass.cull = (rf.flags & 0x04)==0 && pass.blendmode!=1 && pass.blendmode!=4 && (rf.flags & 17)!=17;
		//pass.cull = false; // quick test
		pass.cull = (rf.flags & RENDERFLAGS_TWOSIDED)==0 && rf.blend==0;

		pass.billboard = (rf.flags & RENDERFLAGS_BILLBOARD) != 0;

		pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && pass.billboard && rf.blend>2; //&& rf.blend<5; // Use environmental reflection effects?

		// Disable environmental mapping if its been unchecked.
		//if (pass.useEnvMap && !video.useEnvMapping)
		//	pass.useEnvMap = false;


		//pass.noZWrite = (texdef[pass.tex].flags & 3)!=0;
		/*
		if (name == "Creature\\Turkey\\turkey.m2") // manual fix as I just bloody give up.
			pass.noZWrite = false;
		else
			pass.noZWrite = (pass.blendmode>1);
		*/
			//pass.noZWrite = (pass.blendmode>1) && !(rf.blend==4 && rf.flags==17);
		pass.noZWrite = (rf.flags & RENDERFLAGS_ZBUFFERED) != 0;

		// ToDo: Work out the correct way to get the true/false of transparency
		pass.trans = (pass.blendmode>0) && (pass.opacity>0);	// Transparency - not the correct way to get transparency

		pass.p = ops[geoset].BoundingBox[0].z;

		// Texture flags
		//pass.swrap = (texdef[pass.tex].flags & TEXTURE_WRAPX) != 0; // Texture wrap X
		//pass.twrap = (texdef[pass.tex].flags & TEXTURE_WRAPY) != 0; // Texture wrap Y
		
		//if (animTextures) {
		//	// tex[j].flags: Usually 16 for static textures, and 0 for animated textures.	
		//	if (tex[j].flags & TEXTUREUNIT_STATIC) {
		//		pass.texanim = -1; // no texture animation
		//	} else {
		//		pass.texanim = texanimlookup[tex[j].texanimid];
		//	}
		//} else {
		//	pass.texanim = -1; // no texture animation
		//}



		

		String texName;
		Ptr<ModelNode> newModelNode;
		if (GatherModelType() == ModelType::Character)
			newModelNode = M2SkinShapeNode::Create();
		else
			newModelNode = ShapeNode::Create();

		static int nn=0;
		String objName;
		objName.AppendInt(nn++);
		newModelNode->SetName(objName);

		// Primitive Group
		//SizeT geoset = this->texPtr[numTexture].op;
		// texture
		if (pass.tex <= (int)view->nTex)
		{
			/*if (this->texDefPtr[pass.tex].type == 0)
			{
				texName.AppendRange((char*)this->mapPtr + this->texDefPtr[nTex].nameOfs, this->texDefPtr[nTex].nameLen);
				newModelNode->SetString(Attr::DiffMap0, texName);
			}
			else*/
			{
				// special texture - only on characters and such...
				newModelNode->SetString(Attr::DiffMap0, String("textures:system/white.dds"));
			}
		}

		newModelNode->SetString(Attr::Shader, "shd:static");

		newModelNode->SetFloat4(Attr::BoxCenter, this->vcenter);
		newModelNode->SetFloat4(Attr::BoxExtents, this->vmax - this->vcenter);

		newModelNode->SetString(Attr::MeshResourceId, lodname);
		newModelNode->SetInt(Attr::MeshGroupIndex, j);

		if (newModelNode->IsA(M2SkinShapeNode::RTTI))
		{
			const Ptr<M2SkinShapeNode>& node = newModelNode.downcast<M2SkinShapeNode>();
			node->SetString(Attr::Shader, "shd:skinned");
			node->SetJointArray(frgBoneList[geoset]);
			node->SetGeoset(ops[geoset]);

			newModelNode->SetParent(characterNode.upcast<ModelNode>());
			characterNode->AddChild(newModelNode.upcast<ModelNode>());
		}

		Attr::AttributeContainer attr = newModelNode->GetAttrs();
		newModelNode->LoadFromAttrs(attr);

		this->model->AttachNode(newModelNode);

	}

	g->Close();
	g = 0;

	n_delete_array(showGeosets);
}

} // namespace Models
