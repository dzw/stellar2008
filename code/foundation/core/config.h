#pragma once
#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H
//------------------------------------------------------------------------------
/**
    @file core/config.h

    Nebula3 compiler specific defines and configuration.
    
    (C) 2006 Radon Labs GmbH
*/

//------------------------------------------------------------------------------
/**
    Nebula3 configuration.
*/
#ifdef _DEBUG
#define NEBULA3_DEBUG (1)
#endif

// enable/disable Nebula3 memory stats
#if NEBULA3_DEBUG
#define NEBULA3_MEMORY_STATS (1)
#else
#define NEBULA3_MEMORY_STATS (0)
#endif

// enable/disable support for Nebula2 file formats
#define NEBULA3_LEGACY_SUPPORT (1)
// 在Drakensang中用到的优化格式(.n2)
#define NEBULA3_OPTIMIZEFILEFORMAT (1)

// enable/disable mini dumps
#define NEBULA3_ENABLE_MINIDUMPS (1)

// Nebula3's main window class
#define NEBULA3_WINDOW_CLASS "Nebula3::MainWindow"

// use heap or memory pool
#define NEBULA3_MEMORYPOOL (1)

//------------------------------------------------------------------------------
/**
    Xbox360 specifics.
*/
#ifdef __XBOX360__
#undef __XBOX360__
#endif
#ifdef _XBOX
#define __XBOX360__ (1)
#endif

//------------------------------------------------------------------------------
/**
    Win32 specifics.
*/
#ifdef __WIN32__
#undef __WIN32__
#endif
#ifdef WIN32
#define __WIN32__ (1)
#endif

//------------------------------------------------------------------------------
/**
    Default values and pathes.
*/
#if __WII__
#define DEFAULT_IO_SCHEME "dvd"
#else
#define DEFAULT_IO_SCHEME "file"
#endif

//------------------------------------------------------------------------------
/**
    Visual Studio specifics.
*/
#ifdef _MSC_VER
#define __VC__ (1)
#endif
#ifdef __VC__
#pragma warning( disable : 4251 )       // class XX needs DLL interface to be used...
#pragma warning( disable : 4355 )       // initialization list uses 'this' 
#pragma warning( disable : 4275 )       // base class has not dll interface...
#pragma warning( disable : 4786 )       // symbol truncated to 255 characters
#pragma warning( disable : 4530 )       // C++ exception handler used, but unwind semantics not enabled
#pragma warning( disable : 4995 )       // _OLD_IOSTREAMS_ARE_DEPRECATED
#pragma warning( disable : 4996 )       // _CRT_INSECURE_DEPRECATE, VS8: old string routines are deprecated
#pragma warning( disable : 4512 )       // 'class' : assignment operator could not be generated
#pragma warning( disable : 4610 )       // object 'class' can never be instantiated - user-defined constructor required
#pragma warning( disable : 4510 )       // 'class' : default constructor could not be generated
#endif

//------------------------------------------------------------------------------
/**
    Metrowerks Codewarrior specifics.
*/
#ifdef __MWERKS__
#pragma warning off (10216)      // illegal empty declaration used ( ; ; )
#pragma warning off (10317) 
#endif

//------------------------------------------------------------------------------
/**
    GCC specifics.
*/
#ifndef __GNUC__
#  define  __attribute__(x)  /**/
#endif

//------------------------------------------------------------------------------
#endif

