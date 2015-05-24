#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

#include "../FormUI.h"
#include "../Ctrl/TitleBar.h"
#include <future>

class FDLauncherDlg : public CFormUI
{
private:
	CTextUI*			m_textVersion;

	std::future<void>	m_asyncOpt;
	volatile bool		m_isCancel;

public:
	FDLauncherDlg();
	virtual ~FDLauncherDlg();

	//////////////////////////////////////////////////////////////////////////
	// Event Handler
	DUI_BEGIN_NOTIFY_MAP(FDLauncherDlg)
		DUI_NOTIFY_CONTROL_HANDLER(L"OK", L"click",	OnOKClick)
		DUI_CHAIN_NOTIFY_MAP(CFormUI)
	DUI_END_NOTIFY_MAP()

	void OnOKClick(TNotifyUI& msg, BOOL& bHandled);

protected:
	// override
	DUI_IMPLEMENT_FORM(_T("AboutDlg"), _T("about.xml"))
 	virtual void OnInit();
 	virtual void OnClose();

private:
	void _runLauncher();
	BOOL _checkUpdate(const std::string& currentVersion, BOOL& haveUpdate, std::string& version, std::string& url, std::string& md5);
	BOOL _downloadPackage(const std::string& url, const std::string& downloadPath);
	void _downloadProgress(double total, double now, double speed);
	BOOL _verifyPackage(const std::string& downloadPath, const std::string& packageMd5);
	BOOL _runInstall(const std::string& packagePath);
	BOOL _launchMianApp();
	BOOL _delayDeleteSelf();
	bool _isCancel();
};


#endif	/*__AboutDlg_h__*/