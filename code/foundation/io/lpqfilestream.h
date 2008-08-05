#pragma once
#ifndef IO_LPQFILESTREAM_H
#define IO_LPQFILESTREAM_H
//------------------------------------------------------------------------------
/**
    @class IO::LPQFileStream
   
    (C) 2008 cTuo
*/
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace IO
{
class LPQFileStream : public Stream
{
    DeclareClass(LPQFileStream);
public:
    /// constructor
    LPQFileStream();
    /// destructor
    virtual ~LPQFileStream();
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
	time_t ConvertVersionToTime(DWORD ver);

	friend class LPQArchive;

    Size size;
    Position position;
    unsigned char* buffer;

	DWORD dataTimeVersion;
	long dataFileTime;
};

} // namespace IO
//------------------------------------------------------------------------------
#endif
