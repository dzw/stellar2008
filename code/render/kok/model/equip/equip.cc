//------------------------------------------------------------------------------
//  thing.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/equip/equip.h"
#include "util/string.h"
#include "kok/model/modeldef.h"
#include "kok/model/kokshapenode.h"

namespace KOK
{
ImplementClass(KOK::Equip, 'EQUP', Models::Model);

using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Equip::Equip()
{
}

//------------------------------------------------------------------------------
/**
*/
Equip::~Equip()
{
}

//------------------------------------------------------------------------------
/**
*/
void
Equip::Unload()
{    
	// call parent class
    Model::Unload();
}

bool 
Equip::SetupFromStream(const Ptr<IO::Stream>& stream)
{
	n_assert(stream.isvalid());

	stream->SetAccessMode(Stream::ReadWriteAccess);
	if (!stream->Open())
		return false;

	String thingCode;
	ReadString(stream, thingCode, 5);

	if (thingCode != "THING")
		return false;	// 识别码不对

	stream->Read(&this->meshVersion, sizeof(int));

	// 读取区域范围
	stream->Read(&m_dwHeight, sizeof(DWORD));
	stream->Read(&m_dwWidth, sizeof(DWORD));

	m_pSkeletonHierarchyRoot = n_new(cSkeletonHierarchy);
	m_pSkeletonHierarchyRoot->importFromMemory(stream);
	
	// 读取子模型数量
	int subMeshNum;
	stream->Read(&subMeshNum, sizeof(int));
	if (subMeshNum == 0)
	{
		n_delete(m_pSkeletonHierarchyRoot);
		return false;
	}

	if (subMeshNum > 0)
	{
		String path = this->GetResourceId().Value().ExtractDirName();

		bbox b;
		b.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
		b.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);
		for (SizeT i = 0; i < subMeshNum; i++)
		{
			Ptr<KokShapeNode> node = KokShapeNode::Create();
			node->LoadFromStream(stream, this->meshVersion, false);

			this->AttachNode(node.upcast<ModelNode>());
			node->LoadTextures(path);
			b.extend(node->GetBoundingBox());
		}
		this->SetBoundingBox(b);
	}

	String strAni;
	strAni.Format("mequp:%s", this->GetResourceId().Value().AsCharPtr());
	LoadAnimation(strAni.AsCharPtr());

	return true;
}

//-----------------------------------------------------------------------------
// 修改装备Model Render架构
int 
Equip::LoadAnimation( const char* pLoadFileName )
{
	cAnimationAction* pAnimationAction = n_new(cAnimationAction);

	if( pAnimationAction == NULL || ( pAnimationAction->importAnimationAction(pLoadFileName ) < 0 ) )
	{
		n_delete( pAnimationAction );
		return -2;
	}

	n_delete( m_pAnimRoot );

	String szSkeletonHierarchyName;

	int iAnimationKeysCount = pAnimationAction->getAnimationKeysCount();
	for( int i = 0; i < iAnimationKeysCount; i++ )
	{
		cAnimation* pTempAnimation = NULL;
		cAnimationKeys* pAnimationKeys = pAnimationAction->getAnimationKeys( i );

		if( pAnimationKeys == NULL )
			continue;

		szSkeletonHierarchyName = pAnimationKeys->getSkeletonHierarchyName();

		if( !m_pAnimRoot )
		{
			m_pAnimRoot = n_new(cAnimation);
			pTempAnimation = m_pAnimRoot;
			pTempAnimation->setSkeletonHierarchyName((char*)szSkeletonHierarchyName.AsCharPtr());

			cSkeletonHierarchy *pSkeletonHierarchy = m_pSkeletonHierarchyRoot->findSkeletonHierarchy((char*)szSkeletonHierarchyName.AsCharPtr());
			if( pSkeletonHierarchy ) 
			{
				pSkeletonHierarchy->setAnimation(pTempAnimation);
			}
		}
		else
		{
			pTempAnimation = m_pAnimRoot->findAniamtion((char*)szSkeletonHierarchyName.AsCharPtr());

			if( !pTempAnimation )
			{
				pTempAnimation = m_pAnimRoot->addAnimationChild((char*)szSkeletonHierarchyName.AsCharPtr());

				cSkeletonHierarchy* pSkeletonHierarchy = m_pSkeletonHierarchyRoot->findSkeletonHierarchy((char*)szSkeletonHierarchyName.AsCharPtr());
				if( pSkeletonHierarchy ) 
				{
					pSkeletonHierarchy->setAnimation( pTempAnimation );
				}
			}
		}

		if( pTempAnimation )
		{
			int iActionInfoCount = pAnimationAction->getAnimationActionInfoCount();
			for( int j = 0; j < iActionInfoCount; j++ )
			{
				pTempAnimation->addAnimationKeys( j, pAnimationKeys );
			}
		} 
	}

	m_AnimationActionInfoGroup.removeAllAnimationActionInfo();
	int iActionInfoCount = pAnimationAction->getAnimationActionInfoCount();
	cAnimationActionInfo* pAnimationActionInfo;
	for( int j = 0; j < iActionInfoCount; j++ )
	{ 
		pAnimationActionInfo = pAnimationAction->getAnimationActionInfo( j );
		pAnimationActionInfo->setAnimationACTIndex( 0 );
		m_AnimationActionInfoGroup.addAnimationActionInfo( j, pAnimationActionInfo );
	}

	n_delete( pAnimationAction );
	return 0;
}

} // namespace KOK
