//------------------------------------------------------------------------------
//  we.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "app/kokapplication.h"

ImplementNebulaApplication()

using namespace KOK;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CmdLineArgs& args)
{
    KOKApplication app;
    app.SetCompanyName("cTuo");
    app.SetAppName("kok");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}