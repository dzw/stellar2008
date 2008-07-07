#pragma once
#ifndef IO_MPQFILESTREAM_H
#define IO_MPQFILESTREAM_H
//------------------------------------------------------------------------------
/**
    @class IO::MPQFileStream
    
    Wraps a file in a zip archive into a stream. Allows random access
    to the file by caching the entire file contents into RAM (the
    zip filesystem doesn't allow seeking in files). Also note that
    ZipFileStreams are read-only.
    
    The IO::Server allows transparent access to data in zip files through
    normal "file:" URIs by first checking whether the file is part of
    a mounted zip archive. Only if this is not the case, the file will
    be opened as normal.
    
    To force reading from a zip archive, use an URI of the following 
    format:
    
    zip://[samba server]/bla/blob/archive.zip?file=path/in/zipfile&pwd=password

    This assumes that the URI scheme "zip" has been associated with
    the MPQFileStream class using the IO::Server::RegisterUriScheme()
    method.
    
    The server and local path part of the URI contain the path to
    the zip archive file. The query part contains the path of
    the file in the zip archive and an optional password.
    
    (C) 2006 Radon Labs GmbH
*/
#include "io/stream.h"
#include "mpq/mpq.h"

//------------------------------------------------------------------------------
namespace IO
{
class MPQFileStream : public Stream
{
    DeclareClass(MPQFileStream);
public:
    /// constructor
    MPQFileStream();
    /// destructor
    virtual ~MPQFileStream();
    /// memory streams support reading
    virtual bool CanRead() const;
    /// memory streams support writing
    virtual bool CanWrite() const;
    /// memory streams support seeking
    virtual bool CanSeek() const;
    /// memory streams are mappable
    virtual bool CanBeMapped() const;
    /// get the size of the stream in bytes
    virtual Size GetSize() const;
    /// get the current position of the read/write cursor
    virtual Position GetPosition() const;
    /// open the stream
    virtual bool Open();
    /// close the stream
    virtual void Close();
    /// directly read from the stream
    virtual Size Read(void* ptr, Size numBytes);
    /// seek in stream
    virtual void Seek(Offset offset, SeekOrigin origin);
    /// return true if end-of-stream reached
    virtual bool Eof() const;
    /// map for direct memory-access
    virtual void* Map();
    /// unmap a mapped stream
    virtual void Unmap();

private:
	friend class MPQArchive;
	/// setup file data
	//void Setup(mpq_archive &archive, const int &fileno);
	
	//mpq_archive archive;
	//int fileno;

    Size size;
    Position position;
    unsigned char* buffer;
};

} // namespace IO
//------------------------------------------------------------------------------
#endif
