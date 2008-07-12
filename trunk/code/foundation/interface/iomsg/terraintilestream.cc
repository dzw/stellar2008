//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/terraintilestream.h"
#include "render/terrain/terrainnode.h"

namespace Interface
{
ImplementClass(Interface::TerrainTileStream, 'TTSM', Interface::IOMessage);
ImplementMsgId(TerrainTileStream);

//------------------------------------------------------------------------------
/**
*/
TerrainTileStream::TerrainTileStream()
{
    InitGlobalVBOs();
}

//------------------------------------------------------------------------------
/**
*/
TerrainTileStream::~TerrainTileStream()
{
}


//------------------------------------------------------------------------------
/**
*/
void 
TerrainTileStream::fixnamen(char *name, SizeT len)
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
TerrainTileStream::fixname(String &name)
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
TerrainTileStream::InitGlobalVBOs()
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
void TerrainTileStream::ParseData(const Ptr<Stream>& stream)
{
    if (!stream->IsOpen())
		stream->Open();

	if (stream->GetSize() <= 0 || this->tile == NULL)
		return;

	//解析tile
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

	//	float xbase, zbase;

	while (!stream->Eof())
	{
		Sleep(0);

		stream->Read(&fourcc, 4);
		stream->Read(&size, 4);

		SizeT nextpos = stream->GetPosition() + size;

		if (fourcc == 'MCIN') {
			// mapchunk offsets/sizes
			for (int i=0; i<256; i++) {
				stream->Read(&mcnk_offsets[i],4);
				stream->Read(&mcnk_sizes[i],4);
				stream->Seek(8, Stream::Current);
			}
		}
		else if (fourcc == 'MTEX') {
			// texture lists
			char *buf = new char[size];
			stream->Read(buf, size);
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
                this->tile->textureNames.Append(texpath);
			}
			delete[] buf;
		}
		else if (fourcc == 'MMDX') {
			// models ...
			// MMID would be relative offsets for MMDX filenames
			if (size) {
				char *buf = new char[size];
				stream->Read(buf, size);
				char *p=buf;
				int t=0;
				while (p<buf+size) {
					String path(p);
					p+=strlen(p)+1;
					fixname(path);

					//gWorld->modelmanager.add(path);
					this->tile->modelNames.Append(path);
				}
				delete[] buf;
			}
		}
		else if (fourcc == 'MWMO') {
			// map objects
			// MWID would be relative offsets for MWMO filenames
			if (size) {
				char *buf = new char[size];
				stream->Read(buf, size);
				char *p=buf;
				while (p<buf+size) {
					String path(p);
					p+=strlen(p)+1;
					fixname(path);

					//gWorld->wmomanager.add(path);
                    this->tile->interiorNames.Append(path);
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
				stream->Read(&id, 4);
				/*Model *model = (Model*)gWorld->modelmanager.items[gWorld->modelmanager.get(models[id])];
				ModelInstance inst(model, f);
				modelis.push_back(inst);*/

				stream->Read(&d1, 4);
				stream->Read(ff, 12);
				pos.set(ff[0], ff[1], ff[2]);
				//stream->Read(&w, 4);
				stream->Read(ff, 12);
				dir.set(ff[0], ff[1], ff[2]);
				stream->Read(&scale, 4);
				sc = scale / 1024.0f;

				//CreateModelInstance(models[id], pos, dir, sc, 1.0);
                //直接在这里创建模型
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
				stream->Read(&id, 4);
				/*WMO *wmo = (WMO*)gWorld->wmomanager.items[gWorld->wmomanager.get(wmos[id])];
				WMOInstance inst(wmo, f);
				wmois.push_back(inst);*/

				stream->Read(&instanceId, 4);
				stream->Read(ff,12);
				pos = vector(ff[0],ff[1],ff[2]);
				stream->Read(ff,12);
				dir = vector(ff[0],ff[1],ff[2]);
				stream->Read(ff,12);
				pos2 = vector(ff[0],ff[1],ff[2]);
				stream->Read(ff,12);
				pos3 = vector(ff[0],ff[1],ff[2]);
				stream->Read(&d2,4);
				stream->Read(&d3,4);

				doodadset = (d2 & 0xFFFF0000) >> 16;

				//WMOServer::Instance()->LoadManagedWMO(wmos[id]);
				//WMOServer::Instance()->CreateWMOInstance(wmos[id], pos, dir, doodadset);
			}
		}

		stream->Seek((int)nextpos, Stream::Begin);
	}

    this->tile->SetState(Resource::Loaded);

    for (SizeT i = 0; i < TILECHUNKSIZE; i++)
    {
        for (SizeT j = 0; j < TILECHUNKSIZE; j++)
        {
            stream->Seek(mcnk_offsets[i<<4+j], Stream::Begin);

            String name;
            name.Format("%d_%d", i, j);
            Ptr<TerrainNode> node = TerrainNode::Create();
            node->SetName(name);
            this->tile->AttachNode(node);
            ParseChunk(stream, node);
            //node->Init(stream);
            //this->tile->chunk[i<<4+j] = TerrainChunk::Create();
            //this->tile->chunk[i<<4+j].Init(stream, this->tile);
        }
    }

    //this->tile->LoadResources();
}

//------------------------------------------------------------------------------
/**
	解析每个chunk
*/
void TerrainTileStream::ParseChunk(const Ptr<Stream>& stream, Ptr<TerrainNode>& node)
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

