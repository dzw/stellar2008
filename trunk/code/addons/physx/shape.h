#ifndef PHYSICS_SHAPE_H
#define PHYSICS_SHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::Shape
    
    (C) 2010 
*/
#include "core/refcounted.h"


//------------------------------------------------------------------------------
namespace Physics
{

class PhysXShapeDesc : public Core::RefCounted
{
    DeclareClass(PhysXShapeDesc);
public:

    /// default constructor
    Shape();
    /// destructor
    virtual ~Shape();
	/// return the type of this shape
    ShapeType GetType()const;
	

protected:
	/// all the stuff in common to all shapes.
    NxShapeType type;
	NxMat34 localPose;
	NxU32 uiShapeFlags;
	NxCollisionGroup uiShapeGroup;
	NxMaterialIndex uiMaterialIndex;
	NxReal density;
	NxReal mass;
	NxReal skinWidth;
	NiFixedString shapeName;
	NxU32 uiNonInteractiongCompartmentTypes;
	NxU32 auiCollisionBits[4];


};

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetEntity(PhysicsEntity* e)
{
    this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
PhysicsEntity*
Shape::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
const dMass&
Shape::GetOdeMass() const
{
    return this->odeMass;
}

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
Shape::GetGeomId() const
{
    return this->odeGeomId;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Shape::IsAttached() const
{
    return (0 != this->odeGeomId);
}

//------------------------------------------------------------------------------
/**
*/
inline
dSpaceID
Shape::GetSpaceId() const
{
    return this->odeSpaceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
Shape::Type
Shape::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
Shape::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetMaterialType(MaterialType t)
{
    n_assert(!this->IsAttached());
    this->materialType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
Shape::GetMaterialType() const
{
    return this->materialType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetCategoryBits(uint cat)
{
    this->categoryBits = cat;
    if (0 != this->odeGeomId)
    {
        dGeomSetCategoryBits(this->odeGeomId, this->categoryBits);
    }        
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Shape::GetCategoryBits() const
{
    return this->categoryBits;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetCollideBits(uint bits)
{
    this->collideBits = bits;
    if (0 != this->odeGeomId)
    {
        dGeomSetCollideBits(this->odeGeomId, this->collideBits);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Shape::GetCollideBits() const
{
    return this->collideBits;
}

//------------------------------------------------------------------------------
/**
*/
inline
RigidBody*
Shape::GetRigidBody() const
{
    return this->rigidBody;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetRigidBody(RigidBody* body)
{
    n_assert(!this->IsAttached());
    this->rigidBody = body;
}

//------------------------------------------------------------------------------
/**
*/
inline
Shape*
Shape::GetShapeFromGeom(dGeomID geom)
{
    return (Shape*) dGeomGetData(geom);
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    