/* -------------------------------------------------------------------------
//  File Name   :   FDAppModule.cpp
//  Author      :   zhang fan
//  Create Time :   2015/05/16 0:47:33
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "FDAppModule.h"

//-------------------------------------------------------------------------

HRESULT CFDAppModule::Init(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID /* = NULL */)
{
	CAppModule::Init(pObjMap, hInstance, pLibID);

	m_uiResource.SetInstance(hInstance);

#ifndef _DEBUG
	m_hSkinResource = ::FindResource(hInstance,  MAKEINTRESOURCE(IDR_SKIN), L"RC_SKIN");
	HGLOBAL hGlobal = ::LoadResource(hInstance, m_hSkinResource);
	CHECK_BOOLEX_ERR(hGlobal != NULL, hRet = E_FAIL, L"Load Skin Resource Error.");		
	m_uiResource.SetResourceZip((LPVOID)::LockResource(hGlobal), SizeofResource(hInstance, m_hSkinResource));
#else
	TCHAR fullPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, fullPath);
	_tcscat(fullPath, L"\\skin\\");
	m_uiResource.SetResourcePath(fullPath);
#endif

	bool ret = CStringResourceUI::Instance().Load(&m_uiResource, L"strings.xml", NULL);
	_ASSERT(ret);

	return S_OK;
}

DuiLib::CResourceUI* CFDAppModule::GetResource()
{
	return &m_uiResource;
}

BOOL CFDAppModule::PreTranslateMessage( MSG* pMsg )
{
	return CPaintManagerUI::TranslateMessage(pMsg) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------           