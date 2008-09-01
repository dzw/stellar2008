//------------------------------------------------------------------------------
//  zipfilesystem.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/lpqfs/lpqfilesystem.h"
#include "io/ioserver.h"
#include "LagerPacket/LagerPacket.h"

namespace IO
{
ImplementClass(IO::LPQFileSystem, 'LQFS', Core::RefCounted);
ImplementSingleton(IO::LPQFileSystem);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
LPQFileSystem::LPQFileSystem()
{
	if (!lpqSystem)
	{
		extern __declspec(dllimport) ILpqSystem* g_piLpqSystem;
		lpqSystem = g_piLpqSystem;
	}

    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
LPQFileSystem::~LPQFileSystem()
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
    Resolve a zip archive path into a LPQArchive pointer. Returns 0
    if no archive with that name exists. The filename will be resolved into
    an absolute path internally before the lookup happens.
*/
const Ptr<LPQArchive>&
LPQFileSystem::FindLPQArchive(const URI& uri)
{
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    IndexT index = this->archives.FindIndex(path);
    if (InvalidIndex != index)
    {
        return this->archives.ValueAtIndex(index);
    }
    else
    {
        static Ptr<LPQArchive> invalidPtr;
        return invalidPtr;
    }
}

//------------------------------------------------------------------------------
/**
	根据lPQ 里面的文件名找到所在的archive
*/
const Ptr<LPQArchive>&
LPQFileSystem::FindLPQArchiveByName(const String& name)
{
	IndexT i;
	for (i = 0; i < this->archives.Size(); i++)
	{
		const Ptr<LPQArchive>& arch = this->archives.ValueAtIndex(i);
		if (arch->IsFileExist(name))
			return arch;
	}

	static Ptr<LPQArchive> invalidPtr;
	return invalidPtr;
}
//------------------------------------------------------------------------------
/**
*/
bool
LPQFileSystem::IsMounted(const URI& uri) const
{
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    return this->archives.Contains(path);
}

//------------------------------------------------------------------------------
/**
    This "mounts" a zip file of the given filename by creating a LPQArchive
    object and reading the zip file structure. If opening the zip
    archive fails, 0 will be returned.
*/
Ptr<LPQArchive>
LPQFileSystem::Mount(const URI& uri)
{
    n_assert(!this->IsMounted(uri));
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    Ptr<LPQArchive> newLpqArchive = LPQArchive::Create();
    newLpqArchive->SetURI(uri);
    if (newLpqArchive->Open())
    {
        this->archives.Add(path, newLpqArchive);
    }
    else
    {
        newLpqArchive = 0;
    }
    return newLpqArchive;
}

//------------------------------------------------------------------------------
/**
*/
void
LPQFileSystem::Unmount(const Ptr<LPQArchive>& lpqArchive)
{
    n_assert(this->IsMounted(lpqArchive->GetURI()));
    lpqArchive->Close();
    String path = IoServer::Instance()->ResolveAssigns(lpqArchive->GetURI()).LocalPath();
    this->archives.Erase(path);
}

//------------------------------------------------------------------------------
/**
*/
void
LPQFileSystem::Unmount(const URI& uri)
{
    n_assert(this->IsMounted(uri));
    String path = IoServer::Instance()->ResolveAssigns(uri).LocalPath();
    Ptr<LPQArchive> lpqArchive = this->archives[path];
    lpqArchive->Close();
    this->archives.Erase(path);
}

//------------------------------------------------------------------------------
/**
*/
bool
LPQFileSystem::IsLPQFile(const URI& uri)
{
	String fileExt = uri.LocalPath().GetFileExtension();
	fileExt.ToLower();

	return (fileExt == "sce" || fileExt == "dds" || fileExt == "wav" ||
			fileExt == "nut" || /*fileExt == "xml" ||*/ fileExt == "txt" || 
			fileExt == "dat" || fileExt == "csv" || fileExt == "fnt" ||
			fileExt == "ann" || fileExt == "obj" || fileExt == "act" ||
			fileExt == "gif" || fileExt == "jpg" || fileExt == "htm" ||
			fileExt == "dfm" || fileExt == "lig" || fileExt == "lio" ||
			fileExt == "sto" || fileExt == "atf" || fileExt == "atm" ||
			fileExt == "stb" || fileExt == "xtb" || /*fileExt == "fx"  ||*/
			fileExt == "dat" || fileExt == "sm"  || fileExt == "sh"  ||
			fileExt == "spt" || fileExt == "tga");
}


//------------------------------------------------------------------------------
/**
*/
bool
LPQFileSystem::IsLPQFile(const String& uri)
{
	String fileExt = uri.GetFileExtension();
	fileExt.ToLower();

	return (fileExt == "sce" || fileExt == "dds" || fileExt == "wav" ||
			fileExt == "nut" || /*fileExt == "xml" ||*/ fileExt == "txt" || 
			fileExt == "dat" || fileExt == "csv" || fileExt == "fnt" ||
			fileExt == "ann" || fileExt == "obj" || fileExt == "act" ||
			fileExt == "gif" || fileExt == "jpg" || fileExt == "htm" ||
			fileExt == "dfm" || fileExt == "lig" || fileExt == "lio" ||
			fileExt == "sto" || fileExt == "atf" || fileExt == "atm" ||
			fileExt == "stb" || fileExt == "xtb" || /*fileExt == "fx"  ||*/
			fileExt == "dat" || fileExt == "sm"  || fileExt == "sh"  ||
			fileExt == "spt" || fileExt == "tga");
}

//------------------------------------------------------------------------------
/**
*/
URI
LPQFileSystem::ConvertFileToLPQURIIfExists(const URI& uri)
{
	if (this->IsLPQFile(uri))
	{
		this->uri = uri;
		this->uri.SetScheme("lpq");

		/*const String mshPath = IO::IoServer::Instance()->ResolveAssignsInString("msh:");
		String replace = const_cast<char*>(this->uri.LocalPath().AsCharPtr());
		replace.SubstituteString(mshPath, "");
		this->uri.SetLocalPath(replace);*/

		return this->uri;
	}
	// fallthrough: no match, return original uri
	return uri;
}

} // namespace IO