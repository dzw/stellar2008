#pragma once
#ifndef IO_MPQFILESYSTEM_H
#define IO_MPQFILESYSTEM_H
//------------------------------------------------------------------------------
/**
    @class IO::MPQFileSystem
    
	MPQFileSystem管理mqp文件，每个mqp文件相当于一个Archive，里面包含
	很多的MPQFile。
      
    (C) 2006 by ctuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/string.h"
#include "util/dictionary.h"
#include "io/uri.h"
#include "io/mpqfs/mpqarchive.h"

//------------------------------------------------------------------------------
namespace IO
{
class MPQArchive;

class MPQFileSystem : public Core::RefCounted
{
    DeclareClass(MPQFileSystem);
    DeclareSingleton(MPQFileSystem);
public:
    /// constructor
    MPQFileSystem();
    /// destructor
    virtual ~MPQFileSystem();
    /// "mount" a mpq archive
    Ptr<MPQArchive> Mount(const URI& uri);
    /// "unmount" a mqp archive by its file name
    void Unmount(const URI& uri);
    /// "unmount a mpq archive by pointer
    void Unmount(const Ptr<MPQArchive>& mpqArchive);
    /// return true if a mpq archive is already mounted
    bool IsMounted(const URI& uri) const;
    /// get all mounted mpq archives
    Util::Array<Ptr<MPQArchive> > GetMountedMPQArchives() const;
	/// find a mpq archive by name, returns 0 if not exists
	const Ptr<MPQArchive>& FindMPQArchive(const URI& uri);
	/// find a mpq archive by mpq file name, returns 0 if not exists
	const Ptr<MPQArchive>& FindMPQArchiveByName(const Util::String& uri);
	/// 
	URI ConvertFileToMPQURIIfExists(const URI& uri);
	///
	bool IsMPQFile(const URI& uri);
	/// 
	bool IsMPQFile(const Util::String& uri);
private:
    Util::Dictionary<Util::String,Ptr<MPQArchive> > archives;    // note: use absolute path for name!
	IO::URI uri;
};

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<MPQArchive> >
MPQFileSystem::GetMountedMPQArchives() const
{
    return this->archives.ValuesAsArray();
}

} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    
    