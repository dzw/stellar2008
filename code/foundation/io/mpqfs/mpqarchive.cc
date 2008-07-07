//------------------------------------------------------------------------------
//  ziparchive.cc
//  (C) Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/mpqfs/mpqarchive.h"
#include "io/ioserver.h"

namespace IO
{
ImplementClass(IO::MPQArchive, 'MQAR', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MPQArchive::MPQArchive()
:isOpen(false)
,fileno(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MPQArchive::~MPQArchive()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    This opens the zip archive and reads the table of content as a 
    tree of ZipDirEntry and ZipFileEntry objects.
*/
bool
MPQArchive::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->uri.IsValid());

    // extract the root location of the zip archive
    this->rootPath = this->uri.LocalPath().ExtractDirName();

    // open the zip file
    // TODO: provide a set of file functions using Nebula2's stream API
    URI absPath = IoServer::Instance()->ResolveAssigns(this->uri);
    int result = libmpq_archive_open(&this->mpqFileHandle, (unsigned char*)absPath.LocalPath().AsCharPtr());
    if (result)
    {
		n_error("Error opening archive %s", absPath.AsString().AsCharPtr());
        return false;
    }

	this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    This closes the zip archive, releasing the table of contents and
    closing the zip file.
*/
void
MPQArchive::Close()
{
    n_assert(this->IsOpen());
    libmpq_archive_close(&this->mpqFileHandle);
	isOpen = false;
}

//------------------------------------------------------------------------------
/**
	创建一个MPQFile
*/
//Ptr<MPQFileStream>
//MPQArchive::CreateMPQFile(Util::String name)
//{
//	int fileno = libmpq_file_number(&this->mpqFileHandle, name.AsCharPtr());
//	if (fileno == LIBMPQ_EFILE_NOT_FOUND)
//		return NULL;
//
//	Ptr<MPQFileStream> mpqFileStream = MPQFileStream::Create();
//	this->mpqFileStreams.Append(mpqFileStream);
//	mpqFileStream->Setup(this->mpqFileHandle, fileno);
//	mpqFileStream->Open();
//
//	return mpqFileStream;	
//}


//------------------------------------------------------------------------------
/**
*/
bool 
MPQArchive::FindMPQFile(const String &name)
{
	n_assert(name.IsValid());

	fileno = libmpq_file_number(&this->mpqFileHandle, name.AsCharPtr());
	if (fileno == LIBMPQ_EFILE_NOT_FOUND)
		return false;

	return true;
}

} // namespace IO
