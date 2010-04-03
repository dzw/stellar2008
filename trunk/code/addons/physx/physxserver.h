#ifndef PHYSX_PHYSXSERVER_H
#define PHYSX_PHYSXSERVER_H
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXServer
    
    (C) 2010 
*/
#include "core/refcounted.h"
#include "core/singleton.h"

#include "NxPhysics.h"
#include "NxController.h"
//#include "NxControllerManager.h"
#include "ControllerManager.h"
#include "NxControllerManager.h"
#include "MemoryAllocator.h"
#include "ErrorStream.h"


//------------------------------------------------------------------------------
namespace PhysX
{

class PhysXServer : public Core::RefCounted
{
    DeclareClass(PhysXServer);
	DeclareSingleton(PhysXServer);
public:

    /// default constructor
    PhysXServer();
    /// destructor
    virtual ~PhysXServer();
	virtual void Open();
	virtual void Close();
	void CreateScene();

protected:
	bool InitCooking(NxUserAllocator* allocator=0, NxUserOutputStream* outputStream=0);
	void ReleaseScene();
	const char* GetNxSDKCreateError(const NxSDKCreateError& errorCode);

	NxPhysicsSDK*	physicsSDK;
	NxCookingInterface *cooking;
    NxScene* scene;
	NxControllerManager* controllerManager;
	MemoryAllocator*	allocator;
};

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    