/* -------------------------------------------------------------------------
//  File Name   :   FDAppModule.h
//  Author      :   zhang fan
//  Create Time :   2015/05/16 0:47:29
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __FDAPPMODULE_H__
#define __FDAPPMODULE_H__
         
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

//-------------------------------------------------------------------------

class CFDAppModule
	: public CAppModule
	, public CMessageFilter
{
public:
	HRESULT Init(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
	int  Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT);
	void Exit();
	void Term();

	DuiLib::CResourceUI* GetResource();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	DuiLib::CResourceUI	m_uiResource;
	HRSRC				m_hSkinResource;
};

//--------------------------------------------------------------------------
#endif /* __FDAPPMODULE_H__ */
           