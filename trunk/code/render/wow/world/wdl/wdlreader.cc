//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/wdl/wdlreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/vector3.h"
#include "math/quaternion.h"
#include "system/byteorder.h"
#include "models/modelserver.h"
#include "models/nodes/transformnode.h"


namespace WOW
{
ImplementClass(WOW::WDLReader, 'WDLR', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Attr;
using namespace System;
using namespace Models;
//using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
WDLReader::WDLReader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WDLReader::~WDLReader()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
WDLReader::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanRead());
    n_assert(this->stream->CanSeek());
    n_assert(!this->stream->IsMapped());
    n_assert(this->modelResId.Value().IsValid());

    if (StreamReader::Open())
    {
		ReadWDL();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
WDLReader::Close()
{
    n_assert(this->IsOpen());

    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
WDLReader::FillModel()
{
	n_assert(this->model.isvalid());

	this->SetToFirstModel();
	// go thru hierarchy and build modelnodes
	if (this->SetToFirstModelNode()) do
	{}
	while(this->SetToNextModelNode());

	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WDLReader::SetToFirstModel()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WDLReader::SetToNextModel()
{
    return false;
}

//------------------------------------------------------------------------------
/**
	½âÎöwdlÎÄ¼þ
*/
void 
WDLReader::ReadWDL()
{
	//GraphicsServer* gfxServer = GraphicsServer::Instance();
	//Ptr<Stage> stage = gfxServer->GetStageByName("DefaultStage");


	Util::FourCC fourcc;
	SizeT size;

	while (!this->stream->Eof())
	{
		this->stream->Read(&fourcc, 4);
		this->stream->Read(&size, 4);

		SizeT nextpos = this->stream->GetPosition() + size;

		if (fourcc == 'MOHD')
		{
			
		}
		else if (fourcc == 'MOTX')
		{
			
		}
		else if (fourcc == 'MOMT')
		{
			
		}

		this->stream->Seek((int)nextpos, Stream::Begin);
	}
}

} // namespace Models
