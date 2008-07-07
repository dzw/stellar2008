#pragma once
#ifndef CORE_TYPES_H
#define CORE_TYPES_H
//------------------------------------------------------------------------------
/**
    @class core/types.h

    Basic type definitions for Nebula3.
    
    (C) 2006 Radon Labs GmbH
*/
#include "memory/memory.h"

// fixing Windows defines...
#ifdef DeleteFile
#undef DeleteFile
#endif
#ifdef CopyFile
#undef CopyFile
#endif
#ifdef GetObject
#undef GetObject
#endif

typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef char int8;
typedef unsigned __int16 uint16;
typedef __int16 int16;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned int IndexT;    // the index type
typedef unsigned int SizeT;     // the size type
static const unsigned int InvalidIndex = 0xffffffff;

#define N_ARGB(a,r,g,b) ((uint)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define N_RGBA(r,g,b,a) N_ARGB(a,r,g,b)
#define N_XRGB(r,g,b)   N_ARGB(0xff,r,g,b)
#define N_COLORVALUE(r,g,b,a) N_RGBA((uint)((r)*255.f),(uint)((g)*255.f),(uint)((b)*255.f),(uint)((a)*255.f))

#if __WIN32__
#define n_stricmp stricmp
#define n_snprintf StringCchPrintf
#elif __XBOX360__
#define n_stricmp lstrcmpi
#define n_snprintf StringCchPrintf
#elif __WII__
#define n_stricmp stricmp
#define n_snprintf sprintf
#else
#error "Unsupported platform!"
#endif

#if __WIN32__
#define ThreadLocal __declspec(thread)
#elif __XBOX360__
#define ThreadLocal __declspec(thread)
#elif __WII__
#define ThreadLocal
#else
#error "Unsupported platform!"
#endif

// alignment macro
/*
#if __WIN32__
#define n_align(X) __declspec(align(X))
#elif __XBOX360__
#define n_align(X) __declspec(align(X))
#else
#error "Unsupported platform!"
#endif*/

//------------------------------------------------------------------------------
#endif