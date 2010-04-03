#ifndef PHYSX_PHYSXERRORSTREAM_H
#define PHYSX_PHYSXERRORSTREAM_H
//------------------------------------------------------------------------------
/**
    @class PhysX::ErrorStream
    
    (C) 2010 
*/

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "NxUserOutputStream.h"

//------------------------------------------------------------------------------
namespace PhysX
{

class ErrorStream : public NxUserOutputStream
	{
	public:
	void reportError(NxErrorCode e, const char* message, const char* file, int line)
		{
		n_printf("%s (%d) :", file, line);
		switch (e)
			{
			case NXE_INVALID_PARAMETER:
				n_printf( "invalid parameter");
				break;
			case NXE_INVALID_OPERATION:
				n_printf( "invalid operation");
				break;
			case NXE_OUT_OF_MEMORY:
				n_printf( "out of memory");
				break;
			case NXE_DB_INFO:
				n_printf( "info");
				break;
			case NXE_DB_WARNING:
				n_printf( "warning");
				break;
			default:
				n_printf("unknown error");
			}

		n_printf(" : %s\n", message);
		}

	NxAssertResponse reportAssertViolation(const char* message, const char* file, int line)
		{
		n_printf("access violation : %s (%s line %d)\n", message, file, line);
#ifdef WIN32
		switch (MessageBox(0, message, "AssertViolation, see console for details.", MB_ABORTRETRYIGNORE))
			{
			case IDRETRY:
				return NX_AR_CONTINUE;
			case IDIGNORE:
				return NX_AR_IGNORE;
			case IDABORT:
			default:
				return NX_AR_BREAKPOINT;
			}
#elif LINUX
		assert(0);
#elif _XBOX
		return NX_AR_BREAKPOINT;
#elif __CELLOS_LV2__
		return NX_AR_BREAKPOINT;
#endif
		}

	void print(const char* message)
		{
		n_printf(message);
		}
	};

};

#endif
