#pragma once
#ifndef IO_MPQARCHIVE_H
#define IO_MPQARCHIVE_H
//------------------------------------------------------------------------------
/**
    @class IO::MPQArchive
    
    Private helper class for ZipFileSystem to hold per-Zip-archive data.
    Uses the zlib and the minizip lib for zip file access.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "mpq/mpq.h"
#include "io/stream.h"
#include "io/mpqfilestream.h"

//------------------------------------------------------------------------------
namespace IO
{
class MPQArchive : public Core::RefCounted
{
    DeclareClass(MPQArchive);
public:
    /// constructor
    MPQArchive();
    /// destructor
    virtual ~MPQArchive();
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
	/// create mpq file
	//Ptr<MPQFileStream> CreateMPQFile(Util::String name);
	/// get archive
	mpq_archive GetArchive() const;
	/// get file no.
	const int GetFileNo()const;
	/// find mpq file
	bool FindMPQFile(const Util::String &name);

private:
    URI uri;
    Util::String rootPath;          // location of the zip archive file
	mpq_archive mpqFileHandle;		// mpq file handle
	int fileno;
	bool isOpen;
	//Util::Array<Ptr<MPQFileStream> > mpqFileStreams;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MPQArchive::IsOpen() const
{
    return (0 != this->isOpen);
}

//------------------------------------------------------------------------------
/**
*/
inline void
MPQArchive::SetURI(const URI& u)
{
    this->uri = u;
}

//------------------------------------------------------------------------------
/**
*/
inline const URI&
MPQArchive::GetURI() const
{
    return this->uri;
}

//------------------------------------------------------------------------------
/**
*/
inline const int 
MPQArchive::GetFileNo()const
{
	return this->fileno;
}

//------------------------------------------------------------------------------
/**
*/
inline mpq_archive 
MPQArchive::GetArchive() const
{
	return this->mpqFileHandle;
}

} // namespace IO
//------------------------------------------------------------------------------
#endif
    