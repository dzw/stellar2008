#pragma once
#ifndef THREADING_SCOPELOCK_H
#define THREADING_SCOPELOCK_H
//------------------------------------------------------------------------------
/**
    ��Χ�����ں����ڲ�ʹ�ã��뿪����ʱ�Զ�������

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
    