#pragma once
#ifndef IO_LPQFILESYSTEM_H
#define IO_LPQFILESYSTEM_H
//------------------------------------------------------------------------------
/**
    @class IO::LPQFileSystem
      
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/string.h"
#include "util/dictionary.h"
#include "io/uri.h"
#include "io/lpqfs/lpqarchive.h"

//------------------------------------------------------------------------------
namespace IO
{
class LPQArchive;

class LPQFileSystem : public Core::RefCounted
{
    DeclareClass(LPQFileSystem);
    DeclareSingleton(LPQFileSystem);
public:
    /// constructor
    LPQFileSystem();
    /// destructor
    virtual ~LPQFileSystem();
    /// "mount" a lpq archive
    Ptr<LPQArchive> Mount(const URI& uri);
    /// "unmount" a lqp archive by its file name
    void Unmount(const URI& uri);
    /// "unmount a lpq archive by pointer
    void Unmount(const Ptr<LPQArchive>& lpqArchive);
    /// return true if a lpq archive is already mounted
    bool IsMounted(const URI& uri) const;
    /// get all mounted lpq archives
    Util::Array<Ptr<LPQArchive> > GetMountedLPQArchives() const;
	/// find a lpq archive by name, returns 0 if not exists
	const Ptr<LPQArchive>& FindLPQArchive(const URI& uri);
	/// find a lpq archive by lpq file name, returns 0 if not exists
	const Ptr<LPQArchive>& FindLPQArchiveByName(const Util::String& uri);
	/// 
	URI ConvertFileToLPQURIIfExists(const URI& uri);
	///
	bool IsLPQFile(const URI& uri);
	/// 
	bool IsLPQFile(const Util::String& uri);
	///
	ILpqSystem* GetPackFileLibrary()const;
private:
    Util::Dictionary<Util::String,Ptr<LPQArchive> > archives;    // note: use absolute path for name!
	IO::URI uri;
	ILpqSystem* lpqSystem;
};

inline ILpqSystem* 
LPQFileSystem::GetPackFileLibrary()const
{
	return this->lpqSystem;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<LPQArchive> >
LPQFileSystem::GetMountedLPQArchives() const
{
    return this->archives.ValuesAsArray();
}

} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    
    