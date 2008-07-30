//------------------------------------------------------------------------------
//  modelnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainnode.h"
#include "terrain/terrainnodeinstance.h"
#include "terrain/worldserver.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/streamtextureloader.h"
#include "resources/managedtexture.h"
#include "util/string.h"
#include "math/vector.h"
#include "math/float2.h"
#include "models/attributes.h"


#include "coregraphics/MemoryVertexBufferLoader.h"
#include "coregraphics/MemoryIndexBufferLoader.h"
#include "resources/resourceloader.h"
#include "resources/SharedResourceServer.h"
#include "coregraphics/StreamMeshLoader.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNode, 'TNNE', Models::StateNode  );

using namespace CoreGraphics;
using namespace Models;
using namespace Util;
using namespace IO;
using namespace Math;
using namespace Resources;

bool TerrainNode::coordCreated = false;
Math::float2 TerrainNode::texCoord[mapbufsize];
Math::float2 TerrainNode::alphaCoord[mapbufsize];

//------------------------------------------------------------------------------
/**
*/
TerrainNode::TerrainNode():
loaded(false),
loadDetail(false),
blendbuf(0),
dataBuf(0),
nTextures(0),
areaId(0),
x(0),
z(0),
vertexOffset(-1)
{
	if (!TerrainNode::coordCreated)
		TerrainNode::InitGlobalVBOs();
}

