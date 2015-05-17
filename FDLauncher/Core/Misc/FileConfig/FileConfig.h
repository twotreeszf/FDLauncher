/* -------------------------------------------------------------------------
//	File Name	:	FileConfig.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-13 9:50:07
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifndef __FileConfig_H__
#define __FileConfig_H__

// -------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __GNUC__
#include <unistd.h>
#endif /* __GNUC__ */

#include <stdio.h>
#include <string>

#ifdef _MSC_VER
#define stat64_   _stat64
#define fseek64_  _fseeki64
#endif /* _MSC_VER */

#ifdef __GNUC__
	#if defined (__APPLE__) && defined (__amd64__)
		#define  stat64_ stat
	#else
		#define stat64_   stat64
	#endif

	#define fseek64_  fseeko
#endif /* __GNUC__ */

FILE* fopen_x(const std::string& filename, const std::string& mode);

//--------------------------------------------------------------------------
#endif /* __FileConfig_H__ */