//------------------------------------------------------------------------------
//  
//  
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/Terrain.h"
//#include "wow/world/Terrain/chunknode.h"
#include "models/nodes/shapenode.h"
#include "models/attributes.h"
#include "wow/world/worldmanager.h"
#include "coregraphics/mesh.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "resources/sharedresourceserver.h"

#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "coregraphics/texture.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/streammeshloader.h"

#include "CoreGraphics/debugview.h"

namespace Models
{
ImplementClass(Models::Terrain, 'TERN', Models::Model);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace Math;
using namespace CoreGraphics;
using namespace WOW;

//------------------------------------------------------------------------------
/**
*/
Terrain::Terrain():
	curNode(0)
{
	SetupVertexComponents();
	ReadPrimitiveGroups();
}

//------------------------------------------------------------------------------
/**
*/
Terrain::~Terrain()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
Terrain::UpdateChunks(const Ptr<MapTile>& mapTile)
{
	// ���������chunks
	if (CreateChunkNode(mapTile->GetChunkData()))
	{
		SetState(Resource::Loaded);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
	����������chunk����
*/
bool
Terrain::CreateChunkNode(const Util::Array<ChunkData>& datas)
{
	bool isCreate = !(nodes.Size() == 256);
	String texBlend, meshName;

	DebugView::Instance()->Start();

	int loadNum = 0;
	for (IndexT i = curNode; i < datas.Size(); i++,curNode++)
	{
		if (loadNum++ > 2) 
		{
			DebugView::Instance()->Stop("CreateChunkNode for");
			return false;	// ����ÿ֡��������
		}

		const ChunkData& chunk = datas[i];
		Ptr<ShapeNode> newModelNode;
		if (!isCreate)
		{
			n_assert(nodes[i]->IsA(ShapeNode::RTTI));
			newModelNode = nodes[i].downcast<ShapeNode>();
			newModelNode->UnloadResources();
		}
		else
		{
			newModelNode = ShapeNode::Create();
		}

		if (!chunk.isEmpty) {
			texBlend = CreateTextureManual(chunk.blendbuf, chunk.bufSize);
		}
		//else
		//{
		//	if (this->defName.IsEmpty())
		//	{
		//		// ����һ�ſյ����������ظ����������û��ʹ�ã����ܽ����յ����������ͷŵ�ʱ��
		//		// ����Ϊ�������������
		//		unsigned char *blendbuf;
		//		blendbuf = new unsigned char[64*64*4];
		//		memset(blendbuf, 0, 64*64*4);
		//		this->defName = CreateTextureManual(blendbuf, 64*64*4);
		//	}

		//	texBlend = this->defName;
		//}


		// ���¶�����Ϣ
		meshName = UpdateVertexBuffer(newModelNode->GetManagedMesh(), chunk.vb, chunk.nor, chunk.vbSize);

		// �������
		newModelNode->SetString(Attr::TexBlend0, texBlend);
		newModelNode->SetString(Attr::DiffMap0, texBlend);	// ���û�е�����ͼ��Ĭ��ֵ����
		
		String shaName = chunk.shaderName;
		if (chunk.tex0.Length() > 0)	// ��1������
			newModelNode->SetString(Attr::DiffMap0, chunk.tex0);
		if (chunk.tex1.Length() > 0)
			newModelNode->SetString(Attr::DiffMap1, chunk.tex1);
		if (chunk.tex2.Length() > 0)
			newModelNode->SetString(Attr::DiffMap2, chunk.tex2);
		if (chunk.tex3.Length() > 0)
			newModelNode->SetString(Attr::DiffMap3, chunk.tex3);

		newModelNode->SetString(Attr::Shader, shaName);
		
		newModelNode->SetFloat4(Attr::BoxCenter, chunk.box.center());
		newModelNode->SetFloat4(Attr::BoxExtents, chunk.box.extents());

		newModelNode->SetString(Attr::MeshResourceId, meshName);
		newModelNode->SetInt(Attr::MeshGroupIndex, 0);

		newModelNode->SetFloat4(Attr::Position, chunk.pos);


		String objName;
		objName.AppendInt(i);

		newModelNode->SetName(objName);

		Attr::AttributeContainer attr = newModelNode->GetAttrs();
		newModelNode->LoadFromAttrs(attr);

		this->AttachNode(newModelNode.upcast<ModelNode>());
	}

	DebugView::Instance()->Stop("CreateChunkNode for");


	DebugView::Instance()->Start();
	LoadResources();
	DebugView::Instance()->Stop("CreateChunkNode load");

	return true;
}

//------------------------------------------------------------------------------
/**
	�ֶ�������Ҫ����������alpha��shadow

	��ʱ����д�ɣ�ֱ����DX9�ˡ�Ӧ����չһ��texture�࣬ʵ���ֶ��������ܡ�
*/
const String 
Terrain::CreateTextureManual(const void* srcData, int srcDataSize)
{
	HRESULT hr;
	IDirect3DDevice9* d3d9Device = Direct3D9::D3D9RenderDevice::Instance()->GetDirect3DDevice();
	n_assert(0 != d3d9Device);
	IDirect3DTexture9* d3d9Texture = 0;
	hr = d3d9Device->CreateTexture(64, 64, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d3d9Texture, NULL);
	//hr = D3DXCreateTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9Texture);
	if (FAILED(hr))
	{
		n_error("CreateTextureManual: CreateTexture() failed!");
		//return NULL;
	}

	D3DLOCKED_RECT lr;
	unsigned char *pixel = 0;

	d3d9Texture->LockRect(0, &lr, NULL, 0);
	pixel = (unsigned char*)lr.pBits;
	Memory::Copy(srcData, pixel, srcDataSize);
	d3d9Texture->UnlockRect(0);

	// ��ͬ��StateNodeʹ��ͬһ����������������ͬ��ʵ�ֹ���
	static int totalM2Model = 0;
	String texName = "Tex_";
	texName.AppendInt((int)this);
	texName.AppendInt(totalM2Model);
	totalM2Model++;


	// �ֶ�������Դ�����������
	Ptr<Texture> texture = SharedResourceServer::Instance()->CreateSharedResource(texName, Texture::RTTI, StreamTextureLoader::RTTI).downcast<Texture>();
	texture->SetState(Resource::Loaded);
	texture->SetupFromD3D9Texture(d3d9Texture);

	/*char szout[256];
	sprintf(szout, "d:\\%s", texName.AsCharPtr());
	D3DXSaveTextureToFile(szout, D3DXIFF_TGA, d3d9Texture, NULL);*/

	return texName;
}

//------------------------------------------------------------------------------
/**
*/
void
Terrain::ReadPrimitiveGroups()
{
	n_assert(this->primGroups.IsEmpty());

	IndexT vstart, vcount, istart, icount;
	PrimitiveGroup primGroup;

	vstart = 0;
	vcount = 9*9+8*8;
	istart = 0;
	icount = samplerstripsize;//stripsize;

	primGroup.SetBaseVertex(vstart);
	primGroup.SetNumVertices(vcount);
	primGroup.SetBaseIndex(istart);   // firstTriangle
	primGroup.SetNumIndices(icount);   // numTriangles

	primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	this->primGroups.Append(primGroup);
}

//------------------------------------------------------------------------------
/**
*/
void
Terrain::SetupVertexComponents()
{
	n_assert(this->vertexComponents.IsEmpty());

	this->vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	this->vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	this->vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	this->vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 1, VertexComponent::Float2));
}

//------------------------------------------------------------------------------
/**
*/
const String
Terrain::UpdateVertexBuffer(const Ptr<ManagedMesh>& managMesh, const vector* vb, const vector* nor, int vertices)
{
	//n_assert(!this->vertexBuffer.isvalid());
	n_assert(this->vertexComponents.Size() > 0);

	struct vfvf 
	{
		float x,y,z;
		float nx,ny,nz;
		float2 tex;
		float2 texblend;
	};

	vfvf *dataPtr = n_new_array(vfvf, vertices);
	float2* texCoord = WorldManager::Instance()->GetDetailTexCoord();
	float2* alphaCoord = WorldManager::Instance()->GetAlphaTexCoord();

	for (int i = 0; i < vertices; i++)
	{
		dataPtr[i].x = vb[i].x();
		dataPtr[i].y = vb[i].y();
		dataPtr[i].z = vb[i].z();
		dataPtr[i].tex = texCoord[i];
		dataPtr[i].texblend = alphaCoord[i];

		vector vec = float4::normalize(nor[i]);
		dataPtr[i].nx = vec.x();
		dataPtr[i].ny = vec.y();
		dataPtr[i].nz = vec.z();
	}

	Ptr<Mesh> mesh;
	if (managMesh.isvalid() && managMesh->GetMesh().isvalid())
	{
		mesh = managMesh->GetMesh();
	}
	else
	{
		CreateMesh(mesh);
	}
	n_assert(mesh.isvalid());
	const Ptr<CoreGraphics::VertexBuffer>& vertexbuf = mesh->GetVertexBuffer();
	vfvf* vertexPointer = (vfvf*)vertexbuf->Map(CoreGraphics::VertexBuffer::MapWrite);
	Memory::Copy(dataPtr, vertexPointer, vertices*sizeof(vfvf));
	mesh->GetVertexBuffer()->Unmap();
	n_delete_array(dataPtr);

	return mesh->GetResourceId().Value();
}



//------------------------------------------------------------------------------
/**
���е�ModelNode��������ͬ�Ķ��㣬ֻ��PrimativeGroupIndex��ͬ��
*/
void
Terrain::CreateMesh(Ptr<Mesh>& mesh)
{
	static int totalMesh = 0;
	String meshName = "ck_";
	meshName.AppendInt((int)this);
	meshName.AppendInt(totalMesh++);

	// ��������
	const Ptr<CoreGraphics::IndexBuffer>& indexBuffer = WorldManager::Instance()->GetIndexBuffer();

	// �ֶ�������Դ�����������
	mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, CoreGraphics::StreamMeshLoader::RTTI).downcast<Mesh>();

	//calculate vertex width (numbytes/float)
	SizeT vcount = 8*8+9*9;
	SizeT numComponentBytes = 0;
	SizeT i = 0;
	for(;i < vertexComponents.Size(); i++)
	{
		numComponentBytes += vertexComponents[i].GetByteSize();
	}

	SizeT vertexWidth = numComponentBytes/sizeof(float);

	Ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents, 
		vcount, 
		vcount * vertexWidth * sizeof(float),
		CoreGraphics::VertexBuffer::UsageStaging, 
		CoreGraphics::VertexBuffer::AccessWrite);
	vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	vertexBuffer->Load();
	vertexBuffer->SetLoader(0);
	n_assert(vertexBuffer->GetState() == VertexBuffer::Loaded);

	
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetVertexBuffer(vertexBuffer);
	mesh->SetPrimitiveGroups(primGroups);
	mesh->SetState(Resource::Loaded);
}

} // namespace Models