//------------------------------------------------------------------------------
/**
*/
TerrainNode::~TerrainNode()
{
	if (blendbuf)
		n_delete_array(blendbuf);
	if (dataBuf)
		n_delete_array(dataBuf);
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::ApplySharedState()
{
	StateNode::ApplySharedState();
    
    String feature;
	feature.Format("Terrain%d", this->layer);

	ShaderServer* shdServer = ShaderServer::Instance();
	shdServer->SetFeatureBits(shdServer->FeatureStringToMask(feature));

    WorldServer::Instance()->ApplyCache();
    RenderDevice::Instance()->SetPrimitiveGroup(this->primGroup);
    //this->mesh->ApplyPrimitives(0);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
TerrainNode::CreateNodeInstance() const
{
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) TerrainNodeInstance::Create();
	return newInst;
}

void
TerrainNode::LoadResource()
{
    StateNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
	渲染前设置渲染所需的数据
	可以在判断可见的时候就调用这个函数
*/
void 
TerrainNode::AddToRender()
{
	if (!this->loaded)
		return;

    // 设置顶点
    if (this->vertexOffset == -1)
    {
        this->vertexOffset = WorldServer::Instance()->GetChunkCacha()->AddChunk(dataBuf);
        n_assert(this->vertexOffset != -1);
        SetVertexOffsetInCache(this->vertexOffset);
    }

    // 设置纹理
	if (!loadDetail)
	{
		String blendName = CreateBlendTexture(blendbuf, 64*64*4);
		this->SetString(Attr::TexBlend0, blendName);
		
		LoadResources();
        loadDetail = true;

        CreateTemp();
	}

    //if (!this->shaderInstance.isvalid())
    //    this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(Resources::ResourceId("shd:terrain"));
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::FreeVertexOffset()
{
    if (this->vertexOffset != -1)
    {
        WorldServer::Instance()->GetChunkCacha()->FreeChunk(this->vertexOffset);
        this->vertexOffset = -1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::InitGlobalVBOs()
{
	float2 *vt;
	float tx,ty;

	// init texture coordinates for detail map:
	// 一个MapChunk的纹理坐标，细节纹理  9*9+8*8
	vt = texCoord;
	const float detail_half = 0.5f * detail_size / 8.0f;
	for (int j=0; j<17; j++) {
		for (int i=0; i<((j%2)?8:9); i++) {
			tx = detail_size / 8.0f * i;
			ty = detail_size / 8.0f * j * 0.5f;
			if (j%2) {
				// offset by half
				tx += detail_half;
			}
			*vt++ = float2(tx, ty);
		}
	}



	// init texture coordinates for alpha map:
	// alpha纹理
	vt = alphaCoord;
	const float alpha_half = 0.5f * 1.0f / 8.0f;
	for (int j=0; j<17; j++) {
		for (int i=0; i<((j%2)?8:9); i++) {
			tx = 1.0f / 8.0f * i;
			ty = 1.0f / 8.0f * j * 0.5f;
			if (j%2) {
				// offset by half
				tx += alpha_half;
			}
			//*vt++ = float2(tx*0.95f, ty*0.95f);

			/*const int divs = 32;
			const float inv = 1.0f / divs;
			const float mul = (divs-1.0f);
			*vt++ = float2(tx*(mul*inv), ty*(mul*inv));*/

			if (tx == 0.0f)
				tx += 0.02f;
			else
				tx *= 0.98f;

			if (ty == 0.0f)
				ty += 0.02f;
			else
				ty *= 0.98f;
			*vt++ = float2(tx, ty);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNode::ParseData(const Ptr<Stream>& stream, SizeT offset)
{
    n_assert(stream.isvalid());
    n_assert(this->dataBuf == NULL);
	n_assert(!this->loaded);

	FourCC fourcc;
	SizeT size;
	int lastpos;

	stream->Seek(offset, Stream::Begin);
	stream->Seek(4, Stream::Current);
	stream->Read(&size, 4);
	lastpos = stream->GetPosition()+size;

	MapChunkHeader header;
	stream->Read(&header, 0x80);

	this->x = header.ix;
	this->z = header.iy;
	this->posBase = vector(float(this->x) * CHUNKSIZE/*header.xpos-1.0f + ZEROPOINT*/, header.ypos, float(this->z) * CHUNKSIZE/*header.zpos-1.0f + ZEROPOINT*/);
	this->areaId = header.areaid;
	this->layer = header.nLayers;
	

    this->dataBuf = new TerrainChunkFVF[mapbufsize];

    vector vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
	vector vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);

	
	blendbuf = n_new_array(unsigned char, 64*64*4); // 创建四张贴图，其中前三张为alpha map,最后一张shadow map
	memset(blendbuf, 0, 64*64*4);

	while (stream->GetPosition() < lastpos) {
		stream->Read(&fourcc,4);
		stream->Read(&size, 4);

        Sleep(0);

		size_t nextpos = stream->GetPosition() + size;

		if (fourcc == 'MCNR') {
			nextpos = stream->GetPosition() + 0x1C0; // size fix
			// normal vectors
			char nor[3];
			//vector *ttn = tn;
            TerrainChunkFVF* ttn = this->dataBuf;
			for (int j=0; j<17; j++) {
				for (int i=0; i<((j%2)?8:9); i++) {
					stream->Read(nor,3);
					// order Z,X,Y ?
					//*ttn++ = vector((float)nor[0]/127.0f, (float)nor[2]/127.0f, (float)nor[1]/127.0f);
                    //*ttn++ = vector(-(float)nor[1]/127.0f, (float)nor[2]/127.0f, -(float)nor[0]/127.0f);
					vector vec = float4::normalize(vector(-(float)nor[1]/127.0f, (float)nor[2]/127.0f, -(float)nor[0]/127.0f));
                    ttn->nx = vec.x();
                    ttn->ny = vec.y();
                    ttn->nz = vec.z();
                    ttn++;
				}
			}
		}
		else if (fourcc == 'MCVT') {
            TerrainChunkFVF* ttv = this->dataBuf;

			// vertices
			for (int j=0; j<17; j++) {
				for (int i=0; i<((j%2)?8:9); i++) {
					float h,xpos,zpos;
					stream->Read(&h,4);
					xpos = i * UNITSIZE;
					zpos = j * 0.5f * UNITSIZE;
					if (j%2) {
						xpos += UNITSIZE*0.5f;
					}
					//vector v = vector(/*xbase+*/xpos, /*ybase+*/h, /*zbase+*/zpos);
					//*ttv++ = v;
                    ttv->x = this->posBase.x() + xpos;
                    ttv->y = this->posBase.y() + h;
                    ttv->z = this->posBase.z() + zpos;

					if (ttv->y < vmin.y()) vmin.y() = ttv->y;
					if (ttv->y > vmax.y()) vmax.y() = ttv->y;
                    
                    ttv++;
				}
			}

			vmin.y() += posBase.y();
			vmin.x() = posBase.x();
			vmin.z() = posBase.z();
			vmax.y() += posBase.y();
			vmax.x() = posBase.x() + 8 * UNITSIZE;
			vmax.z() = posBase.z() + 8 * UNITSIZE;
			//r = (vmax - vmin).length() * 0.5f;

		}
		else if (fourcc == 'MCLY') {
			// texture info
			nTextures = (int)size / 16;
			const Ptr<TerrainTile> &tile = this->GetModel().downcast<TerrainTile>();
			for (int i=0; i<nTextures; i++) {
				int tex, flags;
				stream->Read(&tex,4);
				stream->Read(&flags, 4);

				stream->Seek(8, Stream::Current);

				flags &= ~0x100;

				if (flags & 0x80) {
					animated[i] = flags;
				} else {
					animated[i] = 0;
				}

				//texId[i] = tex;
				if (i >= 0)
                    this->SetString(Attr::DiffMap0, tile->GetTextureName(tex));
                if (i >= 1)
                    this->SetString(Attr::DiffMap1, tile->GetTextureName(tex));
                if (i >= 2)
                    this->SetString(Attr::DiffMap2, tile->GetTextureName(tex));
                if (i >= 3)
                    this->SetString(Attr::DiffMap3, tile->GetTextureName(tex));
			}
		}
		else if (fourcc == 'MCSH') {
			// shadow map 64 x 64
			unsigned char sbuf[64*64], *p, c[8];
			p = sbuf;
			for (int j=0; j<64; j++) {
				stream->Read(c,8);
				for (int i=0; i<8; i++) {
					for (int b=0x01; b!=0x100; b<<=1) {
						*p++ = (c[i] & b) ? 85 : 0;         // 85 表示阴影深度（值越大越黑）
					}
				}
			}
			
			for (int p=0; p<64*64; p++) 
				blendbuf[p*4+3] = sbuf[p];
		}
		else if (fourcc == 'MCAL') {
			// alpha maps  64 x 64
			if (nTextures>0) {

				char* bufMap;
				int pos = stream->GetPosition();
				bufMap = (char*)stream->Map();
				//				glGenTextures(nTextures-1, alphamaps);
				for (int i=0; i<nTextures-1; i++) {
					//					glBindTexture(GL_TEXTURE_2D, alphamaps[i]);
					unsigned char amap[64*64], *p;

					char *abuf = (char*)(bufMap + pos);
					p = amap;
					for (int j=0; j<64; j++) {
						for (int i=0; i<32; i++) {
							unsigned char c = *abuf++;
							*p++ = (c & 0x0f) << 4;
							*p++ = (c & 0xf0);
						}

					}
					
					for (int p=0; p<64*64; p++) {
						//blendbuf[p*4+i] = amap[p];

						if (i==0) blendbuf[p*4+2] = amap[p];  // r
						if (i==1) blendbuf[p*4+1] = amap[p];  // g
						if (i==2) blendbuf[p*4+0] = amap[p];  // b
					}
					

					pos += 0x800;
					//stream->Seek(0x800, Stream::Current);
				}
				stream->Unmap();

			} 
            else {
				 //some MCAL chunks have incorrect sizes! :(
				continue;
			}
		}
		else if (fourcc == 'MCLQ') {
			// liquid / water level
			//FourCC fcc1;
			//stream->Read(fcc1,4);
			//
			//if (fcc1 == 'MCSE') {
			//	haswater = false;
			//}
			//else {
			//	haswater = true;
			//	stream->Seek(-4, Stream::Current);
			//	stream->Read(&waterlevel,4);

			//	if (waterlevel > vmax.y) vmax.y = waterlevel;
			//	if (waterlevel < vmin.y) haswater = false;

			//	stream->Seek(4, Stream::Current);

			//	//lq = new Liquid(8, 8, vector(xbase, waterlevel, zbase));
			//	////lq->init(f);
			//	//lq->initFromTerrain(f, chunkflags);
			//}
			// we're done here!
			break;
		}
		stream->Seek((int)nextpos, Stream::Begin);
	}

	vector vcenter = (vmin + vmax) * 0.5f;
    this->SetFloat4(Attr::BoxCenter, vcenter);
    this->SetFloat4(Attr::BoxExtents, (vmax - vmin) * 0.5f);

	// 可以直接放到GPU中计算，减少CPU时间!!!
	for (int i = 0; i < mapbufsize; i++)
	{
		this->dataBuf[i].tex = texCoord[i];
		this->dataBuf[i].texblend = alphaCoord[i];
	}

    this->SetString(Attr::Shader, "shd:terrain");

    this->loaded = true;
}

//------------------------------------------------------------------------------
/**
	手动建立需要填充的纹理，如alpha、shadow
*/
const String
TerrainNode::CreateBlendTexture(void* srcData, SizeT srcNum)
{
	String texName = "Tex_";
	texName.AppendInt((int)this);

	Ptr<Texture> texture = StreamTextureLoader::CreateTexture(texName, 64, 64, 0, PixelFormat::A8R8G8B8, NULL, 0);

	Texture::MapInfo info;
	texture->Map(0, Texture::MapWriteDiscard, info);
    if (info.data != NULL)
    {
	    Memory::Copy(srcData, info.data, srcNum);
	    texture->Unmap(0);
    }

	return texName;
}

void 
TerrainNode::CreateTemp()
{
 //   Util::Array<CoreGraphics::VertexComponent> vertexComponents;
 //   if (vertexComponents.Size() == 0)
	//{
	//	vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	//	vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	//	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	//	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 1, VertexComponent::Float2));
	//}

 //   // setup new vertex buffer
	//Ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create();
	//Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	//vbLoader->Setup(vertexComponents, 8*8+9*9, dataBuf, (8*8+9*9) * sizeof(TerrainChunkFVF));
	//vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	//vertexBuffer->Load();
	//vertexBuffer->SetLoader(0);
	//n_assert(vertexBuffer->GetState() == VertexBuffer::Loaded);


 //   uint16 indexBufferData[samplerstripsize];
 //   for (int j = 0; j < 8; j++)
	//{
	//	for (int i = 0; i < 8; i++)
	//	{
	//		indexBufferData[(j*8+i)*12+0] = j*9+j*8+i;
	//		indexBufferData[(j*8+i)*12+1] = (j+1)*9+j*8+i;
	//		indexBufferData[(j*8+i)*12+2] = (j+1)*9+(j+1)*8+i;

	//		indexBufferData[(j*8+i)*12+3] = j*9+j*8+i;
	//		indexBufferData[(j*8+i)*12+4] = j*9+j*8+i+1;
	//		indexBufferData[(j*8+i)*12+5] = (j+1)*9+j*8+i;

	//		indexBufferData[(j*8+i)*12+6] = (j+1)*9+j*8+i;
	//		indexBufferData[(j*8+i)*12+7] = j*9+j*8+i+1;
	//		indexBufferData[(j*8+i)*12+8] = (j+1)*9+(j+1)*8+i+1;

	//		indexBufferData[(j*8+i)*12+9] = (j+1)*9+(j+1)*8+i;
	//		indexBufferData[(j*8+i)*12+10] = (j+1)*9+j*8+i;
	//		indexBufferData[(j*8+i)*12+11] = (j+1)*9+(j+1)*8+i+1;
	//	}
	//}

 //   Ptr<IndexBuffer> indexBuffer = IndexBuffer::Create();
	//Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	//ibLoader->Setup(IndexType::Index16, samplerstripsize, indexBufferData, sizeof(uint16)*samplerstripsize);
	//indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	//indexBuffer->Load();
	//indexBuffer->SetLoader(0);
	//n_assert(indexBuffer->GetState() == IndexBuffer::Loaded);





 //   Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	//IndexT vstart, vcount, istart, icount;
	//PrimitiveGroup primGroup;

	//vstart = 0;
	//vcount = 9*9+8*8;
	//istart = 0;
	//icount = samplerstripsize;//stripsize;

	//primGroup.SetBaseVertex(vstart);
	//primGroup.SetNumVertices(vcount);
	//primGroup.SetBaseIndex(istart);   // firstTriangle
	//primGroup.SetNumIndices(icount);   // numTriangles

	//primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	//primGroups.Append(primGroup);


 //   static int totalM2Model = 0;
	//String meshName = "ck_";
	//meshName.AppendInt(totalM2Model);
	//totalM2Model++;

 //   this->mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	//mesh->SetState(Resource::Loaded);
	//mesh->SetVertexBuffer(vertexBuffer);
	//mesh->SetIndexBuffer(indexBuffer);
	//mesh->SetPrimitiveGroups(primGroups);
}

} // namespace Models