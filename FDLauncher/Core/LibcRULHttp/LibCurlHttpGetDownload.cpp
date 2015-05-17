/* -------------------------------------------------------------------------
//  File Name   :   LibCurlHttpGetDownload.cpp
//  Author      :   Zhang Fan
//  Create Time :   2012-9-4 9:19:46
//  Description :   
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpGetDownload.h"
#include "../Misc/FileConfig/FileConfig.h"

//-------------------------------------------------------------------------

namespace LibCurl
{
	CHttpGetDownload::CHttpGetDownload()
        : m_lowSpeedTimeoutSec(0)
		, m_pFile(NULL)
        , m_speedFactor(1)
	{

	}

	CHttpGetDownload::CHttpGetDownload(const TransferSetting & transferSetting)
        : m_lowSpeedTimeoutSec(0)
		, m_pFile(NULL)
		, CHttpBase(transferSetting)
	{

	}

	CHttpGetDownload::~CHttpGetDownload()
	{

	}

	void CHttpGetDownload::setProgressDelegate(FileProgressDelegate progress)
	{
		m_progress = progress;
	}

    void CHttpGetDownload::setSpeedFactor(UInt32 factor)
    {
        X_ASSERT(factor);

        m_speedFactor = factor;
    }

	void CHttpGetDownload::setCancelDelegate(LibCurl::CancelDelegate cancel)
	{
		m_cancel = cancel;

		m_cancelDelegate = fastdelegate::MakeDelegate(this, &CHttpGetDownload::_cancelDelegate);
	}

	ErrorCode CHttpGetDownload::downloadFile(const char* url, const std::string& file, UInt32 lowSpeedTimeoutSec /* = 5 * 60 */)
	{
		X_ASSERT(url && *url);
		X_ASSERT(!file.empty());

		ErrorCode ret = EC_OK;
		{
			m_lowSpeedTimeoutSec = lowSpeedTimeoutSec;

			m_pFile = ::fopen_x(file.c_str(), "w+b");
			ERROR_CHECK_BOOLEX(m_pFile, ret = EC_LOCAL_FS_FAIL);

			setURL(url);
			ret = perform();
			ERROR_CHECK_BOOL(EC_OK == ret);
		}

Exit0:
		if (m_pFile)
		{
			::fclose(m_pFile);
			m_pFile = NULL;
		}

		return ret;
	}

	void CHttpGetDownload::_curlSetRecvDataFunction()
	{
		::curl_easy_setopt(m_pLibCURL, CURLOPT_WRITEDATA, (void*)m_pFile);
	}

	void CHttpGetDownload::_curlSetDIY()
	{
		X_ASSERT(m_pFile);
        
        m_cancelDelegate = fastdelegate::MakeDelegate(this, &CHttpGetDownload::_cancelDelegate);
        
		::curl_easy_setopt(m_pLibCURL, CURLOPT_CONNECTTIMEOUT, 10);
		::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_LIMIT, 512);	                        //512 B/s
		::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_TIME, m_lowSpeedTimeoutSec);
		::curl_easy_setopt(m_pLibCURL, CURLOPT_FOLLOWLOCATION, true);

	}

	bool CHttpGetDownload::_cancelDelegate()
	{
		if (!m_progress.empty())
			m_progress(m_downloadTotal, m_downloadNow, m_downloadSpeed / m_speedFactor);

        // check cancel
        if (!m_cancel.empty())
            return m_cancel();
        else
            return false;
	}
}


//--------------------------------------------------------------------------