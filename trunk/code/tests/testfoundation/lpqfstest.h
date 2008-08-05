#ifndef TEST_LPQFSTEST_H
#define TEST_LPQFSTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::LpqFSTest
    
    Test lowlevel lpq filesystem functionality.
    
    (C) 2008 cTuo
*/
#include "testbase/testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class LpqFSTest : public TestCase
{
    DeclareClass(LpqFSTest);
public:
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
#endif

    