#pragma once
#ifndef BASE_FONTBASE_H
#define BASE_FONTBASE_H
//------------------------------------------------------------------------------
/**
    @class Base::FontBase
    
    Base class for font. 
    
    (C) 2008 by ctuo
*/
#include "core/refcounted.h"
#include "coregraphics/fontdesc.h"
#include "math/vector.h"
#include "math/rectangle.h"

//------------------------------------------------------------------------------
namespace Base
{
class FontBase : public Core::RefCounted
{
    DeclareClass(FontBase);
public:
	/// rendering flags (apply to nGfxServer2::DrawText())
	enum RenderFlags
	{
		Bottom     = (1<<0),    // bottom-align text
		Top        = (1<<1),    // top-align text
		Center     = (1<<2),    // centers text horizontally
		Left       = (1<<3),    // align text to the left
		Right      = (1<<4),    // align text to the right
		VCenter    = (1<<5),    // center text vertically
		NoClip     = (1<<6),    // don't clip text (faster)
		ExpandTabs = (1<<7),    // expand tabs, doh
		WordBreak  = (1<<8),    // break words at end of line
	};

	/// constructor
	FontBase();
	/// destructor
	virtual ~FontBase();
	/// set font description
	void Setup(const FontDesc& desc);
	/// get font description
	const FontDesc& GetFontDesc() const;

	/// convert render flag string to enum
	static RenderFlags StringToRenderFlag(const char* str);
	/// convert render flag enum to string
	static const char* RenderFalgToString(RenderFlags renderFlag);

	void DrawText(const Util::String& text, const Math::float4& color, const Math::rectangle<float>& rect, uint flags, bool immediate);
protected:
	FontDesc fontDesc;
};

//------------------------------------------------------------------------------
/**
	Convert the render flag to its corresponded string.
*/
inline
const char*
FontBase::RenderFalgToString(FontBase::RenderFlags renderFlag)
{
	switch (renderFlag)
	{
	case FontBase::Bottom:            return "Bottom";
	case FontBase::Top:               return "Top";
	case FontBase::Center:            return "Center";
	case FontBase::Left:              return "Left";
	case FontBase::Right:             return "Right";
	case FontBase::VCenter:           return "VCenter";
	case FontBase::NoClip:            return "NoClip";
	case FontBase::ExpandTabs:        return "ExpandTabs";
	case FontBase::WordBreak:         return "WordBreak";
	default: n_error("nFont2::RenderFalgToString(): invalid render flag!");
	}
}


//------------------------------------------------------------------------------
/**
Convert the given string to its corresponded render flag.
*/
inline
FontBase::RenderFlags
FontBase::StringToRenderFlag(const char* str)
{
	n_assert(str);
	if (strcmp(str, "Bottom") == 0)     return Bottom;
	if (strcmp(str, "Top") == 0)        return Top;
	if (strcmp(str, "Center") == 0)     return Center;
	if (strcmp(str, "Left") == 0)       return Left;
	if (strcmp(str, "Right") == 0)      return Right;
	if (strcmp(str, "VCenter") == 0)    return VCenter;
	if (strcmp(str, "NoClip") == 0)     return NoClip;
	if (strcmp(str, "ExpandTabs") == 0) return ExpandTabs;
	if (strcmp(str, "WorkBreak") == 0)  return WordBreak;
	return (RenderFlags)0;
}

} // namespace Base
//------------------------------------------------------------------------------
#endif
    