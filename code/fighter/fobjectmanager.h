#pragma once
#ifndef FIGHTER_OBJECTMANAGER_H
#define FIGHTER_OBJECTMANAGER_H
//------------------------------------------------------------------------------
/**

*/
#include "core/refcounted.h"
#include "core/types.h"
#include "core/singleton.h"
#include "graphics/view.h"
#include "math/vector.h"
#include "util/hashmap.h"
#include "fobject.h"

//------------------------------------------------------------------------------
namespace Fighter
{


class FObjectManager : public Core::RefCounted
{
	DeclareClass(FObjectManager);
	DeclareSingleton(FObjectManager);
public:
    /// constructor
    FObjectManager();
    /// destructor
    virtual ~FObjectManager();
	/// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
	/// checking
	bool IsOpen()const;
	///
	void Update();

	Ptr<FObject> CreateObject(BYTE type);
	void RemoveObject(const Ptr<FObject>& obj);
private:
	bool isOpen;
	Util::HashMap<DWORD, Ptr<FObject>, Util::DwordCompare> objects[ObjectType_Num];
};

inline bool 
FObjectManager::IsOpen()const
{
	return this->isOpen;
}

} // namespace Test
//------------------------------------------------------------------------------
#endif
