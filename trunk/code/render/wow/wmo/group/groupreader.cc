//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/wmo/group/groupreader.h"
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
#include "wow/wmo/wmoserver.h"
#include "resources/sharedresourceserver.h"
#include "coregraphics/streammeshloader.h"
#include "graphics/sorting/wmobspnode.h"
#include "graphics/sorting/bspserver.h"

namespace WOW
{
ImplementClass(WOW::WMOGroupReader, 'GRPR', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Attr;
using namespace System;
using namespace Models;
using namespace CoreGraphics;
using namespace Resources;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
WMOGroupReader::WMOGroupReader() :
    isSetToModel(false),
    isSetToModelNode(false),
	curBatch(0),
	vertexBuffer(0),
	indexBuffer(0),
	batches(0),
	wmo(0),
	bspIndices(0),
	bsps(0),
	numBspIndices(0),
	numBsp(0),
	group(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WMOGroupReader::~WMOGroupReader()
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
WMOGroupReader::Open()
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

	this->isSetToModel = false;
	this->isSetToModelNode = false;

    if (StreamReader::Open())
    {
		// 找到当前group归属哪个wmo
		String tmp = modelResId.Value();
		String wmoName;
		wmoName.AppendRange(tmp.AsCharPtr(), tmp.Length()-8);
		wmoName.Append(".wmo");
		this->wmo = WMOServer::Instance()->LookupManagedWMO(wmoName)->GetModel();

		this->group = this->wmo->GetGroup(modelResId);
		n_assert(NULL != this->group);



		//GraphicsServer* gfxServer = GraphicsServer::Instance();
		//Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

		//gc = GroupCell::Create();
		//gc->SetBoundingBox(boundingBox);
		//gc->OnAttachToStage(stage);
		//stage->GetRootCell()->AttachChildCell(gc.upcast<Cell>()); // hardcord
		//
		//group->SetGroupCell(gc);



		this->ReadGroup();
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
WMOGroupReader::Close()
{
    n_assert(this->IsOpen());

	this->primGroups.Clear();
	this->vertexComponents.Clear();

	if (NULL != bsps)
	{
		n_delete_array(bsps);
		bsps = NULL;
	}
	if (NULL != bspIndices)
	{
		n_delete_array(bspIndices);
		bspIndices = NULL;
	}

    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
WMOGroupReader::FillModel()
{
	n_assert(this->model.isvalid());

	this->SetToFirstModel();
	// go thru hierarchy and build modelnodes
	if (this->SetToFirstModelNode()) do
	{}
	while(this->SetToNextModelNode());

	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WMOGroupReader::SetToFirstModel()
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
WMOGroupReader::SetToNextModel()
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
WMOGroupReader::SetToFirstModelNode()
{
    n_assert(this->isOpen);
    n_assert(this->isSetToModel);
    n_assert(!this->isSetToModelNode);

	this->curBatch = 0;
    this->isSetToModelNode = true;
    this->ReadModelNodeData();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WMOGroupReader::SetToNextModelNode()
{
    n_assert(this->isOpen);
    n_assert(this->isSetToModel);
    n_assert(this->isSetToModelNode);

	curBatch++;
	if (curBatch >= (int)this->nBatches)
		return false;

	this->ReadModelNodeData();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
WMOGroupReader::ReadModelData()
{
}

//------------------------------------------------------------------------------
/**
*/
void
WMOGroupReader::ReadModelNodeData()
{
	String texName, shaName;

	Ptr<ModelNode> newModelNode;
	newModelNode = ShapeNode::Create();
	//this->modelNodeRtti = &ShapeNode::RTTI;

	String objName = "wmo_g";
	objName.AppendInt(curBatch);
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


	//GLuint list = listbase + b;

	WMOBatch *batch = &this->batches[curBatch];
	const WMOMaterial *mat = this->wmo->GetMaterial(batch->texture);

	//bool overbright = ((mat->flags & 0x10) && !hascv);
	//bool spec_shader = (mat->specular && !hascv && !overbright);

	/*pair<GLuint, int> currentList;
	currentList.first = list;
	currentList.second = spec_shader ? 1 : 0;*/


	// setup texture 纹理直接获取名字来加载
	//glBindTexture(GL_TEXTURE_2D, mat->tex);
	newModelNode->SetString(Attr::DiffMap0, this->wmo->GetTextureName(mat->tex));

	bool atest = (mat->transparent) != 0;

	/*if (atest) {
		glEnable(GL_ALPHA_TEST);
		float aval = 0;
		if (mat->flags & 0x80) aval = 0.3f;
		if (mat->flags & 0x01) aval = 0.0f;
		glAlphaFunc(GL_GREATER, aval);
	}*/

	if (atest)
		newModelNode->SetType(Models::ModelNodeType::Alpha);
		/*shaName = "shd:sampler_alphatest";
	else
		shaName = "shd:sampler";*/
	newModelNode->SetString(Attr::Shader, "shd:static");

	/*if (mat->flags & 0x04) glDisable(GL_CULL_FACE);
	else glEnable(GL_CULL_FACE);

	// 材质目前不支持
	if (spec_shader) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colorFromInt(mat->col2));
	} else {
		Vec4D nospec(0,0,0,1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nospec);
	}*/


	//if (overbright) {
	//	// TODO: use emissive color from the WMO Material instead of 1,1,1,1
	//	GLfloat em[4] = {1,1,1,1};
	//	glMaterialfv(GL_FRONT, GL_EMISSION, em);
	//}

	// render
	/*glBegin(GL_TRIANGLES);
	for (int t=0, i=batch->indexStart; t<batch->indexCount; t++,i++) {
		int a = indices[i];
		if (indoor && hascv) {
			setGLColor(cv[a]);
		}
		glNormal3f(normals[a].x, normals[a].z, -normals[a].y);
		glTexCoord2fv(texcoords[a]);
		glVertex3f(vertices[a].x, vertices[a].z, -vertices[a].y);
	}
	glEnd();*/

	

	/*if (overbright) {
		GLfloat em[4] = {0,0,0,1};
		glMaterialfv(GL_FRONT, GL_EMISSION, em);
	}

	if (atest) {
		glDisable(GL_ALPHA_TEST);
	}*/

	newModelNode->SetFloat4(Attr::BoxCenter, group->GetBoundBox().center());
	newModelNode->SetFloat4(Attr::BoxExtents, group->GetBoundBox().extents());

	newModelNode->SetString(Attr::MeshResourceId, meshName);
	newModelNode->SetInt(Attr::MeshGroupIndex, curBatch);

	Attr::AttributeContainer attr = newModelNode->GetAttrs();
	newModelNode->LoadFromAttrs(attr);

	this->model->AttachNode(newModelNode);
}

//------------------------------------------------------------------------------
/**
*/
void
WMOGroupReader::ReadPrimitiveGroups()
{
	n_assert(this->primGroups.IsEmpty());

	for (int i = 0; i < nBatches; i++)
	{
		WMOBatch *batch = &this->batches[i];
		PrimitiveGroup primGroup;

		// Primitive Group
		primGroup.SetBaseVertex(batch->vertexStart);
		primGroup.SetNumVertices(batch->vertexEnd - batch->vertexStart);
		primGroup.SetBaseIndex(batch->indexStart);   // firstTriangle
		primGroup.SetNumIndices(batch->indexCount);   // numTriangles

		primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		this->primGroups.Append(primGroup);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
WMOGroupReader::SetupVertexComponents()
{
	n_assert(this->vertexComponents.IsEmpty());

	this->vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
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
WMOGroupReader::UpdateGroupBoundingBoxes()
{
}

//------------------------------------------------------------------------------
/**
	所有的ModelNode都是用相同的顶点，只是PrimativeGroupIndex不同。
*/
void 
WMOGroupReader::SetupManagedMesh()
{
	// 相同的ShapeNode使用同一组VB & IB所以名字相同，实现共享
	static int totalM2Model = 0;
	meshName = "wmo_";
	meshName.AppendInt(totalM2Model);
	totalM2Model++;


	// 手动建立资源，并填充数据
	//Ptr<ManagedMesh> managedMesh = ResourceManager::Instance()->ManualCreateManagedResource(Mesh::RTTI, meshName).downcast<ManagedMesh>();
	//Ptr<Mesh> mesh = managedMesh->GetResource().downcast<Mesh>();
	Ptr<Mesh> mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(this->indexBuffer);
	mesh->SetPrimitiveGroups(this->primGroups);
}

//------------------------------------------------------------------------------
/**
*/
void 
WMOGroupReader::ReadGroup()
{
	vector3 *vertices, *normals;
	float2 *texcoords;
	
	unsigned short *materials;
	//WMOBatch *batches;
	//int nBatches;

	WMOGroupHeader gh;

	short *useLights = 0;
	int nLR = 0;

	struct vfvf 
	{
		vector3 pos;
		vector3 nor;
		float2 tex;
	};

	vfvf *dataPtr = NULL;
	indices = NULL;

	// open group file
	/*char temp[256];
	strcpy(temp, wmo->name.c_str());
	temp[wmo->name.length()-4] = 0;

	char fname[256];
	sprintf(fname,"%s_%03d.wmo",temp, num);

	MPQFile gf(fname);*/
	this->stream->Seek(0x14, Stream::Begin);

	// read header
	this->stream->Read(&gh, sizeof(WMOGroupHeader));
	//WMOFog &wf = wmo->fogs[gh.fogs[0]];
	//if (wf.r2 <= 0) fog = -1; // default outdoor fog..?
	//else fog = gh.fogs[0];

	b1 = vector3(gh.box1[0], gh.box1[2], -gh.box1[1]);
	b2 = vector3(gh.box2[0], gh.box2[2], -gh.box2[1]);

	this->stream->Seek(0x58, Stream::Begin); // first chunk
	FourCC fourcc;
	size_t size;

	unsigned int *cv;
	hascv = false;

	while (!this->stream->Eof()) {
		this->stream->Read(&fourcc,4);
		this->stream->Read(&size, 4);

		size_t nextpos = this->stream->GetPosition() + size;

		// why copy stuff when I can just map it from memory ^_^

		if (fourcc == 'MOPY') {
			// materials per triangle
			nTriangles = (int)size / 2;
			materials = (unsigned short*)((char*)this->stream->Map() + this->stream->GetPosition());
		}
		else if (fourcc == 'MOVI') {
			// indices
			indices =  (uint16*)((char*)this->stream->Map() + this->stream->GetPosition());
			nIndices = (SizeT)(size / sizeof(uint16));
			//indices = n_new_array(uint16, nIndices);
			//Memory::Copy((char*)this->stream->Map() + this->stream->GetPosition(), indices, size);
		}
		else if (fourcc == 'MOVT') {
			nVertices = (int)size / 12;

			dataPtr = n_new_array(vfvf, nVertices);

			// let's hope it's padded to 12 bytes, not 16...
			vertices =  (vector3*)((char*)this->stream->Map() + this->stream->GetPosition());
			vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
			vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);
			rad = 0;
			for (SizeT i=0; i<nVertices; i++) {
				vector3 v(vertices[i].x, vertices[i].z, -vertices[i].y);
				if (v.x < vmin.x()) vmin.x() = v.x;
				if (v.y < vmin.y()) vmin.y() = v.y;
				if (v.z < vmin.z()) vmin.z() = v.z;
				if (v.x > vmax.x()) vmax.x() = v.x;
				if (v.y > vmax.y()) vmax.y() = v.y;
				if (v.z > vmax.z()) vmax.z() = v.z;

				v.set(vertices[i].x, vertices[i].z, -vertices[i].y);
				dataPtr[i].pos = v;
			}
			center = (vmax + vmin) * 0.5f;
			rad = (vmax-center).length();
		}
		else if (fourcc == 'MONR') {
			normals =  (vector3*)((char*)this->stream->Map() + this->stream->GetPosition());
			for (SizeT i = 0; i < nVertices; i++)
				dataPtr[i].nor = vector3(normals->x, normals->z, normals->y);
		}
		else if (fourcc == 'MOTV') {
			texcoords =  (float2*)((char*)this->stream->Map() + this->stream->GetPosition());
			for (SizeT i = 0; i < nVertices; i++)
				dataPtr[i].tex = texcoords[i];
		}
		else if (fourcc == 'MOLR') {
			nLR = (int)size / 2;
			useLights =  (short*)((char*)this->stream->Map() + this->stream->GetPosition());
		}
		else if (fourcc == 'MODR') {
			nDoodads = (int)size / 2;
			ddr = n_new_array(short, nDoodads);
			this->stream->Read(ddr, (int)size);
			group->SetDDR(ddr);
			group->SetDoodads(nDoodads);
		}
		else if (fourcc == 'MOBA') {
			nBatches = (int)size / 24;
			batches = (WMOBatch*)((char*)this->stream->Map() + this->stream->GetPosition());
		}
		else if (fourcc == 'MOCV') {
			hascv = true;
			cv = (unsigned int*)((char*)this->stream->Map() + this->stream->GetPosition());
		}
		else if (fourcc == 'MLIQ') {
			// liquids
			//WMOLiquidHeader hlq;
			//this->stream->Read(&hlq, 0x1E);

			//lq = new Liquid(hlq.A, hlq.B, Vec3D(hlq.pos.x, hlq.pos.z, -hlq.pos.y));
			//lq->initFromWMO(gf, wmo->mat[hlq.type], (flags&0x2000)!=0);
		}
		else if (fourcc == 'MOBN')
		{
			numBsp = (SizeT)(size / sizeof(t_BSP_NODE));
			bsps = n_new_array(t_BSP_NODE, numBsp);
			this->stream->Read(bsps, (SizeT)size);
		}
		else if (fourcc == 'MOBR')
		{
			numBspIndices = (SizeT)(size / sizeof(unsigned short));
			bspIndices = n_new_array(unsigned short, numBspIndices);
			this->stream->Read(bspIndices, (SizeT)size);
		}

		// TODO: figure out/use MFOG ?

		if (this->stream->IsMapped())
			this->stream->Unmap();
		this->stream->Seek((int)nextpos, Stream::Begin);
	}

	// ok, make a display list

	indoor = (flags&8192)!=0;

	//initLighting(nLR,useLights);


	this->ReadPrimitiveGroups();
	this->SetupVertexComponents();

	//////////////////////////////////////////////////////////////////////////
	// setup new vertex buffer
	this->vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(this->vertexComponents, nVertices, dataPtr, nVertices * sizeof(vfvf));
	this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->vertexBuffer->Load();
	this->vertexBuffer->SetLoader(0);
	n_delete_array(dataPtr);
	n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// setup a new index buffer
	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, nIndices, indices, sizeof(unsigned short)*nIndices);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	//n_delete_array(indices);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);
	//////////////////////////////////////////////////////////////////////////

	//URI name = this->stream->GetURI();
	//this->wmo = WMOServer::Instance()->LookupWMO(name.LocalPath());


	// 创建BSP
	if (0&&this->numBsp > 0 && this->numBspIndices > 0)
	{
		Ptr<BspNode> bspRoot = BspNode::Create();
		BuildBsp(bspRoot, 0);

		BspServer::Instance()->AddWMOBsp(modelResId, bspRoot);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
WMOGroupReader::BuildBsp(Ptr<BspNode>& parentNode, IndexT index)
{

	if (bsps[index].children[0] > 0)
	{
		Ptr<BspNode> bsp = CreateBspNode(index);
		parentNode->SetNegChild(bsp);
		BuildBsp(bsp, bsps[index].children[0]);
	}
	if (bsps[index].children[1] > 0)
	{
		Ptr<BspNode> bsp = CreateBspNode(index);
		parentNode->SetPosChild(bsp);
		BuildBsp(bsp, bsps[index].children[1]);
	}

	if (bsps[index].firstface > 0 && bsps[index].numfaces > 0)
	{
		unsigned short indexPos;
		
		Array<unsigned short> ib;
		for (unsigned short i = 0; i < bsps[index].numfaces; i++)
		{
			indexPos = bspIndices[bsps[index].firstface+i] * 3;
			ib.Append(indices[indexPos]);
		}
		parentNode.downcast<WMOBspNode>()->AddNodeData(ib);
	}
}

//------------------------------------------------------------------------------
/**
*/
Ptr<BspNode>
WMOGroupReader::CreateBspNode(IndexT index)
{
	Ptr<BspNode> bsp = NULL;

	IndexT subIndex = bsps[index].children[0];
	if (bsps[subIndex].children[0] > 0 || bsps[subIndex].children[1] > 0)
	{
		bsp = BspNode::Create();
	}
	else
	{
		// 根节点
		Ptr<WMOBspNode> node = WMOBspNode::Create();
		bsp = node.upcast<BspNode>();
	}

	return bsp;
}

} // namespace Models
