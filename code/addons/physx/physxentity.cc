//------------------------------------------------------------------------------
//  physics/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physx/physxentity.h"

namespace PhysX
{
ImplementClass(Physics::PhysicsEntity, 'PENT', Core::RefCounted);

uint PhysicsEntity::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
PhysXEntity::PhysXEntity() 
{
    PhysicsServer::Instance()->RegisterEntity(this);
}

//------------------------------------------------------------------------------
/**
*/
PhysXEntity::~PhysXEntity()
{
    
    PhysicsServer::Instance()->UnregisterEntity(this);
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes active
    (i.e. when it is attached to a game entity).
*/
void
PhysicsEntity::OnActivate()
{
    n_assert(!this->active);
    if (this->composite != 0)
    {
        // composite directly set
        this->composite->SetTransform(this->transform);   
        this->SetAlive(true);
    }
    else if (!this->resourceName.IsEmpty())
    {
        n_assert(this->composite == 0);

        // load and initialize composite object(s), the default
        // physics entity only works with one composite, we just
        // take the first in the physics.xml file
        CompositeLoader compLoader;
        Ptr<Composite> newComposite = compLoader.Load(this->resourceName)[0];
        newComposite->SetTransform(this->transform);    
        this->SetComposite(newComposite);
        this->SetAlive(true);
    }
    else
    {
        n_error("Physics::PhysicsEntity: no valid physics composite name given!");
    }

    this->active = true;
    this->numCollisions = 0;
    this->numStaticCollisions = 0;
    this->numDynamicCollisions = 0;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes inactive,
    (i.e. when it is detached from its game entity).
*/
void
PhysicsEntity::OnDeactivate()
{
    n_assert(this->active);
    this->SetAlive(false);
    // NOTE: WE DON'T RELEASE THE COMPOSITE, INSTEAD WE KEEP
    // THE ENTITY FULLY INTACT HERE ON PURPOSE!!
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is attached to a
    physics level object.

    @param  level   pointer to a level object
*/
void
PhysicsEntity::OnAttachedToLevel(Level* levl)
{
    n_assert(levl);
    n_assert(this->level == 0);
    this->level = levl;
    this->SetEnabled(false);
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is removed from
    its physics level object.
*/
void
PhysicsEntity::OnRemovedFromLevel()
{
    n_assert(this->level != 0);
    this->level = 0;
}

//------------------------------------------------------------------------------
/**
    Set the current transformation in world space. This method should
    only be called once at initialization time, since the main job
    of a physics object is to COMPUTE the transformation for a game entity.

    @param  m   a Math::matrix44 defining the world space transformation
*/
void
PhysicsEntity::SetTransform(const Math::matrix44& m)
{
    if (this->composite != 0)
    {
        this->composite->SetTransform(m);
    }
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
    Get the current transformation in world space. The transformation
    is updated during Physics::PhysicsServer::Trigger().

    @return     transformation of physics entity in world space
*/
Math::matrix44
PhysicsEntity::GetTransform() const
{
    if (this->composite != 0)
    {
        return this->composite->GetTransform();
    }
    else
    {
        return this->transform;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the transformation has changed during the frame.
*/
bool
PhysicsEntity::HasTransformChanged() const
{
    if (this->composite != 0)
    {
        return this->composite->HasTransformChanged();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Get the current world space velocity.
*/
Math::vector
PhysicsEntity::GetVelocity() const
{
    if ((this->composite != 0) && (this->composite->GetMasterBody()))
    {
        return this->composite->GetMasterBody()->GetLinearVelocity();
    }
    else
    {
        return Math::vector(0.0f, 0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of this entity.
*/
void
PhysicsEntity::RenderDebug()
{
    if (this->composite != 0)
    {
        this->composite->RenderDebug();
    }
}


//------------------------------------------------------------------------------
/**
    This method is called by the global collide callback to decide
    if a collision between this entity and a shape should be valid.
*/
bool
PhysicsEntity::OnCollide(Shape* collidee)
{
    n_assert(0 != collidee);
    if (collidee->GetRigidBody())
    {
        // a dynamic collision
        this->numDynamicCollisions++;
    }
    else
    {
        // a collision with a static object
        this->numStaticCollisions++;
    }
    this->numCollisions++;
    return true;
}

//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
PhysicsEntity::ApplyImpulseAtPos(const Math::vector& impulse, const Math::point& pos, bool multByMass)
{
    // get the physics composite
    Ptr<Physics::Composite> curComposite = this->GetComposite();
    
    n_assert(composite.isvalid());

    // apply the impulse to all bodys
    int idxBody;
    for(idxBody = 0; idxBody < curComposite->GetNumBodies(); idxBody++)
    {
        Physics::RigidBody* body = curComposite->GetBodyAt(idxBody);
        body->ApplyImpulseAtPos(impulse, pos, multByMass);
    }

}

//void 
//PhysXEntity::SetActorDesc(const NxActorDesc& actordesc)
//{
//	this->actorDesc = n_new(NxActoerDesc);
//	Memory::Copy(&actordesc, this->actorDesc, sizeof(NxActoerDesc));
//}

} // namespace Physics