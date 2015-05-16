/* -------------------------------------------------------------------------
//	文件名		：	base.h
//	创建者		：	冰峰
//	创建时间	：	2009-10-12 9:05:55
//	功能描述	：	最基本的东西，和具体工程相关的东西请不要随便放到这里
//
//	$Id: $
// -----------------------------------------------------------------------*/
#ifndef __BASE_H__
#define __BASE_H__

#include <string>

//////////////////////////////////////////////////////////////////////////
// CurlHelper返回码相关
#define S_TIMEOUT											2
#define S_CANCEL											3
#define S_STOPPED											4
#define S_COULDNT_CONNECT									5
#define S_SVR_RETRY											7
#define S_SVR_NO_PERMSN										8
#define E_WL_FATEL_ERROR									-6

//////////////////////////////////////////////////////////////////////////
// 编码转换相关,VC6和VC9的都有Bug，这里自己写

#undef USES_CONVERSION
#undef A2W
#undef W2A
#undef A2W_CP
#undef W2A_CP
#undef A2W_CP_EX
#undef W2A_CP_EX

/*
#define A2W_CP(lpa, cp) (\
	((_lpa = lpa) == NULL) ? NULL : (\
 	_convert = (::MultiByteToWideChar(cp, 0, _lpa, (int)(strlen(_lpa)), NULL, 0) + 1), \
	ATLA2WHELPER((LPWSTR) alloca(_convert*sizeof(WCHAR)), _lpa, _convert, cp)))

#define W2A_CP(lpw, cp) (\
	((_lpw = lpw) == NULL) ? NULL : (\
	(_convert = (WideCharToMultiByte(cp, 0, _lpw, (int)(wcslen(_lpw)), NULL, 0, NULL, NULL) + 1), \
	ATLW2AHELPER((LPSTR)(_lpa=(LPCSTR) alloca(_convert), memset((LPSTR)_lpa, 0, _convert), _lpa), _lpw, _convert, cp))))
*/

#define Z_A2W_CP_EX(lpa, nChars, cp) (\
	((_lpa_ex = lpa) == NULL) ? NULL : (\
	_convert_ex = (::MultiByteToWideChar(cp, 0, _lpa_ex, (int)(strlen(_lpa_ex)), NULL, 0) + 1), \
	FAILED(::ATL::AtlMultiply(&_convert_ex, _convert_ex, static_cast<int>(sizeof(WCHAR)))) ? NULL : \
	ATLA2WHELPER(	\
	(LPWSTR)_ATL_SAFE_ALLOCA(_convert_ex, _ATL_SAFE_ALLOCA_DEF_THRESHOLD), \
	_lpa_ex, \
	_convert_ex, \
	(cp))))

#define Z_W2A_CP_EX(lpw, nChars, cp) (\
	((_lpw_ex = lpw) == NULL) ? NULL : (\
	_convert_ex = (::WideCharToMultiByte(cp, 0, _lpw_ex, (int)(wcslen(_lpw_ex)), NULL, 0, NULL, NULL) + 1), \
	ATLW2AHELPER(	\
	(LPSTR)_ATL_SAFE_ALLOCA(_convert_ex, _ATL_SAFE_ALLOCA_DEF_THRESHOLD), \
	_lpw_ex, \
	_convert_ex, \
	(cp))))

// 强制使用安全版本
#define USES_CONVERSION		USES_CONVERSION_EX
#define A2W(lpa)			A2W_EX(lpa, 0)
#define W2A(lpw)			W2A_EX(lpw, 0)
#define A2W_CP_EX(lpa, cp)	Z_A2W_CP_EX(lpa, 0, cp)
#define W2A_CP_EX(lpw, cp)	Z_W2A_CP_EX(lpw, 0, cp)
#define A2W_CP(lpa, cp)		A2W_CP_EX(lpa, cp)
#define W2A_CP(lpw, cp)		W2A_CP_EX(lpw, cp)

#define A2W_UTF8(lpa)	A2W_CP_EX(lpa, CP_UTF8)
#define W2A_UTF8(lpw)	W2A_CP_EX(lpw, CP_UTF8)

inline BSTR A2BSTR_CP(LPCSTR lpa, UINT cp)
{
	USES_CONVERSION;
	return CComBSTR(A2W_CP_EX(lpa, cp)).Detach();
}

inline BSTR A2BSTR_UTF8(LPCSTR lpa)
{
	return A2BSTR_CP(lpa, CP_UTF8);
}

//////////////////////////////////////////////////////////////////////////
//  
inline BOOL IsStrEmpty(LPCSTR szStr)
{
	return !szStr || *szStr == 0;
}
inline BOOL IsStrEmpty(LPCWSTR szStr)
{
	return !szStr || *szStr == 0;
}
inline BOOL IsStrEmpty(BSTR szStr)
{
	return !szStr || *szStr == 0;
}

inline BOOL IsStrEmpty(const CString &strStr)
{
	return strStr.IsEmpty();
}
inline BOOL IsStrEmpty(const CComBSTR &bstrStr)
{
	return bstrStr.Length() == 0;
}
inline BOOL IsStrEmpty(const std::string& strStr)
{
	return strStr.empty();
}
inline BOOL IsStrEmpty(const std::wstring& strStr)
{
	return strStr.empty();
}

//////////////////////////////////////////////////////////////////////////
// delete
#define Z_DELETE(p)															\
{																			\
	if ((p))																\
	{																		\
		delete (p);															\
		(p) = NULL;															\
	}																		\
}

#define Z_ARRAY_DELETE(p)													\
{																			\
	if ((p))																\
	{																		\
		delete[] (p);														\
		(p) = NULL;															\
	}																		\
}

#define Z_RELEASE(p)														\
{																			\
	if ((p))																\
	{																		\
		p->Release();														\
		p = NULL;															\
	}																		\
}

#define Z_CLOSEHANDLE(h)													\
{																			\
	if ((h))																\
	{																		\
		::CloseHandle(h);													\
		h = NULL;															\
	}																		\
}

// -------------------------------------------------------------------------
//	$Log: $

#endif /* __BASE_H__ */
