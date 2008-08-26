#pragma once
#ifndef COREGRAPHICS_MATERIAL_H
#define COREGRAPHICS_MATERIAL_H
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::Material
    
    ≤ƒ÷ ¿‡
    
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "resources/managedtexture.h"

namespace CoreGraphics
{

class Material :public Core::RefCounted
{
	DeclareClass(Material);
public:
	Math::float4 diffuseColor;		// Diffuse color RGBA 
	Math::float4 ambientColor;		// Ambient color RGB 
	Math::float4 specularColor;		// Specular 'shininess' 
	Math::float4 emissiveColor;		// Emissive color RGB 
	float        power;             // Sharpness if specular highlight
	BYTE		 blendMode;			// alpha blend mode
	BYTE		 lightMode;
	BYTE		 cullMode;
	BYTE		 wireFrame;
	Ptr<Resources::ManagedTexture> managedTexture;
};

}
//------------------------------------------------------------------------------
#endif
