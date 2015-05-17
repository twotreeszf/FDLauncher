/* -------------------------------------------------------------------------
//	File Name	:	StringConv.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-13 9:29:50
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifndef __StringConv_H__
#define __StringConv_H__

#include "../../Misc/CrossPlatformConfig.h"
#include <string>

// -------------------------------------------------------------------------

#if defined (_WIN32) || defined(_WIN64)

std::string	    UTF16ToUTF8(const wchar_t* src);
std::wstring    UTF8ToUTF16(const char*src);

std::string 	UTF16ToGBK(const wchar_t* src);
std::wstring	GBKToUTF16(const char*src);

std::string 	GBKToUTF8(const char*src);
std::string 	UTF8ToGBK(const char*src);

    #define UTF8CSZ(str) ::UTF16ToUTF8(L##str).c_str()
    #define UTF8STR(str) ::UTF16ToUTF8(str.c_str())

#else

    #define UTF8CSZ(str) str
    #define UTF8STR(str) str

#endif /*defined (_WIN32) || defined(_WIN64)*/



//--------------------------------------------------------------------------
#endif /* __StringConv_H__ */