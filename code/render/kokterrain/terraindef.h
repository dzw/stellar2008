#pragma once
#ifndef KOK_TERRAINDEF_H
#define KOK_TERRAINDEF_H
//------------------------------------------------------------------------------
/**
    (C) 2008 cTuo
*/

#include "math/vector.h"
#include "math/vector3.h"
#include "math/float2.h"

//------------------------------------------------------------------------------
namespace KOK
{

#define DISTRICT_VERTS          8                             // ��һ���Ŀ��
#define COMP                    5                             // ÿ��Tile�Ŀ��
#define INVCOMP     (1.0f / COMP)                             // 1/COMP
#define TILE2POS(tile) ((0.5f + (tile)) * COMP)               // �������Tile������Position
#define POS2TILE(pos)  (int((pos) * INVCOMP))                 // ��������������!!!

#define CLIFF_HEIGHT            5.0f                          // ���²㼶��㼶�Ĳ�ֵ
#define MESH_HIGH_SIZE          1.0f                          // ��λ�߶�ֵ
#define CLIFF_NO_MAX_SIZE       2                             // ����ȡ����ͬ������ID������

#define TERRAIN_MAXDTC 1200.0f                                // �����Ӿ���
#define TERRAIN_MINDTC 300.0f                                 // ��С���Ӿ���
#define TERRAIN_RANGEDTC (TERRAIN_MAXDTC - TERRAIN_MINDTC)

#define DISTRICTCACHESIZE		16							  // district�����С(4*4)

/*****************************/
/*	map pos <--> tile
/*****************************/
extern void MapPos2Tile(float map_pos_x,float map_pos_z,int map_wide_tile,int* tile_x,int* tile_y);
extern void Tile2MapPos(int tile_x,int tile_y,int map_wide_tile,float* map_pos_x,float* map_pos_z);

#define CELL_MAX_VERTEX_BUFFER  5120                          // CELL ��̬ Vertex Buffer �����ֵ
#define CELL_MAX_INDEX_BUFFER   5120*3                        // CELL ��̬  Index Buffer �����ֵ

#define DRandom(x) (x ? (int)(rand()%((int)x)) : 0)

