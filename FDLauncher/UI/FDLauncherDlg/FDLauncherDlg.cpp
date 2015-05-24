#include "stdafx.h"
#include "FDLauncherDlg.h"
#include "../../Core/Misc/SystemHelper.h"
#include "../../Core/Misc/StringConv/StringConv.h"
#include <time.h>
#include "../../Core/LibcRULHttp/LibCurlHttpGetRequest.h"
#include "../../Core/LibcRULHttp/LibCurlHttpGetDownload.h"
#include "../../Core/JsonCpp/json.h"
#include "../../Core/Misc/FastDelegate/FastDelegate.h"
#include "../../Core/Misc/FileHash.h"
#include "../../Core/Misc/MD5/MD5.h"
#include "../../Core/Misc/StringHelper.h"

FDLauncherDlg::FDLauncherDlg()
	: m_isCancel(false)
{
    m_bShadowEnabled = false;

}

FDLauncherDlg::~FDLauncherDlg()
{

}

void FDLauncherDlg::OnInit()
{

	DUI_BIND_CONTROL_VAR(L"version", m_textVersion, CTextUI);

	CString strVersion;
	strVersion.Format(DUI_LOAD_STRING(STRID_VERSION), L"0.1");
	m_textVersion->SetText(strVersion);

	CenterWindow();

	m_asyncOpt = std::async([=]
	{
		this->_runLauncher();
	});
}

void FDLauncherDlg::OnOKClick(TNotifyUI& msg, BOOL& bHandled)
{
	Close();
}

void FDLauncherDlg::OnClose()
{
	m_isCancel = true;
	m_asyncOpt.wait();

	__super::OnClose();	
	_Module.Exit();
}

void FDLauncherDlg::_runLauncher()
{
	BOOL ret = TRUE;
	{
		// check need install
		CPath mainExePath = SystemHelper::getMainAppPath();
		BOOL exist = ::PathFileExists(mainExePath);

		// install
		if (!exist)
		{
			BOOL haveUpdate = false;
			std::string version, url, md5;
			ret = _checkUpdate("0.0.0.0", haveUpdate, version, url, md5);
			ERROR_CHECK_BOOL(ret);

			X_ASSERT(haveUpdate);
			X_ASSERT(url.length());
			X_ASSERT(md5.length());

			CPath downloadPath = SystemHelper::getTempPath();
			CString fileName;
			fileName.Format(L"%d.exe", time(NULL));
			downloadPath.Append(fileName);

			ret = _downloadPackage(url, UTF16ToUTF8(downloadPath));
			ERROR_CHECK_BOOL(ret);

			ret = _verifyPackage(UTF16ToUTF8(downloadPath), md5);
			ERROR_CHECK_BOOL(ret);

			ret = _runInstall(UTF16ToUTF8(downloadPath));
			ERROR_CHECK_BOOL(ret);
		}
		// check update
		else
		{
			CString currentVersion;
			ret = SystemHelper::getFileVersion(mainExePath, currentVersion);
			ERROR_CHECK_BOOL(ret);

			BOOL haveUpdate = FALSE;
			std::string version, url, md5;
			ret = _checkUpdate(UTF16ToUTF8(currentVersion), haveUpdate, version, url, md5);
			ERROR_CHECK_BOOL(ret);
			
			if (!haveUpdate)
			{
				ret = _launchMianApp();
				ERROR_CHECK_BOOL(ret);
			}
			else
			{
				CPath downloadPath = SystemHelper::getTempPath();
				CString fileName;
				fileName.Format(L"%d.exe", time(NULL));
				downloadPath.Append(fileName);

				ret = _downloadPackage(url, UTF16ToUTF8(downloadPath));
				ERROR_CHECK_BOOL(ret);

				ret = _verifyPackage(UTF16ToUTF8(downloadPath), md5);
				ERROR_CHECK_BOOL(ret);

				ret = _delayDeleteSelf();
				ERROR_CHECK_BOOL(ret);

				ret = _runInstall(UTF16ToUTF8(downloadPath));
				ERROR_CHECK_BOOL(ret);
			}
		}
	}

Exit0:
	if (!ret)
	{
		// post error notify
	}
	else
	{
		// post finish notify
	}
}

