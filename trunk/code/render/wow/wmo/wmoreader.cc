//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/wmo/wmoreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/vector3.h"
#include "math/quaternion.h"
#include "system/byteorder.h"
#include "models/modelserver.h"
#include "models/nodes/transformnode.h"


namespace WOW
{
ImplementClass(WOW::WMOReader, 'WMOR', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Attr;
using namespace System;
using namespace Models;
//using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
WMOReader::WMOReader() :
	wmo(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WMOReader::~WMOReader()
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
WMOReader::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanRead());
    n_assert(this->stream->CanSeek());
    n_assert(!this->stream->IsMapped());
    n_assert(this->modelResId.Value().IsValid());

    if (StreamReader::Open())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
WMOReader::Close()
{
    n_assert(this->IsOpen());

    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
WMOReader::FillModel()
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
WMOReader::SetToFirstModel()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WMOReader::SetToNextModel()
{
    return false;
}

//------------------------------------------------------------------------------
/**
	解析wmo文件头。group在wmogroupReader解析
*/
void 
WMOReader::ReadWMO()
{
	n_assert(0 != this->wmo);

	//GraphicsServer* gfxServer = GraphicsServer::Instance();
	//Ptr<Stage> stage = gfxServer->GetStageByName("DefaultStage");


	Util::FourCC fourcc;
	SizeT size;
	float ff[3];

	char* ddnames;
	char* groupnames;

	char* texbuf = 0;

	while (!this->stream->Eof())
	{
		this->stream->Read(&fourcc, 4);
		this->stream->Read(&size, 4);

		SizeT nextpos = this->stream->GetPosition() + size;

		if (fourcc == 'MOHD')
		{
			unsigned int col;
			// header
			this->stream->Read(&this->wmo->nTextures, 4);
			this->stream->Read(&this->wmo->nGroups, 4);
			this->stream->Read(&this->wmo->nP, 4);
			this->stream->Read(&this->wmo->nLights, 4);
			this->stream->Read(&this->wmo->nModels, 4);
			this->stream->Read(&this->wmo->nDoodads, 4);
			this->stream->Read(&this->wmo->nDoodadSets, 4);
			this->stream->Read(&col, 4);
			this->stream->Read(&this->wmo->nX, 4);
			this->stream->Read(ff,12);
			this->wmo->v1 = vector3(ff[0],ff[1],ff[2]);
			this->stream->Read(ff,12);
			this->wmo->v2 = vector3(ff[0],ff[1],ff[2]);

			//this->wmo->groups = new WMOGroup[nGroups];
			this->wmo->mat = n_new_array(WMOMaterial, this->wmo->nTextures);
		}
		else if (fourcc == 'MOTX')
		{
			// textures
			texbuf = new char[size];
			this->stream->Read(texbuf, size);
		}
		else if (fourcc == 'MOMT')
		{
			// materials
			//WMOMaterialBlock bl;

			for (int i=0; i<this->wmo->nTextures; i++) {
				WMOMaterial *m = &this->wmo->mat[i];
				this->stream->Read(m, 0x40);
				
				m->tex = i;

				String texpath(texbuf+m->nameStart);
				fixname(texpath);

				// 增加纹理，到具体用的时候再加，现在只加路径
				//m->tex = video.textures.add(texpath);
				this->wmo->textures.Append(texpath);
			}
		}
		else if (fourcc == 'MOGN')
		{
			groupnames = (char*)((char*)this->stream->Map() + this->stream->GetPosition());
			this->stream->Unmap();
		}
		else if (fourcc == 'MOGI')
		{
			int flags;
			String name, tmp;
			vector v1, v2;

			// 暂时不处理
			// group info - important information! ^_^
			for (int i=0; i<this->wmo->nGroups; i++) {
				//groups[i].init(this, f, i, groupnames);

				this->stream->Read(&flags, 4);
				this->stream->Read(ff, 12);
				v1 = vector(ff[0], ff[2], -ff[1]);
				this->stream->Read(ff, 12);
				v2 = vector(ff[0], ff[2], -ff[1]);
				int nameOfs;
				this->stream->Read(&nameOfs, 4);

				/*if (nameOfs > 0)
				{
					name = String(groupnames + nameOfs);
				}else name = "(no name)";

				tmp = name;
				tmp[name.Length()-4] = 0;*/
				
				tmp = this->wmo->GetResourceId().Value();
				tmp[tmp.Length()-4] = 0;
				char szout[256];
				sprintf(szout, "%s_%03d.wmo", tmp.AsCharPtr(), i);
				name = szout;

				// 创建Group
				//Ptr<Model> group = ModelServer::Instance()->LoadManagedModel(name)->GetModel();
				//this->wmo->groups.Append(group);
				//this->wmo->groupsName.Append(name);
				vector center = (v1 + v2) * 0.5f;
				vector vmax = float4::maximize(v1, v2);
				bbox b(center, vmax - center);
				/*b.pmin = v1;
				b.pmax = v2;*/
				this->wmo->CreateGroup(name, b, flags);
			}
		}
		else if (fourcc == 'MOLT') {
			// Lights?
			/*for (int i=0; i<nLights; i++) {
				WMOLight l;
				l.init(f);
				lights.push_back(l);
			}*/
		}
		else if (fourcc == 'MODN') {
			// models ...
			// MMID would be relative offsets for MMDX filenames
			if (size) {

				/*GraphicsServer* gfxServer = GraphicsServer::Instance();
				Ptr<Stage> stage = gfxServer->GetStageByName("DefaultStage");*/

				ddnames = (char*)((char*)this->stream->Map() + this->stream->GetPosition());
				fixnamen(ddnames, size);

				//char *p=ddnames,*end=p+size;
				//int t=0;
				//while (p<end) {
				//	String path(p);
				//	p+=strlen(p)+1;
				//	while ((p<end) && (*p==0)) p++;

				//	//gWorld->modelmanager.add(path);
				//	//models.Append(path);

				//	// 添加m2 模型
				//	/*Ptr<ModelEntity> m2Model = ModelEntity::Create();
				//	m2Model->SetResourceId(path);
				//	stage->AttachEntity(m2Model);
				//	entytise.Append(m2Model);*/
				//}
				this->stream->Unmap();
				this->stream->Seek((int)size, Stream::Current);
			}
		}
		else if (fourcc == 'MODS') {
			for (int i=0; i<this->wmo->nDoodadSets; i++) {
				WMODoodadSet dds;
				this->stream->Read(&dds, 32);
				this->wmo->doodadsets.Append(dds);
			}
		}
		else if (fourcc == 'MODD') {
			// 这里只将所有模型信息添加到列表，实例化WMO时，再将模型创建出来。

			//GraphicsServer* gfxServer = GraphicsServer::Instance();
			//Ptr<Stage> stage = gfxServer->GetStageByName("DefaultStage");
			String tmp;

			this->wmo->nModels = (int)size / 0x28;
			for (int i=0; i<this->wmo->nModels; i++) {
				int ofs;
				this->stream->Read(&ofs,4);
				
				ModelInfo info;
				tmp = String(ddnames + ofs);
				info.m2Name.Clear();
				info.m2Name.AppendRange(tmp.AsCharPtr(), tmp.Length()-4);
				info.m2Name.Append(".m2");
				// 添加m2 模型
				/*Ptr<Model> m2 = ModelServer::Instance()->LoadManagedModel(m2name)->GetModel();
				this->wmo->modelis.Append(m2);
				this->wmo->models.Append(m2name);*/

				// 设置变换
				float d1;
				this->stream->Read(ff, 12);
				info.pos.set(ff[0], ff[2], -ff[1]);
				this->stream->Read(&info.w, 4);
				this->stream->Read(ff, 12);
				info.dir.set(ff[0], ff[1], ff[2]);
				this->stream->Read(&info.sc, 4);
				this->stream->Read(&d1, 4);
				//lcol = vector3(((d1&0xff0000)>>16) / 255.0f, ((d1&0x00ff00)>>8) / 255.0f, (d1&0x0000ff) / 255.0f);
				
				this->wmo->models.Append(info);

				/*Array<Ptr<ModelNode>> m2Node = m2->GetNodes();
				for (SizeT n = 0; n < m2Node.Size(); n++)
				{
					m2Node[n].downcast<TransformNode>()->SetPosition(pos);
					m2Node[n].downcast<TransformNode>()->SetRotate(quaternion(dir.y - 90, -dir.x, dir.z, 1.0));
					m2Node[n].downcast<TransformNode>()->SetScale(vector(sc, sc, sc));
				}*/

				/*matrix44 transform;
				transform.translation(pos);
				transform.rotationyawpitchroll(dir.y - 90, -dir.x, dir.z);
				transform.scaling(sc);
				m2Model->SetTransform(transform);*/

				/*Model *m = (Model*)gWorld->modelmanager.items[gWorld->modelmanager.get(ddnames + ofs)];
				ModelInstance mi;
				mi.init2(m,f);
				modelis.Append(mi);*/
			}

		}
		else if (fourcc == 'MOSB') {
			/*if (size>4) {
				String path = this->stream->GetPosition();
				fixname(path);
				if (path.Length()) {

					sbid = gWorld->modelmanager.add(path);
					skybox = (Model*)gWorld->modelmanager.items[sbid];

					if (!skybox->ok) {
						gWorld->modelmanager.del(sbid);
						skybox = 0;
					}
				}
			}*/
		}
		else if (fourcc == 'MOPV') {
			WMOPV p;
			for (int i=0; i<this->wmo->nP; i++) {
				this->stream->Read(ff,12);
				p.a = vector3(ff[0],ff[2],-ff[1]);
				this->stream->Read(ff,12);
				p.b = vector3(ff[0],ff[2],-ff[1]);
				this->stream->Read(ff,12);
				p.c = vector3(ff[0],ff[2],-ff[1]);
				this->stream->Read(ff,12);
				p.d = vector3(ff[0],ff[2],-ff[1]);
				this->wmo->pvs.Append(p);
			}
		}
		else if (fourcc == 'MOPR') {
			int nn = (int)size / 8;
			WMOPR *pr = (WMOPR*)((char*)this->stream->Map() + this->stream->GetPosition());
			for (int i=0; i<nn; i++) {
				this->wmo->prs.Append(*pr++);
			}
			this->stream->Unmap();
		}
		else if (fourcc == 'MFOG') {
			/*int nfogs = (int)size / 0x30;
			for (int i=0; i<nfogs; i++) {
				WMOFog fog;
				fog.init(f);
				fogs.Append(fog);
			}*/
		}

		this->stream->Seek((int)nextpos, Stream::Begin);
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
WMOReader::fixnamen(char *name, SizeT len)
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
WMOReader::fixname(String &name)
{
	for (SizeT i=0; i<name.Length(); i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

} // namespace Models
