#pragma once
#ifndef TIMING_TIME_H
#define TIMING_TIME_H
//------------------------------------------------------------------------------
/**
    @file timing/time.h
  
    Typedefs for the Timing subsystem
    
    (C) 2006 Radon Labs GmbH
*/    
#include "core/types.h"
#include <mmsystem.h>

//------------------------------------------------------------------------------
namespace Timing
{
/// the time datatype
typedef double Time;

//------------------------------------------------------------------------------
/**
    Put current thread to sleep for specified amount of seconds.
*/
inline void
Sleep(Time t)
{
    n_sleep(t);
}

//------------------------------------------------------------------------------
/**
    获得时间，以秒为单位，系统运行的时间
*/
inline Time
n_Seconds()
{
#if __WIN32__
    //DWORD  L,H;
    //__asm
    //{
    //    rdtsc           // RDTSC - Pentium+ time stamp register to EDX:EAX
    //    mov [L],eax     // Save low value.
    //    mov [H],edx     // Save high value.
    //}
    //return ((double)L + 4294967296.0 * (double)H) * GSecondsPerCycle + 16777216.0;

    static Time timeCounter = 0.0;
    static DWORD initialTime = timeGetTime();

    DWORD newTime = timeGetTime();
    timeCounter += (newTime - initialTime) * (1./1000.);
    initialTime = newTime;

    return timeCounter;
#endif
    return 0.0;
}

};
//------------------------------------------------------------------------------
#endif
