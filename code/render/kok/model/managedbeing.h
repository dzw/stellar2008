#pragma once
#ifndef TERRAIN_MANAGEDBEING_H
#define TERRAIN_MANAGEDBEING_H
//------------------------------------------------------------------------------
/**
	模型LOD、部件换装、换纹理

    (C) 2008 cTuo
*/
#include "models/managedmodel.h"
#include "kok/model/being.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ManagedBeing : public Models::ManagedModel
{
    DeclareClass(ManagedBeing);
public:
	ManagedBeing();
    /// get contained model resource
    const Ptr<Being>& GetBeing() const;
	
	void SetTextureId(int id);
	int  GetTextureId()const;
	void LoadTexture();

	void SetPartType(int type);
	int  GetPartType()const;

	void Update();
private:
	int textureId;
	int partType;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Being>&
ManagedBeing::GetBeing() const
{
    return this->GetResource().downcast<Being>();
}

inline void 
ManagedBeing::SetTextureId(int id)
{
	this->textureId = id;
}

inline int 
ManagedBeing::GetTextureId()const
{
	return this->textureId;
}

inline void 
ManagedBeing::LoadTexture()
{
	GetBeing()->LoadTexture(this->textureId);
}

inline void 
ManagedBeing::SetPartType(int type)
{
	this->partType = type;
}

inline int  
ManagedBeing::GetPartType()const
{
	return this->partType;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
