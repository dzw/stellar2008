#pragma once
#ifndef WOW_WORLD_H
#define WOW_WORLD_H
//------------------------------------------------------------------------------
/**
    @class Models::World
    
    A World represents the template for a renderable object, consisting
    of a hierarchy of ModelNodes which represent transformations and shapes.
    Models should generally be created through the ModelServer, which 
    guarantees that a given World is only loaded once into memory. To
    render a World, at least one ModelInstance must be created from the
    World. Usually one ModelInstance is created per game object. Generally
    speaking, all per-instance data lives in the ModelInstance objects, while
    all constant data lives in the World object. 

    (C) 2007 Radon Labs GmbH
*/
#include "resources/resource.h"
#include "math/bbox.h"
//#include "models/modelnode.h"
//#include "attr/attributecontainer.h"
//#include "models/visresolvecontainer.h"
//#include "wow/m2/modelheaders.h"
#include "math/vector3.h"
#include "math/vector.h"
#include "graphics/modelentity.h"
//#include "wow/world/adt/adt.h"
//#include "wow/world/wdt/wdt.h"
//#include "wow/world/wdl/wdl.h"


//------------------------------------------------------------------------------
namespace WOW
{
class WorldReader;


class World : public Resources::Resource
{
    DeclareClass(World);
public:
    /// constructor
    World();
    /// destructor
    virtual ~World();

    /// unload the resource, or cancel the pending load
    virtual void Unload();





	//const int MAPTILECACHESIZE = 16;

	//Ptr<MapTile> maptilecache[MAPTILECACHESIZE];
	//Ptr<MapTile> current[3][3];
	//int ex,ez;


	//Util::String basename;

	//bool maps[64][64];
	//IndexT lowrestiles[64][64];
	//bool autoheight;

	//Util::Array<Util::String> gwmos;
	////std::vector<WMOInstance> gwmois;
	//int gnWMO, nMaps;

	//float mapdrawdistance, modeldrawdistance, doodaddrawdistance, highresdistance;
	//float mapdrawdistance2, modeldrawdistance2, doodaddrawdistance2, highresdistance2;

	//float culldistance, culldistance2, fogdistance;

	//float l_const, l_linear, l_quadratic;

	////Skies *skies;
	//float time,animtime;

	//bool hadSky;

	//bool thirdperson,lighting,drawmodels,drawdoodads,drawterrain,drawwmo,/*loading,*/drawhighres,drawfog;
	//bool uselowlod;
	//bool useshaders;

	//IndexT detailtexcoords, alphatexcoords;

	//short *mapstrip,*mapstrip2;

	//IndexT water;
	//vector camera, lookat;
	////Frustum frustum;
	//int cx,cz;
	//bool oob;

	////WMOManager wmomanager;
	////ModelManager modelmanager;

	////OutdoorLighting *ol;
	////OutdoorLightStats outdoorLightStats;

	//IndexT minimap;



	/// 每帧更新：判断当前位置，是否需要加载新的地图
	//void Update();
	/// 资源是否加载完成
	//bool IsLoaded();
	/// 初始化加载
	//void Init(Util::String name);

	/*Ptr<WDT> wdt;
	Ptr<WDL> wdl;
	Ptr<MapTile> maptile;*/
};

//------------------------------------------------------------------------------
/**
*/
//inline bool 
//World::IsLoaded()
//{
//	n_assert(0 != wdt && 0 != maptile);
//	if (wdt->IsLoaded() && maptile->IsLoaded())
//		return true;
//	return false;
//}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    