#ifndef PHYSX_PHYSXENTITY_H
#define PHYSX_PHYSXENTITY_H
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXEntity

    (C) 2010
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "math/vector.h"
#include "math/point.h"

//------------------------------------------------------------------------------
namespace PhysX
{

class PhysXEntity : public Core::RefCounted
{
	DeclareClass(PhysXEntity);
public:
    /// constructor
    PhysXEntity();
    /// destructor
    virtual ~PhysXEntity();

    /// render the debug visualization of the entity
    virtual void RenderDebug();
	void SetResourceName(const Util::String& name);
	const Util::String& GetResourceName()const;
	/*void SetActorDesc(const NxActorDesc& actordesc);
	const NxActorDesc& GetActorDesc()const;
	void SetShapeDesc(const NxShapeDesc& shapedesc);
	const NxShapeDesc& GetShapeDesc()const;
	void SetBodyDesc(const NxBodyDesc& bodydesc);
	const NxBodyDesc& GetBodyDesc()const;*/
protected:
    Util::String resourceName;
    NxActorDesc* actorDesc;
	NxShapeDesc* shapeDesc;
	NxBodyDesc* bodyDesc;
};

//------------------------------------------------------------------------------
/**
    Set the resource name of the entity which will be loaded in
    OnActivate().
*/
inline void
PhysXEntity::SetResourceName(const Util::String& name)
{
    this->resourceName = name;
}

//------------------------------------------------------------------------------
/**
    Get the composite name of the entity.
*/
inline const Util::String&
PhysXEntity::GetResourceName() const
{
    return this->resourceName;
}

//inline const NxActorDesc* 
//PhysXEntity::GetActorDesc()const
//{
//	return this->actorDesc;
//}
//
//inline const NxShapeDesc& 
//PhysXEntity::GetShapeDesc()const
//{
//}
//
//inline void 
//PhysXEntity::SetBodyDesc(const NxBodyDesc& bodydesc)
//{
//}
//
//inline const NxBodyDesc& 
//PhysXEntity::GetBodyDesc()const
//{
//}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
