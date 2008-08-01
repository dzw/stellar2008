#pragma once
#ifndef THREADING_SCOPELOCK_H
#define THREADING_SCOPELOCK_H
//------------------------------------------------------------------------------
/**
    范围锁，在函数内部使用，离开函数时自动解锁。

	cTuo
*/
#include "threading/criticalsection.h"

namespace Threading
{
class ScopeLock
{
	CriticalSection* synchObject;

	ScopeLock();
	ScopeLock(ScopeLock* inScopeLock);
	ScopeLock& operator=(ScopeLock& inScopeLock){return *this;}

public:
	ScopeLock(CriticalSection* inSynchObject):
	  synchObject(inSynchObject)
	  {
		  n_assert(synchObject);
		  synchObject->Enter();
	  }
	  
	  ~ScopeLock()
	  {
		  n_assert(synchObject);
		  synchObject->Leave();
	  }
};
}
//------------------------------------------------------------------------------
#endif
    