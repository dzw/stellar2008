#pragma once
#ifndef KOK_TERRAINRENDER_H
#define KOK_TERRAINRENDER_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainRender
  
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "coregraphics/shaderinstance.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainRender : public Core::RefCounted
{
    DeclareClass(TerrainRender);
public:
    /// constructor
    TerrainRender();
    /// destructor
    virtual ~TerrainRender();

    /// 
    virtual bool ApplySharedState(const Ptr<Resources::ManagedTexture>& tex);
    /// 
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance() const;
	void Init();
protected:
	/// Œ∆¿Ì±‰¡ø
	Ptr<CoreGraphics::ShaderVariable> shaderVariable;

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
TerrainRender::GetShaderInstance() const
{
    return this->shaderInstance;
}

}
//------------------------------------------------------------------------------
#endif
