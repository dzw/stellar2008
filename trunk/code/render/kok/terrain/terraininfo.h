#pragma once
#ifndef KOK_TERRAININFO_H
#define KOK_TERRAININFO_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainInfo
    
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace KOK
{

class TerrainInfo
{
public:
    /// constructor
    TerrainInfo();
    /// destructor
    virtual ~TerrainInfo();

	/// �����tile ����
	unsigned int GetTileCountX() { return m_iMapWide; };
	/// ȫ��tile����
	unsigned int GetTotalTileCount() { return m_iMapSize; };
	/// ����district����
	unsigned int GetDistrictCountX() { return m_iDistrictWide; };
	/// ȫ��district����
	unsigned int GetTotalDistrictCount() { return m_iDistrictSize; };
	/// grid��������
	unsigned int GetGridCountX() { return m_iMapNodeWide; };
	/// grid������
	unsigned int GetTotalGridCount() { return m_iMapNodeSize; };

	/// set tile count X
	void SetTileCountX(unsigned int count);
	/// set map ID string 
	void SetMapIDString(char* sz_mapid) { m_szMapID=sz_mapid; };

	bool IsReady() { return m_bIsReady; };

	enum{DistrcitTileCount=8};
private:
	Util::String	m_szMapID;			// ��ͼID
	unsigned int	m_iMapSize;			// ��ͼ��С���˿�վ�Ŀռ䣩ȫ���ĸ�����
	unsigned int	m_iMapWide;			// ��ͼ��ȣ��˿�վ�Ŀռ䣩һ�еĸ�����
	unsigned int	m_iMapNodeWide;		// �ر�ڵ�Ŀ��
	unsigned int	m_iMapNodeSize;		// �ر�ڵ�Ĵ�С
	unsigned int	m_iDistrictWide;	// ����Ŀ��
	unsigned int	m_iDistrictSize;	// ����Ĵ�С

	bool m_bIsReady;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    