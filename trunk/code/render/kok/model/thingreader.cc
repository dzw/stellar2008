//------------------------------------------------------------------------------
//  thingreader.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/load/thinreader.h"
#include "util/string.h"
#include "kokmodel/kokshapenode.h"
#include "models/attributes.h "
#include "memory/memory.h"

namespace KOK
{
ImplementClass(KOK::ThingReader, 'OMRR', IO::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Memory;

//------------------------------------------------------------------------------
/**
*/
ThingReader::ThingReader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ThingReader::~ThingReader()
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
ThingReader::Open()
{
	n_assert(mapBuffer == NULL);
	n_assert(this->stream.isvalid());
	n_assert(this->model.isvalid());

	this->thing = this->model.downcast<Thing>();
	this->stream->SetAccessMode(Stream::ReadWriteAccess);
    if (this->stream->Open())
    {
		String thingCode;
		ReadString(thingCode, 5);

		if (thingCode != "THING")
			return false;	// Ê¶±ðÂë²»¶Ô

		return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ThingReader::Close()
{
    n_assert(this->IsOpen());

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ThingReader::FillModel()
{
    return true;
}





// ÁÙÊ±º¯Êý
void
ThingReader::LoadSkeletonHierarchy()
{
	int Offset;
	int iTemp;
	char *cTempChar  = NULL;
	String strTemp;
	float fTmp;
	bool nextFlag, childFlag;

	this->stream->Read(&iTemp, sizeof(int));

	if (iTemp > 0)
	{
		ReadString(&strTemp, iTemp);
	}

	// Åª¨úSkeleton®æ¦¡
	this->stream->Read(&iTemp, sizeof(int));
	// Åª¨ú¥­²¾¯x°}(TX)
	this->stream->Read(&fTmp, sizeof(float));

	// Åª¨ú¥­²¾¯x°}(TY)
	this->stream->Read(&fTmp, sizeof(float));

	// Åª¨ú¥­²¾¯x°}(TZ)
	this->stream->Read(&fTmp, sizeof(float));

	// Åª¨ú¤U¼h§PÂ_½X
	this->stream->Read(&nextFlag, sizeof(bool));

	this->stream->Read(&childFlag, sizeof(bool));

	if(nextFlag) //
	{
		LoadSkeletonHierarchy();
	}

	if(childFlag)
	{
		LoadSkeletonHierarchy();
	}
}

} // namespace Models
