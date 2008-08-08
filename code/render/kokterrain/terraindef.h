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

#define DISTRICT_VERTS          8                             // 第一区的宽度
#define COMP                    5                             // 每个Tile的宽度
#define INVCOMP     (1.0f / COMP)                             // 1/COMP
#define TILE2POS(tile) ((0.5f + (tile)) * COMP)               // 求出所在Tile的中心Position
#define POS2TILE(pos)  (int((pos) * INVCOMP))                 // 不可以四舍五入!!!

#define CLIFF_HEIGHT            5.0f                          // 悬崖层级与层级的差值
#define MESH_HIGH_SIZE          1.0f                          // 单位高度值
#define CLIFF_NO_MAX_SIZE       2                             // 乱数取悬崖同类异型ID的上限

#define TERRAIN_MAXDTC 1200.0f                                // 最大可视距离
#define TERRAIN_MINDTC 300.0f                                 // 最小可视距离
#define TERRAIN_RANGEDTC (TERRAIN_MAXDTC - TERRAIN_MINDTC)

#define DISTRICTCACHESIZE		16							  // district缓冲大小(4*4)

/*****************************/
/*	map pos <--> tile
/*****************************/
extern void MapPos2Tile(float map_pos_x,float map_pos_z,int map_wide_tile,int* tile_x,int* tile_y);
extern void Tile2MapPos(int tile_x,int tile_y,int map_wide_tile,float* map_pos_x,float* map_pos_z);

#define CELL_MAX_VERTEX_BUFFER  5120                          // CELL 动态 Vertex Buffer 的最大值
#define CELL_MAX_INDEX_BUFFER   5120*3                        // CELL 动态  Index Buffer 的最大值

#define DRandom(x) (x ? (int)(rand()%((int)x)) : 0)

	// 060123 cyhsieh Terrain Lod
#define TERRAIN_LODLV1_DIST     400.0f

// 地图阻挡点资料
enum
{
	STOPPOINT_FLAG_NONE   = 0,            // 无
	STOPPOINT_FLAG_LAND   = ( 1 << 0 ),   // 地面
	STOPPOINT_FLAG_SKY    = ( 1 << 1 ),   // 天空
	STOPPOINT_FLAG_ARROW  = ( 1 << 2 ),   // 弓箭
	FISHINGSPOT_FLAG      = ( 1 << 3 ),   // 可钓鱼点
	SWIMSPOT_FLAG         = ( 1 << 4 ),   // 可游泳点
};

// 各资料栏位的代码
enum TERFILE
{
	TERFILE_SCENE_MAP_ID       =  0x00000100,   // 场景代码
	TERFILE_TILE_TABLE         =  0x00000101,   // 地表群组
	TERFILE_SPECTRUM_NAME      =  0x00000102,   // 光谱代码
	TERFILE_FOG_TABLE          =  0x00000103,   // 雾的代码
	TERFILE_MAP_SIZE           =  0x00000104,   // 地图的大小
	TERFILE_CLIFF_TABLE        =  0x00000105,   // 悬崖总类表
	TERFILE_CLIFF_LEVEL1       =  0x00000106,   // 各点的悬崖高度（未修正）
	TERFILE_WATER_LEVEL1       =  0x00000107,   // 各点的水池高度（未修正）
	TERFILE_TRANS_LEVEL1       =  0x00000108,   // 各点的斜坡高度（未修正）
	TERFILE_TILE               =  0x00000109,   // 地表贴图
	TERFILE_MAP_HIGH1          =  0x00000110,   // 地形高度（未修正）
	TERFILE_CLIFF_ID1          =  0x00000111,   // 各点的悬崖类别（未修正）
	TERFILE_THING_LIST         =  0x00000112,   // 地上物清单
	TERFILE_SOUND_SOURCE_LIST  =  0x00000113,   // 音效档清单
	TERFILE_SOUND_LIST         =  0x00000114,   // 音效清单
	TERFILE_SMALL_MAP_COLOR    =  0x00000115,   // 小地图用的索引色
	TERFILE_LIGHT_OBJ_LIST     =  0x00000116,   // 地上光源清单
	TERFILE_CLIFF_LEVEL2       =  0x00000206,   // 各点的悬崖高度（修正）
	TERFILE_WATER_LEVEL2       =  0x00000207,   // 各点的水池高度（修正）
	TERFILE_TRANS_LEVEL2       =  0x00000208,   // 各点的斜坡高度（修正）
	TERFILE_MAP_HIGH2          =  0x00000210,   // 地形高度（修正）
	TERFILE_CLIFF_ID2          =  0x00000211,   // 各点的悬崖类别（修正）
	TERFILE_STOP_POINT         =  0x00000300,   // 阻挡点资料
	TERFILE_FIXED_TIME         =  0x00000301,   // 固定场景时间
	TERFILE_SHOW_SUN           =  0x00000302,   // 显示太阳
	TERFILE_MESH_SCALE         =  0x00000303,   // 地形放大倍数
	TERFILE_FOG                =  0x00000304,   // 雾设定
	TERFILE_WATERREFLECT_COLOR =  0x00000305,   // 水面反射颜色
	TERFILE_SPEEDTREE          =  0x00000306,   // SpeedTree
	TERFILE_SUN_PITCHYAWANGLE  =  0x00000307,   // 太阳光角度
	TERFILE_AREARANGE_LIST     =  0x00000308,   // 区域范围
	TERFILE_VENDOR_AREA        =  0x00000350,   // 摆摊区范围
	TERFILE_DANCE_AREA         =  0x00000351,   // 跳舞区范围
	TERFILE_SAFE_AREA          =  0x00000352,   // 安全区范围
	TERFILE_FISHING_AREA       =  0x00000353,   // 钓鱼区范围
	TERFILE_CLOUD_TEXNAME1     =  0x00000400,   // 天空贴图1
	TERFILE_CLOUD_TEXNAME2     =  0x00000401,   // 天空贴图2
	TERFILE_CLOUD_TEXNAME3     =  0x00000402,   // 天空贴图3
	TERFILE_WATER_TEXNAME      =  0x00000500,   // 水贴图
};

