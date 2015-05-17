/* -------------------------------------------------------------------------
//  File Name   :   LibCurlHttpPostDownload.cpp
//  Author      :   Zhang Fan
//  Create Time :   2012-3-21 15:18:46
//  Description :   
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpPostDownload.h"
#include "../Misc/FileConfig/FileConfig.h"

//-------------------------------------------------------------------------

namespace LibCurl
{
    CHttpPostDownload::CHttpPostDownload()
        : m_isBinaryMode(false)
        , m_lowSpeedTimeoutSec(0)
        , m_pFile(NULL)
        , m_fileBufferSize(0)
    {

    }

    CHttpPostDownload::CHttpPostDownload(const TransferSetting & transferSetting)
        : m_isBinaryMode(false)
        , m_lowSpeedTimeoutSec(0)
        , m_pFile(NULL)
        , m_fileBufferSize(0)
        , CHttpBase(transferSetting)
    {

    }

    CHttpPostDownload::~CHttpPostDownload()
    {

    }

    void CHttpPostDownload::setProgressDelegate(FileProgressDelegate progress)
    {
        m_progress = progress;
    }

    void CHttpPostDownload::setCancelDelegate(LibCurl::CancelDelegate cancel)
    {
        m_cancel = cancel;
    }

    void CHttpPostDownload::setBinaryMode(bool isBinaryMode)
    {
        m_isBinaryMode = isBinaryMode;
    }

    void CHttpPostDownload::setPostData(std::string &postData)
    {
        X_ASSERT(!postData.empty());

        m_postData = postData;
    }

    void CHttpPostDownload::setFileBufferSize(UInt32 bufferSize)
    {
        m_fileBufferSize = bufferSize;
    }
    
    ErrorCode CHttpPostDownload::downloadFile(const char* url, const std::string& file, UInt32 lowSpeedTimeoutSec /* = 5 * 60 */)
    {
        X_ASSERT(url && *url);
        X_ASSERT(!file.empty());

        ErrorCode ret = EC_OK;
        {
            m_lowSpeedTimeoutSec = lowSpeedTimeoutSec;

            m_pFile = ::fopen_x(file, "w+b");
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

    void CHttpPostDownload::_curlSetRecvDataFunction()
    {
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_WRITEDATA, (void*)m_pFile);
    }

    void CHttpPostDownload::_curlSetDIY()
    {
        X_ASSERT(m_pFile);

        m_cancelDelegate = fastdelegate::MakeDelegate(this, &CHttpPostDownload::_cancelDelegate);

        ::curl_easy_setopt(m_pLibCURL, CURLOPT_CONNECTTIMEOUT, 60);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_LIMIT, 512);	                        //512 B/s
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_LOW_SPEED_TIME, m_lowSpeedTimeoutSec);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_FOLLOWLOCATION, true);
        
        if (!m_isBinaryMode)
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDS, m_postData.c_str());
        else
        {
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDS, m_postData.data()); 
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDSIZE, m_postData.size());
        }
    }

    bool CHttpPostDownload::_cancelDelegate()
    {
        X_ASSERT(!m_cancel.empty());


        // notify progress
        if (!m_progress.empty())
            m_progress(m_downloadTotal, m_downloadNow, m_downloadSpeed);

        // check cancel
        if (!m_cancel.empty())
            return m_cancel();
        else
            return false;
    }
}

//--------------------------------------------------------------------------