#pragma once
#ifndef WOW_WORLDMANAGER_H
#define WOW_WORLDMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Models::WorldManager
  
    The WorldManager loads and creates shared Model objects.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
//#include "models/model.h"
#include "resources/resourceid.h"/*
#include "models/managedmodel.h"
#include "resources/resourcemapper.h"*/
#include "io/uri.h"
#include "wow/world/world.h"
#include "wow/world/managedWorld.h"
#include "wow/world/worldserver.h"
#include "graphics/modelentity.h"
#include "coregraphics/indexbuffer.h"
#include "math/float2.h"
#include "wow/wmo/managedwmo.h"
#include "wow/world/adt/maptile.h"
#include "graphics/MapTileentity.h"
#include "wow/world/Terrain/Terrainentity.h"

//------------------------------------------------------------------------------
namespace WOW
{

class WorldManager : public Core::RefCounted
{
    DeclareClass(WorldManager);
    DeclareSingleton(WorldManager);
public:
    /// constructor
    WorldManager();
    /// destructor
    virtual ~WorldManager();


    /// open the model server
    void Open();
    /// close the model server
    void Close();
    /// return true if model server is open
    bool IsOpen() const;
	

	void InitWorld(const Util::String& worldName);
	//void InitMinimap();
	//void InitDisplay();
	////void InitWMOs();
	//void InitLowresTerrain();

	//void enterTile(int x, int z);
	//MapTile *loadTile(int x, int z);
	//void tick(float dt);
	//void draw();

	//void outdoorLighting();
	//void outdoorLights(bool on);
	//void setupFog();

	///// Get the tile on wich the camera currently is on
	//unsigned int getAreaID();


	//////////////////////////////////////////////////////////////////////////
	/// �����ͼ
	void Go(/*const Math::vector& pos*/);
	/// ���maps����ָ��
	void SetMaps(bool* maps, int numMaps);
	/// ����ʵ��
	void AddManagedWMO(const Util::String& path);
	/// ����wmo����
	void SetWMOs(int numWmos);
	/// �����ͼ
	void EnterTile(int x, int z);
	/// ���ص�ͼ
	const Ptr<Graphics::TerrainEntity>& LoadTile(int x, int z);
	/// �жϵ�ͼ���Ƿ�����Ч��Χ
	bool oktile(int i, int j);
	/// ����
	void Update();
	/// �����������ָ��
	const Ptr<CoreGraphics::IndexBuffer>& GetIndexBuffer();
    ///
    void SetupIndexBuffer();
	///
	int IndexMapBuf(int x, int y);
	///
	Math::float2* GetDetailTexCoord();
	///
	Math::float2* GetAlphaTexCoord();
	/// ����ģ����Դ��������Loader������أ�������ƻ����ض��С�
	void AddModel(Ptr<Graphics::ModelEntity>& maptile, const Ptr<Graphics::ModelEntity>& model);


	bool isOpen;
	Ptr<WorldServer> worldServer;
	/// ����WDT��Դ
	Ptr<ManagedWorld> managedWDT;
	/// ͨ�õ���������
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	/// �������е����������ͷš�
	//Util::Array<Ptr<ManagedTexture> > managedTexture;

	Math::float2 texCoord[mapbufsize], alphaCoord[mapbufsize];



	/*const int MAPTILECACHESIZE = 16;

	Ptr<MapTile> maptilecache[MAPTILECACHESIZE];
	Ptr<MapTile> current[3][3];
	int ex,ez;*/


	Util::String basename;

	bool maps[64][64];
	/*IndexT lowrestiles[64][64];
	bool autoheight;*/

	Util::Array<Util::String> gwmos;
	Util::Array<Ptr<ManagedWMO> > gwmois;
	int gnWMO, nMaps;

	/// ��ǰ��ͼ 3*3
	Ptr<Graphics::TerrainEntity> curMaptile[3][3];
	/// ��ͼ���� 4*4
	Ptr<Graphics::TerrainEntity> mapTileCache[MAPTILECACHESIZE];

	Ptr<Graphics::GraphicsEntity> camera;
	bool loading;

	//float mapdrawdistance, modeldrawdistance, doodaddrawdistance, highresdistance;
	//float mapdrawdistance2, modeldrawdistance2, doodaddrawdistance2, highresdistance2;

	//float culldistance, culldistance2, fogdistance;

	//float l_const, l_linear, l_quadratic;

	////Skies *skies;
	//float time,animtime;

	//bool hadSky;

	//bool thirdperson,lighting,drawmodels,drawdoodads,drawterrain,drawwmo,loading,drawhighres,drawfog;
	//bool uselowlod;
	//bool useshaders;

	//IndexT detailtexcoords, alphatexcoords;

	//short *mapstrip,*mapstrip2;

	//IndexT water;
	//vector camera, lookat;
	////Frustum frustum;
	//int cx,cz;
	//bool oob;

	//WMOManager wmomanager;
	//ModelManager modelmanager;

	//OutdoorLighting *ol;
	//OutdoorLightStats outdoorLightStats;

	IndexT minimap;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
WorldManager::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::IndexBuffer>& 
WorldManager::GetIndexBuffer()
{
	n_assert(0 != this->indexBuffer);
	return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::float2* 
WorldManager::GetDetailTexCoord()
{
	return texCoord;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::float2*
WorldManager::GetAlphaTexCoord()
{
	return alphaCoord;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
