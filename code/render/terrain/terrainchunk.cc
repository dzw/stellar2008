//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "Terrain/Terrainchunkentity.h"
#include "coregraphics/shaperenderer.h"
#include "resources/resourcemanager.h"
#include "wow/world/streamworldloader.h"

#include "timing/timer.h"
#include "coregraphics/debugview.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainChunk, 'TNCK', Core::RefCount);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Resources;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
TerrainChunk::TerrainChunk()
{
    //this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:terrain"));
    this->diffMap[0] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
    this->diffMap[1] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap1"));
    this->diffMap[2] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap2"));
    this->diffMap[3] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap3"));
    this->diffMap[4] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("TexBlend0"));
    for (IndexT i = 0; i < 5; i++)
        this->tex[i] = 0;
}

//------------------------------------------------------------------------------
/**
*/
TerrainChunk::~TerrainChunk()
{
}

void
TerrainChunk::LoadResource()
{
    // create a managed mesh resource
	ResourceId meshResId = this->GetString(Attr::MeshResourceId);
	this->primGroupIndex = this->GetInt(Attr::MeshGroupIndex);
	this->managedMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, meshResId).downcast<ManagedMesh>();
	n_assert(this->managedMesh.isvalid());
	DebugView::Instance()->Stop("ChunkNode load", true);

    // create texture
    ResourceId resId;
    
    // 纹理不能这样直接创建！使用纹理缓冲。
	if (this->HasAttr(Attr::DiffMap0))
	{
		resId = this->GetString(Attr::DiffMap0);
		tex[0] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
	if (this->HasAttr(Attr::DiffMap1))
	{
		resId = this->GetString(Attr::DiffMap1);
		tex[1] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
	if (this->HasAttr(Attr::DiffMap2))
	{
		tex[2] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
		resId = this->GetString(Attr::DiffMap2);
	}
	if (this->HasAttr(Attr::DiffMap3))
	{
		resId = this->GetString(Attr::DiffMap3);
		tex[3] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}

	if (this->HasAttr(Attr::TexBlend0))
	{
		resId = this->GetString(Attr::TexBlend0);
		tex[4] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
}

void
TerrainChunk::Init(const Ptr<Stream>& stream, const Ptr<TerrainTile>& tile)
{
	bool supportShaders = true;
	bool createAlphaTex = false;

	vector tn[mapbufsize], tv[mapbufsize];

	//stream->Seek((int)mcnk_offsets[x*16+y], Stream::Begin);
	stream->Seek(4, Stream::Current);

	Util::FourCC fourcc;
	int size;
	int texId[4];

	stream->Read(&size, 4);

	// okay here we go ^_^
	int lastpos = stream->GetPosition() + size;

	//char header[0x80];
	MapChunkHeader header;
	stream->Read(&header, 0x80);

	areaID = header.areaid;

	zbase = header.zpos;
	xbase = header.xpos;
	ybase = header.ypos;

	int holes = header.holes;
	int chunkflags = header.flags;

	hasholes = (holes != 0);

	// correct the x and z values ^_^
	zbase = zbase*-1.0f + ZEROPOINT;
	xbase = xbase*-1.0f + ZEROPOINT;

	//zbase = float(x) * CHUNKSIZE;//+1000.0f;
	//xbase = float(y) * CHUNKSIZE;//+1000.0f;
	//ybase = 0;


	vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
	vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);

	unsigned char *blendbuf;
	if (supportShaders) {
		blendbuf = n_new_array(unsigned char, 64*64*4);
		memset(blendbuf, 0, 64*64*4);
	}

	while (stream->GetPosition() < lastpos) {
		stream->Read(&fourcc,4);
		stream->Read(&size, 4);

        Sleep(0);

		size_t nextpos = stream->GetPosition() + size;

		if (fourcc == 'MCNR') {
			nextpos = stream->GetPosition() + 0x1C0; // size fix
			// normal vectors
			char nor[3];
			vector *ttn = tn;
			for (int j=0; j<17; j++) {
				for (int i=0; i<((j%2)?8:9); i++) {
					stream->Read(nor,3);
					// order Z,X,Y ?
					//*ttn++ = vector((float)nor[0]/127.0f, (float)nor[2]/127.0f, (float)nor[1]/127.0f);
					*ttn++ = vector(-(float)nor[1]/127.0f, (float)nor[2]/127.0f, -(float)nor[0]/127.0f);
				}
			}
		}
		else if (fourcc == 'MCVT') {
			vector *ttv = tv;

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
					vector v = vector(/*xbase+*/xpos, /*ybase+*/h, /*zbase+*/zpos);
					*ttv++ = v;
					if (v.y() < vmin.y()) vmin.y() = v.y();
					if (v.y() > vmax.y()) vmax.y() = v.y();
				}
			}

			vmin.y() += ybase;
			vmin.x() = xbase;
			vmin.z() = zbase;
			vmax.y() += ybase;
			vmax.x() = xbase + 8 * UNITSIZE;
			vmax.z() = zbase + 8 * UNITSIZE;
			r = (vmax - vmin).length() * 0.5f;

		}
		else if (fourcc == 'MCLY') {
			// texture info
			nTextures = (int)size / 16;
			//gLog("=\n");
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

				texId[i] = tex;
				//textures[i] = video.textures.get(mt->textures[tex]);
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
						*p++ = (c[i] & b) ? 85 : 0;
					}
				}
			}

			if (supportShaders) {
				for (int p=0; p<64*64; p++) {
					blendbuf[p*4+3] = sbuf[p];
				}

				//CreateTextureManual(blendbuf, 64*64*sizeof(unsigned int));
			}
			createAlphaTex = true;
		}
		else if (fourcc == 'MCAL') {
			// alpha maps  64 x 64
			if (nTextures>0) {
				createAlphaTex = true;

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

					if (supportShaders) {
						for (int p=0; p<64*64; p++) {
							//blendbuf[p*4+i] = amap[p];

							if (i==0) blendbuf[p*4+2] = amap[p];  // r
							if (i==1) blendbuf[p*4+1] = amap[p];  // g
							if (i==2) blendbuf[p*4+0] = amap[p];  // b
						}
					}

					pos += 0x800;
					//stream->Seek(0x800, Stream::Current);
				}
				stream->Unmap();

			} else {
				// some MCAL chunks have incorrect sizes! :(
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

	vcenter = (vmin + vmax) * 0.5f;

	TerrainChunkFVF *dataPtr = n_new_array(TerrainChunkFVF, mapbufsize);

	for (int i = 0; i < mapbufsize; i++)
	{
		dataPtr[i].x = tv[i].x();
		dataPtr[i].y = tv[i].y();
		dataPtr[i].z = tv[i].z();
		dataPtr[i].tex = texCoord[i];
		dataPtr[i].texblend = alphaCoord[i];

		vector vec = float4::normalize(tn[i]);
		dataPtr[i].nx = vec.x();
		dataPtr[i].ny = vec.y();
		dataPtr[i].nz = vec.z();
	}



	blendbuf = blendbuf;
	bufSize = 64*64*4;
	isEmpty = !(supportShaders && createAlphaTex);

	dataBuf = dataPtr;
	dataSize = mapbufsize;
	
	shaderName = "shd:static";
	if (nTextures > 0)
	{
        shaderName = "shd:terrain";

		switch (nTextures)
		{
		case 1:// 第1层纹理
			tex[0] = CreateTextureResource(textures[texId[0]]);
			break;
		case 2:
			chunk.tex1 = CreateTextureResource(textures[texId[1]]);
			break;
		case 3:
			tex[1] = CreateTextureResource(textures[texId[1]]);
			tex[2] = CreateTextureResource(textures[texId[2]]);
			break;
		case 4:
			tex[1] = CreateTextureResource(textures[texId[1]]);
			tex[2] = CreateTextureResource(textures[texId[2]]);
			tex[3] = CreateTextureResource(textures[texId[3]]);
			break;
		}
	}
	else
	{
		// 暂时代替
		//newModelNode->SetString(Attr::DiffMap0, texBlend);
	}

    this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId(shaderName));

	box.set(vcenter, vmax - vcenter);
	pos = vector(xbase, ybase, zbase);

    isOk = true;
}

void 
TerrainChunk::Render()
{

		RenderDevice::Instance()->SetPrimitiveGroup(this->primGroup);

		String feature = "Terrain1";
		if (tex[0].isvalid())
			diffMap[0]->SetTexture(tex[0]->GetTexture());
		if (tex[1].isvalid())
		{
			diffMap[1]->SetTexture(tex[1]->GetTexture());
			feature = "Terrain2";
		}
		if (tex[2].isvalid())
		{
			diffMap[2]->SetTexture(tex[2]->GetTexture());
			feature = "Terrain3";
		}
		if (tex[3].isvalid())
		{
			diffMap[3]->SetTexture(tex[3]->GetTexture());
			feature = "Terrain4";
		}
		if (tex[4].isvalid())
			diffMap[4]->SetTexture(tex[4]->GetTexture());

		ShaderServer* shdServer = ShaderServer::Instance();
		shdServer->SetFeatureBits(shdServer->FeatureStringToMask(feature));
	}
	else
	{
		mesh->ApplyPrimitives(0);
	}

	ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);
}

} // namespace Graphics
