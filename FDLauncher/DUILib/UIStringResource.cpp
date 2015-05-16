#include "stdafx.h"
#include "UIStringResource.h"
#include "UIMarkup.h"

namespace DuiLib{

LPCTSTR __stdcall LoadString(LPCTSTR strID)
{
	return CStringResourceUI::Instance().GetString(strID);
}

CStringResourceUI::CStringResourceUI(void)	
{
	
}

CStringResourceUI::~CStringResourceUI(void)
{
}

CStringResourceUI& CStringResourceUI::Instance()
{
	static CStringResourceUI __obj;
	return __obj;
}

LPCTSTR CStringResourceUI::GetString( LPCTSTR strID )
{
	ASSERT(strID != NULL);
	string_iterator it = m_stringTable.find(tstring(strID));
	if(it == m_stringTable.end())
		return NULL;
	return it->second.c_str();
}

bool CStringResourceUI::Load(CResourceUI* pResouce, STRINGorID xml, LPCTSTR type )
{
	CMarkup xmlLoader;
	if( HIWORD(xml.m_lpstr) != NULL ) {
		if( *(xml.m_lpstr) == _T('<') ) {
			if( !xmlLoader.Load(xml.m_lpstr) ) return false;
		}
		else {
			if( !xmlLoader.LoadFromFile(pResouce, xml.m_lpstr) ) return false;
			pResouce->RemoveZipCache(xml.m_lpstr);
		}
	}
	else {
		HRSRC hResource = ::FindResource(pResouce->GetResourceDll(), xml.m_lpstr, type);
		if( hResource == NULL ) return false;
		HGLOBAL hGlobal = ::LoadResource(pResouce->GetResourceDll(), hResource);
		if( hGlobal == NULL ) {
			FreeResource(hResource);
			return false;
		}
		
		if( !xmlLoader.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(pResouce->GetResourceDll(), hResource) )) return false;
		::FreeResource(hResource);		
	}

	return LoadFromXml(xmlLoader);
}

bool CStringResourceUI::LoadFromXml( CMarkup& xml )
{
	CMarkupNode root = xml.GetRoot();
	if( !root.IsValid() ) return false;

	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() )
	{
		if(_tcsicmp(node.GetName(), _T("Group")) == 0)
		{	
			for( CMarkupNode strNode = node.GetChild();strNode.IsValid(); strNode = strNode.GetSibling() )
			{
				if(_tcsicmp(strNode.GetName(), _T("String")) == 0)
				{
					tstring key = strNode.GetAttributeValue(_T("name"));
					tstring val = strNode.GetAttributeValue(_T("value"));
					if(!key.empty())
					{
						m_stringTable.insert(std::make_pair(key, val));
					}					
				}				
			}
		}
		else if(_tcsicmp(node.GetName(), _T("String")) == 0)
		{
			tstring key = node.GetAttributeValue(_T("name"));
			tstring val = node.GetAttributeValue(_T("value"));
			if(!key.empty())
			{
				m_stringTable.insert(std::make_pair(key, val));
			}			
		}
	}
	return true;
}

}
