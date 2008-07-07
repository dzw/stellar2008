#pragma once
#ifndef DIRECT3D9_D3D9FONT_H
#define DIRECT3D9_D3D9FONT_H
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9Font

	A D3D9 specific subclass of FontBase.
    
    (C) 2008 by ctuo
*/    
#include "coregraphics/base/fontbase.h"
#include "coregraphics/fontdesc.h"


namespace CoreGraphics
{
	class FontServer;
}

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9Font : public Base::FontBase
{
    DeclareClass(D3D9Font);
public:
	/// constructor
	D3D9Font();
	/// destructor
	virtual ~D3D9Font();
	
	/// setup the fontdesc
	void Setup(const Base::FontDesc& c);
	/// get pointer to d3d9 CD3DFont
	ID3DXFont* GetD3DFont() const;

	void DrawText(const Util::String& text, const Math::float4& color, const Math::rectangle<float>& rect, uint flags, bool immediate);
	void DrawTextImmediate(const Util::String& text, const Math::float4& color, const Math::rectangle<float>& rect, uint flags);
private:
	friend class CoreGraphics::FontServer;

	/// discard the vertex layout object
	void Discard();
	/// set d3d9 vertex declaration pointer
	//void SetD3D9Font(ID3DXFont* ptr);

	//nAutoRef<nD3D9Server> refD3D9Server;
	ID3DXFont* d3dFont;
};

//------------------------------------------------------------------------------
/**
*/
inline
ID3DXFont*
D3D9Font::GetD3DFont() const
{
	n_assert(this->d3dFont);
	return this->d3dFont;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
#endif
