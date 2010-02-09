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
ImplementSingleton(Fighter::FObjectManager);

using namespace Graphics;
using namespace Resources;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
FObjectManager::FObjectManager():
isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FObjectManager::~FObjectManager()
{
	if (this->IsOpen())
    {
        this->Close();
    }
	DestructSingleton;
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

	for (int i = 0; i < ObjectType_Num; i++)
	{
		const Array<Ptr<FObject>>& objs = this->objects[i];
		for (IndexT j = 0; j < objs.Size(); j++)
		{
			objs[j]->Clear();
		}
		this->objects[i].Clear();
	}
	this->isOpen = false;
}

void 
FObjectManager::Update()
{
	for (int i = 0; i < ObjectType_Num; i++)
	{
		const Array<Ptr<FObject>>& objs = this->objects[i];
		for (IndexT j = 0; j < objs.Size(); j++)
		{
			objs[j]->Update();
		}
	}
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
	object->Init();

	this->objects[type].InsertSorted(object);
	return object;
}

void 
FObjectManager::RemoveObject(const Ptr<FObject>& obj)
{
	if (obj.isvalid())
	{
		obj->Clear();
		IndexT ind = this->objects[obj->GetObjectType()].FindIndex(obj);
		if (ind != InvalidIndex)
		{
			this->objects[obj->GetObjectType()].EraseIndex(ind);
		}
	}
}

} // namespace Tools
