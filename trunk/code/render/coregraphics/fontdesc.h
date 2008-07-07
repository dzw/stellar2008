#ifndef BASE_FONTDESC_H
#define BASE_FONTDESC_H
//------------------------------------------------------------------------------
/**
	@class FontDesc
	@ingroup Gfx2

	A font description object used to create a nFont object.

	(C) 2003 RadonLabs GmbH
*/
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Base
{
class FontDesc
{
public:
	/// font weights
	enum Weight
	{
		Thin,
		Light,
		Normal,
		Bold,
		ExtraBold,
		InvalidWeight,
	};

	/// constructor
	FontDesc();
	/// destructor
	~FontDesc();
	/// equality operator
	bool operator==(const FontDesc& rhs) const;
	/// set font height
	void SetHeight(int h);
	/// get font height
	int GetHeight() const;
	/// set font weight
	void SetWeight(Weight w);
	/// get font weight
	Weight GetWeight() const;
	/// set italic flag
	void SetItalic(bool b);
	/// get italic flag
	bool GetItalic() const;
	/// set underline flag
	void SetUnderline(bool b);
	/// get underline flag
	bool GetUnderline() const;
	/// set antialiased flag
	void SetAntiAliased(bool b);
	/// get antialiased flag
	bool GetAntiAliased() const;
	/// set typeface name
	void SetTypeFace(const char* n);
	/// get typeface name
	const char* GetTypeFace() const;
	/// set optional font file name
	void SetFilename(const char* n);
	/// get optional font file name
	const char* GetFilename() const;

	/// convert render flag string to enum
	static Weight StringToWeight(const Util::String& str);
	/// convert render flag enum to string
	static Util::String WeightToString(Weight renderFlag);

private:
	int height;
	Weight weight;
	bool italic;
	bool underline;
	bool antiAliased;
	Util::String typeFace;
	Util::String filename;
};

//------------------------------------------------------------------------------
/**
*/
inline
FontDesc::FontDesc() :
	height(10),
	weight(Normal),
	italic(false),
	underline(false),
	antiAliased(true),
	typeFace("Arial")
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline
FontDesc::~FontDesc()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FontDesc::operator==(const FontDesc& rhs) const
{
	return ((this->height == rhs.height) &&
		(this->weight == rhs.weight) &&
		(this->italic == rhs.italic) &&
		(this->underline == rhs.underline) &&
		(this->antiAliased == rhs.antiAliased) &&
		(this->typeFace == rhs.typeFace));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetHeight(int h)
{
	this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
FontDesc::GetHeight() const
{
	return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetWeight(Weight w)
{
	this->weight = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
FontDesc::Weight
FontDesc::GetWeight() const
{
	return this->weight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetItalic(bool b)
{
	this->italic = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FontDesc::GetItalic() const
{
	return this->italic;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetUnderline(bool b)
{
	this->underline = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FontDesc::GetUnderline() const
{
	return this->underline;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetAntiAliased(bool b)
{
	this->antiAliased = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FontDesc::GetAntiAliased() const
{
	return this->antiAliased;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetTypeFace(const char* n)
{
	n_assert(n);
	this->typeFace = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
FontDesc::GetTypeFace() const
{
	return this->typeFace.IsEmpty() ? 0 : this->typeFace.AsCharPtr();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FontDesc::SetFilename(const char* n)
{
	n_assert(n);
	this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
FontDesc::GetFilename() const
{
	return this->filename.IsEmpty() ? 0 : this->filename.AsCharPtr();
}

//------------------------------------------------------------------------------
/**
Convert the weight to its corresponded string.
*/
inline
Util::String
FontDesc::WeightToString(FontDesc::Weight renderFlag)
{
	switch (renderFlag)
	{
	case FontDesc::Thin:          return Util::String("Thin");
	case FontDesc::Light:         return Util::String("Light");
	case FontDesc::Normal:        return Util::String("Normal");
	case FontDesc::Bold:          return Util::String("Bold");
	case FontDesc::ExtraBold:     return Util::String("ExtraBold");
	default:
		n_error("FontDesc::WeightToString(): invalid weight!");
	}
}


//------------------------------------------------------------------------------
/**
Convert the given string to its corresponded weight.
*/
inline
FontDesc::Weight
FontDesc::StringToWeight(const Util::String& str)
{
	n_assert(!str.IsEmpty());
	if ("Thin" == str)
	{
		return Thin;
	}
	if ("Light" == str)
	{
		return Light;
	}
	if ("Normal" == str)
	{
		return Normal;
	}
	if ("Bold" == str)
	{
		return Bold;
	}
	if ("ExtraBold" == str)
	{
		return ExtraBold;
	}
	n_error("FontDesc::StringToWeight(): invalid weight!");
	return InvalidWeight;
}

}

//------------------------------------------------------------------------------
#endif

