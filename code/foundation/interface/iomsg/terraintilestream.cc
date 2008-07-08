//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/terraintilestream.h"

namespace Interface
{
ImplementClass(Interface::TerrainTileStream, 'TTSM', Interface::IOMessage);
ImplementMsgId(TerrainTileStream);

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
TerrainParse::fixnamen(char *name, SizeT len)
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
TerrainParse::fixname(String &name)
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
TerrainParse::InitGlobalVBOs()
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
            this->tile->chunk[i<<4+j] = TerrainChunk::Create();
            this->tile->chunk[i<<4+j].Init(stream, this->tile);
        }
    }
}

//------------------------------------------------------------------------------
/**
	解析每个chunk
*/
void TerrainTileStream::ParseChunk(const Ptr<Stream>& stream, Ptr<TerrainChunk>& chunk)
{
    
}

Ptr<ManagedTexture>
TerrainTileStream::CreateTextureResource(const String& name)
{
    return ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, name).downcast<ManagedTexture>();
}

} // namespace IO
