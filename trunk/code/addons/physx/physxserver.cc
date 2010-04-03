//------------------------------------------------------------------------------
//  physics/shape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physx/physxserver.h"
#include "NxCooking.h"

namespace PhysX
{
ImplementClass(PhysX::PhysXServer, 'PHXS', Core::RefCounted);
ImplementSingleton(PhysX::PhysXServer);

const NxVec3			DefaultGravity(0.0f, -50.1f, 0.0f);

//------------------------------------------------------------------------------
/**
*/
PhysXServer::PhysXServer() 
{
    
}

//------------------------------------------------------------------------------
/**
*/
PhysXServer::~PhysXServer()
{

}

//------------------------------------------------------------------------------
/**

*/
void
PhysXServer::Open()
{
	allocator = n_new(MemoryAllocator);

	bool status = InitCooking();
	if (!status) {
		n_printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
		return;
	}

	// Initialize PhysicsSDK
	ErrorStream errorStream;
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, allocator, &errorStream, desc, &errorCode);
	if(physicsSDK == NULL) 
	{
		n_printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK.\n\n", errorCode, GetNxSDKCreateError(errorCode));
		return;
	}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (physicsSDK->getFoundationSDK().getRemoteDebugger())
		physicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	physicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);
	physicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	physicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);

	CreateScene();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXServer::Close()
{
	ReleaseScene();
	if (physicsSDK) NxReleasePhysicsSDK(physicsSDK);
	physicsSDK = NULL;

	if(allocator!=NULL)
	{
		n_delete(allocator);
		allocator=NULL;
	}
}

void
PhysXServer::CreateScene()
{
	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = DefaultGravity;
	scene = physicsSDK->createScene(sceneDesc);
	if(scene == NULL) 
	{
		n_printf("\nError: Unable to create a PhysX scene.\n\n");
		return;
	}

	NxMaterial * defaultMaterial = scene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
}

bool
PhysXServer::InitCooking(NxUserAllocator* allocator, NxUserOutputStream* outputStream)
{
	cooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if (!cooking) return false;
	return cooking->NxInitCooking(allocator, outputStream);
}

void 
PhysXServer::ReleaseScene()
{
	if(physicsSDK)
	{
		if(scene)
		{
			//releaseCharacterControllers(*scene);
			controllerManager->purgeControllers();
			NxReleaseControllerManager(controllerManager);
			physicsSDK->releaseScene(*scene);
			scene = NULL;
		}
	}
}

const char* 
PhysXServer::GetNxSDKCreateError(const NxSDKCreateError& errorCode) 
{
	switch(errorCode) 
	{
		case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
		case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
		case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
		case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
		case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
		case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
		case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
		default: return "Unknown error";
	}
};

}