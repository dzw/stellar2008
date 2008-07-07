#pragma once
#ifndef IO_MPQFILE_H
#define IO_MPQFILE_H
//------------------------------------------------------------------------------
/**
    @class IO::MPQFile
  
    A file entry in a zip archive.
    
    (C) 2006 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "io/stream.h"
#include "mpq/mpq.h"

//------------------------------------------------------------------------------
namespace IO
{
class MPQFile : public Core::RefCounted
{
    DeclareClass(MPQFile);
public:
    /// constructor
    MPQFile();
    /// destructor
    ~MPQFile();
	/// disable copying
	MPQFile(const MPQFile &rhs);
	/// disable copying
	void operator=(const MPQFile &rhs);
	/// end of file
	bool IsEof();
	/// read memory
	SizeT Read(void *dest, SizeT bytes);
	/// get file size
	SizeT GetSize();
	/// get current position
	SizeT GetPos();
	/// get buffer
	uchar* MapBuffer();
	/// get buffer of pointer
	uchar* MapPointer();
	/// unmap buffer,this function be called after MapBuffer and MapPointer
	void Unmap();
	/// seek position at begin
	void Seek(int offset);
	/// seek position at current
	void SeekRelative(int offset);
	/// close file
	void Close();

private:
    friend class MPQArchive;
	/// setup file data
	void Setup(mpq_archive &archive, const int &fileno);

	Ptr<IO::Stream> stream;
};

} // namespace IO
//------------------------------------------------------------------------------
#endif

