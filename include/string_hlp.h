/* -------------------------------------------------------------------------
// �ļ���		:	string/string_hlp.h
// ������		:	����
// ����ʱ��		:	2008-7-4 ������ 23:28:56
// ��������     :	�ַ���������������Ҫ�ǽ����׼����û�е��ַ�������
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __STRING_STRING_HLP_H__
#define __STRING_STRING_HLP_H__

namespace String_Hlp {
//////////////////////////////////////////////////////////////////////////
// ���Ȳ���
inline char * strstr(const char * str1, const char * str2, int len)
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	const char *pEnd = str1 + len;
	while (cp < pEnd)
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ��Сд�����еĲ���
inline char * strstr_nocase(const char * str1, const char * str2, int len)
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	const char *pEnd = str1 + len;
	while (cp < pEnd)
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !((*s1|0x20)-(*s2|0x20)) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return NULL;
}
inline char * strstr_nocase(const char * str1, const char * str2)
{
	return strstr_nocase(str1, str2, strlen(str1));
}

inline wchar_t * __cdecl wcsstr_nocase (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
	wchar_t *cp = (wchar_t *) wcs1;
	wchar_t *s1, *s2;

	if ( !*wcs2)
		return (wchar_t *)wcs1;

	while (*cp)
	{
			s1 = cp;
			s2 = (wchar_t *) wcs2;

			while ( *s1 && *s2 && !((*s1|0x0020)-(*s2|0x0020)) )
					s1++, s2++;

			if (!*s2)
					return(cp);

			cp++;
	}

	return(NULL);
}

//////////////////////////////////////////////////////////////////////////
// ���ݷָ��ַ����ַ����и������
typedef std::vector<CString> SplitData;
inline void Split(LPCWSTR szData, WCHAR cSplit, SplitData &vectData)
{
	// Normal
	LPCWSTR pos1, pos2;
	pos1 = szData;
	while (TRUE)
	{
		// =
		pos2 = wcschr(pos1, cSplit);
		if (!pos2)
			break;
		vectData.push_back(CString(pos1, (int)(pos2 - pos1)));

		pos1 = pos2 + 1;
	}

	// �������һ��
	if (wcslen(pos1) > 0)
		vectData.push_back(pos1);
}

// eg: 1|2||3| -> '1','2','','3',''
inline void SplitEx(LPCWSTR szData, WCHAR cSplit, SplitData &vectData)
{
	// Normal
	LPCWSTR pos1, pos2;
	pos1 = szData;
	while (TRUE)
	{
		pos2 = wcschr(pos1, cSplit);
		if (!pos2)
			break;
		vectData.push_back(CString(pos1, (int)(pos2 - pos1)));

		pos1 = pos2 + 1;
	}

	// �������һ��
	if (!((CString)szData).IsEmpty())
		vectData.push_back(pos1);
}

// ���ַ�������ϲ�Ϊ�ָ�ֵ�ַ���
template <typename T>
inline CString Merge(const T &t, LPCWSTR szSplit = L",")
{
	CString strArray;
	for (T::const_iterator it = t.begin(); it != t.end(); ++it)
	{
		if (it != t.begin())
			strArray += szSplit;
		strArray += *it;
	}
	return strArray;
}

} //String_Hlp

// -------------------------------------------------------------------------
// $Log: $

#endif /* __STRING_STRING_HLP_H__ */
