#pragma once
#ifndef COREGRAPHICS_FONTSERVER_H
#define COREGRAPHICS_FONTSERVER_H
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::FontServer
    
    
    (C) 2008 by ctuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/font.h"
#include "math/vector.h"
#include "math/rectangle.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
class Font;

class FontServer : public Core::RefCounted
{
    DeclareClass(FontServer);
    DeclareSingleton(FontServer);
public:
    /// constructor
    FontServer();
    /// destructor
    virtual ~FontServer();
    
    /// open the server
    void Open();
    /// close the server
    void Close();
    /// return true if open
    bool IsOpen() const;
    
    /// create shared vertex layout object
	Ptr<Font> CreateSharedFont(const Util::String& rsrcName, const Base::FontDesc& fontDesc);


	/// set global scale factor for fonts
	void SetFontScale(float s);
	/// get global scale factor for fonts
	float GetFontScale() const;
	/// set minimum font height
	void SetMinFontHeight(int s);
	/// get minimum font height
	int GetMinFontHeight() const;

	void SetFont(Ptr<Font>& font);
	void DrawText(const Util::String& text, const Math::float4& color, const Math::rectangle<float>& rect, uint flags, bool immediate);
	
private:
	float fontScale;
	int fontMinHeight;
	Ptr<Font> refFont;
	Ptr<Font> refDefaultFont;

    bool isOpen;
	Util::Dictionary<Util::String, Ptr<Font>> fonts;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
FontServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontServer::SetMinFontHeight(int s)
{
	this->fontMinHeight = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
FontServer::GetMinFontHeight() const
{
	return this->fontMinHeight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontServer::SetFontScale(float s)
{
	this->fontScale = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
FontServer::GetFontScale() const
{
	return this->fontScale;
}

} // namespace FontServer
//------------------------------------------------------------------------------
#endif


    