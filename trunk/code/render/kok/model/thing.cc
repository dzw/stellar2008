//------------------------------------------------------------------------------
//  thing.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thing.h"
#include "kok/model/modeldef.h"
#include "kok/model/kokshapenode.h"
#include "util/string.h"

namespace KOK
{
ImplementClass(KOK::Thing, 'THNG', Models::Model);

using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Thing::Thing()
{
}

//------------------------------------------------------------------------------
/**
*/
Thing::~Thing()
{
}

//------------------------------------------------------------------------------
/**
*/
void
Thing::Unload()
{    
	// call parent class
    Model::Unload();
}

bool 
Thing::SetupFromStream(const Ptr<IO::Stream>& stream)
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
		bbox b;
		b.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
		b.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);
		for (SizeT i = 0; i < subMeshNum; i++)
		{
			Ptr<KokShapeNode> node = KokShapeNode::Create();
			node->LoadFromStream(stream, this->meshVersion, false);

			this->AttachNode(node.upcast<ModelNode>());
			b.extend(node->GetBoundingBox());

			//if (m_iType == Thing::WAVER_FLOWER_THING || m_iType == Thing::WAVER_TREE_THING)
			//{
			//	for (SizeT j = 0; j < node->verticesBufferSize; j++)
			//	{
			//		MDLVTX& vertex = node->verticesBuffer[j];
			//		//if (vertex)
			//		{
			//			const bbox& b = node->GetBoundingBox();
			//			float dif = b.pmax.y() - b.pmin.y();
			//			float pct = (vertex.p.y - b.pmin.y()) / dif;
			//			pct = 1.0f - pct;
			//			vertex.fBlend = (1-m_fWaverFactor) + pct * m_fWaverFactor;
			//		}
			//	}
			//}
		}
		this->SetBoundingBox(b);
	}

	// 地物特效
	if( this->meshVersion >= 5 )
	{
		DWORD dwType;
		stream->Read(&m_dw3dsMaxEmitDataSize, sizeof(DWORD));
		m_pp3dsMaxEmitDatas = n_new_array(c3dsMaxEmitDataBase*, m_dw3dsMaxEmitDataSize);
		for( unsigned int i = 0; i < m_dw3dsMaxEmitDataSize; i++ )
		{
			m_pp3dsMaxEmitDatas[i] = NULL;

			stream->Read(&dwType, sizeof(DWORD));
			switch( dwType )
			{
			case 1: // PCloud
				m_pp3dsMaxEmitDatas[i] = n_new(c3dsMaxEmitDataCloud);
				m_pp3dsMaxEmitDatas[i]->ImportFromMemory( stream );
				break;

			case 2: // Super Spray
				m_pp3dsMaxEmitDatas[i] = n_new(c3dsMaxEmitDataSuperSpray);
				m_pp3dsMaxEmitDatas[i]->ImportFromMemory( stream );
				break;
			}
		}
	}

	char ch;

	stream->Read(&ch, sizeof(char));
	if (ch == '@')
	{
		if (!stream->Eof())
		{
			m_dwStopPoint = m_dwHeight * m_dwWidth;

			n_delete_array( m_pStaticStopPoint );
			m_pScopePoint = m_pLandStopPoint = m_pArrowStopPoint = m_pSkyStopPoint = NULL;

			if (m_dwStopPoint > 0)
			{
				// 一次建立四个阻档点阵列
				m_pStaticStopPoint = n_new_array(bool, m_dwStopPoint * 4);

				m_pScopePoint    = m_pStaticStopPoint;
				m_pLandStopPoint = m_pScopePoint + m_dwStopPoint ;
				m_pArrowStopPoint= m_pLandStopPoint + m_dwStopPoint ;
				m_pSkyStopPoint  = m_pArrowStopPoint + m_dwStopPoint ;

				for (SizeT j = 0; j < m_dwHeight; j++)
				{
					for (SizeT i = 0; i < m_dwWidth; i++)
					{
						int iMapPoint = ((m_dwWidth) * j) + i;

						// 地面阻档点
						stream->Read(&m_pLandStopPoint[iMapPoint], sizeof(bool));

						// 天空阻档点
						stream->Read(&m_pSkyStopPoint[iMapPoint], sizeof(bool));

						// 弓箭阻档点
						stream->Read(&m_pArrowStopPoint[iMapPoint], sizeof(bool));

						// 范围的点
						stream->Read(&m_pScopePoint[iMapPoint], sizeof(bool));
					}
				}
			}
		}
	}

	// 动作档
	String szFileName = GetResourceId().Value();
	//szFileName += m_pFileName;
	szFileName += ".act";

	if(szFileName.Length() > 0)
	{
		//加载.act文件
	}

	//m_pSkeletonHierarchyRoot->countSkeletonHierarchy("m_light", dwLightSize);
	cSkeletonHierarchy::SkeletonHierarchyMap* pSkeletonHierarchyLightLinkerMap =  m_pSkeletonHierarchyRoot->getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyLightLinker);
	m_dwLightSize = pSkeletonHierarchyLightLinkerMap->size();

	if (m_dwLightSize > 0)
	{
		n_delete_array( m_pLightSkeletonHierarchyTable );
		m_pLightSkeletonHierarchyTable = n_new_array(cSkeletonHierarchy*, m_dwLightSize);

		// 所有地物光片SkeletonHierarchy
		cSkeletonHierarchy::SkeletonHierarchyMap::iterator map_iterator, map_iterator_iend;
		map_iterator = pSkeletonHierarchyLightLinkerMap->begin();
		map_iterator_iend = pSkeletonHierarchyLightLinkerMap->end();
		SizeT j = 0;
		for (; map_iterator != map_iterator_iend; ++map_iterator)
		{
			m_pLightSkeletonHierarchyTable[j] = map_iterator->second;
			j++;
		}
	}

	//-----------------------------------------------
	/*for(SizeT i = 0 ; i < subMeshNum ; i++)
	{
		nodes[i].smoothSubMeshDuplicateVectexIndexCollectionNormalVector();
	}*/

	// 阻档点资料
	//szFileName = this->GetResourceId().Value();
	////szFileName += m_pFileName;
	//szFileName += ".sto";

	

	//if( szFileName.Length() > 0 )
	//{
	//	szName = _NEW char[iFileNameLength];
	//	strcpy(szName,szFileName.c_str());
	//}

	//cStopFile* pStopFile = n_new(cStopFile);
	//if( pStopFile )
	//{
	//	m_iDynamicStopSize = 0;

	//	if(pStopFile->LoadFromLPQFile(szName) == 0)
	//	{
	//		// 弄Θ
	//		cStop* pframeChild = (cStop *)pStopFile->m_pFieldRoot;

	//		m_dwStopPoint = m_dwHeight * m_dwWidth;

	//		SAFE_DELETE_ARRAY(m_pDynamicStopPoint);
	//		SAFE_DELETE_ARRAY(m_pClsPoint);    

	//		if (m_dwStopPoint > 0)
	//		{
	//			if (m_pStaticStopPoint == NULL)
	//			{
	//				m_pStaticStopPoint = _NEW bool[m_dwStopPoint * 4];
	//				m_pScopePoint    = m_pStaticStopPoint ;
	//				m_pLandStopPoint = m_pScopePoint + m_dwStopPoint ;
	//				m_pArrowStopPoint= m_pLandStopPoint + m_dwStopPoint ;
	//				m_pSkyStopPoint  = m_pArrowStopPoint + m_dwStopPoint ;
	//			}

	//			ZeroMemory( m_pStaticStopPoint, sizeof(bool) * (m_dwStopPoint * 4));
	//		}

	//		while (pframeChild != NULL)
	//		{
	//			m_dwWidth     = (int)pframeChild->wStopWidth ;
	//			m_dwHeight    = (int)pframeChild->wStopHeight;
	//			m_dwStopPoint = m_dwHeight * m_dwWidth;

	//			switch(pframeChild->GetDiscernID())
	//			{
	//			case STOP_FIELD_SCOPE:
	//				memcpy(m_pScopePoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);
	//				break;
	//			case STOP_FIELD_LAND:
	//				memcpy(m_pLandStopPoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);
	//				break;
	//			case STOP_FIELD_SKY:
	//				memcpy(m_pSkyStopPoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);
	//				break;
	//			case STOP_FIELD_ARROW:
	//				memcpy(m_pArrowStopPoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);
	//				break;
	//			case STOP_FIELD_DYNAMIC:
	//				if(m_dwStopPoint > 0)
	//				{
	//					m_pDynamicStopPoint = _NEW bool[m_dwStopPoint];
	//					memcpy(m_pDynamicStopPoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);

	//					for(i = 0 ; i < m_dwStopPoint ; i++)
	//					{
	//						if( m_pDynamicStopPoint[i] )
	//							m_iDynamicStopSize++;
	//					}
	//				}
	//				break;
	//			case STOP_FIELD_CLS:
	//				if(m_dwStopPoint > 0)
	//				{
	//					m_pClsPoint = _NEW bool[m_dwStopPoint];
	//					memcpy(m_pClsPoint, pframeChild->m_pStopPoint, sizeof(bool)*m_dwStopPoint);
	//				}
	//				break;
	//			}

	//			pframeChild = (cStop *)pframeChild->m_pNext;
	//		}
	//	}
	//	delete pStopFile;
	//}

	//SAFE_DELETE_ARRAY( szName );

	return true;
}

} // namespace Models
