//------------------------------------------------------------------------------
//  thingreader.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/load/thinreader.h"
#include "util/string.h"
#include "kokmodel/kokshapenode.h"
#include "models/attributes.h "
#include "memory/memory.h"

namespace KOK
{
ImplementClass(KOK::ThingReader, 'OMRR', IO::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Memory;

//------------------------------------------------------------------------------
/**
*/
ThingReader::ThingReader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ThingReader::~ThingReader()
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
ThingReader::Open()
{
	n_assert(mapBuffer == NULL);
	n_assert(this->stream.isvalid());
	n_assert(this->model.isvalid());

	this->thing = this->model.downcast<Thing>();
	this->stream->SetAccessMode(Stream::ReadWriteAccess);
    if (this->stream->Open())
    {
		String thingCode;
		ReadString(thingCode, 5);

		if (thingCode != "THING")
			return false;	// 识别码不对

		return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ThingReader::Close()
{
    n_assert(this->IsOpen());

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ThingReader::FillModel()
{
	n_assert(this->stream.isvalid());

	this->stream->Read(&this->thing->meshVersion, sizeof(int));

	// 读取区域范围
	this->stream->Read(&this->thing->height, sizeof(DWORD));
	this->stream->Read(&this->thing->width, sizeof(DWORD));

	this->thing->skeletonHierarchyRoot = n_new cSkeletonHierarchy;
	this->thing->skeletonHierarchyRoot->importFromMemory((BYTE*)Buffer);

	// 读取子模型数量
	int subMeshNum;
	this->stream->Read(&subMeshNum, sizeof(int));
	if (subMeshNum == 0)
	{
		n_delete this->thing->skeletonHierarchyRoot;
		return false;
	}

	if (subMeshNum > 0)
	{
		for (SizeT i = 0; i < subMeshNum; i++)
		{
			Ptr<KokShapeNode> node = CreateNode( this->thing->meshVersion, false);

			if (this->thing->type == Thing::WAVER_FLOWER_THING || this->thing->type == Thing::WAVER_TREE_THING)
			{
				for (SizeT j = 0; j < node->verticesBufferSize; j++)
				{
					MDLVTX* vertex = node->verticesBuffer[j];
					if (vertex)
					{
						const bbox& b = node->GetBoundingBox();
						float dif = b.pmax.y() - b.pmin.y();
						float pct = (vertex->p.y - b.pmin.y()) / dif;
						pct = 1.0f - pct;
						float weight = (1-this->thing->waverFactor) + pct * this->thing->waverFactor;
					}
				}
			}
		}
	}


    return true;
}

Ptr<KokShapeNode>
ThingReader::CreateNode(int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection)
{
	Ptr<KokShapeNode> node = KokShapeNode::Create();

	String strTmp;
	int nameLen;
	this->stream->Read(&nameLen, sizeof(int));
	ReadString(strTmp, nameLen);

	node->SetName(strTmp);

	// 无用？浪费！！！
	matrix44 mat;
	this->stream->Read(&mat.getrow0(), sizeof(float)*16);

	// 读取顶点数据
	bbox boundingbox;
	boundingbox.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
	boundingbox.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);
	this->stream->Read(&node->verticesBufferSize, sizeof(DWORD));
	if (node->verticesBufferSize > 0)
	{
		node->verticesBuffer = new MDLVTX[node->verticesBufferSize];

		for (SizeT i = 0; i < node->verticesBufferSize; i++)
		{
			this->stream->Read(&(node->verticesBuffer[i].p), sizeof(Math::vector3));
			//node->addSubMeshDuplicateVertexCollections(i, bComputieDuplicateVertexCollection);
			this->stream->Read(&(node->verticesBuffer[i].n), sizeof(Math::vector3));

			if (bMirrorZ)
			{
				node->verticesBuffer[i].p.z = -node->verticesBuffer[i].p.z;
				node->verticesBuffer[i].n.z = -node->verticesBuffer[i].n.z;
			}
			this->stream->Read(&(node->verticesBuffer[i].tu), sizeof(float)*2);

			boundingbox.extend(point(node->verticesBuffer[i].p.x,
				node->verticesBuffer[i].p.y,
				node->verticesBuffer[i].p.z));
		}
	}
	node->SetBoundingBox(boundingbox);

	// 读取索引数据
	this->stream->Read(&node->indexBufferSize, sizeof(DWORD));
	if (node->indexBufferSize > 0)
	{
		node->indexBuffer = n_new DWORD[node->indexBufferSize];

		if (bMirrorZ)
		{
			for (SizeT i = 0; i < node->indexBufferSize; i += 3)
			{
				this->stream->Read(node->indexBuffer[i],   sizeof(WORD));
				this->stream->Read(node->indexBuffer[i+2], sizeof(WORD));
				this->stream->Read(node->indexBuffer[i+1], sizeof(WORD));
			}
		}
		else
		{
			this->stream->Read(node->indexBuffer, sizeof(WORD), node->indexBufferSize);
		}
	}

	// 读取skinWeight资料的长度
	this->stream->Read(&node->skinWeightNum, sizeof(DWORD));
	if (node->skinWeightNum > 0)
	{
		//node->skinWeights[node->skinWeightNum];

		//for (SizeT i = 0; i < node->skinWeightNum; i++)
		//{
		//	node->skinWeights[i].importSkinWeightFromMemory();
		//}
	}

	if (iVersion >= 5)
	{
		this->stream->Read(&node->meshType,     sizeof(DWORD));
		this->stream->Read(&node->ribbonWidth,  sizeof(float));
		this->stream->Read(&node->ribbonLength, sizeof(float));
	}
	node->meshType = GetThingType(node->GetName());

	// 读取attribTable资料
	this->stream->Read(&node->attributeTableSize, sizeof(DWORD));
	if (node->attributeTableSize > 0)
	{
		node->attributeTable = n_new AttributeRange[node->attributeTableSize];
		this->stream->Read(node->attributeTable, sizeof(AttributeRange)*node->attributeTableSize);

		// 读取material资料
		this->stream->Read(&node->materialNum, sizeof(DWORD));
		if (node->materialNum > 0)
		{
			node->material = new cMaterial[node->materialNum];
			node->materialAnimator = n_new cMaterialAnimator[node->materialNum];

			for (SizeT i = 0; i < node->materialNum; i++)
				ImportMaterialFromMemory(i, iVersion);
		}
	}

}

Thing::EThingSubMeshSpecialType 
ThingReader::GetThingType(const String& name)
{
	if (name.Length())
	{
		if (name.CheckStringExist("m_hd"))
			return Thing::TYPE_HD;
		else if (name.CheckStringExist("m_light"))
			return Thing::TYPE_LIGHT; 
		else if (name.CheckStringExist("m_repeat"))
			return Thing::TYPE_REPEAT; 
		else if (name.CheckStringExist("m_select"))
			return Thing::TYPE_SELECT; 
		else if (name.CheckStringExist("m_water"))
			return Thing::TYPE_WATER; 
		else if (name.CheckStringExist("e_ff"))
			return Thing::TYPE_EFF; 
		else if (name.CheckStringExist("e_flame"))
			return Thing::TYPE_EFLAME; 
		else
			return Thing::TYPE_NORMAL;
	}
	else
		return Thing::TYPE_NORMAL;
}

void 
ThingReader::ReadString(String& str, int len)
{
	MemoryMark mm(MemStack);

	char* buf = new(MemStack, len+1)char;
	this->stream->Read(buf, len);
	buf[len] = 0;
	str = buf;
}
} // namespace Models
