//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "FObjectManager.h"
#include "fhero.h"
#include "fnpc.h"
#include "fplayer.h"

namespace Fighter
{
ImplementClass(Fighter::FObjectManager, 'OBJM', Core::RefCounted);

using namespace Graphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FObjectManager::FObjectManager():
isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FObjectManager::~FObjectManager()
{

}

bool 
FObjectManager::Open()
{
	n_assert(!this->IsOpen());

	this->isOpen = true;

	return true;
}

void 
FObjectManager::Close()
{
	n_assert(this->IsOpen());

	
	this->isOpen = false;
}

void 
FObjectManager::Update()
{
}

Ptr<FObject> 
FObjectManager::CreateObject(BYTE type)
{
	Ptr<FObject> object;
	switch(type)
	{
	case ObjectType_Hero:
		object = FHero::Create();
		break;
	case ObjectType_Npc:
		object = FNpc::Create();
		break;
	case ObjectType_Player:
		object = FPlayer::Create();
		break;
	}
	this->objects[type].Add((DWORD)object, object);

	return object;
}

void 
FObjectManager::RemoveObject(const Ptr<FObject>& obj)
{
	this->objects[obj->GetObjectType()].Erase((DWORD)obj);
}

} // namespace Tools
