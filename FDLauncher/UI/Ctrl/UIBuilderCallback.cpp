#include "stdafx.h"
#include "UIBuilderCallback.h"
#include "TitleBar.h"

UIBuilderCallback::UIBuilderCallback()
{

}

CControlUI* UIBuilderCallback::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("TitleBar")) == 0 ) return new CTitleBarUI();

	return NULL;
}