	TerrainChunkFVF *dataPtr = node->GetVertexData();     //n_new_array(TerrainChunkFVF, mapbufsize);

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



	//blendbuf = blendbuf;
	//bufSize = 64*64*4;
	//isEmpty = !(supportShaders && createAlphaTex);

	//dataBuf = dataPtr;
	//dataSize = mapbufsize;
	//
	//shaderName = "shd:static";
	//if (nTextures > 0)
	//{
 //       shaderName = "shd:terrain";

	//	switch (nTextures)
	//	{
	//	case 1:// 第1层纹理
	//		tex[0] = CreateTextureResource(textures[texId[0]]);
	//		break;
	//	case 2:
	//		chunk.tex1 = CreateTextureResource(textures[texId[1]]);
	//		break;
	//	case 3:
	//		tex[1] = CreateTextureResource(textures[texId[1]]);
	//		tex[2] = CreateTextureResource(textures[texId[2]]);
	//		break;
	//	case 4:
	//		tex[1] = CreateTextureResource(textures[texId[1]]);
	//		tex[2] = CreateTextureResource(textures[texId[2]]);
	//		tex[3] = CreateTextureResource(textures[texId[3]]);
	//		break;
	//	}
	//}
	//else
	//{
	//	// 暂时代替
	//	//newModelNode->SetString(Attr::DiffMap0, texBlend);
	//}

 //   this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId(shaderName));

	//box.set(vcenter, vmax - vcenter);
	//pos = vector(xbase, ybase, zbase);

    // 混合纹理
	node->SetString(Attr::TexBlend0, texBlend);

	
	String shaName = "shd:static";

	if (nTextures > 0)
	{
        shaName = "shd:terrain";
		// 第1层纹理
		node->SetString(Attr::DiffMap0, textures[texId[0]]);
		switch (nTextures)
		{
		case 1:
			break;
		case 2:
			node->SetString(Attr::DiffMap1, textures[texId[1]]);
			break;
		case 3:
			node->SetString(Attr::DiffMap1, textures[texId[1]]);
			node->SetString(Attr::DiffMap2, textures[texId[2]]);
			break;
		case 4:
			node->SetString(Attr::DiffMap1, textures[texId[1]]);
			node->SetString(Attr::DiffMap2, textures[texId[2]]);
			node->SetString(Attr::DiffMap3, textures[texId[3]]);
			break;
		}
	}
	else
	{
		// 暂时代替
		node->SetString(Attr::DiffMap0, texBlend);
	}

	node->SetString(Attr::Shader, shaName);
	
	node->SetFloat4(Attr::BoxCenter, vcenter);
	node->SetFloat4(Attr::BoxExtents, vmax - vcenter);

	//node->SetString(Attr::MeshResourceId, meshName);
	//node->SetInt(Attr::MeshGroupIndex, 0);
    node->LoadResources();

    isOk = true;
}

Ptr<ManagedTexture>
TerrainTileStream::CreateTextureResource(const String& name)
{
    return ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, name).downcast<ManagedTexture>();
}

} // namespace IO
