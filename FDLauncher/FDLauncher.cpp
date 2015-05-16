// FDLauncher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FDLauncher.h"
#include "FDAppModule.h"

CFDAppModule _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HRESULT ret = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(ret));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	ret = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(ret));

	int nRet = _Module.Run(lpCmdLine, nCmdShow);
	_Module.Term();
	::CoUninitialize();

	return nRet;
}