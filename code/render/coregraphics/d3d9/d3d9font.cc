//------------------------------------------------------------------------------
//  d3d9vertexlayout.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/d3d9/d3d9font.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/displaymode.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/fontserver.h"
#include "math/vector.h"
#include "math/rectangle.h"

namespace Direct3D9
{
ImplementClass(Direct3D9::D3D9Font, 'D9FT', Base::FontBase);

using namespace CoreGraphics;
using namespace Util;
using namespace Base;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
D3D9Font::D3D9Font() :
	d3dFont(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9Font::~D3D9Font()
{
	n_assert(0 == this->d3dFont);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9Font::Setup(const FontDesc& c)
{
	n_assert(0 == this->d3dFont);

	// call parent class
	Base::FontBase::Setup(c);

	// check if the font resource must be created...
	//if (this->fontDesc.GetFilename())
	//{
	//	// mangle path
	//	String mangledPath = nFileServer2::Instance()->ManglePath(this->fontDesc.GetFilename());
	//	int numFonts = AddFontResource(mangledPath.Get());
	//	n_assert(numFonts > 0);
	//}

	int fontFlags = 0;
	switch (this->fontDesc.GetWeight())
	{
	case FontDesc::Thin:
		fontFlags |= FW_THIN;
		break;
	case FontDesc::Light:
		fontFlags |= FW_LIGHT;
		break;
	case FontDesc::Normal:
		fontFlags |= FW_NORMAL;
		break;
	case FontDesc::Bold:
		fontFlags |= FW_BOLD;
		break;
	case FontDesc::ExtraBold:
		fontFlags |= FW_EXTRABOLD;
		break;
	}

	Ptr<FontServer> fontServer = FontServer::Instance();
	SizeT height = SizeT(float(this->fontDesc.GetHeight()) * fontServer->GetFontScale());
	if (height < SizeT(fontServer->GetMinFontHeight()))
	{
		height = fontServer->GetMinFontHeight();
	}
	HRESULT hr = D3DXCreateFont(RenderDevice::Instance()->GetDirect3DDevice(),
								height, 0,
								fontFlags, 0,
								this->fontDesc.GetItalic(),
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								this->fontDesc.GetAntiAliased() ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE,
								this->fontDesc.GetTypeFace(),
								&this->d3dFont);

	n_assert(hr == S_OK);
	n_assert(this->d3dFont);
	
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9Font::Discard()
{
	n_assert(0 != this->d3dFont);
	this->d3dFont->Release();
	this->d3dFont = 0;
	//FontBase::Discard();
}

//------------------------------------------------------------------------------
/**
	Public text rendering routine. Either draws the text immediately,
	or stores the text internally in a text element array and draws it
	towards the end of frame.
*/
void
D3D9Font::DrawText(const String& text, 
				   const float4& color, 
				   const rectangle<float>& rect, 
				   uint flags, 
				   bool immediate)
{
	n_assert(d3dFont);
	
	//if (immediate)
	{
		this->DrawTextImmediate(text, color, rect, flags);
	}
	/*else
	{
	TextElement textElement(this->refFont, text, color, rect, flags);
	this->textElements.Append(textElement);
	}*/
	
}

//------------------------------------------------------------------------------
/**
	Internal text rendering routine.

	@param  text    the text to draw
	@param  color   the text color
	@param  rect    screen space rectangle in which to draw the text
	@param  flags   combination of nFont2::RenderFlags
*/
void
D3D9Font::DrawTextImmediate(const String& text, const float4& color, const rectangle<float>& rect, uint flags)
{
	n_assert(d3dFont);

	if (!text.IsValid())
	{
		// nothing to do.
		return;
	}

	DisplayMode mode = DisplayDevice::Instance()->GetDisplayMode();
	float dispWidth  = (float)mode.GetWidth();
	float dispHeight = (float)mode.GetHeight();
	RECT r;
	r.left   = (LONG)(rect.left * dispWidth);
	r.top    = (LONG)(rect.top * dispHeight);
	r.right  = (LONG)(rect.right * dispWidth);
	r.bottom = (LONG)(rect.bottom * dispHeight);

	DWORD d3dFlags = 0;
	String wordBreakString;
	if (flags & Font::Bottom)     d3dFlags |= DT_BOTTOM;
	if (flags & Font::Top)        d3dFlags |= DT_TOP;
	if (flags & Font::Center)     d3dFlags |= DT_CENTER;
	if (flags & Font::Left)       d3dFlags |= DT_LEFT;
	if (flags & Font::Right)      d3dFlags |= DT_RIGHT;
	if (flags & Font::VCenter)    d3dFlags |= DT_VCENTER;
	if (flags & Font::NoClip)     d3dFlags |= DT_NOCLIP;
	if (flags & Font::ExpandTabs) d3dFlags |= DT_EXPANDTABS;

	DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x(), color.y(), color.z(), color.w());
	
	//this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	//if (flags & Font::WordBreak)
	//{
	//	//d3dFlags |= DT_WORDBREAK;
	//	this->BreakLines(text, rect, wordBreakString);
	//	d3dFont->DrawText(NULL, wordBreakString.Get(), -1, &r, d3dFlags, d3dColor);
	//}
	//else
	{
		d3dFont->DrawText(NULL, text.AsCharPtr(), -1, &r, d3dFlags, d3dColor);
	}
	//this->d3dSprite->End();
}

} // namespace Direct3D9
