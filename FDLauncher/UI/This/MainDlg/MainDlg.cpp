/* -------------------------------------------------------------------------
//  File Name   :   MainDlg.cpp
//  Author      :   zhang fan
//  Create Time :   2015/05/27 15:46:15
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "MainDlg.h"

#include "../../../Core/Misc/SystemHelper.h"
#include "../../../Core/Misc/StringConv/StringConv.h"
#include <time.h>
#include "../../../Core/LibcRULHttp/LibCurlHttpGetRequest.h"
#include "../../../Core/LibcRULHttp/LibCurlHttpGetDownload.h"
#include "../../../Core/JsonCpp/json.h"
#include "../../../Core/Misc/FastDelegate/FastDelegate.h"
#include "../../../Core/Misc/FileHash.h"
#include "../../../Core/Misc/MD5/MD5.h"
#include "../../../Core/Misc/StringHelper.h"
#include "../../../Core/NotificationCenter/NotificationCenter.h"
#include "../../../Core/This/CommandLineDef.h"


//-------------------------------------------------------------------------

CMainDlg::CMainDlg()
	: CFormUI(false)
	, m_isCancel(false)
	, m_message(NULL)
	, m_progress(NULL)
	, m_runType(EnumRunType::None)
	, m_state(EnumState::None)
{
	m_bShadowEnabled = false;
}

CMainDlg::~CMainDlg()
{

}

void CMainDlg::onClickClose(TNotifyUI& msg, BOOL& bHandled)
{
	TRACE_STACK;

	Close();
}

void CMainDlg::OnInit()
{
	TRACE_STACK;

	__super::OnInit();

	DUI_BIND_CONTROL_VAR(L"message", m_message, CTextUI);
	DUI_BIND_CONTROL_VAR(L"progress", m_progress, CProgressBarUI);
	DUI_BIND_CONTROL_VAR(L"progress_text", m_progressText, CTextUI);

	CenterWindow();
	SetWindowText(DUI_LOAD_STRING(STRID_APPNAME));

	m_asyncOpt = std::async([=]
	{
	TRACE_STACK;
	this->_runLauncher();
	});
}

void CMainDlg::OnClose()
{
	TRACE_STACK;

	__super::OnClose();

	m_isCancel = true;
	if (m_asyncOpt.valid())
		m_asyncOpt.wait();
	m_isCancel = false;

	_Module.Exit();
}

void CMainDlg::_runLauncher()
{
	TRACE_STACK;

	BOOL ret = TRUE;
	{
		// check need install
		CPath mainExePath = SystemHelper::getMainAppPath();
		BOOL exist = ::PathFileExists(mainExePath);

		// install
		if (!exist)
		{
			if (!SystemHelper::isElevated())
			{
			ret = _elevateSelf();
			ERROR_CHECK_BOOL(ret);
			return;
			}

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_runType	= EnumRunType::Install;
				m_state		= EnumState::CheckEnv;
			});

			UInt64 freeSpace = 0;
			ret = SystemHelper::getDiskFreeSpace(L"C:", freeSpace);
			ERROR_CHECK_BOOL(ret);
			ret = (freeSpace > (1.2 * 1024.0 * 1024.0 * 1024.0));
			ERROR_CHECK_BOOL(ret);

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_state		= EnumState::CheckUpdate;

				m_progress->SetVisible(true);
				m_message->SetVisible(false);
			});

			BOOL haveUpdate = false;
			std::string version;
			std::string downloadURL;
			std::string md5;
			ret = _checkUpdate("0.0.0.0", haveUpdate, version, downloadURL, md5);
			ERROR_CHECK_BOOL(ret);

			X_ASSERT(haveUpdate);
			X_ASSERT(downloadURL.length());
			X_ASSERT(md5.length());

			CPath downloadPath = SystemHelper::getTempPath();
			CString fileName;
			fileName.Format(L"%d.exe", time(NULL));
			downloadPath.Append(fileName);

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_state = EnumState::Downloading;
			});

			ret = _downloadPackage(downloadURL, UTF16ToUTF8(downloadPath));
			ERROR_CHECK_BOOL(ret);

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_state = EnumState::Verfiying;
				m_progress->SetVisible(false);
				m_message->SetVisible(true);
				m_message->SetText(DUI_LOAD_STRING(STRID_INSTALLING));
			});

			ret = _verifyPackage(UTF16ToUTF8(downloadPath), md5);
			ERROR_CHECK_BOOL(ret);

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_state = EnumState::Installing;
				m_progress->SetValue(120);
			});

			ret = _runInstall(UTF16ToUTF8(downloadPath));
			ERROR_CHECK_BOOL(ret);

			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_progress->SetValue(140);
			});

			_deleteSelfByHelperProcess();
		}
		// check update
		else
		{
			CNotificationCenter::defaultCenter().postNotification([=]
			{
				m_runType = EnumRunType::Update;
				m_state = EnumState::CheckUpdate;

				m_progress->SetVisible(true);
				m_message->SetVisible(false);
			});

			CString currentVersion;
			ret = SystemHelper::getFileVersion(mainExePath, currentVersion);
			ERROR_CHECK_BOOL(ret);

			BOOL haveUpdate = FALSE;
			std::string version, url, md5;
			ret = _checkUpdate(UTF16ToUTF8(currentVersion), haveUpdate, version, url, md5);
			ERROR_CHECK_BOOL(ret);

			if (!haveUpdate)
				QUIT();
			else
			{
				if (!SystemHelper::isElevated())
				{
					ret = _elevateSelf();
					ERROR_CHECK_BOOL(ret);
					return;
				}

				CNotificationCenter::defaultCenter().postNotification([=]
				{
					m_state = EnumState::Downloading;
				});

				CPath downloadPath = SystemHelper::getTempPath();
				CString fileName;
				fileName.Format(L"%d.exe", time(NULL));
				downloadPath.Append(fileName);

				ret = _downloadPackage(url, UTF16ToUTF8(downloadPath));
				ERROR_CHECK_BOOL(ret);

				CNotificationCenter::defaultCenter().postNotification([=]
				{
					m_state = EnumState::Verfiying;
				});

				ret = _verifyPackage(UTF16ToUTF8(downloadPath), md5);
				ERROR_CHECK_BOOL(ret);

				CNotificationCenter::defaultCenter().postNotification([=]
				{
					m_state = EnumState::Installing;
				});

				_delayDeleteSelf();

				ret = _runInstall(UTF16ToUTF8(downloadPath));
				ERROR_CHECK_BOOL(ret);
			}
		}
	}

Exit0:
	if (!ret)
	{
		if (m_isCancel)
			return;

		CNotificationCenter::defaultCenter().postNotification([=]
		{
			m_progress->SetVisible(false);
			m_message->SetTextColor(0xFFD74C50);
			m_message->SetVisible(true);

			CString message;
			if (EnumRunType::Install == m_runType)
			{
				if (EnumState::CheckEnv == m_state)
					message = DUI_LOAD_STRING(STRID_NO_ENOUGH_SPACE);
				else
					message = DUI_LOAD_STRING(STRID_DOWNLOAD_FAIL);
			}
				
			else
				message = DUI_LOAD_STRING(STRID_UPDATE_FAIL);

			CString error = DUI_LOAD_STRING(STRID_ERROR_CODE);
			error.AppendFormat(L"(%d,%d)", m_runType, m_state);

			message += error;

			if ((EnumRunType::Install == m_runType) ||
				(EnumState::Downloading == m_state ||
				EnumState::Verfiying == m_state ||
				EnumState::Installing == m_state))
			{
				m_message->SetText(message);
			}
		});

		Sleep(10 * 1000);

		CNotificationCenter::defaultCenter().postNotification([=]
		{
			if (EnumRunType::Update == m_runType)
				_launchMianApp();
			Close();
		});
	}
	else
	{
		CNotificationCenter::defaultCenter().postNotification([=]
		{
			m_state = EnumState::Finish;
			_launchMianApp();
			Close();
		});
	}
}

BOOL CMainDlg::_checkUpdate(const std::string& currentVersion, BOOL& haveUpdate, std::string& version, std::string& downloadURL, std::string& md5)
{
	TRACE_STACK;

	BOOL ret = TRUE;
	{
		ErrorCode ec = EC_OK;
		LibCurl::CHttpGetRequest get;
		std::string url = StringHelper::format(
			"http://web.limixuexi.com/index.php?r=config/info&version=%s",
			currentVersion.c_str());
		ec = get.requestURL("http://web.limixuexi.com/index.php?r=config/info", 5);
		ERROR_CHECK_BOOL(EC_OK == ec, ret = FALSE);

		const std::string& responseData = get.getRecvData();
		Json::Value recvJsonAll;
		ret = Json::Reader().parse(responseData, recvJsonAll);
		ERROR_CHECK_BOOLEX(ret);

		const Json::Value& recvJson = recvJsonAll["data"];
		version = recvJson["version"].asString();
		downloadURL = recvJson["renewurl"].asString();
		md5 = recvJson["renewmd5"].asString();
		haveUpdate = url.length();
	}

Exit0:
	return ret;
}

BOOL CMainDlg::_downloadPackage(const std::string& url, const std::string& downloadPath)
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		LibCurl::CHttpGetDownload download;
		download.setProgressDelegate(fastdelegate::bind(&CMainDlg::_downloadProgress, this));
		download.setCancelDelegate(fastdelegate::bind(&CMainDlg::_isCancel, this));

		ErrorCode ec = download.downloadFile(url.c_str(), downloadPath);

		if (EC_OK != ec)
			DeleteFile(UTF8ToUTF16(downloadPath.c_str()).c_str());
		ERROR_CHECK_BOOLEX(EC_OK == ec, Ret = FALSE);
	}
Exit0:

	return Ret;
}

void CMainDlg::_downloadProgress(double total, double now, double speed)
{
	CNotificationCenter::defaultCenter().postNotification([=]
	{
		int value = now * 100.0 / total;
		value = max(value, 4);

		m_progress->SetValue(value);

		CString str;
		str.Format(L"%d%%", value);
		m_progressText->SetText(str);
	});
}

BOOL CMainDlg::_verifyPackage(const std::string& downloadPath, const std::string& packageMd5)
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		CFileHash<MD5> fileMd5;
		Byte result[16] = { 0 };
		ErrorCode ec = fileMd5.hashFile(downloadPath, result);
		ERROR_CHECK_BOOLEX(EC_OK == ec, Ret = FALSE);
		std::string downloadMd5 = StringHelper::ByteBufferToHexString(result, 16);

		Ret = (StringHelper::stricmp(packageMd5.c_str(), downloadMd5.c_str()) == 0);
		LOG(INFO) << "return md5:" << packageMd5 << std::endl;
		LOG(INFO) << "real md5" << downloadMd5 << std::endl;
		ERROR_CHECK_BOOL(Ret);
	}

Exit0:
	return Ret;
}

BOOL CMainDlg::_runInstall(const std::string& packagePath)
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		CPath path = UTF8ToUTF16(packagePath.c_str()).c_str();
		SystemHelper::removeOpenFileWarning(path);
		
		HANDLE h = SystemHelper::shellExecute(path, L"/S", !SystemHelper::isUnderXP());
		ERROR_CHECK_BOOLEX(h, Ret = FALSE);

		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);
		DeleteFile(path);
	}

Exit0:
	return Ret;
}

BOOL CMainDlg::_launchMianApp()
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		CPath mainExePath = SystemHelper::getMainAppPath();
		Ret = PathFileExists(mainExePath);
		ERROR_CHECK_BOOL(Ret);
		HANDLE h = SystemHelper::shellExecute(mainExePath, L"", FALSE);
		ERROR_CHECK_BOOLEX(h, Ret = FALSE);
		::CloseHandle(h);

		Close();
	}

Exit0:
	return Ret;
}

BOOL CMainDlg::_delayDeleteSelf()
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		CPath currentPath = SystemHelper::getModulePath();
		CPath randomPath = currentPath;
		randomPath.RemoveFileSpec();
		randomPath.Append(UTF8ToUTF16(StringHelper::intToString(::time(NULL)).c_str()).c_str());

		Ret = MoveFile(currentPath, randomPath);
		ERROR_CHECK_BOOL(Ret);

		MoveFileEx(randomPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}

Exit0:
	return Ret;
}

BOOL CMainDlg::_deleteSelfByHelperProcess()
{
	TRACE_STACK;

	BOOL Ret = TRUE;
	{
		CPath self = SystemHelper::getModulePath();
		CPath temp = SystemHelper::getTempPath();
		CString random;
		random.Format(L"%d.exe", time(NULL));
		temp.Append(random);

		Ret = CopyFile(self, temp, FALSE);
		ERROR_CHECK_BOOL(Ret);

		SystemHelper::removeOpenFileWarning(temp);

		CString cmd;
		cmd.Format(L"%s %s \"%s\" %s %d", 
			CommandLine::DELETE_ME, 
			CommandLine::PATH, self, 
			CommandLine::PROCESS_ID, GetCurrentProcessId());

		HANDLE h = SystemHelper::shellExecute(temp, cmd, FALSE);
		ERROR_CHECK_BOOLEX(h, Ret = FALSE);

		CloseHandle(h);
	}

Exit0:
	return Ret;
}

BOOL CMainDlg::_elevateSelf()
{
	BOOL ret = TRUE;
	{
		CPath selfPath = SystemHelper::getModulePath();
		CString cmd;
		cmd.Format(L"%s %s %d", CommandLine::WAIT_EXIT, CommandLine::PROCESS_ID, GetCurrentProcessId());

		HANDLE h = SystemHelper::shellExecute(selfPath, cmd, TRUE);
		ERROR_CHECK_BOOLEX(h, ret = FALSE);

		CloseHandle(h);

		CNotificationCenter::defaultCenter().postNotification([=]
		{
			Close();
		});
	}

Exit0:
	return ret;
}

bool CMainDlg::_isCancel()
{
	return m_isCancel;
}

//--------------------------------------------------------------------------         
