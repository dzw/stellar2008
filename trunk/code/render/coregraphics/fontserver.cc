//------------------------------------------------------------------------------
//  vertexlayoutserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/fontserver.h"
#include "coregraphics/font.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::FontServer, 'FTSV', Core::RefCounted);
ImplementSingleton(CoreGraphics::FontServer);

using namespace Util;
using namespace Base;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
FontServer::FontServer() :
    isOpen(false),
	fontScale(1.0f),
	fontMinHeight(12),
	refFont(0),
	refDefaultFont(0)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FontServer::~FontServer()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
FontServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(this->fonts.IsEmpty());
    this->isOpen = true;

	FontDesc fontDesc;
	fontDesc.SetAntiAliased(true);
	fontDesc.SetHeight(16);
	fontDesc.SetWeight(FontDesc::Bold);
	fontDesc.SetTypeFace("Arial");
	Ptr<Font> font = this->CreateSharedFont("Default", fontDesc);
	
	this->refDefaultFont = font;
	this->SetFont(font);

	// create sprite object for batched rendering
	//HRESULT hr = D3DXCreateSprite(RenderDevice::Instance()->GetDirect3DDevice(), &this->d3dSprite);
	//n_dxtrace(hr, "D3DCreateSprite() failed!");
}

//------------------------------------------------------------------------------
/**
*/
void
FontServer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
    IndexT i;
    for (i = 0; i < this->fonts.Size(); i++)
    {
        this->fonts.ValueAtIndex(i)->Discard();
    }
    this->fonts.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Font>
FontServer::CreateSharedFont(const String& rsrcName, const FontDesc& fontDesc)
{
    n_assert(this->IsOpen());

    // get sharing signature from vertex components
    if (this->fonts.Contains(rsrcName))
    {
        // return existing instance
        return this->fonts[rsrcName];
    }
    else
    {
        // create new instance
		Ptr<Font> newFont = Font::Create();
		newFont->Setup(fontDesc);
		this->fonts.Add(rsrcName, newFont);
		return newFont;
    }
}

//------------------------------------------------------------------------------
/**
	Set the current font object for rendering.

	@param  font        pointer to a Font object
*/
void
FontServer::SetFont(Ptr<Font>& font)
{
	this->refFont = font;
}

//------------------------------------------------------------------------------
/**
	Public text rendering routine. Either draws the text immediately,
	or stores the text internally in a text element array and draws it
	towards the end of frame.
*/
void
FontServer::DrawText(const String& text, const float4& color, const rectangle<float>& rect, uint flags, bool immediate)
{
	if (this->refFont.isvalid())
	{
		//if (immediate)
		{
			this->refFont->DrawText(text, color, rect, flags, immediate);
		}
		/*else
		{
			TextElement textElement(this->refFont, text, color, rect, flags);
			this->textElements.Append(textElement);
		}*/
	}
}

//------------------------------------------------------------------------------
/**
	Draws the accumulated text elements and flushes the text buffer.
*/
//void
//FontServer::DrawTextBuffer()
//{
//	int i;
//	int num = this->textElements.Size();
//	for (i = 0; i < num; i++)
//	{
//		const TextElement& textElement = this->textElements[i];
//		this->DrawTextImmediate(textElement.refFont, textElement.text, textElement.color, textElement.rect, textElement.flags);
//	}
//	this->textElements.Clear();
//}



//------------------------------------------------------------------------------
/**
Get text extents.

- 16-Feb-04     floh    hmm, ID3DXFont extent computation is confused by spaces,
now uses GDI functions to compute text extents
*/
//float2
//FontServer::GetTextExtent(const String& text)
//{
//	int width = 0;
//	int height = 0;
//	float dispWidth  = (float)this->windowHandler.GetDisplayMode().GetWidth();
//	float dispHeight = (float)this->windowHandler.GetDisplayMode().GetHeight();
//	if (this->refFont.isvalid())
//	{
//		if (!this->refFont->IsLoaded())
//		{
//			this->refFont->Load();
//		}
//		ID3DXFont* d3dFont = ((nD3D9Font*)this->refFont.get())->GetD3DFont();
//		RECT rect = { 0 };
//		if (text.IsValid())
//		{
//			d3dFont->DrawTextA(NULL, text.Get(), -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
//		}
//		else
//		{
//			// hmm, an empty text should give us at least the correct height
//			d3dFont->DrawTextA(NULL, " ", -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, 0);
//			rect.right = 0;
//			rect.left = 0;
//		}
//		width = rect.right - rect.left;
//		height = rect.bottom - rect.top;
//	}
//	return vector2((float(width) / dispWidth), (float(height) / dispHeight));
//}
} // namespace CoreGraphics
