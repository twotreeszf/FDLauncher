/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpBase.cpp
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-19 17:11:08
//	Description	:	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpBase.h"
#include <string.h>
#include "..\Misc\StringHelper.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
	ProxyInfo::ProxyInfo()
	{
		reset();
	}

	void ProxyInfo::reset()
	{
		m_ip = "";
		m_port = 0;
		m_havepwd = false;
		m_user = "";
		m_passwd = "";
	}

	TransferSetting::TransferSetting()
	{
		reset();
	}

	void TransferSetting::reset()
	{
		m_uploadBandWidth = 0;
		m_downloadBandWidth = 0;
		m_proxyMode = ProxyMode::NoProxy;
		m_proxyInfo.reset();
	}

    CHttpBase::CHttpBase() 
        : m_transferSetting()
        , m_url()
        , m_pLibCURL()
        , m_recvData()
        , m_recvHeader()
        , m_pSendHeader(0)
        , m_httpCode(0)
        , m_timeOut(CHttpBase::CURL_HTTP_DEFAULT_TIMEOUT)
        , m_downloadTotal(0.0)
        , m_downloadNow(0.0)
        , m_downloadSpeed(0.0)
        , m_uploadTotal(0.0)
        , m_uploadNow(0.0)
        , m_uploadSpeed(0.0)
    {

    }

    CHttpBase::CHttpBase( const TransferSetting& transferSetting )
        : m_transferSetting(transferSetting)
        , m_url()
        , m_pLibCURL()
        , m_recvData()
        , m_recvHeader()
        , m_pSendHeader(0)
        , m_httpCode(0)
        , m_timeOut(CHttpBase::CURL_HTTP_DEFAULT_TIMEOUT)
        , m_downloadTotal(0.0)
        , m_downloadNow(0.0)
        , m_downloadSpeed(0.0)
        , m_uploadTotal(0.0)
        , m_uploadNow(0.0)
        , m_uploadSpeed(0.0)
    {

    }

    CHttpBase::~CHttpBase()
    {

    }

    void CHttpBase::setUploadBandWidth( UInt32 bandWidth )
    {
        m_transferSetting.m_uploadBandWidth = bandWidth;
    }

    void CHttpBase::setDownLoadBandWidth( UInt32 bandWidth )
    {
        m_transferSetting.m_downloadBandWidth = bandWidth;
    }

    void CHttpBase::setTimeout( Int32 timeout )
    {
        m_timeOut = timeout;
    }

    void CHttpBase::setURL( const char* url )
    {
        X_ASSERT(url && *url);
        m_url = url;
    }

    void CHttpBase::setSendHeader(const char* headers)
    {
        const char* pos1 = NULL;
        const char* pos2 = NULL;
        pos1 = headers;
        while (true)
        {
            pos2 = strchr(pos1, '\n');
            if (!pos2)
            {
                m_pSendHeader = curl_slist_append(m_pSendHeader, pos1);
                break;
            }

            m_pSendHeader = curl_slist_append(m_pSendHeader, std::string(pos1, (Int32)(pos2 - pos1)).c_str());

            pos1 = pos2 + 1;
        }

        ::curl_easy_setopt(m_pLibCURL, CURLOPT_HTTPHEADER, m_pSendHeader);
    }

    void CHttpBase::setCancelDelegate(CancelDelegate cancel)
    {
        m_cancelDelegate = cancel;
    }

    const std::string& CHttpBase::getRecvData()
    {
        return m_recvData;
    }

    const std::string& CHttpBase::getRecvHeader()
    {
        return m_recvHeader;
    }

    std::string CHttpBase::getErrorMessage()
    {
        std::string errorMessage = &m_errorMessage[0];
        return errorMessage;
    }

    UInt32 CHttpBase::getHttpCode()
    {
        return m_httpCode;
        
    }

    ErrorCode CHttpBase::perform()
    {
        ErrorCode result = EC_OK;
        {
            result = _initCurl();
            ERROR_CHECK_BOOL(EC_OK == result);

            _curlSetUrlAddress();  
            _curlSetTimeOutMS();            
            _curlSetRecvDataFunction();                
            _curlSetRecvHeaderFunction();               
            _curlSetProtocols();            
            _curlSetProxy();                
            _curlSetProgressFunction();     
            _curlSetSendHeader();   
            _curlSetErrorBuffer();
            _curlSetDIY();                  

            CURLcode curlresult = ::curl_easy_perform(m_pLibCURL);

            if (curlresult != CURLE_OK)
            {
                result = (ErrorCode)(curlresult | EC_CURL_MASK);
                
                std::string error = StringHelper::format("CurlError:%d, %s, Detail:%s", curlresult, curl_easy_strerror(curlresult), getErrorMessage().c_str());
                LOG_ERROR(error.c_str());
            }
            else
            {
                long httpCode = 0;
                curlresult = ::curl_easy_getinfo(m_pLibCURL, CURLINFO_RESPONSE_CODE, &httpCode);
                m_httpCode = httpCode;
                
                if (CURLE_OK != curlresult)
                    result = EC_HTTP_NORETURN;
                else if (httpCode < 200 || httpCode >= 300)
                    result = (ErrorCode)(httpCode | EC_HTTP_MASK);
                ERROR_CHECK_BOOL(EC_OK == result);
            }
        }

Exit0:
        _cleanUpCurl();
        return result;
    }

    ErrorCode CHttpBase::_initCurl()
    {
        X_ASSERT(!m_pLibCURL);

        m_pLibCURL = ::curl_easy_init();
        if (m_pLibCURL == 0)
        {
            X_ASSERT(false);
            return EC_CURL_INTERERROR;
        }
        else 
            return EC_OK;
    }

    void CHttpBase::_curlSetUrlAddress()
    {
        X_ASSERT(m_pLibCURL);
        X_ASSERT(!m_url.empty());
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_URL, m_url.c_str());
    }

    void CHttpBase::_curlSetUploadSpeed(UInt32 limit)
    {
        X_ASSERT(m_pLibCURL);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_MAX_SEND_SPEED_LARGE, (curl_off_t)(limit * 1024));
    }

    void CHttpBase::_curlSetDownloadSpeed(UInt32 limit)
    {
        X_ASSERT(m_pLibCURL);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)(limit * 1024));
    }

    void CHttpBase::_curlSetTimeOutMS()
    {
        X_ASSERT(m_pLibCURL);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_TIMEOUT_MS, m_timeOut);
    }

    void CHttpBase::_curlSetSendHeader()
    {
        X_ASSERT(m_pLibCURL);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_HTTPHEADER, m_pSendHeader);
    }

    void CHttpBase::setProxymode( ProxyMode::type mode )
    {
        m_transferSetting.m_proxyMode = mode;
    }

    void CHttpBase::setProxyInfo( ProxyInfo &proxySetting )
    {
        m_transferSetting.m_proxyInfo = proxySetting;
    }

    void CHttpBase::_curlSetRecvDataFunction()
    {
        X_ASSERT(m_pLibCURL != 0);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_WRITEFUNCTION, CHttpBase::defaultWriteStreamFunc);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_WRITEDATA, &m_recvData);
    }

    // -----------------------------------------------------------------------------
    // private   virtual 
    void CHttpBase::_curlSetProgressFunction()
    {
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_NOPROGRESS, 0L);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROGRESSFUNCTION, CHttpBase::progressCallback);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROGRESSDATA, this); 
    }

    void CHttpBase::_curlSetDIY()
    {

    }

    void CHttpBase::_curlSetRecvHeaderFunction()
    {
        X_ASSERT(m_pLibCURL != 0);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_HEADERFUNCTION, CHttpBase::defaultWriteStreamFunc);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_HEADERDATA, &m_recvHeader);
    }

    void CHttpBase::_curlSetProtocols()
    {
        X_ASSERT(m_pLibCURL);
        if (_isUsingHttps())
        {
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_SSL_VERIFYPEER, 0L);
        }
    }

    void CHttpBase::_curlSetProxy()
    {
        X_ASSERT(m_pLibCURL);
        switch (m_transferSetting.m_proxyMode)
        {
        case ProxyMode::NoProxy:
            return;
            break;

        case ProxyMode::UsingBrowserProxy :
            X_ASSERT(!"only using application setting, in kernel can't setting it.");
            return;
            break;

        case ProxyMode::ProxyHTTP :
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
            break;

        case ProxyMode::ProxySock4 :      
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
            break;

        case ProxyMode::ProxySock5 :          
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
            break;
        default :
            X_ASSERT(false);
            return;
        }
        
        X_ASSERT(!m_transferSetting.m_proxyInfo.m_ip.empty());
        X_ASSERT(m_transferSetting.m_proxyInfo.m_port);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXY, m_transferSetting.m_proxyInfo.m_ip.c_str());
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXYPORT, m_transferSetting.m_proxyInfo.m_port); 

        if (m_transferSetting.m_proxyInfo.m_havepwd)
        {
            std::string usrAndpwdStr(m_transferSetting.m_proxyInfo.m_user);
            usrAndpwdStr.append(":");
            usrAndpwdStr.append(m_transferSetting.m_proxyInfo.m_passwd);
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_PROXYUSERPWD, usrAndpwdStr.c_str());
        }

        if (_isUsingHttps())
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_HTTPPROXYTUNNEL, 1);
        else
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_HTTPPROXYTUNNEL, 0);
    }

    void CHttpBase::_curlSetErrorBuffer()
    {
        m_errorMessage.clear();
        m_errorMessage.resize(1024);
        curl_easy_setopt(m_pLibCURL, CURLOPT_ERRORBUFFER, &m_errorMessage[0]); 
    }

    void  CHttpBase::_cleanUpCurl()
    {
        X_ASSERT(m_pLibCURL != 0);

        ::curl_slist_free_all(m_pSendHeader);
        ::curl_easy_cleanup(m_pLibCURL);
		
		m_transferSetting.reset();
        
        m_url.clear();
        m_pLibCURL      = 0;
        m_pSendHeader   = 0;
        m_timeOut       = CHttpBase::CURL_HTTP_DEFAULT_TIMEOUT;

        m_downloadTotal     = 0.0;
        m_downloadNow       = 0.0;
        m_downloadSpeed     = 0.0;
        m_uploadTotal       = 0.0;
        m_uploadNow         = 0.0;
        m_uploadSpeed       = 0.0;
    }


    bool CHttpBase::_isUsingHttps()
    {
        X_ASSERT(!m_url.empty());
        return ((m_url.find("https://") == 0));
    }

    ErrorCode CHttpBase::initGlobal()
    {
        CURLcode result = ::curl_global_init(CURL_GLOBAL_ALL);
        return (ErrorCode)(result | EC_CURL_MASK);
    }

    void CHttpBase::uninitGlobal()
    {
        ::curl_global_cleanup();
    }

    size_t CHttpBase::defaultWriteStreamFunc( void* ptr, size_t msize, size_t n, std::string* stream )
    {
        X_ASSERT(msize == 1);
        std::string* strStream = stream;
        try {
            strStream->append(static_cast<const char*>(ptr), msize * n);
        }
        catch(...) {
            return 0;
        }
        return msize * n;
    }

    Int32 CHttpBase::progressCallback(void* userData, double dlTotal, double dlNow, double ulTotal, double ulNow)
    {
        CHttpBase* pThis = (CHttpBase*)userData;
        pThis->m_downloadTotal  = dlTotal;
        pThis->m_downloadNow    = dlNow;
        pThis->m_uploadTotal    = ulTotal;
        pThis->m_uploadNow      = ulNow;
        ::curl_easy_getinfo(pThis->m_pLibCURL, CURLINFO_SPEED_DOWNLOAD, &pThis->m_downloadSpeed);
        ::curl_easy_getinfo(pThis->m_pLibCURL, CURLINFO_SPEED_UPLOAD, &pThis->m_uploadSpeed);

        if (pThis->m_cancelDelegate.empty())
            return 0;

        if (pThis->m_cancelDelegate()) // true for cancel
            return -1;
        else
            return 0;
    }
}

//--------------------------------------------------------------------------