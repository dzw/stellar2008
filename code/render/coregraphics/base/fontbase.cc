//------------------------------------------------------------------------------
//  vertexlayoutbase.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/fontbase.h"

namespace Base
{
ImplementClass(Base::FontBase, 'FOTB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FontBase::FontBase()
{
}

//------------------------------------------------------------------------------
/**
*/
FontBase::~FontBase()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
FontBase::Setup(const FontDesc& desc)
{
	this->fontDesc = desc;
}

//------------------------------------------------------------------------------
/**
*/
const FontDesc& 
FontBase::GetFontDesc() const
{
	return this->fontDesc;
}

//------------------------------------------------------------------------------
/**
*/
void 
FontBase::DrawText(const Util::String& text, const Math::float4& color, const Math::rectangle<float>& rect, uint flags, bool immediate)
{
}

} // namespace Base
