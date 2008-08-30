//------------------------------------------------------------------------------
//  thing.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/being.h"
#include "kok/model/modeldef.h"
#include "kok/model/beingnode.h"
#include "util/string.h"

namespace KOK
{
ImplementClass(KOK::Being, 'BEIG', Models::Model);

using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Being::Being():
	computeMeshRepeat(false)
{
}

//------------------------------------------------------------------------------
/**
*/
Being::~Being()
{
}

//------------------------------------------------------------------------------
/**
*/
void
Being::Unload()
{    
	// call parent class
    Model::Unload();
}

bool 
Being::SetupFromStream(const Ptr<IO::Stream>& stream)
{
	n_assert(stream.isvalid());

	// 获取文件路径
	//String path = stream->LocalPath();
	//path.

	stream->SetAccessMode(Stream::ReadWriteAccess);
	if (!stream->Open())
		return false;

	// 读取ATM识别码
	String ATFChar;
	ReadString(stream, ATFChar, 3);

	// 读取版本
	stream->Read(&this->meshVersion, sizeof(int));
	
	// 检查ATF识别码
	if (ATFChar != "ATM" )
		return false;

	// 读取物件的数量
	int subMeshNum;
	stream->Read(&subMeshNum, sizeof(int));
	if (subMeshNum == 0)
	{
		return false;
	}
	
	if (subMeshNum > 0)
	{
		bbox b;
		b.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
		b.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);
		for (SizeT i = 0; i < subMeshNum; i++)
		{
			Ptr<BeingNode> node = BeingNode::Create();
			node->LoadFromStream(stream, this->meshVersion, false, computeMeshRepeat);
			
			this->AttachNode(node.upcast<ModelNode>());
			//node->LoadTextures("mbtex:");
			b.extend(node->GetBoundingBox());
		}
		this->SetBoundingBox(b);
	}

	return true;
}

void 
Being::LoadTexture(int texId)
{
	String path = "mbtex:";
	const Array<Ptr<ModelNode>>& nodes = this->GetNodes();

	for (SizeT i = 0; i < nodes.Size(); i++)
	{
		nodes[i].downcast<KokShapeNode>()->LoadTextures(path, texId);
	}
}

} // namespace KOK
