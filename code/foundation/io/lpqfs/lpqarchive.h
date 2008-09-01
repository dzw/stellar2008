#pragma once
#ifndef IO_LPQARCHIVE_H
#define IO_LPQARCHIVE_H
//------------------------------------------------------------------------------
/**
    @class IO::LPQArchive
    
    Private helper class for ZipFileSystem to hold per-Zip-archive data.
    Uses the zlib and the minizip lib for zip file access.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/stream.h"
#include "io/lpqfilestream.h"
#include "LagerPacket/LagerPacket.h"

//------------------------------------------------------------------------------
namespace IO
{
class LPQArchive : public Core::RefCounted
{
    DeclareClass(LPQArchive);
public:
    /// constructor
    LPQArchive();
    /// destructor
    virtual ~LPQArchive();
    /// set the uri of the zip archive
    void SetURI(const URI& n);
    /// get the uri of the zip archive
    const URI& GetURI() const;
    /// open the zip archive
    bool Open();
    /// close the zip archive
    void Close();
    /// return true if the zip archive is open
    bool IsOpen() const;
	/// get archive
	ILagerPacket* GetArchive() const;
	/// check lpq file
	bool IsFileExist(const Util::String &name);

private:
    URI uri;
    Util::String rootPath;          // location of the zip archive file
	ILagerPacket* lpqFileHandle;	// lpq file handle
	bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
LPQArchive::IsOpen() const
{
    return (0 != this->isOpen);
}

//------------------------------------------------------------------------------
/**
*/
inline void
LPQArchive::SetURI(const URI& u)
{
    this->uri = u;
}

//------------------------------------------------------------------------------
/**
*/
inline const URI&
LPQArchive::GetURI() const
{
    return this->uri;
}

//------------------------------------------------------------------------------
/**
*/
inline ILagerPacket* 
LPQArchive::GetArchive() const
{
	return this->lpqFileHandle;
}

} // namespace IO
//------------------------------------------------------------------------------
#endif
    