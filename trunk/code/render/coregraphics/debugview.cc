//------------------------------------------------------------------------------
//  win32minidump.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "CoreGraphics/debugview.h"
#include "coregraphics/fontserver.h"
#include "math/rectangle.h"
#include "core/sysfunc.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::DebugView, 'DBVW', Core::RefCounted);
ImplementSingleton(CoreGraphics::DebugView);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DebugView::DebugView()
{
	ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DebugView::~DebugView()
{
	DestructSingleton;
}

void 
DebugView::AddDebugString(const Util::String& head, DWORD& content)
{
	String cc;
	cc.Format("%d", content);
	AddDebugString(head, cc);
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugView::AddDebugString(const Util::String& head, const Util::String& content)
{
	IndexT index = debugString.FindIndex(head);
	if (InvalidIndex != index)
		this->debugString.EraseAtIndex(index);

	this->debugString.Add(head, content);
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugView::Render()
{
	//return;

	rectangle<float> rect(0, 0, 1, 1);
	String output;

	for (IndexT i = 0; i < debugString.Size(); i++)
	{
		output.Clear();
		output.Append(debugString.KeyAtIndex(i));
		output.Append(" : ");
		output.Append(debugString.ValueAtIndex(i));

		FontServer::Instance()->DrawText(output, float4(1, 0, 0, 1), rect, 0, true);

		rect.top += 0.021;
		rect.bottom += 0.021;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugView::Start()
{
	tt.Reset();
	tt.Start();
}

//------------------------------------------------------------------------------
/**
*/
float 
DebugView::Stop(Util::String head, bool dbgOut)
{
	tt.Stop();
	Timing::Time t = tt.GetTime();

	String output;
	output.AppendFloat((float)t);
	this->AddDebugString(head, output);

	if (dbgOut)
	{
		String out;
		out = head;
		out.Append(" ");
		out.Append(output);
		out.Append("\n");
		Core::SysFunc::DebugOut(out.AsCharPtr());
	}
	return (float)t;
}

} // namespace Debug
