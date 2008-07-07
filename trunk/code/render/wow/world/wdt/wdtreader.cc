//------------------------------------------------------------------------------
//  n2modelreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/wdt/wdtreader.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/vector3.h"
#include "math/quaternion.h"
#include "system/byteorder.h"
#include "models/modelserver.h"
#include "models/nodes/transformnode.h"
#include "wow/wmo/wmoserver.h"
#include "wow/world/worldmanager.h"

namespace WOW
{
ImplementClass(WOW::WDTReader, 'WDTR', Models::ModelReader);

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
WDTReader::WDTReader() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WDTReader::~WDTReader()
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
WDTReader::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanRead());
    n_assert(this->stream->CanSeek());
    n_assert(!this->stream->IsMapped());
    n_assert(this->modelResId.Value().IsValid());

    if (StreamReader::Open())
    {
		ReadWDT();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
WDTReader::Close()
{
    n_assert(this->IsOpen());

    StreamReader::Close();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
WDTReader::FillModel()
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
WDTReader::SetToFirstModel()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
WDTReader::SetToNextModel()
{
    return false;
}

//------------------------------------------------------------------------------
/**
	½âÎöwdtÎÄ¼þ¡£
*/
void 
WDTReader::ReadWDT()
{
	WorldManager *wm = WorldManager::Instance();

	Util::FourCC fourcc;
	SizeT size;

	while (!this->stream->Eof())
	{
		this->stream->Read(&fourcc, 4);
		this->stream->Read(&size, 4);

		SizeT nextpos = this->stream->GetPosition() + size;

		if (fourcc == 'MAIN') {
			bool maps[64][64];
			int nMaps = 0;
			for (int j=0; j<64; j++) {
				for (int i=0; i<64; i++) {
					int d;
					this->stream->Read(&d, 4);
					if (d) {
						maps[j][i] = true;
						nMaps++;
					} else maps[j][i] = false;
					this->stream->Read(&d, 4);
				}
			}
			wm->SetMaps(&maps[0][0], nMaps);
		}
		else if (fourcc == 'MODF') {
			// global wmo instance data
			//gnWMO = (int)size / 64;
			// WMOS and WMO-instances are handled below in initWMOs()
			wm->SetWMOs((int)size / 64);
		}
		else if (fourcc == 'MWMO') {
			// global map objects
			if (size) {
				char *buf = new char[size];
				this->stream->Read(buf, size);
				char *p=buf;
				while (p<buf+size) {
					String path(p);
					p+=strlen(p)+1;

					//wmomanager.add(path);
					//this->wdt->gwmos.Append(path);
					
					//this->wdt->gwmois.Append(WMOServer::Instance()->LoadManagedWMO(path));
					wm->AddManagedWMO(path);
				}
				delete[] buf;
			}
		}
		else if (fourcc == 'MODF')
		{
			int id;

			vector pos = vector(0,0,0);
			vector dir, pos2, pos3;
			float ff[3];
			int d2, d3, doodadset, instanceId;

			// global wmo instance data
			int gnWMO = (int)size / 64;
			for (int i=0; i<gnWMO; i++) {
				this->stream->Read(&id, 4);
				/*WMO *wmo = (WMO*)wmomanager.items[wmomanager.get(gwmos[id])];

				WMOInstance inst(wmo, f);
				gwmois.push_back(inst);*/

				this->stream->Read(&instanceId, 4);
				this->stream->Read(ff,12);
				pos = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				dir = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				pos2 = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(ff,12);
				pos3 = vector(ff[0],ff[1],ff[2]);
				this->stream->Read(&d2,4);
				this->stream->Read(&d3,4);

				doodadset = (d2 & 0xFFFF0000) >> 16;
				
				WMOServer::Instance()->CreateWMOInstance(wm->gwmos[id], pos, dir, doodadset);
			}
		}
		
		this->stream->Seek((int)nextpos, Stream::Begin);
	}
}

} // namespace Models
