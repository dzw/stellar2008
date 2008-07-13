//------------------------------------------------------------------------------
//  model.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/world.h"
#include "graphics/graphicsserver.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"
#include "math/matrix44.h"
#include "math/quaternion.h"

namespace Terrain
{
ImplementClass(Terrain::World, 'WOLD', Resources::Resource);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace Graphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
World::World()
{
	//for (int i=0; i<MAPTILECACHESIZE; i++) maptilecache[i] = 0;

	//autoheight = false;

	////skies = 0;
	////ol = 0;

	//for (int j=0; j<64; j++) {
	//	for (int i=0; i<64; i++) {
	//		lowrestiles[j][i] = 0;
	//	}
	//}

	//gnWMO = 0;
	//nMaps = 0;
	//water = 0;

	//time = 1450;
	//animtime = 0;

	//ex = ez = -1;

	//drawfog = false;

	//memset(maps,0,sizeof(maps));
}

//------------------------------------------------------------------------------
/**
*/
World::~World()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
World::Unload()
{    
    // call parent class
    Resource::Unload();
}

//------------------------------------------------------------------------------
/**
*/
//void 
//World::Init(Util::String name)
//{
//	
//}

} // namespace Models
