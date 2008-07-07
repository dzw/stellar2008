//------------------------------------------------------------------------------
//  zipfilestream.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/MPQfilestream.h"
#include "io/mpqfs/mpqfilesystem.h"

namespace IO
{
ImplementClass(IO::MPQFileStream, 'MFST', IO::Stream);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MPQFileStream::MPQFileStream() :
    size(0),
    position(0),
    buffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MPQFileStream::~MPQFileStream()
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
MPQFileStream::CanRead() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MPQFileStream::CanWrite() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
MPQFileStream::CanSeek() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MPQFileStream::CanBeMapped() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
Stream::Size
MPQFileStream::GetSize() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
Stream::Position
MPQFileStream::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
    Open the stream for reading. This will decompress the entire file
    from the zip archive into memory.
*/
bool
MPQFileStream::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->buffer);
    n_assert(ReadAccess == this->accessMode);

    if (Stream::Open())
    {
		String str = this->uri.LocalPath().AsCharPtr();
		str.SubstituteChar('/', '\\');

		Ptr<MPQArchive> arch = MPQFileSystem::Instance()->FindMPQArchiveByName(str);
		if (arch.isvalid())
		{
			this->size = libmpq_file_info(&arch->GetArchive(), LIBMPQ_FILE_UNCOMPRESSED_SIZE, arch->GetFileNo());
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
			int success = libmpq_file_getdata(&arch->GetArchive(), arch->GetFileNo(), (uchar*)this->buffer);
			if (success == LIBMPQ_TOOLS_SUCCESS)
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
MPQFileStream::Close()
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
MPQFileStream::Read(void* ptr, Size numBytes)
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
MPQFileStream::Seek(Offset offset, SeekOrigin origin)
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
MPQFileStream::Eof() const
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
MPQFileStream::Map()
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
MPQFileStream::Unmap()
{
    n_assert(this->IsOpen());
    Stream::Unmap();
}

//------------------------------------------------------------------------------
/**
*/
//void 
//MPQFileStream::Setup(mpq_archive &archive, const int &fileno)
//{
//	this->archive = archive;
//	this->fileno = fileno;
//}



} // namespace IO