	// 060123 cyhsieh Terrain Lod
#define TERRAIN_LODLV1_DIST     400.0f

// ��ͼ�赲������
enum
{
	STOPPOINT_FLAG_NONE   = 0,            // ��
	STOPPOINT_FLAG_LAND   = ( 1 << 0 ),   // ����
	STOPPOINT_FLAG_SKY    = ( 1 << 1 ),   // ���
	STOPPOINT_FLAG_ARROW  = ( 1 << 2 ),   // ����
	FISHINGSPOT_FLAG      = ( 1 << 3 ),   // �ɵ����
	SWIMSPOT_FLAG         = ( 1 << 4 ),   // ����Ӿ��
};

// ��������λ�Ĵ���
enum TERFILE
{
	TERFILE_SCENE_MAP_ID       =  0x00000100,   // ��������
	TERFILE_TILE_TABLE         =  0x00000101,   // �ر�Ⱥ��
	TERFILE_SPECTRUM_NAME      =  0x00000102,   // ���״���
	TERFILE_FOG_TABLE          =  0x00000103,   // ��Ĵ���
	TERFILE_MAP_SIZE           =  0x00000104,   // ��ͼ�Ĵ�С
	TERFILE_CLIFF_TABLE        =  0x00000105,   // ���������
	TERFILE_CLIFF_LEVEL1       =  0x00000106,   // ��������¸߶ȣ�δ������
	TERFILE_WATER_LEVEL1       =  0x00000107,   // �����ˮ�ظ߶ȣ�δ������
	TERFILE_TRANS_LEVEL1       =  0x00000108,   // �����б�¸߶ȣ�δ������
	TERFILE_TILE               =  0x00000109,   // �ر���ͼ
	TERFILE_MAP_HIGH1          =  0x00000110,   // ���θ߶ȣ�δ������
	TERFILE_CLIFF_ID1          =  0x00000111,   // ������������δ������
	TERFILE_THING_LIST         =  0x00000112,   // �������嵥
	TERFILE_SOUND_SOURCE_LIST  =  0x00000113,   // ��Ч���嵥
	TERFILE_SOUND_LIST         =  0x00000114,   // ��Ч�嵥
	TERFILE_SMALL_MAP_COLOR    =  0x00000115,   // С��ͼ�õ�����ɫ
	TERFILE_LIGHT_OBJ_LIST     =  0x00000116,   // ���Ϲ�Դ�嵥
	TERFILE_CLIFF_LEVEL2       =  0x00000206,   // ��������¸߶ȣ�������
	TERFILE_WATER_LEVEL2       =  0x00000207,   // �����ˮ�ظ߶ȣ�������
	TERFILE_TRANS_LEVEL2       =  0x00000208,   // �����б�¸߶ȣ�������
	TERFILE_MAP_HIGH2          =  0x00000210,   // ���θ߶ȣ�������
	TERFILE_CLIFF_ID2          =  0x00000211,   // ������������������
	TERFILE_STOP_POINT         =  0x00000300,   // �赲������
	TERFILE_FIXED_TIME         =  0x00000301,   // �̶�����ʱ��
	TERFILE_SHOW_SUN           =  0x00000302,   // ��ʾ̫��
	TERFILE_MESH_SCALE         =  0x00000303,   // ���ηŴ���
	TERFILE_FOG                =  0x00000304,   // ���趨
	TERFILE_WATERREFLECT_COLOR =  0x00000305,   // ˮ�淴����ɫ
	TERFILE_SPEEDTREE          =  0x00000306,   // SpeedTree
	TERFILE_SUN_PITCHYAWANGLE  =  0x00000307,   // ̫����Ƕ�
	TERFILE_AREARANGE_LIST     =  0x00000308,   // ����Χ
	TERFILE_VENDOR_AREA        =  0x00000350,   // ��̯����Χ
	TERFILE_DANCE_AREA         =  0x00000351,   // ��������Χ
	TERFILE_SAFE_AREA          =  0x00000352,   // ��ȫ����Χ
	TERFILE_FISHING_AREA       =  0x00000353,   // ��������Χ
	TERFILE_CLOUD_TEXNAME1     =  0x00000400,   // �����ͼ1
	TERFILE_CLOUD_TEXNAME2     =  0x00000401,   // �����ͼ2
	TERFILE_CLOUD_TEXNAME3     =  0x00000402,   // �����ͼ3
	TERFILE_WATER_TEXNAME      =  0x00000500,   // ˮ��ͼ
};

// ���϶�ȡ״̬
enum TERFILE_STATUS
{
	TERFILE_STATUS_CLOSE      = 0 ,             // �ر���
	TERFILE_STATUS_LOAD       = 1 ,             // ����������
	TERFILE_STATUS_RENDER_OK  = 2 ,             // ������������
};

// 060504 cyhsieh �a�ε���Loading�ֿ�ִ��
enum
{
	TERRAIN_LOADING_STEP1   = 0,    // 0:     ��ʼ�趨���ȡSCEʶ����
	TERRAIN_LOADING_STEP2   = 1,    // 1~30:  ��ȡSCE��
	TERRAIN_LOADING_STEP3   = 31,   // 31:    ��ȡ��������ӰSM��
	TERRAIN_LOADING_STEP4   = 32,   // 32~36: �����������
	TERRAIN_LOADING_STEP5   = 37,   // 37~41: ��ȡ�ر���ͼ
	TERRAIN_LOADING_STEP6   = 42,   // 42~46: �趨������������
	TERRAIN_LOADING_STEP7   = 47,   // 47~61: ����LOD������ͼ
	TERRAIN_LOADING_STEP8   = 62,   // 62~71: �趨������
	TERRAIN_LOADING_STEP9   = 72,   // 72~75: ˮ�淴����ͼ
	TERRAIN_LOADING_STEP10  = 76,   // 76:    SpeedTree Initialize
	TERRAIN_LOADING_STEP11  = 77,   // 77~89: SpeedTree Generate Instances
	TERRAIN_LOADING_STEP12  = 90,   // 90:    SpeedTree Setup Geometry
	TERRAIN_LOADING_STOP    = 91,   // 91:    ֹͣ
	TERRAIN_LOADING_NOFILE  = 92,   // 92:    û��SCE��
};

// 070926 cyhsieh ����Χ
enum                      // ������������
{
	E_AREATYPE_VENDOR = 0,  // ��̯��
	E_AREATYPE_DANCE,       // ������
	E_AREATYPE_SAFE,        // ��ȫ��
	E_AREATYPE_FISHING,     // ������
	E_AREATYPE_CONTEST,     // ������
	E_AREATYPE_TEAKWOOD,    // ��ľ��
	E_AREATYPE_IRON,        // ������
	E_AREATYPE_CRYSTAL,     // ˮ������
	E_AREATYPE_COTTON,      // �޻���
	E_AREATYPE_COW,         // Ұţ��
	E_AREATYPE_TOTAL,       // ������������
};

/// ��ͼ��TILE������ͼ
typedef struct tagDWORD 
{
	union
	{
		BYTE bBYTE[4];
		DWORD dwDWORD;
	};
}stDWORD;

struct ThingTex 
{
public:
	DWORD dBakTile;			// ��¼�ϴε����ϣ�����ʶ�Ƿ�ı�
	DWORD dTile;
	stDWORD stTile;

