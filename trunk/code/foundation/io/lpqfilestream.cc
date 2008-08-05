//------------------------------------------------------------------------------
//  zipfilestream.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/lPQfilestream.h"
#include "io/lpqfs/lpqfilesystem.h"

namespace IO
{
ImplementClass(IO::LPQFileStream, 'LFST', IO::Stream);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
LPQFileStream::LPQFileStream() :
    size(0),
    position(0),
    buffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LPQFileStream::~LPQFileStream()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(0 == this->buffer);
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileStream::CanRead() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileStream::CanWrite() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileStream::CanSeek() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileStream::CanBeMapped() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
Stream::Size
LPQFileStream::GetSize() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
Stream::Position
LPQFileStream::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
    Open the stream for reading. This will decompress the entire file
    from the zip archive into memory.
*/
bool
LPQFileStream::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->buffer);
    n_assert(ReadAccess == this->accessMode);

    if (Stream::Open())
    {
		String str = this->uri.LocalPath().AsCharPtr();
		str.SubstituteChar('/', '\\');

		Ptr<LPQArchive> arch = LPQFileSystem::Instance()->FindLPQArchiveByName(str);
		if (arch.isvalid())
		{
			this->size = arch->GetArchive()->GetBlockOrigSize((char*)str.AsCharPtr());
			arch->GetArchive()->GetBlockVersion((char*)str.AsCharPtr(), NULL, &this->dataTimeVersion);
			this->dataFileTime = (long)ConvertVersionToTime(this->dataTimeVersion);

			if (this->size <= 1)
			{
				this->size = 0;
				this->position = 0;
				this->buffer = 0;
				return true;
			}

			this->position = 0;
			this->buffer = (uchar*) Memory::Alloc(this->size);
			n_assert(0 != this->buffer);
			DWORD success = arch->GetArchive()->ExtractToMemory((char*)str.AsCharPtr(), this->buffer);
			if (success == this->size)
				return true;
		}

        // fallthrough: failure
        this->Close();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
LPQFileStream::Close()
{
    n_assert(this->IsOpen());
    Stream::Close();
    if (0 != this->buffer)
    {
        Memory::Free(this->buffer);
        this->buffer = 0;
    }
    this->position = 0;
    this->size = 0;
}

//------------------------------------------------------------------------------
/**
*/
Stream::Size
LPQFileStream::Read(void* ptr, Size numBytes)
{
    n_assert(this->IsOpen());
    n_assert(!this->IsMapped()); 
    n_assert(ReadAccess == this->accessMode)
    n_assert((this->position >= 0) && (this->position <= this->size));

    // check if end-of-stream is near
    Size readBytes = n_min(numBytes, this->size - this->position);
    n_assert((this->position + readBytes) <= this->size);
    if (readBytes > 0)
    {
        Memory::Copy(this->buffer + this->position, ptr, readBytes);
        this->position += readBytes;
    }
    return readBytes;
}

//------------------------------------------------------------------------------
/**
*/
void
LPQFileStream::Seek(Offset offset, SeekOrigin origin)
{
    n_assert(this->IsOpen());
    n_assert(!this->IsMapped()); 
    n_assert((this->position >= 0) && (this->position <= this->size));
    switch (origin)
    {
        case Begin:
            this->position = offset;
            break;
        case Current:
            this->position += offset;
            break;
        case End:
            this->position = this->size + offset;
            break;
    }

    // make sure read/write position doesn't become invalid
    this->position = Math::n_iclamp(this->position, 0, this->size);
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileStream::Eof() const
{
    n_assert(this->IsOpen());
    n_assert(!this->IsMapped());
    n_assert((this->position >= 0) && (this->position <= this->size));
    return (this->position == this->size);
}

//------------------------------------------------------------------------------
/**
*/
void*
LPQFileStream::Map()
{
    n_assert(this->IsOpen());
    n_assert(ReadAccess == this->accessMode);
    Stream::Map();
    n_assert(this->GetSize() > 0);
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
*/
void
LPQFileStream::Unmap()
{
    n_assert(this->IsOpen());
    Stream::Unmap();
}

//------------------------------------------------------------------------------
/**
	Convert Version To Time
*/
time_t  
LPQFileStream::ConvertVersionToTime(DWORD ver)
{
	WORD y,m,d,h,s;
	DWORD v = ver & 0x00ffffff;
	y = (WORD)(v>>20);
	m = (WORD)((v-(y<<20))>>16);
	d = (WORD)((v-(y<<20)-(m<<16))>>11);
	h = (WORD)((v-(y<<20)-(m<<16)-(d<<11))>>5);
	s = (WORD)(v&0x1f);
	struct tm time_tm;
	time_tm.tm_sec=0;
	time_tm.tm_min=0;
	time_tm.tm_hour=h;
	time_tm.tm_mday=d;
	time_tm.tm_mon=m-1;  // bcz, January is 0
	time_tm.tm_year=(y+2003)-1900;

	time_t t=mktime(&time_tm);
	return t;
}

} // namespace IO
