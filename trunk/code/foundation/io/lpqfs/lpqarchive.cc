//------------------------------------------------------------------------------
//  ziparchive.cc
//  (C) Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/lpqfs/lpqarchive.h"
#include "io/ioserver.h"
#include "io/lpqfs/lpqfilesystem.h"

namespace IO
{
ImplementClass(IO::LPQArchive, 'LQAR', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
LPQArchive::LPQArchive()
:isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LPQArchive::~LPQArchive()
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
LPQArchive::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->uri.IsValid());

    // extract the root location of the zip archive
    this->rootPath = this->uri.LocalPath().ExtractDirName();

    //URI absPath = IoServer::Instance()->ResolveAssigns(this->uri);
	this->lpqFileHandle = LPQFileSystem::Instance()->GetPackFileLibrary()->OpenLPQFile((char*)this->uri.LocalPath().AsCharPtr());
    if (!this->lpqFileHandle)
    {
		n_error("Error opening archive %s", this->uri.LocalPath().AsCharPtr());
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
LPQArchive::Close()
{
    n_assert(this->IsOpen());
    LPQFileSystem::Instance()->GetPackFileLibrary()->DestroyLPQ(this->lpqFileHandle);
	isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
LPQArchive::IsFileExist(const String &name)
{
	if (!name.IsValid())
		return false;
	if (!this->lpqFileHandle)
		return false;

	DWORD block_size = this->lpqFileHandle->GetBlockOrigSize((char*)name.AsCharPtr());
	if (block_size)
		return true;

	return false;
}

} // namespace IO
