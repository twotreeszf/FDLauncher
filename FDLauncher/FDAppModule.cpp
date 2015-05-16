/* -------------------------------------------------------------------------
//  File Name   :   FDAppModule.cpp
//  Author      :   zhang fan
//  Create Time :   2015/05/16 0:47:33
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "FDAppModule.h"
#include "UI/FormUI.h"
#include "UI/Test/AboutDlg.h"

#define FD_SINGLE_MUTEX				L"{8ca5a5d7-ba47-4921-8c5f-cca8fbff2bc3}"

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

int CFDAppModule::Run(LPTSTR lpstrCmdLine /*= NULL*/, int nCmdShow /*= SW_SHOWDEFAULT*/)
{
	HANDLE mutex = 0;
	int nRet = 0;
	{
		mutex = ::CreateMutex(NULL, TRUE, FD_SINGLE_MUTEX);
		ASSERT(mutex);
		BOOL bHasRun = ERROR_ALREADY_EXISTS == ::GetLastError();
		if (bHasRun)
			goto Exit0;

		CMessageLoop theLoop;
		theLoop.AddMessageFilter(this);
		this->AddMessageLoop(&theLoop);

		ULONG_PTR m_gdiplusToken = NULL;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		CAboutDlg* about = new CAboutDlg();
		about->Create(NULL);
		about->ShowWindow(SW_SHOWNORMAL);

		nRet = theLoop.Run();
		theLoop.RemoveMessageFilter(this);
		_Module.RemoveMessageLoop();

		GdiplusShutdown(m_gdiplusToken);
	}

Exit0:
	::CloseHandle(mutex);
	return nRet;
}

void CFDAppModule::Exit()
{		
	CFormUIManager::Instance().CloseAll();
	::PostQuitMessage(0);
}

void CFDAppModule::Term()
{
	CPaintManagerUI::Term();
	CAppModule::Term();

	if (m_hSkinResource)
		::FreeResource(m_hSkinResource);
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