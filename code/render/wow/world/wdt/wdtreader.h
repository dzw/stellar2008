#pragma once
#ifndef WOW_WDTREADER_H
#define WOW_WDTREADER_H
//------------------------------------------------------------------------------
/**
    @class Models::WDTReader
    
    Legacy M2 binary reader, reads a subset of .n2 files into a Model.
    NOTE: This class is very inefficient and shouldn't be used
    for production code.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/load/modelreader.h"
#include "util/stack.h"
//#include "wow/wmo/modelheaders.h"
//#include "wow/world/wdt/wdt.h"

//------------------------------------------------------------------------------
namespace WOW
{
class WDTReader : public Models::ModelReader
{
    DeclareClass(WDTReader);
public:
    /// constructor
    WDTReader();
    /// destructor
    virtual ~WDTReader();

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

	//void SetWDT(const Ptr<WDT>& w);
	void ReadWDT();

	
private:

	//Ptr<WDT> wdt;
};

//------------------------------------------------------------------------------
/**
*/
//inline void 
//WDTReader::SetWDT(const Ptr<WDT>& w)
//{
//	wdt = w;
//}

//------------------------------------------------------------------------------
/**
*/
inline bool 
WDTReader::SetToFirstModelNode()
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
WDTReader::SetToNextModelNode()
{
	return true;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    