#pragma once
#ifndef DEBUG_DEBUGVIEW_H
#define DEBUG_DEBUGVIEW_H
//------------------------------------------------------------------------------
/**
    @class Win32::Win32MiniDump
  
    Win32 implementation of MiniDump.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/string.h"
#include "util/dictionary.h"
#include "timing/timer.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
class DebugView : public Core::RefCounted
{
	DeclareClass(DebugView);
	DeclareSingleton(DebugView);
public:
	DebugView();
	~DebugView();
	///
	void AddDebugString(const Util::String& head, const Util::String& content);
	void AddDebugString(const Util::String& head, DWORD content);
	void Render();

	/// ¼ÆÊ±
	void Start();
	float Stop(Util::String head, bool dbgOut = false);
private:
	Timing::Timer tt;
	Util::Dictionary<Util::String, Util::String> debugString;
};
 
} // namespace Win32
//------------------------------------------------------------------------------
#endif
