//------------------------------------------------------------------------------
//  zipfileentry.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/mpqfs/mqpfile.h"
#include "io/memorystream.h"

namespace IO
{
ImplementClass(IO::MPQFile, 'MQFE', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MPQFile::MPQFile() :
    stream(0)
{
}

//------------------------------------------------------------------------------
/**
*/
MPQFile::~MPQFile()
{
    this->stream = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
MPQFile::IsEof()
{
	return this->stream->Eof();
}

//------------------------------------------------------------------------------
/**
*/
void 
MPQFile::Setup(mpq_archive &archive, const int &fileno)
{
	SizeT size = libmpq_file_info(&archive, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);
	if (size <= 1)
	{
		this->stream = MemoryStream::Create();
		return;
	}

	this->stream = MemoryStream::Create();
	this->stream->SetAccessMode(Stream::ReadAccess);
	this->stream->SetSize(size);
	this->stream->Open();

	libmpq_file_getdata(&archive, fileno, (uchar*)this->stream->Map());

	this->stream->Unmap();
	this->stream->Close();
}

//------------------------------------------------------------------------------
/**
*/
SizeT 
MPQFile::Read(void *dest, SizeT bytes)
{
	if(this->stream->Eof()) return 0;

	return this->stream->Read(dest, bytes);
}

//------------------------------------------------------------------------------
/**
*/
SizeT 
MPQFile::GetSize()
{
	return this->stream->GetSize();
}

//------------------------------------------------------------------------------
/**
*/
SizeT 
MPQFile::GetPos()
{
	return this->stream->GetPosition();
}

//------------------------------------------------------------------------------
/**
*/
uchar* 
MPQFile::MapBuffer()
{
	if (!this->stream->IsOpen())
		this->stream->Open();
	return (uchar*)this->stream->Map();
}

//------------------------------------------------------------------------------
/**
*/
uchar* 
MPQFile::MapPointer()
{
	if (!this->stream->IsOpen())
		this->stream->Open();
	return (uchar*)((uchar*)this->stream->Map() + this->stream->GetPosition());
}

//------------------------------------------------------------------------------
/**
*/
void
MPQFile::Unmap()
{
	if (this->stream->IsOpen())
		this->stream->Unmap();
}

//------------------------------------------------------------------------------
/**
*/
void 
MPQFile::Seek(int offset)
{
	this->stream->Seek(offset, Stream::Begin);
}

//------------------------------------------------------------------------------
/**
*/
void 
MPQFile::SeekRelative(int offset)
{
	this->stream->Seek(offset, Stream::Current);
}

//------------------------------------------------------------------------------
/**
*/
void 
MPQFile::Close()
{
	this->stream->Close();
}

} // namespace MPQFile
