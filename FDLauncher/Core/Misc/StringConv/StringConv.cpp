/* -------------------------------------------------------------------------
//	File Name	:	StringConv.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-13 9:30:10
//	Description	:	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "StringConv.h"

#if defined (_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

// -------------------------------------------------------------------------

#if defined (_WIN32) || defined(_WIN64)

std::string UTF16ToUTF8(const wchar_t* src)
{
    int cch2 = ::WideCharToMultiByte(CP_UTF8, 0, src, ::wcslen(src), 0, 0, 0, 0);
	char* str2  = new char[cch2 + 1];
    ::WideCharToMultiByte(CP_UTF8, 0, src, ::wcslen(src), str2, cch2 + 1, 0, 0);
	str2[cch2] = '\0';
    std::string destStr = str2;
	delete []str2;
	return destStr;
}

std::wstring UTF8ToUTF16(const char*src)
{
    int cch2 = ::MultiByteToWideChar(CP_UTF8, 0, src, ::strlen(src), NULL, 0);
	wchar_t* str2 = new wchar_t[cch2 + 1];
    ::MultiByteToWideChar(CP_UTF8, 0, src, ::strlen(src), str2, cch2);
	str2[cch2] = L'\0';
    std::wstring destStr = str2;
	delete []str2;
	return destStr;
}

std::string UTF16ToGBK(const wchar_t* src)
{
    int cch2 = ::WideCharToMultiByte(CP_ACP, 0, src, ::wcslen(src), 0, 0, 0, 0);
	char* str2  = new char[cch2 + 1];
    ::WideCharToMultiByte(CP_ACP, 0, src, ::wcslen(src), str2, cch2 + 1, 0, 0);
	str2[cch2] = '\0';
    std::string destStr = str2;
	delete []str2;
	return destStr;
}

std::wstring GBKToUTF16(const char*src)
{
    int cch2 = ::MultiByteToWideChar(CP_ACP, 0, src, ::strlen(src), NULL, 0);
	wchar_t* str2 = new wchar_t[cch2 + 1];
    ::MultiByteToWideChar(CP_ACP, 0, src, ::strlen(src), str2, cch2);
	str2[cch2] = L'\0';
    std::wstring destStr = str2;
	delete []str2;
	return destStr;
}

std::string GBKToUTF8(const char*src)
{
	return ::UTF16ToUTF8(::GBKToUTF16(src).c_str());
}

std::string UTF8ToGBK(const char*src)
{
	return ::UTF16ToGBK(::UTF8ToUTF16(src).c_str());
}

#endif /*defined (_WIN32) || defined(_WIN64)*/

//--------------------------------------------------------------------------