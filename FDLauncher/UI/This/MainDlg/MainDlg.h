/* -------------------------------------------------------------------------
//  File Name   :   MainDlg.h
//  Author      :   zhang fan
//  Create Time :   2015/05/27 15:46:20
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __MAINDLG_H__
#define __MAINDLG_H__

#include "../../Dlg/IrregularForm.h"
#include <future>

//-------------------------------------------------------------------------

class CMainDlg : public CIrregularForm
{
private:
	struct EnumRunType
	{
		enum type
		{
			None = 0,
			Install,
			Update,
		};
	};

	struct EnumState
	{
		enum type
		{
			None = 0,
			CheckUpdate,
			Downloading,
			Verfiying,
			Installing,
			Finish
		};
	};

	CTextUI*			m_message;
	CProgressUI*		m_progressNormal;
	CProgressUI*		m_progressError;

	EnumRunType::type	m_runType;
	EnumState::type		m_state;
	std::future<void>	m_asyncOpt;
	volatile bool		m_isCancel;


public:
	CMainDlg();
	~CMainDlg();

public:
	DUI_BEGIN_NOTIFY_MAP(CAboutDlg)
		DUI_NOTIFY_CONTROL_HANDLER(L"close", L"click", onClickClose)
		DUI_CHAIN_NOTIFY_MAP(CFormUI)
	DUI_END_NOTIFY_MAP()

	void onClickClose(TNotifyUI& msg, BOOL& bHandled);

protected:
	DUI_IMPLEMENT_FORM(_T("MainDlg"), _T("main.xml"))

	void OnInit();
	void OnClose();

private:
	void _runLauncher();
	BOOL _checkUpdate(const std::string& currentVersion, BOOL& haveUpdate, std::string& version, std::string& url, std::string& md5);
	BOOL _downloadPackage(const std::string& url, const std::string& downloadPath);
	void _downloadProgress(double total, double now, double speed);
	BOOL _verifyPackage(const std::string& downloadPath, const std::string& packageMd5);
	BOOL _runInstall(const std::string& packagePath);
	BOOL _launchMianApp();
	BOOL _delayDeleteSelf();
	BOOL _deleteSelfByHelperProcess();
	bool _isCancel();
};

//--------------------------------------------------------------------------
#endif /* __MAINDLG_H__ */
