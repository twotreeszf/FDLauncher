#include "stdafx.h"
#include "AboutDlg.h"

CAboutDlg::CAboutDlg()
{
    m_bShadowEnabled = false;

}

CAboutDlg::~CAboutDlg()
{

}

void CAboutDlg::OnInit()
{

	DUI_BIND_CONTROL_VAR(L"version", m_textVersion, CTextUI);

	CString strVersion;
	strVersion.Format(DUI_LOAD_STRING(STRID_VERSION), L"0.1");
	m_textVersion->SetText(strVersion);

	CenterWindow();
}

void CAboutDlg::OnOKClick(TNotifyUI& msg, BOOL& bHandled)
{
	OnClose();
}

void CAboutDlg::OnDestroy()
{
	__super::OnDestroy();	
}

void CAboutDlg::OnClose()
{
	__super::OnClose();	

	_Module.Exit();
}

