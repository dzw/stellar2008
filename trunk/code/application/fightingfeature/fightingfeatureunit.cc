//------------------------------------------------------------------------------
//  game/fightingfeature.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fightingfeature/fightingfeatureunit.h"
#include "corefeature/corefeatureunit.h"
#include "game/gameserver.h"
#include "app/application.h"
#include "io/filestream.h"
#include "resources/simpleresourcemapper.h"
#include "coregraphics/texture.h"
#include "coregraphics/mesh.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/streammeshloader.h"
#include "coregraphics/streamanimationloader.h"
#include "resources/managedtexture.h"
#include "resources/managedmesh.h"
#include "graphics/quadtreestagebuilder.h"
#include "graphics/simplestagebuilder.h"
#include "math/bbox.h"
#include "lighting/spotlightentity.h"
#include "input/keyboard.h"
#include "managers/focusmanager.h"
#include "basegametiming/systemtimesource.h"
#include "addons/nebula2/nebula2wrapper.h"
#include "apprender/platformconfig.h"

#if __USE_HTTP__
#include "http/httpserver.h"
#include "coregraphics/debug/displaypagehandler.h"
#include "coregraphics/debug/texturepagehandler.h"
#include "coregraphics/debug/meshpagehandler.h"
#include "coregraphics/debug/shaderpagehandler.h"
#endif

// include all properties for known by managers::factorymanager
#include "properties/inputproperty.h"
#include "properties/graphicsproperty.h"
#include "properties/actorgraphicsproperty.h"
#include "properties/environmentgraphicsproperty.h"
#include "properties/lightproperty.h"
#include "properties/cameraproperty.h"
#include "properties/chasecameraproperty.h"
#include "properties/simplemoveproperty.h"

namespace FightingFeature
{
ImplementClass(FightingFeatureUnit, 'FFTX' , Game::FeatureUnit);
ImplementSingleton(FightingFeatureUnit);

using namespace App;
using namespace Core;
using namespace Util;
using namespace Math;
using namespace IO;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Resources;
using namespace Frame;
using namespace Models;
using namespace Lighting;
using namespace Input;
using namespace Game;
using namespace CoreFeature;
using namespace BaseGameFeature;
using namespace Nebula2;

//------------------------------------------------------------------------------
/**
*/
FightingFeatureUnit::FightingFeatureUnit()
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FightingFeatureUnit::~FightingFeatureUnit()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
FightingFeatureUnit::OnActivate()
{
	FeatureUnit::OnActivate();

	this->inputRuleManager = InputRuleManager::Create();
	this->skillManager = SkillManager::Create();

    /// add this feature for render debug callback
    //this->AddRenderDebugCallback(this, "Graphics");
}

//------------------------------------------------------------------------------
/**
*/
void
FightingFeatureUnit::OnDeactivate()
{
	this->inputRuleManager = 0;
	this->skillManager = 0;
    FeatureUnit::OnDeactivate();
}

}; // namespace Game
