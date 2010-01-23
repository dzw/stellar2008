//------------------------------------------------------------------------------
//  zipfilesystem.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/mpqfs/mpqfilesystem.h"
#include "io/ioserver.h"

namespace IO
{
ImplementClass(IO::MPQFileSystem, 'MQFS', Core::RefCounted);
ImplementSingleton(IO::MPQFileSystem);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MPQFileSystem::MPQFileSystem()
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MPQFileSystem::~MPQFileSystem()
{
    // unmount all mounted filesystems
    while (!this->archives.IsEmpty())
    {
        this->Unmount(this->archives.ValueAtIndex(0));
    }
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Resolve a zip archive path into a MPQArchive pointer. Returns 0
    if no archive with that name exists. The filename will be resolved into
    an absolute path internally before the lookup happens.
*/
const Ptr<MPQArchive>&
MPQFileSystem::FindMPQArchive(const URI& uri)
{
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    IndexT index = this->archives.FindIndex(path);
    if (InvalidIndex != index)
    {
        return this->archives.ValueAtIndex(index);
    }
    else
    {
        static Ptr<MPQArchive> invalidPtr;
        return invalidPtr;
    }
}

//------------------------------------------------------------------------------
/**
	根据MPQ 里面的文件名找到所在的archive
*/
const Ptr<MPQArchive>&
MPQFileSystem::FindMPQArchiveByName(const String& name)
{
	IndexT i;
	for (i = 0; i < this->archives.Size(); i++)
	{
		const Ptr<MPQArchive>& arch = this->archives.ValueAtIndex(i);
		if (arch->FindMPQFile(name))
			return arch;
	}

	static Ptr<MPQArchive> invalidPtr;
	return invalidPtr;
}
//------------------------------------------------------------------------------
/**
*/
bool
MPQFileSystem::IsMounted(const URI& uri) const
{
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    return this->archives.Contains(path);
}

//------------------------------------------------------------------------------
/**
    This "mounts" a zip file of the given filename by creating a MPQArchive
    object and reading the zip file structure. If opening the zip
    archive fails, 0 will be returned.
*/
Ptr<MPQArchive>
MPQFileSystem::Mount(const URI& uri)
{
    n_assert(!this->IsMounted(uri));
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    Ptr<MPQArchive> newMpqArchive = MPQArchive::Create();
    newMpqArchive->SetURI(uri);
    if (newMpqArchive->Open())
    {
        this->archives.Add(path, newMpqArchive);
    }
    else
    {
        newMpqArchive = 0;
    }
    return newMpqArchive;
}

//------------------------------------------------------------------------------
/**
    Unmount a zip archive, this will close the MPQArchive object and
    remove it from the internal registry.
*/
void
MPQFileSystem::Unmount(const Ptr<MPQArchive>& mpqArchive)
{
    n_assert(this->IsMounted(mpqArchive->GetURI()));
    mpqArchive->Close();
    String path = IoServer::Instance()->ResolveAssigns(mpqArchive->GetURI()).LocalPath();
    this->archives.Erase(path);
}

//------------------------------------------------------------------------------
/**
    Unmount a zip archive by zip filename. 
*/
void
MPQFileSystem::Unmount(const URI& uri)
{
    n_assert(this->IsMounted(uri));
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    Ptr<MPQArchive> zipArchive = this->archives[path];
    zipArchive->Close();
    this->archives.Erase(path);
}

//------------------------------------------------------------------------------
/**
*/
bool
MPQFileSystem::IsMPQFile(const URI& uri)
{
	String fileExt = uri.LocalPath().GetFileExtension();
	fileExt.ToLower();

	return (fileExt == "m2" || fileExt == "blp" || fileExt == "wmo" ||
			fileExt == "wdt"|| fileExt == "wdl" || fileExt == "adt" || 
			fileExt == "mdx" );
}


//------------------------------------------------------------------------------
/**
*/
bool
MPQFileSystem::IsMPQFile(const String& uri)
{
	String fileExt = uri.GetFileExtension();
	fileExt.ToLower();

	return (fileExt == "m2" || fileExt == "blp" || fileExt == "wmo" ||
			fileExt == "wdt"|| fileExt == "wdl" || fileExt == "adt" ||
			fileExt == "mdx");
}

//------------------------------------------------------------------------------
/**
*/
URI
MPQFileSystem::ConvertFileToMPQURIIfExists(const URI& uri)
{
	if (this->IsMPQFile(uri))
	{
		this->uri = uri;
		this->uri.SetScheme("mpqModel");

		/*const String mshPath = IO::IoServer::Instance()->ResolveAssignsInString("msh:");
		String replace = const_cast<char*>(this->uri.LocalPath().AsCharPtr());
		replace.SubstituteString(mshPath, "");
		this->uri.SetLocalPath(replace);*/

		return this->uri;
	}
	// fallthrough: no match, return original uri
	return uri;
}

SizeT 
MPQFileSystem::GetMPQFileSize(const Util::String& name)
{
	String str = name;
	str.SubstituteChar('/', '\\');
	const Ptr<MPQArchive> archive = FindMPQArchiveByName(str);
	if (!archive.isvalid())
		return 0;
	return archive->GetFileSize(str);
}

} // namespace IO