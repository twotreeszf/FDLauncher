/* -------------------------------------------------------------------------
//	File Name	:	FileConfig.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-13 9:50:24
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifdef __GNUC__
	#ifndef  _GNU_SOURCE   
		#define  _GNU_SOURCE
	#endif /* _GNU_SOURCE */
#endif /* __GNUC__ */

#include "stdafx.h"
#include "FileConfig.h"
#include <stdio.h>
#include "../StringConv/StringConv.h"

// -------------------------------------------------------------------------

// note: input string must encoding UTF8
FILE* fopen_x(const std::string& filename, const std::string& mode)
{
#if defined (_MSC_VER)

	std::wstring wfilename = ::UTF8ToUTF16(filename.c_str());
	std::wstring wfileMode = ::UTF8ToUTF16(mode.c_str());
	
    FILE* tempFile = _wfsopen(wfilename.c_str(), wfileMode.c_str(), _SH_DENYNO);
	return tempFile;

#else

	return fopen(((std::string)filename).c_str(), mode.c_str());

#endif /* _MSC_VER */
}

//--------------------------------------------------------------------------