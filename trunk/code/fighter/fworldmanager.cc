//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/FWorldManager.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/view.h"

namespace Fighter
{
ImplementClass(Fighter::FWorldManager, 'FWRD', Core::RefCounted);
ImplementSingleton(Fighter::FWorldManager);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
FWorldManager::FWorldManager():
isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FWorldManager::~FWorldManager()
{
    if (this->IsOpen())
    {
        this->Close();
    }
	DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
FWorldManager::Open()
{
    n_assert(!this->IsOpen());

	const Ptr<Stage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage();
	this->curLevel = FLevel::Create();
	this->curLevel->Init(stage);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FWorldManager::Close()
{
	n_assert(this->IsOpen());

	this->curLevel->Clear();
	this->isOpen = false;
}

} // namespace Tools
