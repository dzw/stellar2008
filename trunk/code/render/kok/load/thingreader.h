#pragma once
#ifndef KOK_OBJMODELREADER_H
#define KOK_OBJMODELREADER_H
//------------------------------------------------------------------------------
/**
    @class KOK::ThingReader
    
    (C) 2008 cTuo
*/
#include "models/load/modelreader.h"
#include "util/string.h"
#include "resources/resourceid.h "
#include "kokmodel/thing.h"

//------------------------------------------------------------------------------
namespace KOK
{

class ThingReader : public Models::ModelReader
{
    DeclareClass(ThingReader);
public:
    /// constructor
    ThingReader();
    /// destructor
    virtual ~ThingReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();
    /// parse resource
    virtual bool FillModel();
private:
	void ReadString(Util::String& str, int len);

	Ptr<Thing>	thing;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    