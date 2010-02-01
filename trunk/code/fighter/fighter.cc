//------------------------------------------------------------------------------
//  fighter.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/fmain.h"

ImplementNebulaApplication()

using namespace Fighter;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CmdLineArgs& args)
{
    FighterApplication app;
    app.SetCompanyName("cTuo");
    app.SetAppName("fighter");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}