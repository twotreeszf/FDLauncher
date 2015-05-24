/* -------------------------------------------------------------------------
//	FileName    : StringHelper.h
//	Author      : tanzhongbei
//	Create Time : 2012-3-20-18:01:23
//	Description : 	
//
// -----------------------------------------------------------------------*/

#ifndef __StringHelper_h__
#define __StringHelper_h__

#include "CrossPlatformConfig.h"
#include <string>
#include <vector>
#include <cstdlib>

namespace StringHelper
{
	void toLowerCase(std::string &str);
	void toUpperCase(std::string &str);

    std::string& trimLeft(std::string& str);
    std::string& trimRight(std::string& str);

	bool replace(std::string &str, const char* Oldchar, const char* NewChar);
    std::string format(const char *pszFormat, ...);

	std::string right(const std::string &src, Int32 nCount);
	std::string left(const std::string &src, Int32 nCount);

	typedef std::vector<std::string> SplitData;
	void split(const char* szData, char cSplit, SplitData &vectData);
	std::string merge(std::vector<std::string> vec, const char* szSplit = ",");

    bool isEmpty(const std::string &str);
    bool isEmpty(const char* str);

	std::string ByteBufferToHexString(Byte bytes[], Int32 nLen);
    void HexStringToByteBuffer(
        const std::string &hexString, UInt32 countOfBytes, Byte* pByteBuffer, bool reverseByte = false);

    bool isDigit(char ch);
    bool isLetter(char ch);
    bool isHexString(const std::string &hexString);
	
	int stricmp(const char* s1, const char* s2);
	int strnicmp(const char* s1, const char* s2, size_t n);

    Int64 stringToI64(const std::string& str);
    UInt64 stringToUI64(const std::string& str);

    std::string i64ToString(Int64 i64);
    std::string ui64ToString(UInt64 ui64);

    Int32 stringToInt(const std::string str);
    std::string intToString(Int32 i);

    std::string& removeBackslash(std::string& path);
    std::string& addBackslash(std::string& path);

	struct CompareResult
	{
		enum Type
		{
			Less = -1,
			Equal = 0,
			Greater = 1
		};
	};

	CompareResult::Type compareVersion(const std::string& L, const std::string& R);
}

#endif	/*__StringHelper_h__*/