#ifndef PHYSX_PHYSXUTIL_H
#define PHYSX_PHYSXUTIL_H
//------------------------------------------------------------------------------
/**
	@class PhysX::PhysXUtil

    (C) 2010
*/

#include "NxPhysics.h"
#include "math/matrix44.h"

//------------------------------------------------------------------------------
namespace PhysX
{

class PhysXUtil
{
public:
    /// do a normal stabbing test and return closest contact
    //static bool RayCheck(const Math::vector& from, const Math::vector& to, const FilterSet& excludeSet, ContactPoint& outContact);
    /// do a stabbing test into the world with a ray bundle, return distance to intersection
    //static bool RayBundleCheck(const Math::vector& from, const Math::vector& to, const Math::vector& upVec, const Math::vector& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist);
	static const char* GetNxSDKCreateError(const NxSDKCreateError& errorCode);

	static Math::matrix44 GetMatrix(const NxVec3& pos, const NxMat33& orient);
	static void DrawActor(NxActor* actor, bool useShapeUserData);
	static void DrawShape(NxShape* shape, bool useShapeUserData);
	static void DrawBox(NxShape* box);
	static void DrawPlane(NxShape* plane);
	static void DrawSphere(NxShape* sphere);
	static void DrawCapsule(NxShape* capsule);
	static void DrawMesh(NxShape* mesh, bool useShapeUserData);
};

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
