#pragma once
#ifndef WOW_WMOREADER_H
#define WOW_WMOREADER_H
//------------------------------------------------------------------------------
/**
    @class Models::WMOReader
    
    Legacy M2 binary reader, reads a subset of .n2 files into a Model.
    NOTE: This class is very inefficient and shouldn't be used
    for production code.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/load/modelreader.h"
#include "util/stack.h"
//#include "wow/wmo/modelheaders.h"
#include "wow/wmo/wmo.h"

//------------------------------------------------------------------------------
namespace WOW
{
class WMOReader : public Models::ModelReader
{
    DeclareClass(WMOReader);
public:
    /// constructor
    WMOReader();
    /// destructor
    virtual ~WMOReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();
	/// parse resource and build model hierarchy
	virtual bool FillModel();

    /// set the read cursor to the first Model in the stream
    virtual bool SetToFirstModel();
    /// set the read cursor to the next Model in the stream
    virtual bool SetToNextModel();

	/// set the read cursor to the first ModelNode in the current Model
	virtual bool SetToFirstModelNode();
	/// set the read cursor to the next ModelNode in the current Model
	virtual bool SetToNextModelNode();

	void SetWMO(const Ptr<WMO>& w);
	void ReadWMO();

private:
	
	void fixnamen(char *name, SizeT len);
	void fixname(Util::String &name);

	Ptr<WMO> wmo;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
WMOReader::SetWMO(const Ptr<WMO>& w)
{
	wmo = w;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
WMOReader::SetToFirstModelNode()
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
WMOReader::SetToNextModelNode()
{
	return true;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    