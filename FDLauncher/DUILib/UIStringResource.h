/********************************************************************
	created:	2012/05/28	
	filename: 	UIStringResource.h	
	author:		tabu
	email:      mgl_jl@hotmail.com
	purpose:	字符串资源加载器,从xml文件加载
*********************************************************************/
#ifndef __UISTRINGRESOURCE_H__
#define __UISTRINGRESOURCE_H__

#pragma once
#include <string>
#include <hash_map>

namespace DuiLib{

//////////////////////////////////////////////////////////////////////////
#pragma warning( push )
#pragma warning(disable:4251)

class UILIB_API CStringResourceUI
{	
public:	
	~CStringResourceUI(void);

	static CStringResourceUI& Instance();

	bool Load(CResourceUI* pResouce, STRINGorID xml, LPCTSTR type);

	LPCTSTR	GetString( LPCTSTR strID );
private:
	CStringResourceUI(void);

	bool LoadFromXml(CMarkup& xml);
private:
	typedef std::basic_string<TCHAR> tstring;
	typedef stdext::hash_map<tstring, tstring> string_table;
	typedef stdext::hash_map<tstring, tstring>::iterator string_iterator;
	typedef stdext::hash_map<tstring, tstring>::const_iterator const_string_iterator;

	string_table m_stringTable;
};

UILIB_API LPCTSTR __stdcall LoadString(LPCTSTR strID);

#ifdef _UNICODE
  #define DUI_LOAD_STRING( resID )	\
  DuiLib::LoadString(L#resID)	
#else
  #define DUI_LOAD_STRING( resID )	\
  DuiLib::LoadString(#resID)	
#endif


#pragma warning( pop )

} //end namespace

#endif
