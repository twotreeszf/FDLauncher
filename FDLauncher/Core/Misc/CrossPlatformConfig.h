/* -------------------------------------------------------------------------
//	File Name	:	CrossPlatformConfig.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-10 16:00:21
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifndef __CrossPlatformConfig_H__
#define __CrossPlatformConfig_H__

// -------------------------------------------------------------------------

// basic types
#if defined(_WIN32) || defined(_WIN64)
	#include <basetsd.h>
	typedef INT16  Int16;
	typedef UINT16 UInt16;
	typedef INT32  Int32;
	typedef UINT32 UInt32;
	typedef INT64  Int64;
	typedef UINT64 UInt64;
#else
	#include <stdint.h>
	typedef int16_t		Int16;
	typedef uint16_t	UInt16;
	typedef int32_t		Int32;
	typedef uint32_t	UInt32;
	typedef int64_t		Int64;
	typedef uint64_t	UInt64;
	#define UNREFERENCED_PARAMETER(p) {static_cast<void>(p)}
#endif // defined(_WIN32) || defined(_WIN64)

typedef unsigned char Byte;

//--------------------------------------------------------------------------
#endif /* __CrossPlatformConfig_H__ */