#pragma once
#ifndef FIGHTER_FWORLD_H
#define FIGHTER_FWORLD_H
//------------------------------------------------------------------------------
/**
    
*/
#include "core/refcounted.h"
#include "core/types.h"
#include "core/singleton.h"
#include "flevel.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FWorldManager : public Core::RefCounted
{
	DeclareClass(FWorldManager);
	DeclareSingleton(FWorldManager);
public:
    /// constructor
    FWorldManager();
    /// destructor
    virtual ~FWorldManager();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
	/// checking
	bool IsOpen()const;

private:
    bool isOpen;
	Ptr<FLevel> curLevel;
	Ptr<Graphics::ModelEntity> mainActor;	// Ö÷½Ç
};

inline bool 
FWorldManager::IsOpen()const
{
	return this->isOpen;
}

} // namespace Test
//------------------------------------------------------------------------------
#endif
