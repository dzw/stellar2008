//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/load/m2modelreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "models/attributes.h"
//#include "models/nodes/transformnode.h"
#include "models/nodes/shapenode.h"
#include "system/byteorder.h"
//#include "wow/m2model/m2modelnode.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "resources/resourceloader.h"
#include "resources/resourcemanager.h"

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
	texLookUpPtr(0)
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
	String texName, shaName;

	this->modelNodeRtti = &ShapeNode::RTTI;

	String objName;
	objName.AppendInt(numTexture);
	this->modelNodeName = objName;
	if (this->modelNodeStack.IsEmpty())
	{
		this->modelNodeParentName.Clear();
	}
	else
	{
		this->modelNodeParentName = this->modelNodeStack.Peek();
	}
	this->modelNodeStack.Push(objName);


	//Ptr<ManagedTexture> managedTexture = ManagedTexture::Create();
	//Ptr<ShaderInstance> shaderInstance = ShaderInstance::Create();
	PrimitiveGroup primGroup;

	// Primitive Group
	SizeT geoset = this->texPtr[numTexture].op;
	primGroup.SetBaseVertex(this->opsPtr[geoset].vstart);
	primGroup.SetNumVertices(this->opsPtr[geoset].vcount);
	primGroup.SetBaseIndex(this->opsPtr[geoset].istart);   // firstTriangle
	primGroup.SetNumIndices(this->opsPtr[geoset].icount);   // numTriangles

	// texture
	int nTex = this->texLookUpPtr[this->texPtr[numTexture].textureid];	// 取纹理号
	if (this->header->nTextures > 0 && nTex <= (int)this->header->nTextures)
	{
		if (this->texDefPtr[nTex].type == 0)
		{
			texName.AppendRange((char*)this->mapPtr + this->texDefPtr[nTex].nameOfs, this->texDefPtr[nTex].nameLen);
			this->modelNodeAttrs.SetString(Attr::DiffMap0, texName);
		}
		else
		{
			// special texture - only on characters and such...
		}
	}

	// shader of alpha test
	if (this->renderFlags[this->texPtr[numTexture].flagsIndex].blend == 1)
		shaName = "shd:sampler_alphatest";
	else
		shaName = "shd:sampler";
	this->modelNodeAttrs.SetString(Attr::Shader, shaName);


	// 暂时不做bound，不能去掉
	this->modelNodeAttrs.SetFloat4(Attr::BoxCenter, point(0.0, 0.0, 0.0));
	this->modelNodeAttrs.SetFloat4(Attr::BoxExtents, vector(0.0, 0.0, 1.0));

	this->modelNodeAttrs.SetString(Attr::MeshResourceId, meshName);
	this->modelNodeAttrs.SetInt(Attr::MeshGroupIndex, geoset);
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

	this->vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float4));
	this->vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
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
		float x,y,z,w;
		float nx,ny,nz;
		float2 tex;
	};

	vfvf *dataPtr = new vfvf[this->header->nVertices];
	ModelVertex *origVertices = (ModelVertex*)(this->mapPtr + this->header->ofsVertices);

	for (size_t i = 0; i < this->header->nVertices; i++)
	{
		dataPtr[i].x = origVertices[i].pos.x;//= FixCoordSystem(vector(origVertices[i].pos.x, origVertices[i].pos.y, origVertices[i].pos.z));
		dataPtr[i].y = origVertices[i].pos.z;
		dataPtr[i].z = origVertices[i].pos.y;
		dataPtr[i].w = 1.0;
		//dataPtr[i].nx = origVertices[i].normal.x;//FixCoordSystem(vector(origVertices[i].normal.x, origVertices[i].normal.y, origVertices[i].normal.z));
		//dataPtr[i].ny = origVertices[i].normal.z;
		//dataPtr[i].nz = origVertices[i].normal.y;
		dataPtr[i].tex = float2(origVertices[i].texcoords.x, origVertices[i].texcoords.y);

		//dataPtr[i].normal = float4::normalize(dataPtr[i].normal);
		vector vec(origVertices[i].normal.x, origVertices[i].normal.z, origVertices[i].normal.y);

		vec = float4::normalize(vec);
		dataPtr[i].nx = 1.0f;//vec.x();
		dataPtr[i].ny = 1.0f;//vec.y();
		dataPtr[i].nz = 1.0f;//vec.z();
	}

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
	uint16 *indices = new uint16[nIndices];

	for (size_t i = 0; i < nIndices; i++)
		indices[i] = indexLookup[triangles[i]];

	// setup a new index buffer
	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, nIndices, indices, sizeof(uint16)*nIndices);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	delete[] indices;
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
	Ptr<ManagedMesh> managedMesh = ResourceManager::Instance()->ManualCreateManagedResource(Mesh::RTTI, meshName).downcast<ManagedMesh>();
	Ptr<Mesh> mesh = managedMesh->GetResource().downcast<Mesh>();
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(this->indexBuffer);
	mesh->SetPrimitiveGroups(this->primGroups);
}

} // namespace Models
