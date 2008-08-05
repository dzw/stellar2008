//------------------------------------------------------------------------------
//  lpqfstest.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lpqfstest.h"
#include "io/ioserver.h"
#include "io/filestream.h"
#include "io/lpqfs/lpqfilesystem.h"
#include "io/lpqfs/lpqarchive.h"

namespace Test
{
ImplementClass(Test::LpqFSTest, 'LPTS', Test::TestCase);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
LpqFSTest::Run()
{
    Ptr<IoServer> ioServer = IoServer::Create();
	ioServer->RegisterUriScheme("file", FileStream::RTTI);
    ioServer->RegisterUriScheme("lpq", LPQFileStream::RTTI);

    // mount a zip file
	Ptr<LPQArchive> lpqArchive = LPQFileSystem::Instance()->Mount("home:config.lpq");
    this->Verify(lpqArchive.isvalid());
    if (lpqArchive.isvalid())
    {
        this->Verify(lpqArchive->IsOpen());

        LPQArchive* archive = LPQFileSystem::Instance()->FindLPQArchive("home:config.lpq");
        this->Verify(archive == lpqArchive);
        this->Verify(LPQFileSystem::Instance()->IsMounted("home:config.lpq"));

        // lpq中文件安显示的文件名（带目录）访问
        this->Verify(archive->IsFileExist("config\\skilldef.txt") != 0);
    }
}

}