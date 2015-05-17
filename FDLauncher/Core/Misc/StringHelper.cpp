/* -------------------------------------------------------------------------
//  File Name   :   StringHelper.cpp
//  Author      :   Zhang Fan
//  Create Time :   2012-3-29 20:53:56
//  Description :   
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "StringHelper.h"

#include <cstring>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//-------------------------------------------------------------------------

namespace StringHelper
{
    void toLowerCase(std::string &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

    void toUpperCase(std::string &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    std::string& trimLeft(std::string& str)
    {
        int index = -1;
        for (std::string::iterator it = str.begin(); it != str.end(); NULL)
        {
            if (' ' == *it) 
            {
                ++index;
                ++it;
            }
            else
                break;
        }

        if (-1 != index)
            str = str.substr(index + 1, str.length() - index - 1);
        return str;
    }

    std::string& trimRight(std::string& str)
    {
        int index = str.length();
        for (std::string::reverse_iterator it = str.rbegin(); it != str.rend(); NULL)
        {
            if (' ' == *it)
            {
                --index;
                ++it;
            }
            else
                break;
        }

        if (str.length() != index)
            str = str.substr(0, index);

        return str;
    }

    bool replace(std::string &str, const char* Oldchar, const char* NewChar)
    {
        Int32 nStart = 0;
        nStart = str.find(Oldchar, 0);
        if( nStart < 0 )  
            return false;  

        while(nStart >= 0)  
        {     
            str.replace(nStart, strlen(Oldchar), NewChar);

            nStart += strlen(NewChar);  
            nStart = str.find(Oldchar, nStart);  
        }     

        return true;
    }

    std::string format(const char *pszFormat, ...)
    {
        std::string retStr;

#ifdef _MSC_VER
		va_list argptr;

		va_start(argptr, pszFormat);
		size_t len = _vscprintf(pszFormat, argptr);

		char* buffer = new char[len + 1];
		int nWritten = vsprintf_s(buffer, len + 1, pszFormat, argptr);
		retStr = buffer;        
		va_end(argptr);

		delete buffer;
#else
		va_list argptr;
		va_start(argptr, pszFormat);
		char* buffer;
		vasprintf(&buffer, pszFormat, argptr);
		retStr = buffer;
		va_end(argptr);
		delete buffer;
#endif
        return retStr;
    }

    std::string right(const std::string &src, Int32 nCount)
    {
        if(nCount > src.length())
            return src;	// as cstring
        return src.substr(src.length() - nCount, nCount);
    }

    std::string left(const std::string &src, Int32 nCount)
    {
        return src.substr(0, nCount);
    }

    typedef std::vector<std::string> SplitData;
    void split(const char* szData, char cSplit, SplitData &vectData)
    {
        // Normal
        const char* pos1;
        const char* pos2;
        pos1 = szData;
        while (true)
        {
            pos2 = strchr(pos1, cSplit);
            if (!pos2)
                break;
            vectData.push_back(std::string(pos1, (Int32)(pos2 - pos1)));

            pos1 = pos2 + 1;
        }

        // deal last
        if (strlen(pos1) > 0)
            vectData.push_back(pos1);
    }

    std::string merge(std::vector<std::string> vec, const char* szSplit /*= ","*/)
    {
        std::string strArray;
        for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
        {
            if (it != vec.begin())
                strArray += szSplit;
            strArray += *it;
        }
        return strArray;
    }

    bool isEmpty(const std::string &str)
    {
        return str.empty();
    }

    bool isEmpty(const char* str)
    {
        return !str || !*str;
    }

    std::string ByteBufferToHexString(Byte bytes[], Int32 nLen)
    {
        X_ASSERT(bytes);
        X_ASSERT(nLen > 0);

        std::string strResult;
        for (int i = 0; i < nLen; ++i)
        {
            strResult.append(format("%02x", bytes[i]));
        }

        return strResult;
    }

    bool isEmpty(const char* str);
    bool isHexString(const std::string &hexString);
    void HexStringToByteBuffer(
        const std::string &hexString, UInt32 countOfBytes, Byte* pByteBuffer, bool reverseByte /*= false*/)
    {
        X_ASSERT(!isEmpty(hexString));
 //       X_ASSERT(isHexString(hexString));
        X_ASSERT(hexString.size() == countOfBytes * 2);

        const char* pHexString = hexString.c_str();
        for (UInt32 i = 0; i < countOfBytes; ++i)
        {
            char szOneByte[3] = {0};
            if (reverseByte)
            {
                szOneByte[0] = pHexString[i * 2 + 1];		// 服务器那边要求反转一个BYTE的前四位和后四位
                szOneByte[1] = pHexString[i * 2];
            }
            else
            {
                szOneByte[0] = pHexString[i * 2]; 
                szOneByte[1] = pHexString[i * 2 + 1];
            }

            UInt32 oneByte = 0;
            sscanf(szOneByte, "%02x", &oneByte);
            pByteBuffer[i] = oneByte;
        }
    }

    bool isDigit(char ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    bool isLetter(char ch)
    {
        return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
    }

    bool isHexString(const std::string &hexString)
    {
        if(hexString.empty())
        {
            return false;
        }

        for(int i = hexString.size() - 1; i >= 0; --i)
        {
            if(!isDigit(hexString[i]) && !isLetter(hexString[i]))
            {
                return false;
            }
        }

        return true;
    }
	
	int stricmp(const char* s1, const char* s2)
	{
		#ifdef __APPLE__
		return ::strcasecmp(s1, s2);
		#else
		return ::_stricmp(s1, s2);
		#endif
	}
	
	int strnicmp(const char* s1, const char* s2, size_t n)
	{
		#ifdef __APPLE__
		return ::strncasecmp(s1, s2, n);
		#else
		return ::_strnicmp(s1, s2, n);
		#endif
	}

    Int64 stringToI64(const std::string& str)
    {
#ifdef _MSC_VER
        return ::_strtoi64(str.c_str(), NULL, 10);
#else
        return ::strtoll(str.c_str(), NULL, 10);
#endif
    }

    UInt64 stringToUI64(const std::string& str)
    {
#ifdef _MSC_VER
        return ::_strtoui64(str.c_str(), NULL, 10);
#else
        return ::strtoull(str.c_str(), NULL, 10);
#endif
    }

    std::string i64ToString(Int64 i64)
    {
#ifdef _MSC_VER
        return format("%I64d", i64);
#else
        return format("%lld", i64);
#endif
    }

    std::string ui64ToString(UInt64 ui64)
    {
#ifdef _MSC_VER
        return format("%I64u", ui64);
#else
        return format("%llu", ui64);
#endif
    }

    Int32 stringToInt(const std::string str)
    {
        return atoi(str.c_str());
    }

    std::string intToString(Int32 i)
    {
        return format("%d", i);
    }

    std::string& removeBackslash(std::string& path)
    {
        if (path.length() && path.at(path.length() - 1) == '/')
            path.erase(path.length() - 1);

        return path;
    }

    std::string& addBackslash(std::string& path)
    {
        if (!path.length() || path.at(path.length() - 1) != '/')
            path.append("/");

        return path;
    }
}

//--------------------------------------------------------------------------