	ThingTex()
	{
		dBakTile = 255;
		dTile	 = 0;
		dTile	|= (1);
		stTile.dwDWORD = 1;
	}
};

struct TileMesh
{
	Math::vector p;		// ��vector?vector3?
	Math::vector n;
	Math::float2 t1;
	Math::float2 t2;
	Math::float2 t3;
	Math::float2 t4;
	Math::float2 t5;
};

// ����
struct MeshTest 
{
	Math::vector p;
};

struct tApexRepeat
{
	int         iSize;
	WORD*       m_pIndxe;
	Math::vector vP;
	bool        bCountNormalize;

	tApexRepeat()
	{
		iSize           = 0;
		m_pIndxe        = NULL;
		bCountNormalize = false ;
	};

	~tApexRepeat()
	{
		//SAFE_DELETE_ARRAY( m_pIndxe )
	};
};

struct TerrainMeshData 
{
  TileMesh      meshData[4];
  BYTE          byWaterLevel[4];
  BYTE          byLayer[4];
  WORD          wVertexStartIndex;

  //cThingMesh*   m_pCliffData;        // �a�V���ҫ�
  DWORD         m_dwCliffMeshSize;   // Vertices ����Ƥj�p
  //MDLVTX*       m_pCliffMesh;        // �s�� Vertices �����
  BYTE          m_byBaseLayer;       // �̧C�h
  BYTE          m_byCliffNO;         // �a�V�P��������id
  BYTE          m_byCliffTableID;
  BYTE          m_byMinCliffLevel;   // �̧C���@�h 
  BYTE          m_byMinWaterLevel;   // �̧C���@�h  
  bool          m_bCliff;            // �O�_���a�V 
  bool          m_bTrans;            // �O�_���שY
  
  int           m_iApexRepeatSize;  // �p�� Cliff ���汵�I
  tApexRepeat*  m_pApexRepeat;

  TerrainMeshData()
  //:m_pCliffData(NULL), m_pCliffMesh(NULL), m_pApexRepeat(NULL)
  {
    wVertexStartIndex = 0;
    m_dwCliffMeshSize = 0;
    m_byBaseLayer = 0;
    m_byCliffNO = 0;
    m_byCliffTableID = 0;
    m_byMinCliffLevel = 0;
    m_byMinWaterLevel = 0;
    m_bCliff = false;
    m_bTrans = false;
    m_iApexRepeatSize = 0;
    
    for( int i = 0; i < 4; i++ )
    {
      byLayer[i] = 0x00;
      byWaterLevel[i] = 0x00;
    }
  }
  
  ~TerrainMeshData()
  {
    //SAFE_DELETE_ARRAY( m_pCliffMesh ); 
    //SAFE_DELETE_ARRAY( m_pApexRepeat ); 
  }

  void SetTextureData( int iTexture, int iID, int iLayer );
};


} // namespace Models
//------------------------------------------------------------------------------
#endif
    