// 资料读取状态
enum TERFILE_STATUS
{
	TERFILE_STATUS_CLOSE      = 0 ,             // 关闭中
	TERFILE_STATUS_LOAD       = 1 ,             // 载入资料中
	TERFILE_STATUS_RENDER_OK  = 2 ,             // 可以正常绘制
};

// 060504 cyhsieh 地形Loading分开执行
enum
{
	TERRAIN_LOADING_STEP1   = 0,    // 0:     初始设定与读取SCE识别码
	TERRAIN_LOADING_STEP2   = 1,    // 1~30:  读取SCE档
	TERRAIN_LOADING_STEP3   = 31,   // 31:    读取地上物阴影SM档
	TERRAIN_LOADING_STEP4   = 32,   // 32~36: 计算地形资料
	TERRAIN_LOADING_STEP5   = 37,   // 37~41: 读取地表贴图
	TERRAIN_LOADING_STEP6   = 42,   // 42~46: 设定其他地形资料
	TERRAIN_LOADING_STEP7   = 47,   // 47~61: 产生LOD空照贴图
	TERRAIN_LOADING_STEP8   = 62,   // 62~71: 设定地上物
	TERRAIN_LOADING_STEP9   = 72,   // 72~75: 水面反射贴图
	TERRAIN_LOADING_STEP10  = 76,   // 76:    SpeedTree Initialize
	TERRAIN_LOADING_STEP11  = 77,   // 77~89: SpeedTree Generate Instances
	TERRAIN_LOADING_STEP12  = 90,   // 90:    SpeedTree Setup Geometry
	TERRAIN_LOADING_STOP    = 91,   // 91:    停止
	TERRAIN_LOADING_NOFILE  = 92,   // 92:    没有SCE档
};

// 070926 cyhsieh 区域范围
enum                      // 特殊区域种类
{
	E_AREATYPE_VENDOR = 0,  // 摆摊区
	E_AREATYPE_DANCE,       // 跳舞区
	E_AREATYPE_SAFE,        // 安全区
	E_AREATYPE_FISHING,     // 钓鱼区
	E_AREATYPE_CONTEST,     // 竞技区
	E_AREATYPE_TEAKWOOD,    // 伐木区
	E_AREATYPE_IRON,        // 铁矿区
	E_AREATYPE_CRYSTAL,     // 水晶矿区
	E_AREATYPE_COTTON,      // 棉花区
	E_AREATYPE_COW,         // 野牛区
	E_AREATYPE_TOTAL,       // 特殊区域总数
};

/// 地图的TILE各层贴图
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
	DWORD dBakTile;			// 纪录上次的资料，来辩识是否改变
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
	Math::vector p;		// 用vector?vector3?
	Math::vector n;
	Math::float2 t1;
	Math::float2 t2;
	Math::float2 t3;
	Math::float2 t4;
	Math::float2 t5;
};

// 测试
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

  //cThingMesh*   m_pCliffData;        // 腶盫家
  DWORD         m_dwCliffMeshSize;   // Vertices 戈
  //MDLVTX*       m_pCliffMesh;        //  Vertices 戈
  BYTE          m_byBaseLayer;       // 程糷
  BYTE          m_byCliffNO;         // 腶盫摸钵id
  BYTE          m_byCliffTableID;
  BYTE          m_byMinCliffLevel;   // 程糷 
  BYTE          m_byMinWaterLevel;   // 程糷  
  bool          m_bCliff;            // 琌腶盫 
  bool          m_bTrans;            // 琌弊℡
  
  int           m_iApexRepeatSize;  // 璸衡 Cliff ユ钡翴
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
    