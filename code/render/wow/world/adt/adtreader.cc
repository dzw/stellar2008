//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/adt/adtreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/vector3.h"
#include "math/quaternion.h"
#include "system/byteorder.h"
#include "models/modelserver.h"
#include "models/nodes/transformnode.h"
#include "models/nodes/shapenode.h"
#include "models/attributes.h"
#include "resources/resourcemanager.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/modelentity.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "wow/wmo/wmoserver.h"
#include "coregraphics/renderdevice.h"
#include "resources/managedtexture.h"
#include "coregraphics/texture.h"
#include "wow/world/worldmanager.h"
#include "resources/sharedresourceserver.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/streammeshloader.h"
#include "wow/world/terrain/chunknode.h"

namespace WOW
{
ImplementClass(WOW::MapTileReader, 'ADTR', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Attr;
using namespace System;
using namespace Models;
using namespace Resources;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Direct3D9;


//------------------------------------------------------------------------------
/**
*/
MapTileReader::MapTileReader() :
	ii(0),
	jj(0)
{
    defName.Clear();
}

//------------------------------------------------------------------------------
/**
*/
MapTileReader::~MapTileReader()
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
MapTileReader::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanRead());
    n_assert(this->stream->CanSeek());
    n_assert(!this->stream->IsMapped());
    n_assert(this->modelResId.Value().IsValid());
	
