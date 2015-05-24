/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpPostUpload.cpp
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-21 9:52:18
//	Description	:	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpPostUpload.h"
#include "../Misc/FileConfig/FileConfig.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
    CHttpPostUpload::CHttpPostUpload()
        : m_lowSpeedTimeoutSec(0)
        , m_pFile(NULL)
        , m_fileBufferSize(0)
    {

    }

    CHttpPostUpload::CHttpPostUpload(const TransferSetting & transferSetting)
        : m_lowSpeedTimeoutSec(0)
        , m_pFile(NULL)
        , m_fileBufferSize(0)
        , CHttpBase(transferSetting)
    {

    }

    CHttpPostUpload::~CHttpPostUpload()
    {

    }

    void CHttpPostUpload::setProgressDelegate(FileProgressDelegate progress)
    {
        m_progress = progress;
    }

    void CHttpPostUpload::setCancelDelegate(LibCurl::CancelDelegate cancel)
    {
        m_cancel = cancel;
    }
    
    void CHttpPostUpload::setFileBufferSize(UInt32 bufferSize)
    {
        m_fileBufferSize = bufferSize;
    }

    ErrorCode CHttpPostUpload::uploadFile(const char* url, const std::string& file, UInt32 lowSpeedTimeoutSec /* = 5 * 60 */)
    {
        X_ASSERT(url && *url);
        X_ASSERT(!file.empty());

        ErrorCode ret = EC_OK;
        {
            m_lowSpeedTimeoutSec = lowSpeedTimeoutSec;

            m_pFile = ::fopen_x(file, "rb");
            ERROR_CHECK_BOOLEX(m_pFile, ret = EC_LOCAL_FS_FAIL);

            if (m_fileBufferSize)
            {
                int iRet = ::setvbuf(m_pFile, NULL, _IOFBF, m_fileBufferSize);
                ERROR_CHECK_BOOLEX(!iRet, ret = EC_LOCAL_FS_FAIL);
            }

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

    void CHttpPostUpload::_curlSetDIY()
    {
        X_ASSERT(m_pFile);

        m_cancelDelegate = fastdelegate::MakeDelegate(this, &CHttpPostUpload::_cancelDelegate);

        ::curl_easy_setopt(m_pLibCURL, CURLOPT_CONNECTTIMEOUT, 10);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_LIMIT, 512);	                        // 512 B/s
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_TIME, m_lowSpeedTimeoutSec);

        ::fseek(m_pFile, 0L, SEEK_END );
        UInt32 fileSize = (UInt32)::ftell(m_pFile);
        ::fseek(m_pFile, 0L, SEEK_SET);

        ::curl_easy_setopt(m_pLibCURL, CURLOPT_POST, true);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDSIZE, fileSize);

        ::curl_easy_setopt(m_pLibCURL, CURLOPT_READFUNCTION, CHttpPostUpload::_readDataFunction);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_READDATA, m_pFile);

        m_pSendHeader = curl_slist_append(m_pSendHeader, "Accept:");
        m_pSendHeader = curl_slist_append(m_pSendHeader, "Expect:");
        m_pSendHeader = curl_slist_append(m_pSendHeader, "Content-Type:");
    }

    size_t CHttpPostUpload::_readDataFunction(void* outBuffer, size_t size, size_t n, void* dataStream)
    {
        return ::fread(outBuffer, size, n, (FILE*)dataStream);
    }

    bool CHttpPostUpload::_cancelDelegate()
    {
        X_ASSERT(!m_cancel.empty());

        // notify progress
        if (!m_progress.empty())
            m_progress(m_uploadTotal, m_uploadNow, m_uploadSpeed);

        // check cancel
        if (!m_cancel.empty())
            return m_cancel();
        else
            return false;
    }
}

//--------------------------------------------------------------------------