BOOL FDLauncherDlg::_checkUpdate(const std::string& currentVersion, BOOL& haveUpdate, std::string& version, std::string& url, std::string& md5)
{
	BOOL ret = EC_OK;
	{
		CString queryUrl;
		queryUrl.Format(L"http://apiv4.fudaodashi.com/index.php?r=Upgrade/info&version=%s", UTF8ToUTF16(currentVersion.c_str()).c_str());

		LibCurl::CHttpGetRequest get;
		ErrorCode ec = get.requestURL(UTF16ToUTF8(queryUrl).c_str(), 10);
		ERROR_CHECK_BOOLEX(EC_OK == ec, ret = FALSE);

		const std::string& recvData = get.getRecvData();
		Json::Value recvJson;
		ret = Json::Reader().parse(recvData, recvJson);
		ERROR_CHECK_BOOL(ret);

		haveUpdate = recvJson["result"].asInt();
		if (haveUpdate)
		{
			const Json::Value& data = recvJson["data"];
			version = data["version"].asString();
			url		= data["url"].asString();
			md5		= data["md5"].asString();

			ERROR_CHECK_BOOLEX(version.length() && url.length() && md5.length(), ret = FALSE);
		}
	}

Exit0:
	return ret;
}

BOOL FDLauncherDlg::_downloadPackage(const std::string& url, const std::string& downloadPath)
{
	BOOL Ret = TRUE;
	{
		LibCurl::CHttpGetDownload download;
		download.setProgressDelegate(fastdelegate::bind(&FDLauncherDlg::_downloadProgress, this));
		download.setCancelDelegate(fastdelegate::bind(&FDLauncherDlg::_isCancel, this));

		ErrorCode ec = download.downloadFile(url.c_str(), downloadPath);
		ERROR_CHECK_BOOLEX(EC_OK == ec, Ret = FALSE);	
	}
Exit0:

	return Ret;
}

void FDLauncherDlg::_downloadProgress(double total, double now, double speed)
{

}

BOOL FDLauncherDlg::_verifyPackage(const std::string& downloadPath, const std::string& packageMd5)
{
	BOOL Ret = TRUE;
	{
		CFileHash<MD5> fileMd5;
		Byte result[16] = { 0 };
		ErrorCode ec = fileMd5.hashFile(downloadPath, result);
		ERROR_CHECK_BOOLEX(EC_OK == ec, Ret = FALSE);
		std::string downloadMd5 = StringHelper::ByteBufferToHexString(result, 16);

		Ret = (StringHelper::stricmp(packageMd5.c_str(), downloadMd5.c_str()) == 0);
		ERROR_CHECK_BOOL(Ret);
	}

Exit0:
	return Ret;
}

BOOL FDLauncherDlg::_runInstall(const std::string& packagePath)
{
	BOOL Ret = TRUE;
	{
		CPath path = UTF8ToUTF16(packagePath.c_str()).c_str();
		HANDLE h = SystemHelper::shellExecute(path, L"/S", TRUE);
		ERROR_CHECK_BOOLEX(h, Ret = FALSE);

		WaitForSingleObject(h, INFINITE);
		::Sleep(200);
		DeleteFile(UTF8ToUTF16(packagePath.c_str()).c_str());
	}
	
Exit0:
	return Ret;
}

BOOL FDLauncherDlg::_launchMianApp()
{
	BOOL Ret = TRUE;
	{
		CPath mainExePath = SystemHelper::getMainAppPath();
		HANDLE h = SystemHelper::shellExecute(mainExePath, L"", FALSE);
		ERROR_CHECK_BOOLEX(h, Ret = FALSE);
		::CloseHandle(h);

		Close();
	}

Exit0:
	return Ret;
}

BOOL FDLauncherDlg::_delayDeleteSelf()
{
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

bool FDLauncherDlg::_isCancel()
{
	return m_isCancel;
}