    if (StreamReader::Open())
    {
		//this->indexBuffer = WorldManager::Instance()->GetIndexBuffer();
		//this->texCoord = WorldManager::Instance()->GetDetailTexCoord();
		//this->alphaCoord = WorldManager::Instance()->GetAlphaTexCoord();
		loadFinish = false;

		//SetupVertexComponents();
		////InitGlobalVBOs();
		////InitIndexBuffer();
		////SetupIndexBuffer();
		//ReadPrimitiveGroups();

		ReadADT();
		FillModel();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
MapTileReader::Close()
{
    n_assert(this->IsOpen());

    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
MapTileReader::FillModel()
{
	//n_assert(this->model.isvalid());

	//this->SetToFirstModel();
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
MapTileReader::SetToFirstModel()
{
	/*float4 center(0, 0, 0, 0);
	float4 extents(99999, 99999, 99999, 99999);

	this->modelAttrs.SetFloat4(Attr::BoxCenter, center);
	this->modelAttrs.SetFloat4(Attr::BoxExtents, extents);*/

	//this->modelAttrs.SetFloat4(Attr::Position, point(100000.0f, 0.0f, 100000.0f));

	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MapTileReader::SetToNextModel()
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
MapTileReader::SetToFirstModelNode()
{
	InitMapChunk(jj, ii);
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
MapTileReader::SetToNextModelNode()
{
	/*if (jj < 16)
		jj++;
	else
	{
		jj = 0;
		if (ii < 16) 
			ii++;
		else 
			return false;
	}*/

	if (this->loadFinish)
		return false;

	InitMapChunk(ii, jj);

	return true;
}

//------------------------------------------------------------------------------
/**
	解析adt文件头
*/
void 
MapTileReader::ReadADT()
{
	//n_assert(0 != this->maptile);

	/*Array<String> textures;
	Array<String> models;
	Array<String> wmos;
	int nMDX, nWMO;*/

	Util::FourCC fourcc;
	SizeT size;
	float ff[3];

	Array<String> wmos;
	Array<String> models;

	//std::vector<WMOInstance> wmois;
	//std::vector<ModelInstance> modelis;
	int nWMO;
	int nMDX;

//	int x, z;
//	bool ok;

	//World *world;

//	float xbase, zbase;


	while (!this->stream->Eof())
	{
		this->stream->Read(&fourcc, 4);
		this->stream->Read(&size, 4);

		SizeT nextpos = this->stream->GetPosition() + size;

		if (fourcc == 'MCIN') {
			// mapchunk offsets/sizes
			for (int i=0; i<256; i++) {
				this->stream->Read(&mcnk_offsets[i],4);
				this->stream->Read(&mcnk_sizes[i],4);
				this->stream->Seek(8, Stream::Current);
			}
		}
		else if (fourcc == 'MTEX') {
			// texture lists
			char *buf = new char[size];
			this->stream->Read(buf, size);
			char *p=buf;
			int t=0;
			while (p<buf+size) {
				String texpath(p);
				p+=strlen(p)+1;
				fixname(texpath);

				//if (supportShaders) {
				//	// load the specular texture instead
				//	texpath.insert(texpath.length()-4,"_s");
				//}

				//video.textures.add(texpath);
				textures.Append(texpath);
			}
			delete[] buf;
		}
		else if (fourcc == 'MMDX') {
			// models ...
			// MMID would be relative offsets for MMDX filenames
			if (size) {
				char *buf = new char[size];
				this->stream->Read(buf, size);
				char *p=buf;
				int t=0;
				while (p<buf+size) {
					String path(p);
					p+=strlen(p)+1;
					fixname(path);

					//gWorld->modelmanager.add(path);
					models.Append(path);
				}
				delete[] buf;
			}
		}
		else if (fourcc == 'MWMO') {
			// map objects
			// MWID would be relative offsets for MWMO filenames
			if (size) {
				char *buf = new char[size];
				this->stream->Read(buf, size);
				char *p=buf;
				while (p<buf+size) {
					String path(p);
					p+=strlen(p)+1;
					fixname(path);

					//gWorld->wmomanager.add(path);
					wmos.Append(path);
				}
				delete[] buf;
			}
		}
		else if (fourcc == 'MDDF') {
			// model instance data
			
			float d1, sc/*, w*/;
			vector pos, dir;
			unsigned int scale;

			nMDX = (int)size / 36;
			for (int i=0; i<nMDX; i++) {
				int id;
				this->stream->Read(&id, 4);
				/*Model *model = (Model*)gWorld->modelmanager.items[gWorld->modelmanager.get(models[id])];
				ModelInstance inst(model, f);
				modelis.push_back(inst);*/

				this->stream->Read(&d1, 4);
				this->stream->Read(ff, 12);
				pos.set(ff[0], ff[1], ff[2]);
				//this->stream->Read(&w, 4);
				this->stream->Read(ff, 12);
				dir.set(ff[0], ff[1], ff[2]);
				this->stream->Read(&scale, 4);
				sc = scale / 1024.0f;

				CreateModelInstance(models[id], pos, dir, sc, 1.0);
			}
		}
		else if (fourcc == 'MODF') {
			// wmo instance data

			vector pos = vector(0,0,0);
			vector dir, pos2, pos3;
			float ff[3];
			int d2, d3, doodadset, instanceId;


			nWMO = (int)size / 64;
			for (int i=0; i<nWMO; i++) {
				int id;
				this->stream->Read(&id, 4);
				/*WMO *wmo = (WMO*)gWorld->wmomanager.items[gWorld->wmomanager.get(wmos[id])];
				WMOInstance inst(wmo, f);
				wmois.push_back(inst);*/

				this->stream->Read(&instanceId, 4);
				this->stream->Read(ff,12);
				pos = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				dir = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				pos2 = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				pos3 = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(&d2,4);
				this->stream->Read(&d3,4);

				doodadset = (d2 & 0xFFFF0000) >> 16;

				//WMOServer::Instance()->LoadManagedWMO(wmos[id]);
				//WMOServer::Instance()->CreateWMOInstance(wmos[id], pos, dir, doodadset);
			}
		}

		this->stream->Seek((int)nextpos, Stream::Begin);
	}

	// read individual map chunks
	//for (int j=0; j<16; j++) {
	//	for (int i=0; i<16; i++) {
	//		this->stream->Seek((int)mcnk_offsets[j*16+i], Stream::Begin);
	//		//chunks[j][i].init(this, f);
	//		InitMapChunk(j, i);

	//		return;
	//	}
	//}

	// init quadtree
	//topnode.setup(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
MapTileReader::InitMapChunk(int x, int y)
{
	bool supportShaders = true;
	bool createAlphaTex = false;

	//MapChunk* mapChunk = &this->maptile->chunks[x][y];

	vector tn[mapbufsize], tv[mapbufsize];

	this->stream->Seek((int)mcnk_offsets[x*16+y], Stream::Begin);
	this->stream->Seek(4, Stream::Current);

	Util::FourCC fourcc;
	int size;
	int texId[4];

	this->stream->Read(&size, 4);

	// okay here we go ^_^
	int lastpos = this->stream->GetPosition() + size;

	//char header[0x80];
	MapChunkHeader header;
	this->stream->Read(&header, 0x80);

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

	//zbase = float(x) * CHUNKSIZE+1000.0f;
	//xbase = float(y) * CHUNKSIZE+1000.0f;
	//ybase = 0;


	vmin = vector( 9999999.0f, 9999999.0f, 9999999.0f);
	vmax = vector(-9999999.0f,-9999999.0f,-9999999.0f);

	unsigned char *blendbuf;
	if (supportShaders) {
		blendbuf = n_new_array(unsigned char, 64*64*4);
		memset(blendbuf, 0, 64*64*4);
	}

	while (this->stream->GetPosition() < lastpos) {
		this->stream->Read(&fourcc,4);
		this->stream->Read(&size, 4);

		size_t nextpos = this->stream->GetPosition() + size;

		if (fourcc == 'MCNR') {
			nextpos = this->stream->GetPosition() + 0x1C0; // size fix
			// normal vectors
			char nor[3];
			vector *ttn = tn;
			for (int j=0; j<17; j++) {
				for (int i=0; i<((j%2)?8:9); i++) {
					this->stream->Read(nor,3);
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
					this->stream->Read(&h,4);
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
				this->stream->Read(&tex,4);
				this->stream->Read(&flags, 4);

				this->stream->Seek(8, Stream::Current);

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
				this->stream->Read(c,8);
				for (int i=0; i<8; i++) {
					for (int b=0x01; b!=0x100; b<<=1) {
						*p++ = (c[i] & b) ? 85 : 0;
					}
				}
			}
			/*glGenTextures(1, &shadow);
			glBindTexture(GL_TEXTURE_2D, shadow);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 64, 64, 0, GL_ALPHA, GL_UNSIGNED_BYTE, sbuf);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/

			
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
				int pos = this->stream->GetPosition();
				bufMap = (char*)this->stream->Map();
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
					/*glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 64, 64, 0, GL_ALPHA, GL_UNSIGNED_BYTE, amap);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/

					if (supportShaders) {
						for (int p=0; p<64*64; p++) {
							//blendbuf[p*4+i] = amap[p];

							if (i==0) blendbuf[p*4+2] = amap[p];  // r
							if (i==1) blendbuf[p*4+1] = amap[p];  // g
							if (i==2) blendbuf[p*4+0] = amap[p];  // b
						}
					}

					pos += 0x800;
					//this->stream->Seek(0x800, Stream::Current);
				}
				this->stream->Unmap();

			} else {
				// some MCAL chunks have incorrect sizes! :(
				continue;
			}
		}
		else if (fourcc == 'MCLQ') {
			// liquid / water level
			//FourCC fcc1;
			//this->stream->Read(fcc1,4);
			//
			//if (fcc1 == 'MCSE') {
			//	haswater = false;
			//}
			//else {
			//	haswater = true;
			//	this->stream->Seek(-4, Stream::Current);
			//	this->stream->Read(&waterlevel,4);

			//	if (waterlevel > vmax.y) vmax.y = waterlevel;
			//	if (waterlevel < vmin.y) haswater = false;

			//	this->stream->Seek(4, Stream::Current);

			//	//lq = new Liquid(8, 8, vector(xbase, waterlevel, zbase));
			//	////lq->init(f);
			//	//lq->initFromTerrain(f, chunkflags);
			//}
			// we're done here!
			break;
		}
		this->stream->Seek((int)nextpos, Stream::Begin);
	}

	// create vertex buffers
	/*glGenBuffersARB(1,&vertices);
	glGenBuffersARB(1,&normals);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, mapbufsize*3*sizeof(float), tv, GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, normals);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, mapbufsize*3*sizeof(float), tn, GL_STATIC_DRAW_ARB);*/

	//if (hasholes) initStrip(holes);
	

	//mt = this->maptile;

	vcenter = (vmin + vmax) * 0.5f;

	//String texBlend;
	//if (supportShaders && createAlphaTex) {
	//	/*glGenTextures(1, &blend);
	//	glBindTexture(GL_TEXTURE_2D, blend);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, blendbuf);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	//	
	//	texBlend = CreateTextureManual(blendbuf, 64*64*4);
	//	delete[] blendbuf;
	//}
	//else
	//{
	//	if (this->defName.IsEmpty())
	//	{
	//		// 建立一张空的纹理，避免重复建立，如果没有使用，不能建立空的纹理，否则释放的时候
	//		// 会因为计数错误而出错。
	//		unsigned char *blendbuf;
	//		blendbuf = new unsigned char[64*64*4];
	//		memset(blendbuf, 0, 64*64*4);
	//		this->defName = CreateTextureManual(blendbuf, 64*64*4);
	//	}

	//	texBlend = this->defName;
	//}


	//SetupVertexBuffer(tv, tn, 9*9+8*8);
	////SetupManagedMesh();
	//
	//Ptr<ModelNode> newModelNode;
	//if (this->nodes.Size() >= 256)
	//{
	//	// 已经创建了node,改变mesh和texture即可
	//	newModelNode = nodes[ii*16+jj];
	//	newModelNode->UnloadResources();
	//}
	//else
	//{
	//	// 第一次创建node
	//	newModelNode = ShapeNode::Create();
	//}
	//

	//// 混合纹理
	//newModelNode->SetString(Attr::TexBlend0, texBlend);

	//
	//String shaName = "shd:static";

	//if (nTextures > 0)
	//{
	//	// 第1层纹理
	//	newModelNode->SetString(Attr::DiffMap0, textures[texId[0]]);
	//	switch (nTextures)
	//	{
	//	case 1:
	//		shaName = "shd:terrain1";
	//		break;
	//	case 2:
	//		shaName = "shd:terrain2";
	//		newModelNode->SetString(Attr::DiffMap1, textures[texId[1]]);
	//		break;
	//	case 3:
	//		shaName = "shd:terrain3";
	//		newModelNode->SetString(Attr::DiffMap1, textures[texId[1]]);
	//		newModelNode->SetString(Attr::DiffMap2, textures[texId[2]]);
	//		break;
	//	case 4:
	//		shaName = "shd:terrain4";
	//		newModelNode->SetString(Attr::DiffMap1, textures[texId[1]]);
	//		newModelNode->SetString(Attr::DiffMap2, textures[texId[2]]);
	//		newModelNode->SetString(Attr::DiffMap3, textures[texId[3]]);
	//		break;
	//	}
	//}
	//else
	//{
	//	// 暂时代替
	//	newModelNode->SetString(Attr::DiffMap0, texBlend);
	//}

	//newModelNode->SetString(Attr::Shader, shaName);
	//
	//newModelNode->SetFloat4(Attr::BoxCenter, vcenter);
	//newModelNode->SetFloat4(Attr::BoxExtents, vmax - vcenter);

	//newModelNode->SetString(Attr::MeshResourceId, meshName);
	//newModelNode->SetInt(Attr::MeshGroupIndex, 0);
	//
	////////////////////////////////////////////////////////////////////////////
	//// 修正mapchunk之间的缝隙
	////static float xx = 0.0f, zz = 0.0f;

	////if (xx != 0.0f)
	////{
	////	float fx = xbase - xx - CHUNKSIZE;
	////	float fz = zbase - zz - CHUNKSIZE;
	////	//xbase -= fx;
	////	//zbase -= fz;
	////	//xbase = xx + CHUNKSIZE;
	////	//zbase = zz + CHUNKSIZE;
	////	xbase += 0.02f;
	////	zbase += 0.02f;
	////}

	///*char szout[256];
	//sprintf(szout, "%f, %f, %f", xbase, ybase, zbase);
	//OutputDebugString(szout);
	//OutputDebugString("\n");*/

	////static int count = 0;
	////static float modify = 0.0f;

	////if (count % 3 == 0)
	//	//xbase -= 0.0001;
	//	//modify += 0.001f;

	////xbase -= modify;
	////count++;
	////////////////////////////////////////////////////////////////////////////

	//newModelNode->SetFloat4(Attr::Position, point(xbase, ybase, zbase));

	////xx = xbase;
	////zz = zbase;

	//String objName;
	//objName.AppendInt(x*16+y);
	////this->modelNodeName = objName;

	//newModelNode->SetName(objName);

	//Attr::AttributeContainer attr = newModelNode->GetAttrs();
	//newModelNode->LoadFromAttrs(attr);

	//this->model->AttachNode(newModelNode);




	ChunkData chunk;
	chunk.blendbuf = blendbuf;
	chunk.bufSize = 64*64*4;
	chunk.isEmpty = !(supportShaders && createAlphaTex);

	chunk.dataBuf = n_new_array(TerrainChunkFVF, mapbufsize);
	chunk.dataSize = mapbufsize;

	/*chunk.vb = n_new_array(vector, mapbufsize);
	Memory::Copy(tv, chunk.vb, mapbufsize * sizeof(vector));
	chunk.nor = n_new_array(vector, mapbufsize);
	Memory::Copy(tn, chunk.nor, mapbufsize * sizeof(vector));
	chunk.vbSize = mapbufsize;*/
	
	chunk.shaderName = "shd:static";
	if (nTextures > 0)
	{
		// 第1层纹理
		chunk.tex0 = textures[texId[0]];
		switch (nTextures)
		{
		case 1:
			chunk.shaderName = "shd:terrain1";
			break;
		case 2:
			chunk.shaderName = "shd:terrain2";
			chunk.tex1 = textures[texId[1]];
			break;
		case 3:
			chunk.shaderName = "shd:terrain3";
			chunk.tex1 = textures[texId[1]];
			chunk.tex2 = textures[texId[2]];
			break;
		case 4:
			chunk.shaderName = "shd:terrain4";
			chunk.tex1 = textures[texId[1]];
			chunk.tex2 = textures[texId[2]];
			chunk.tex3 = textures[texId[3]];
			break;
		}
	}
	else
	{
		// 暂时代替
		//newModelNode->SetString(Attr::DiffMap0, texBlend);
	}
	
	chunk.box.set(vcenter, vmax - vcenter);
	chunk.pos = vector(xbase, ybase, zbase);

	chunks.Append(chunk);

	if (jj < 15)
		jj++;
	else
	{
		jj = 0;
		if (ii < 15) 
			ii++;
		else
			this->loadFinish = true;
	}

	
	
	//if (jj >= 1)
	//	this->loadFinish = true;


	//this->modelNodeAttrs.SetString(Attr::DiffMap1, texName);

	//String modelName;
	//static int totalM2Model = 0;
	//modelName = "mtM_";
	//modelName.AppendInt(totalM2Model);
	//totalM2Model++;
	//Ptr<ManagedModel> managedModel = ResourceManager::Instance()->ManualCreateManagedResource(Model::RTTI, modelName).downcast<ManagedModel>();
	//Ptr<Model> model = managedModel->GetModel();
	//Ptr<ShapeNode> modelNode = ShapeNode::Create();
	//modelNode->SetName(String("1"));
	//modelNode->LoadFromAttrs(this->modelNodeAttrs);
	//model->AttachNode(modelNode);
	//model->LoadResources();

	//// 直接创建到显示列表
	//GraphicsServer* gfxServer = GraphicsServer::Instance();
	//Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	//Ptr<ModelEntity> modelEntity = ModelEntity::Create();
	//modelEntity->SetResourceId(modelName);
	//stage->AttachEntity(modelEntity.upcast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void 
MapTileReader::fixnamen(char *name, SizeT len)
{
	for (size_t i=0; i<len; i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
MapTileReader::fixname(String &name)
{
	for (SizeT i=0; i<name.Length(); i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
MapTileReader::CreateModelInstance(const String& m2Name, const vector& vpos, const vector& vdir, const float fscale, const float fw)
{
	/*static int bb = 0;
	if (bb < 1)
		bb++;
	else
		return;*/

	String name = m2Name;
	name.ToLower();
	if (name.CheckFileExtension("mdx"))
	{
		name.SubstituteString(".mdx", ".m2");
	}

	//GraphicsServer* gfxServer = GraphicsServer::Instance();
	//Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	Ptr<ModelEntity> model = ModelEntity::Create();
	model->SetResourceId(ResourceId(name));

	// 设置变换
	matrix44 doodadsTransform = matrix44::identity();
	matrix44 transf = matrix44::identity();
	if (vdir.z())
	{
		transf = matrix44::rotationx(2.0f*N_PI*vdir.z() / 360.0f);
		doodadsTransform = matrix44::multiply(doodadsTransform, transf);
	}
	//if (vdir.y())
	{
		transf = matrix44::rotationy(2.0f*N_PI*vdir.y() / 360.0f - N_PI/2.0f);
		doodadsTransform = matrix44::multiply(doodadsTransform, transf);
	}
	if (vdir.x())
	{
		transf = matrix44::rotationz(2.0f*N_PI*vdir.x() / 360.0f);
		doodadsTransform = matrix44::multiply(doodadsTransform, transf);
	}
	//quaternion q(2.0f*N_PI*vdir.z() / 360.0f, 2.0f*N_PI*vdir.y() / 360.0f - N_PI/2.0f, 2.0f*N_PI*vdir.x() / 360.0f, 1.0f);
	//doodadsTransform = matrix44::multiply(doodadsTransform, matrix44::rotationquaternion(q));
	doodadsTransform = matrix44::multiply(doodadsTransform, matrix44::scaling(fscale, fscale, fscale));
	doodadsTransform = matrix44::multiply(doodadsTransform, matrix44::translation(vpos));
	

	//doodadsTransform = matrix44::translation(vpos);
	model->SetTransform(doodadsTransform);

	//stage->AttachEntity(model.upcast<GraphicsEntity>());
}


//------------------------------------------------------------------------------
/**
*/
void
MapTileReader::ReadPrimitiveGroups()
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
MapTileReader::SetupVertexComponents()
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
void
MapTileReader::SetupVertexBuffer(const vector* vb, const vector* nor, int vertices)
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

	// setup new vertex buffer
	this->vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(this->vertexComponents, vertices, dataPtr, vertices * sizeof(vfvf));
	this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->vertexBuffer->Load();
	this->vertexBuffer->SetLoader(0);
	n_delete_array(dataPtr);
	n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);
}



//------------------------------------------------------------------------------
/**
	所有的ModelNode都是用相同的顶点，只是PrimativeGroupIndex不同。
*/
void 
MapTileReader::SetupManagedMesh()
{
	
	return;

	// 相同的ShapeNode使用同一组VB & IB所以名字相同，实现共享
	static int totalM2Model = 0;
	meshName = "ck_";
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
	//mesh->IncrUseCount();	// 多加1个使用计数，避免被释放掉，注意最后退出的时候要多减一次！！
}

//------------------------------------------------------------------------------
/**
*/
//uint16* 
//MapTileReader::InitIndexBuffer()
//{
//	
//
//	return indices;
//}

//------------------------------------------------------------------------------
/**
	手动建立需要填充的纹理，如alpha、shadow

	暂时这样写吧，直接用DX9了。应该扩展一下texture类，实现手动创建功能。
*/
String 
MapTileReader::CreateTextureManual(const void* srcData, int srcDataSize)
{
	HRESULT hr;
	IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();
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

	// 相同的StateNode使用同一张纹理所以名字相同，实现共享
	static int totalM2Model = 0;
	String texName = "Tex_";
	texName.AppendInt(totalM2Model);
	totalM2Model++;

	
	// 手动建立资源，并填充数据
	//Ptr<ManagedTexture> managedTexture = ResourceManager::Instance()->ManualCreateManagedResource(Texture::RTTI, texName).downcast<ManagedTexture>();
	//const Ptr<Texture>& texture = managedTexture->GetResource().downcast<Texture>();
	Ptr<Texture> texture = SharedResourceServer::Instance()->CreateSharedResource(texName, Texture::RTTI, StreamTextureLoader::RTTI).downcast<Texture>();
	texture->SetState(Resource::Loaded);
	texture->SetupFromD3D9Texture(d3d9Texture);

	/*char szout[256];
	sprintf(szout, "d:\\%s", texName.AsCharPtr());
	D3DXSaveTextureToFile(szout, D3DXIFF_TGA, d3d9Texture, NULL);*/

	return texName;
}

} // namespace Models
