#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

#include "../FormUI.h"
#include "../Ctrl/TitleBar.h"

class CAboutDlg : public CFormUI
{
private:
	CTextUI*	m_textVersion;

public:
	CAboutDlg();
	virtual ~CAboutDlg();

	//////////////////////////////////////////////////////////////////////////
	// Event Handler
	DUI_BEGIN_NOTIFY_MAP(CAboutDlg)
		DUI_NOTIFY_CONTROL_HANDLER(L"OK", L"click",	OnOKClick)
		DUI_CHAIN_NOTIFY_MAP(CFormUI)
	DUI_END_NOTIFY_MAP()

	void OnOKClick(TNotifyUI& msg, BOOL& bHandled);

protected:
	// override
	DUI_IMPLEMENT_FORM(_T("AboutDlg"), _T("about.xml"))
 	virtual void OnInit();
 	virtual void OnDestroy();
 	virtual void OnClose();
};


#endif	/*__AboutDlg_h__*/