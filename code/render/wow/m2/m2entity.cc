//------------------------------------------------------------------------------
//  ActorEntity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2entity.h"

namespace WOW
{
ImplementClass(WOW::M2Entity, 'M2EI', Graphics::ModelEntity);

//------------------------------------------------------------------------------
/**
*/
M2Entity::M2Entity():
curAnim(-1),
secondaryAnim(-1)
{

}

//------------------------------------------------------------------------------
/**
*/
M2Entity::~M2Entity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
M2Entity::OnActivate()
{
    ModelEntity::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
M2Entity::OnDeactivate()
{
    ModelEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    
*/
void
M2Entity::OnUpdate()
{
    ModelEntity::OnUpdate();

	if (!this->m2Char.isvalid() && this->modelInstance.isvalid())
	{
		const Ptr<Models::ModelInstance>& modelInstance = this->GetModelInstance();
        const Ptr<Models::ModelNodeInstance>& nodeInst = modelInstance->LookupNodeInstanceByRTTI(M2CharacterNodeInstance::RTTI);
        n_assert(nodeInst->IsA(M2CharacterNodeInstance::RTTI));
		this->m2Char = nodeInst.downcast<M2CharacterNodeInstance>();
	}
}

} // namespace Graphics
